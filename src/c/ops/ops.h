/**
 * @file ops.h
 * @brief The catalogue behind the two pickable ops slots. Each entry pairs an LCARS holder-box
 * word with a glyph and a value formatter, so picking a readout swaps all three together.
 *
 * The face draws its labels and glyphs at runtime rather than baking them into the frame, which
 * is what lets a slot change without new artwork for each theme.
 *
 * @ingroup watchface-lcars
 */
#pragma once
#include <pebble.h>

/**
 * @addtogroup watchface-lcars
 * @{
 */

/**
 * @brief What a slot can show.
 *
 * The pick is persisted as its number, so entries are only ever appended. Reordering these
 * would repoint every watch in the field at a different readout.
 */
typedef enum
{
    OPS_HEART,
    OPS_STEPS,
    OPS_BATTERY,
    OPS_CALORIES,
    OPS_SLEEP,
    OPS_ACTIVE,
    OPS_MOON_PCT,
    OPS_MOON_PHASE,
    OPS_MOON_NEXT,
    OPS_SUNRISE,
    OPS_SUNSET,
    OPS_DAYLIGHT,
    OPS_SUN_NEXT,
    OPS_HUMIDITY,
    OPS_WIND,
    OPS_UV,
    OPS_HILO,
    OPS_JULIAN,
    OPS_DAY_OF_YEAR,
    OPS_WEEK,
    OPS_EMPTY,
    // this sits after the empty row rather than beside the other weather ones
    // the order here is the saved wire value so only the end of it is free
    OPS_WEATHER_TEMP,
    OPS_WEATHER_COND,
    OPS_SENSORS,
    OPS_EPOCH,
    OPS_BEATS,
    OPS_ZONE_1,
    OPS_NEXT_ALARM,
    // the panels bigger than one slot. a wide one fills a row and a full one fills all four
    OPS_WIDE_DAYLIGHT,
    OPS_WIDE_STEPS,
    OPS_FULL_HEART,
    OPS_FULL_STEPS,
    OPS_WIDE_HEART,
    OPS_WIDE_MUSIC,
    OPS_FULL_MUSIC,
    OPS_FULL_AGENDA,
    OPS_WIDE_EVENT,
    // how far through the hour, day, week, and year the clock is, as a reading and as a bar a row wide
    OPS_HOUR_PROGRESS,
    OPS_DAY_PROGRESS,
    OPS_WEEK_PROGRESS,
    OPS_YEAR_PROGRESS,
    OPS_WIDE_HOUR_PROGRESS,
    OPS_WIDE_DAY_PROGRESS,
    OPS_WIDE_WEEK_PROGRESS,
    OPS_WIDE_YEAR_PROGRESS,
    // the chance of rain and the air readings, then the countdowns to the end of the year and the
    // days of the week
    OPS_PRECIP,
    OPS_FEELS_LIKE,
    OPS_PRESSURE,
    OPS_DEW_POINT,
    OPS_DAYS_LEFT,
    OPS_WEEKS_LEFT,
    OPS_WEEKDAYS,
    // the hourly and four day forecasts, each across all four
    OPS_FULL_FORECAST_HOURLY,
    OPS_FULL_FORECAST_DAILY,
    // a row each: the UV index and the beat day as segmented bars, the moon's phase with its
    // countdown, and the next alarm with how long until it
    OPS_WIDE_UV,
    OPS_WIDE_MOON,
    OPS_WIDE_ALARM,
    OPS_WIDE_BEATS,
    OPS_COUNT
} OpsId;

#define OPS_CELLS_MAX 4 ///< The cells the ops grid holds, two columns of two

/**
 * @brief The ops grid: how many columns and rows it holds, and where each one sits.
 *
 * The cells are numbered down each column, so a cell's column is its number divided by the rows,
 * and its row is what is left over. On a 2x2 grid that runs left top, left bottom, right top,
 * right bottom.
 */
