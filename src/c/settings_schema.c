/**
 * @file settings_schema.c
 * @brief The face's settings schema, the struct its settings are saved in and read back from.
 *
 * Key 5 and the 21-byte v1 size never change, since changing either makes every watch
 * already running the face lose its settings. The daily goals sit in a second struct on key 6,
 * chained on after it.
 *
 * @ingroup watchface-lcars
 */
#include "settings_schema.h"
#include "ops/ops.h"
#include "system/settings/settings_catalog.h"
#include "system/settings/setting_values.h"
#include "clock/zone_setting.h"
#include "health/goals.h"
#include "layout/layout_string.h"
#include "persist_keys.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define LCARS_SETTINGS_VERSION 8
// smallest versioned blob accepted. fields are append-only so this never changes
#define LCARS_SETTINGS_V1_SIZE 21

// how many frame themes the picker offers. the night and Quiet Time overrides count the same set,
// so adding a theme is one bump here rather than three spread through the field table
#define LCARS_THEME_COUNT 9

/**
 * @addtogroup watchface-lcars
 * @{
 */

/**
 * @brief lcars's persisted settings.
 *
 * The byte layout is frozen: it shipped as a flat blob (version byte, then
 * each field in this order), so members may be renamed but never reordered,
 * retyped, or inserted. Only appended. The static asserts below guard it.
 */
typedef struct LcarsSettings
{
    uint8_t version;
    uint8_t temperature_unit;
    char    date_format[16];
    uint8_t theme;
    uint8_t steps_mode;
    uint8_t time_format;
    bool    bluetooth_icon;
    uint8_t vibe_connect;
    uint8_t vibe_disconnect;
    bool    quiet_time_icon;
    uint8_t hourly_vibe;
    uint8_t slot_lt;
    uint8_t slot_lb;
    uint8_t slot_rt;
    uint8_t slot_rb;
    char    time_zone_offset_1[32]; // "offset,City, Region, CC" as Clay's location search sends it
    // --- added in v7 ---
    uint8_t theme_mode;       // a ThemeMode
    bool    use_night_theme;
    uint8_t night_theme;
    bool    use_quiet_theme;
    uint8_t quiet_theme;
    // --- added in v8 ---
    uint8_t layout;            // a LcarsLayout
    char    slots_tactical[48];  // Tactical's twelve picks as "84,20,20,20,...", in cell order
} LcarsSettings;

_Static_assert(sizeof(LcarsSettings) == 116, "lcars blob size is frozen at 116 bytes (v8)");

// twelve picks of up to three digits with commas between, and the end of the string, so the list
// keeps every cell once the catalogue passes a hundred readouts
_Static_assert(OPS_COUNT <= 1000, "a Tactical pick has to fit three digits");
_Static_assert(sizeof(((LcarsSettings *)0)->slots_tactical) >= 12 * 3 + 11 + 1,
               "slots_tactical has to hold twelve three-digit picks");
_Static_assert(offsetof(LcarsSettings, bluetooth_icon) == LCARS_SETTINGS_V1_SIZE,
               "lcars v1 floor is frozen; the v2 fields must stay appended after it");

static LcarsSettings s_settings;

// the four ops slots are this face's own and are numbered past the shared ones
// the library still defaults and cleans and saves them. it will not index them for
// a typed read though so the accessors below reach into the struct
// the cast is there because the field table's id is a SettingId and mixing enums
// is an error
#define SETTING_SLOT_LT ((SettingId)(SETTING_COUNT))
#define SETTING_SLOT_LB ((SettingId)(SETTING_COUNT + 1))
#define SETTING_SLOT_RT ((SettingId)(SETTING_COUNT + 2))
#define SETTING_SLOT_RB ((SettingId)(SETTING_COUNT + 3))

// the frame-theme mode and its night and Quiet Time overrides, also this face's own
#define SETTING_THEME_MODE     ((SettingId)(SETTING_COUNT + 4))
#define SETTING_NIGHT_THEME_ON ((SettingId)(SETTING_COUNT + 5))
#define SETTING_NIGHT_THEME    ((SettingId)(SETTING_COUNT + 6))
#define SETTING_QUIET_THEME_ON ((SettingId)(SETTING_COUNT + 7))
#define SETTING_QUIET_THEME    ((SettingId)(SETTING_COUNT + 8))

