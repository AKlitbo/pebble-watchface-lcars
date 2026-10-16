/**
 * @file settings_schema.h
 * @brief The face's settings schema. A blob saved before the settings had a version is
 * lifted across by a migration. Key 5 and the 21-byte v1 size never change, since
 * changing either makes every watch already running the face lose its settings.
 *
 * @ingroup watchface-lcars
 */
#pragma once
#include "system/settings/settings.h"

/**
 * @addtogroup watchface-lcars
 * @{
 */

/**
 * @brief How the frame theme is chosen.
 *
 * Fixed keeps the one theme the wearer picked. Day / Night / Quiet keeps it by day and swaps to the
 * night or Quiet Time theme when those are turned on.
 */
typedef enum
{
    THEME_MODE_FIXED = 0,
    THEME_MODE_DAYNIGHT,
    THEME_MODE_COUNT
} ThemeMode;

/**
 * @brief Gets the settings schema for this face.
 *
 * @return A pointer to the schema.
 */
const SettingsSchema *lcars_settings_schema(void);

/**
 * @brief Which way the frame theme is chosen, a ThemeMode.
 *
 * @return THEME_MODE_FIXED or THEME_MODE_DAYNIGHT.
 */
uint8_t lcars_theme_mode(void);

/**
 * @brief Whether Day / Night / Quiet mode swaps to the night theme after dark.
 *
 * @return True when the night theme is turned on.
 */
bool lcars_use_night_theme(void);

/**
 * @brief The theme to show after dark in Day / Night / Quiet mode.
 *
 * @return A theme setting value.
 */
uint8_t lcars_night_theme(void);

/**
 * @brief Whether Day / Night / Quiet mode swaps to the Quiet Time theme while Quiet Time is on.
 *
 * @return True when the Quiet Time theme is turned on.
 */
bool lcars_use_quiet_theme(void);

/**
 * @brief The theme to show while Quiet Time is on in Day / Night / Quiet mode.
 *
 * @return A theme setting value.
 */
uint8_t lcars_quiet_theme(void);

/**
 * @brief What the upper left slot is set to show.
 *
 * The face's own settings rather than the library's, so they carry ids past SETTING_COUNT and are
 * read back here instead of through settings_u8.
 *
 * @return An OpsId.
 */
uint8_t lcars_slot_lt(void);

/**
 * @brief What the lower left slot is set to show. Ignored while the upper left holds the tall
 * weather block, which fills the whole column.
 *
 * @return An OpsId.
 */
uint8_t lcars_slot_lb(void);

/**
 * @brief What the upper right slot is set to show.
 *
 * @return An OpsId.
 */
uint8_t lcars_slot_rt(void);

/**
 * @brief What the lower right slot is set to show.
 *
 * @return An OpsId.
 */
uint8_t lcars_slot_rb(void);

/**
 * @brief Whether the first alternate time zone is picked at all.
 *
 * The setting is empty until the wearer picks a zone, and empty again once they clear the picker.
 *
 * @return True when a zone is set.
 */
bool lcars_zone_1_is_set(void);

/**
 * @brief How far the first alternate time zone runs from UTC.
 *
 * The setting arrives from Clay's location search as "offset,City, Region, CC", so this is the
 * number in front of the first comma.
 *
 * @return Minutes ahead of UTC, negative behind it. Zero when nothing is set.
 */
int16_t lcars_zone_1_offset_minutes(void);

/**
 * @brief The first alternate time zone's place name, for the holder box.
 *
 * @return Everything after the first comma, or "" when nothing is set. Never NULL.
 */
const char *lcars_zone_1_name(void);

/** @} */