typedef struct
{
    uint8_t cols;          ///< Columns across the grid
    uint8_t rows;          ///< Rows down each column
    int16_t col_x[2];      ///< Each column's bar, from the left of the screen
    int16_t col_right[2];  ///< How far a reading in each column may run
    int16_t row_y[2];      ///< Each row's bar, from the top of the screen
    int16_t bottom;        ///< Where the last row ends, so it is as tall as the others
} OpsGrid;

/**
 * @brief How many cells a grid holds.
 *
 * @param grid The grid.
 * @return Its columns times its rows.
 */
static inline int ops_cell_count(const OpsGrid *grid)
{
    return grid->cols * grid->rows;
}

/**
 * @brief How much of the ops grid a readout fills.
 *
 * A big readout is saved in the slot it starts from, and the slots it covers are saved as empty.
 */
typedef enum
{
    OPS_SHAPE_SINGLE, ///< One slot
    OPS_SHAPE_TALL,   ///< The left column, starting upper left
    OPS_SHAPE_WIDE,   ///< A row, starting from its left slot
    OPS_SHAPE_FULL,   ///< All four, starting upper left
} OpsShape;

/**
 * @brief One catalogue entry: the holder-box word, the glyph, and the value formatter.
 *
 * The two _for hooks are for readouts whose word or glyph moves with the reading, like the sun
 * countdown flipping between DAWN IN and DUSK IN. An entry sets the plain field or the hook,
 * never both.
 */
typedef struct
{
    const char *label;                      ///< Fixed holder-box word, or NULL when label_for supplies it
    const char *(*label_for)(void);         ///< Word that moves with the reading, or NULL
    uint32_t    icon;                       ///< Fixed resource id, 0 for no glyph
    uint32_t  (*icon_for)(void);            ///< Glyph that moves with the reading, or NULL
    void      (*text)(char *out, size_t n); ///< Value formatter, or NULL to leave the slot blank
    OpsShape    shape;                      ///< How much of the grid it fills. Anything past one slot is drawn by the face, not the table
} OpsReadout;

/**
 * @brief The catalogue entry for a stored pick.
 *
 * A number past the end of the catalogue falls back to the empty entry, so a blob saved by a later
 * version can never point the face at nothing.
 *
 * @param id The stored OpsId.
 * @return The entry, never NULL.
 */
const OpsReadout *ops_entry(uint8_t id);

/**
 * @brief The holder-box word for an entry, resolved through label_for when it has one.
 *
 * @param entry The catalogue entry.
 * @return The word, or "" when the slot draws no label.
 */
const char *ops_label(const OpsReadout *entry);

/**
 * @brief The glyph for an entry, resolved through icon_for when it has one.
 *
 * @param entry The catalogue entry.
 * @return The resource id, or 0 when the slot draws no glyph.
 */
uint32_t ops_icon(const OpsReadout *entry);

/**
 * @brief Format an entry's value into a buffer, writing "" for a slot that shows nothing.
 *
 * @param entry The catalogue entry.
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text(const OpsReadout *entry, char *out, size_t n);

/**
 * @brief How much of the grid a pick fills.
 *
 * @param id The stored OpsId.
 * @return Its shape, OPS_SHAPE_SINGLE for an ordinary readout.
 */
OpsShape ops_shape(uint8_t id);

/**
 * @brief Which cell fills each cell of a grid, from the saved picks.
 *
 * A big pick only counts in a cell where the face draws it: the tall block in the left column, a
 * full panel from the left column, and a wide panel from the left of its row. When two picks claim a
 * cell the earlier one keeps it, and a big pick that does not fit owns nothing. The settings page's
 * slot builder makes the same calls, so the watch never shows a different face from the one the
 * builder drew.
 *
 * @param grid The ops grid.
 * @param picks The saved picks, one a cell in cell order.
 * @param owner Receives the cell that fills each cell, or -1 for one nothing fills.
 */
void ops_owners(const OpsGrid *grid, const uint8_t *picks, int8_t *owner);

/** @} */