// which layout the face is drawn as, also this face's own
#define SETTING_LAYOUT         ((SettingId)(SETTING_COUNT + 9))

// Tactical's picks on a fresh install: the clock across the top, the Sensors Block beside the heart
// rate and steps in the middle, as the Stardate layout starts, and the date across the bottom
#define TACTICAL_SLOTS_DEFAULT "84,20,20,20,23,20,0,1,85,20,20,20"

// lcars subscribes to every known setting in its frozen struct order. every KNOWN_ entry takes
// its default from the settings page, so the date opens on the numeric stardate-style "%Y.%m%d"
static const SettingField s_fields[] = {
    KNOWN_TEMPERATURE_UNIT(offsetof(LcarsSettings, temperature_unit)),
    KNOWN_DATE_FORMAT(LcarsSettings, date_format),
    KNOWN_THEME(offsetof(LcarsSettings, theme), LCARS_THEME_COUNT),
    KNOWN_STEPS_MODE(offsetof(LcarsSettings, steps_mode)),
    KNOWN_TIME_FORMAT(offsetof(LcarsSettings, time_format)),
    KNOWN_BLUETOOTH_ICON(offsetof(LcarsSettings, bluetooth_icon)),
    KNOWN_BLUETOOTH_VIBE_CONNECT(offsetof(LcarsSettings, vibe_connect)),
    KNOWN_BLUETOOTH_VIBE_DISCONNECT(offsetof(LcarsSettings, vibe_disconnect)),
    KNOWN_QUIET_TIME_ICON(offsetof(LcarsSettings, quiet_time_icon)),
    KNOWN_HOURLY_VIBE(offsetof(LcarsSettings, hourly_vibe)),

    // --- this face's own ---
    // an untouched face shows the weather block filling the left column with the
    // heart rate over the step count on the right. picking these defaults means
    // someone upgrading keeps the watch they are already looking at
    { .id = SETTING_SLOT_LT, .message_key = &MESSAGE_KEY_APPEARANCE_SLOT_LEFT_TOP,
      .type = SETTING_ENUM_U8, .offset = offsetof(LcarsSettings, slot_lt),
      .enum_count = OPS_COUNT, .default_num = OPS_SENSORS },
    { .id = SETTING_SLOT_LB, .message_key = &MESSAGE_KEY_APPEARANCE_SLOT_LEFT_BOTTOM,
      .type = SETTING_ENUM_U8, .offset = offsetof(LcarsSettings, slot_lb),
      .enum_count = OPS_COUNT, .default_num = OPS_EMPTY },
    { .id = SETTING_SLOT_RT, .message_key = &MESSAGE_KEY_APPEARANCE_SLOT_RIGHT_TOP,
      .type = SETTING_ENUM_U8, .offset = offsetof(LcarsSettings, slot_rt),
      .enum_count = OPS_COUNT, .default_num = OPS_HEART },
    { .id = SETTING_SLOT_RB, .message_key = &MESSAGE_KEY_APPEARANCE_SLOT_RIGHT_BOTTOM,
      .type = SETTING_ENUM_U8, .offset = offsetof(LcarsSettings, slot_rb),
      .enum_count = OPS_COUNT, .default_num = OPS_STEPS },

    // the frame-theme mode and its two overrides. the override themes count the same nine themes
    // as the main picker above, so they stay in step with it
    { .id = SETTING_THEME_MODE, .message_key = &MESSAGE_KEY_APPEARANCE_THEME_MODE,
      .type = SETTING_ENUM_U8, .offset = offsetof(LcarsSettings, theme_mode),
      .enum_count = THEME_MODE_COUNT, .default_num = THEME_MODE_FIXED },
    { .id = SETTING_NIGHT_THEME_ON, .message_key = &MESSAGE_KEY_APPEARANCE_NIGHT_THEME_ON,
      .type = SETTING_BOOL, .offset = offsetof(LcarsSettings, use_night_theme),
      .default_num = 0 },
    { .id = SETTING_NIGHT_THEME, .message_key = &MESSAGE_KEY_APPEARANCE_NIGHT_THEME,
      .type = SETTING_ENUM_U8, .offset = offsetof(LcarsSettings, night_theme),
      .enum_count = LCARS_THEME_COUNT, .default_num = 0 },
    { .id = SETTING_QUIET_THEME_ON, .message_key = &MESSAGE_KEY_APPEARANCE_QUIET_THEME_ON,
      .type = SETTING_BOOL, .offset = offsetof(LcarsSettings, use_quiet_theme),
      .default_num = 0 },
    { .id = SETTING_QUIET_THEME, .message_key = &MESSAGE_KEY_APPEARANCE_QUIET_THEME,
      .type = SETTING_ENUM_U8, .offset = offsetof(LcarsSettings, quiet_theme),
      .enum_count = LCARS_THEME_COUNT, .default_num = 0 },

    // the Stardate layout keeps the four slot keys above, and Tactical keeps its twelve picks in one
    // text key, so its grids need no hidden store a cell on the settings page. the id is SETTING_COUNT
    // for the same reason as the zone below
    { .id = SETTING_LAYOUT, .message_key = &MESSAGE_KEY_APPEARANCE_LAYOUT,
      .type = SETTING_ENUM_U8, .offset = offsetof(LcarsSettings, layout),
      .enum_count = LCARS_LAYOUT_COUNT, .default_num = LCARS_LAYOUT_STARDATE },
    { .id = SETTING_COUNT, .message_key = &MESSAGE_KEY_APPEARANCE_SLOTS_TACTICAL, .type = SETTING_CSTRING,
      .offset = offsetof(LcarsSettings, slots_tactical),
      .size = sizeof(s_settings.slots_tactical),
      .default_str = TACTICAL_SLOTS_DEFAULT },

    // the ZONE 1 readout's zone. the id is SETTING_COUNT because nothing reads this through the
    // library's typed accessors, only the two helpers at the bottom of this file
    { .id = SETTING_COUNT, .message_key = &MESSAGE_KEY_CLOCK_TIMEZONE_1, .type = SETTING_CSTRING,
      .offset = offsetof(LcarsSettings, time_zone_offset_1),
      .size = sizeof(s_settings.time_zone_offset_1),
      .default_str = "" },
};

