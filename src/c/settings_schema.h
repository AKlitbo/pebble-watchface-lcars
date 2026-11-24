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

/** @brief The layouts the face can be drawn as. The number is what is saved. */
typedef enum
{
    LCARS_LAYOUT_STARDATE, ///< The default: the big clock over a 2x2 ops grid
    LCARS_LAYOUT_TACTICAL,   ///< Tactical: three 2x2 grids, top, middle, and bottom, with the clock and date placed as panels
    LCARS_LAYOUT_ENGINEERING,     ///< Engineering: the rail is a power core sweeping into both strips, the clock stacked, and six panels in one column
    LCARS_LAYOUT_COUNT
} LcarsLayout;

/// How many picks the Layout picker offers. Each layout saves as twice its LcarsLayout, and its
/// mirrored frame, with the rail down the right, as one more, so Stardate is 0, Stardate Mirrored 1,
/// Tactical 2, Tactical Mirrored 3, Engineering 4, and Engineering Mirrored 5
#define LCARS_LAYOUT_PICKS (LCARS_LAYOUT_COUNT * 2)

/// How many frame themes the picker offers. The night and Quiet Time overrides count the same set,
/// and so does the frame table in theme.c, so adding a theme is one bump here
#define LCARS_THEME_COUNT 9

/**
 * @brief A side of the face. Bar Labels saves it as its number. The rail's side rides the Layout pick
 * instead, read through lcars_rail_right.
 */
typedef enum
{
    LCARS_SIDE_LEFT,  ///< The default for both
    LCARS_SIDE_RIGHT,
    LCARS_SIDE_COUNT
} LcarsSide;

/**
 * @brief Whether the frame is mirrored, with its rail down the right. The Layout picker offers this
 * as a mirrored version of each layout.
 *
 * @return True for the right.
 */
bool lcars_rail_right(void);

/**
 * @brief Whether each bar's word sits at its right end.
 *
 * @return True for the right.
 */
bool lcars_labels_right(void);

/// How many cells Tactical holds, three grids of four
#define LCARS_TACTICAL_CELLS 12

/**
 * @brief Which layout the face is drawn as, whichever side its rail is on.
 *
 * @return A LcarsLayout.
 */
uint8_t lcars_layout(void);

/**
 * @brief What one cell of Tactical is set to show.
 *
 * The saved list is read from the front, so a list longer than the twelve cells gives its first
 * twelve picks and the rest are never read.
 *
 * @param cell The cell, 0 to 11. Each grid's four run down each column, top grid first.
 * @return An OpsId, OPS_EMPTY for a cell the saved list leaves out or holds no number for.
 */
uint8_t lcars_slot_tactical(int cell);

/// How many cells Engineering holds, one column of six
#define LCARS_ENGINEERING_CELLS 6

/**
 * @brief What one cell of Engineering is set to show, read from its saved list the same way as Tactical's.
 *
 * @param cell The cell, 0 to 5, top to bottom.
 * @return An OpsId, OPS_EMPTY for a cell the saved list leaves out or holds no number for.
 */
uint8_t lcars_slot_engineering(int cell);

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