/**
 * @brief The pre-versioning layout (no version field) that shipped under key 5.
 *
 * Kept only so the migration can lift an existing blob into the current struct.
 */
typedef struct ClaySettingsV0
{
    bool TemperatureUnit;
    char DateFormat[16];
    uint8_t Theme;
    uint8_t StepsMode;
    uint8_t TimeFormat;
} ClaySettingsV0;

/**
 * @brief Lift a pre-versioning blob, preserving every field.
 *
 * Recognized purely by its size, which never collides with a versioned blob.
 *
 * @param stored_size The size of the stored blob.
 * @return True if migrated, false otherwise.
 */
static bool lcars_migrate(int stored_size)
{
    if (stored_size != (int)sizeof(ClaySettingsV0))
    {
        return false;
    }

    ClaySettingsV0 legacy;
    persist_read_data(LCARS_SETTINGS_KEY, &legacy, sizeof(legacy));

    s_settings.temperature_unit = legacy.TemperatureUnit;
    memcpy(s_settings.date_format, legacy.DateFormat, sizeof(s_settings.date_format));
    s_settings.theme = legacy.Theme;
    s_settings.steps_mode = legacy.StepsMode;
    s_settings.time_format = legacy.TimeFormat;

    return true;
}

#define LCARS_GOALS_VERSION 1

/**
 * @brief The daily goals and the goal buzz, saved on their own key beside the main blob.
 *
 * Keeping them apart leaves the main blob at its frozen layout, so a saved watch reads back every
 * setting it holds. Like the main one, members may be renamed but only ever appended.
 */
typedef struct LcarsGoalSettings
{
    uint8_t version;               ///< the goals blob's own version
    uint8_t goal_steps;            ///< an index into goals_steps
    uint8_t goal_distance;         ///< an index into goals_distance_m
    uint8_t goal_calories;         ///< an index into goals_calories
    uint8_t goal_active;           ///< an index into goals_active_min
    uint8_t goal_sleep;            ///< an index into goals_sleep_min
    uint8_t distance_unit;         ///< a DistanceUnit, for the Distance Goal Bar
    char    goal_vibe[64];         ///< the Goal Met Vibration pick
    char    goal_vibe_custom[122]; ///< the Custom Vibration Pattern
} LcarsGoalSettings;

// the goals blob's first size. fields are append-only so this never changes
#define LCARS_GOALS_V1_SIZE 193

_Static_assert(sizeof(LcarsGoalSettings) == LCARS_GOALS_V1_SIZE, "lcars goals blob size is frozen at 193 bytes (v1)");

static LcarsGoalSettings s_goals;

// the heart rate limit is left out, since no panel here draws a heart rate bar against it
static const SettingField s_goal_fields[] = {
    KNOWN_GOAL_STEPS(offsetof(LcarsGoalSettings, goal_steps)),
    KNOWN_GOAL_DISTANCE(offsetof(LcarsGoalSettings, goal_distance)),
    KNOWN_GOAL_CALORIES(offsetof(LcarsGoalSettings, goal_calories)),
    KNOWN_GOAL_ACTIVE(offsetof(LcarsGoalSettings, goal_active)),
    KNOWN_GOAL_SLEEP(offsetof(LcarsGoalSettings, goal_sleep)),
    KNOWN_DISTANCE_UNIT(offsetof(LcarsGoalSettings, distance_unit)),
    KNOWN_GOAL_VIBE(LcarsGoalSettings, goal_vibe),
    KNOWN_GOAL_VIBE_CUSTOM(LcarsGoalSettings, goal_vibe_custom),
};

static const SettingsSchema s_goals_schema = {
    .key = LCARS_GOALS_KEY,
    .version = LCARS_GOALS_VERSION,
    SETTINGS_BLOB(s_goals, LCARS_GOALS_V1_SIZE),
    .fields = s_goal_fields,
    .field_count = ARRAY_LENGTH(s_goal_fields),
};

static const SettingsSchema s_schema = {
    .key = LCARS_SETTINGS_KEY,
    .version = LCARS_SETTINGS_VERSION,
    SETTINGS_BLOB(s_settings, LCARS_SETTINGS_V1_SIZE),
    .fields = s_fields,
    .field_count = ARRAY_LENGTH(s_fields),
    .migrate = lcars_migrate,
    .companion = &s_goals_schema,
};

const SettingsSchema *lcars_settings_schema(void)
{
    return &s_schema;
}

uint8_t lcars_slot_lt(void)
{
    return s_settings.slot_lt;
}

uint8_t lcars_slot_lb(void)
{
    return s_settings.slot_lb;
}

uint8_t lcars_slot_rt(void)
{
    return s_settings.slot_rt;
}

uint8_t lcars_slot_rb(void)
{
    return s_settings.slot_rb;
}

uint8_t lcars_theme_mode(void)
{
    return s_settings.theme_mode;
}

bool lcars_use_night_theme(void)
{
    return s_settings.use_night_theme;
}

uint8_t lcars_night_theme(void)
{
    return s_settings.night_theme;
}

bool lcars_use_quiet_theme(void)
{
    return s_settings.use_quiet_theme;
}

uint8_t lcars_quiet_theme(void)
{
    return s_settings.quiet_theme;
}

uint8_t lcars_layout(void)
{
    return s_settings.layout;
}

uint8_t lcars_slot_tactical(int cell)
{
    const char *cursor = s_settings.slots_tactical;

    // the picks are read in order up to the one asked for. a list that runs out, or holds anything
    // but numbers and commas, leaves the rest of the cells empty rather than reading past it
    for (int index = 0; index <= cell; index++)
    {
        if (*cursor < '0' || *cursor > '9')
        {
            return OPS_EMPTY;
        }

        int value = layout_parse_int(&cursor);

        if (index == cell)
        {
            return value >= 0 && value < OPS_COUNT ? (uint8_t)value : OPS_EMPTY;
        }

        if (*cursor != ',')
        {
            return OPS_EMPTY;
        }

        cursor++;
    }

    return OPS_EMPTY;
}

bool lcars_zone_1_is_set(void)
{
    return zone_setting_is_set(s_settings.time_zone_offset_1);
}

int16_t lcars_zone_1_offset_minutes(void)
{
    return zone_setting_offset(s_settings.time_zone_offset_1);
}

const char *lcars_zone_1_name(void)
{
    return zone_setting_label(s_settings.time_zone_offset_1);
}

/** @} */
