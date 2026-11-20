/**
 * @file ops.c
 * @brief The ops catalogue: one row per readout a slot can show.
 *
 * Indexed by OpsId so a row can never drift from the number that is persisted for it. The words
 * are the LCARS vocabulary the rest of the frame already speaks (SENSORS, STARDATE), which is why
 * the heart rate reads VITALS rather than "heart rate".
 */
#include "ops/ops.h"

#include "ops/ops_text.h"
#include "ui/readouts.h"

// a word wider than the holder box gets clipped rather than wrapped, so these stay inside the
// nine characters LBL_OPS_A can hold at Antonio 10
static const OpsReadout s_catalog[OPS_COUNT] = {
    [OPS_HEART]       = {.label = "VITALS",    .icon = RESOURCE_ID_ICON_HEART,       .text = readout_hr},
    [OPS_STEPS]       = {.label = "TRAVERSAL", .icon = RESOURCE_ID_ICON_FEET,        .text = readout_steps},
    [OPS_BATTERY]     = {.label = "POWER",     .icon = RESOURCE_ID_ICON_POWER,       .text = ops_text_battery},
    [OPS_CALORIES]    = {.label = "METABOLIC", .icon = RESOURCE_ID_ICON_CALORIES,    .text = ops_text_calories},
    [OPS_SLEEP]       = {.label = "REGEN",     .icon = RESOURCE_ID_ICON_SLEEP,       .text = ops_text_sleep},
    [OPS_ACTIVE]      = {.label = "EXERTION",  .icon = RESOURCE_ID_ICON_ACTIVE,      .text = ops_text_active},

    [OPS_MOON_PCT]    = {.label = "LUNAR",     .icon_for = ops_moon_icon,            .text = ops_text_moon_pct},
    [OPS_MOON_PHASE]  = {.label = "LUNAR",     .icon_for = ops_moon_icon,            .text = ops_text_moon_phase},
    [OPS_MOON_NEXT]   = {.label_for = ops_moon_next_label,
                         .icon_for = ops_moon_icon,                                  .text = ops_text_moon_next},

    [OPS_SUNRISE]     = {.label = "DAWN",      .icon = RESOURCE_ID_ICON_SUNRISE,     .text = ops_text_sunrise},
    [OPS_SUNSET]      = {.label = "DUSK",      .icon = RESOURCE_ID_ICON_SUNSET,      .text = ops_text_sunset},
    [OPS_DAYLIGHT]    = {.label = "DAYLIGHT",  .icon = RESOURCE_ID_ICON_SUN,         .text = ops_text_daylight},
    [OPS_SUN_NEXT]    = {.label_for = ops_sun_next_label,
                         .icon_for = ops_sun_next_icon,                              .text = ops_text_sun_next},

    [OPS_HUMIDITY]    = {.label = "ATMOS",     .icon = RESOURCE_ID_ICON_HUMIDITY,    .text = ops_text_humidity},
    [OPS_WIND]        = {.label = "AIRFLOW",   .icon = RESOURCE_ID_ICON_WIND,        .text = ops_text_wind},
    [OPS_UV]          = {.label = "UV INDEX",  .icon = RESOURCE_ID_ICON_UV,          .text = ops_text_uv},
    // the weather block's thermometer is cut 13x17 for its own taller box, so this row takes a
    // shorter one that fits the 14x14 ops glyph without losing its bulb
    [OPS_HILO]        = {.label = "RANGE",     .icon = RESOURCE_ID_ICON_THERMOMETER_SM, .text = ops_text_hilo},

    // the three calendar counts share one glyph. they are all a way of numbering a day
    [OPS_JULIAN]      = {.label = "JULIAN",    .icon = RESOURCE_ID_ICON_CALENDAR,    .text = ops_text_julian},
    [OPS_DAY_OF_YEAR] = {.label = "SOL",       .icon = RESOURCE_ID_ICON_CALENDAR,    .text = ops_text_day_of_year},
    [OPS_WEEK]        = {.label = "CYCLE",     .icon = RESOURCE_ID_ICON_CALENDAR,    .text = ops_text_week},

    // the row stays in the table so the empty pick resolves like any other
    [OPS_EMPTY]       = {0},

    // the same two readings the SENSORS block shows but sized to fit an ops row
    [OPS_WEATHER_TEMP] = {.label = "THERMAL", .icon = RESOURCE_ID_ICON_THERMOMETER_SM,
                          .text = readout_weather_temp},
    [OPS_WEATHER_COND] = {.label = "SKY",     .icon_for = ops_wx_icon,
                          .text = readout_weather_cond},

    // the tall weather block. one bar over a big condition glyph and a large
    // temperature filling both rows of its column
    // the row here carries only the word. layout draws the rest from its own slots
    // rather than through this table
    [OPS_SENSORS]      = {.label = "SENSORS", .shape = OPS_SHAPE_TALL},

    // three other ways of saying what time it is
    // epoch takes no glyph on purpose: ten digits only fit once the row hands its icon space
    // back to the value, which layout does for any row with no glyph
    [OPS_EPOCH]        = {.label = "EPOCH",   .text = ops_text_epoch},
    [OPS_BEATS]        = {.label = "BEATS",   .icon = RESOURCE_ID_ICON_GLOBE,
                          .text = ops_text_beats},
    [OPS_ZONE_1]       = {.label_for = ops_zone_1_label, .icon = RESOURCE_ID_ICON_GLOBE,
                          .text = ops_text_zone_1},

    // the watch's own alarm, read straight off the alarm service
    [OPS_NEXT_ALARM]   = {.label = "ALARM",   .icon = RESOURCE_ID_ICON_ALARM,
                          .text = ops_text_next_alarm},

    // the bigger panels carry only the word, and widgets/panels.c draws the rest
    [OPS_WIDE_DAYLIGHT] = {.label = "DAYLIGHT",  .shape = OPS_SHAPE_WIDE},
    [OPS_WIDE_STEPS]    = {.label = "TRAVERSAL", .shape = OPS_SHAPE_WIDE},
    [OPS_FULL_HEART]    = {.label = "VITALS TODAY",    .shape = OPS_SHAPE_FULL},
    [OPS_FULL_STEPS]    = {.label = "TRAVERSAL TODAY", .shape = OPS_SHAPE_FULL},
    [OPS_WIDE_HEART]    = {.label = "VITALS",    .shape = OPS_SHAPE_WIDE},
    [OPS_WIDE_MUSIC]    = {.label = "AUDIO",     .shape = OPS_SHAPE_WIDE},
    [OPS_FULL_MUSIC]    = {.label = "AUDIO",     .shape = OPS_SHAPE_FULL},
    [OPS_FULL_AGENDA]   = {.label = "SCHEDULE",  .shape = OPS_SHAPE_FULL},
    [OPS_WIDE_EVENT]    = {.label = "SCHEDULE",  .shape = OPS_SHAPE_WIDE},

    // how far through each period the clock is. the hour and the day take a clock face, and the week
    // and the year the calendar
    [OPS_HOUR_PROGRESS] = {.label = "HOUR",    .icon = RESOURCE_ID_ICON_CLOCK,
                           .text = ops_text_hour_progress},
    [OPS_DAY_PROGRESS]  = {.label = "DAY",   .icon = RESOURCE_ID_ICON_CLOCK,
                           .text = ops_text_day_progress},
    [OPS_WEEK_PROGRESS] = {.label = "WEEK",     .icon = RESOURCE_ID_ICON_CALENDAR,
                           .text = ops_text_week_progress},
    [OPS_YEAR_PROGRESS] = {.label = "YEAR",    .icon = RESOURCE_ID_ICON_CALENDAR,
                           .text = ops_text_year_progress},
    [OPS_WIDE_HOUR_PROGRESS] = {.label = "HOUR",  .shape = OPS_SHAPE_WIDE},
    [OPS_WIDE_DAY_PROGRESS]  = {.label = "DAY", .shape = OPS_SHAPE_WIDE},
    [OPS_WIDE_WEEK_PROGRESS] = {.label = "WEEK",   .shape = OPS_SHAPE_WIDE},
    [OPS_WIDE_YEAR_PROGRESS] = {.label = "YEAR",  .shape = OPS_SHAPE_WIDE},

    [OPS_PRECIP]        = {.label = "PRECIP",    .icon = RESOURCE_ID_ICON_WX_SM_RAIN,
                           .text = ops_text_precip},
    [OPS_FEELS_LIKE]    = {.label = "FEELS LIKE", .icon = RESOURCE_ID_ICON_THERMOMETER_SM,
                           .text = ops_text_feels_like},
    // no glyph, so a four digit reading and its unit get the glyph's room
    [OPS_PRESSURE]      = {.label = "PRESSURE",  .text = ops_text_pressure},
    [OPS_DEW_POINT]     = {.label = "DEW POINT",  .icon = RESOURCE_ID_ICON_HUMIDITY,
                           .text = ops_text_dew_point},
    [OPS_DAYS_LEFT]     = {.label = "DAYS LEFT", .icon = RESOURCE_ID_ICON_CALENDAR,
                           .text = ops_text_days_left},
    [OPS_WEEKS_LEFT]    = {.label = "WEEKS LEFT", .icon = RESOURCE_ID_ICON_CALENDAR,
                           .text = ops_text_weeks_left},
    // a single slot that widgets/panels.c paints, since the days are drawn rather than written
    [OPS_WEEKDAYS]      = {.label = "WEEKDAY"},

    [OPS_FULL_FORECAST_HOURLY] = {.label = "HOURLY", .shape = OPS_SHAPE_FULL},
    [OPS_FULL_FORECAST_DAILY]  = {.label = "4-DAY",  .shape = OPS_SHAPE_FULL},

    [OPS_WIDE_UV]    = {.label = "UV INDEX", .shape = OPS_SHAPE_WIDE},
    [OPS_WIDE_MOON]  = {.label = "LUNAR",    .shape = OPS_SHAPE_WIDE},
    [OPS_WIDE_ALARM] = {.label = "ALARM",    .shape = OPS_SHAPE_WIDE},
    [OPS_WIDE_BEATS] = {.label = "BEATS",    .shape = OPS_SHAPE_WIDE},
};

const OpsReadout *ops_entry(uint8_t id)
{
    // a blob written by a later build can carry a number this one has no row for, so fall back
    // to the empty row rather than reading off the end of the table
    if (id >= OPS_COUNT)
    {
        return &s_catalog[OPS_EMPTY];
    }

    return &s_catalog[id];
}

const char *ops_label(const OpsReadout *entry)
{
    if (entry->label_for)
    {
        return entry->label_for();
    }

    return entry->label ? entry->label : "";
}

uint32_t ops_icon(const OpsReadout *entry)
{
    if (entry->icon_for)
    {
        return entry->icon_for();
    }

    return entry->icon;
}

void ops_text(const OpsReadout *entry, char *out, size_t n)
{
    if (!entry->text)
    {
        out[0] = '\0';
        return;
    }

    entry->text(out, n);
}

OpsShape ops_shape(uint8_t id)
{
    return ops_entry(id)->shape;
}

/**
 * @brief The cells a pick fills when it starts from a cell.
 *
 * A big pick fills the cells after the one it starts from, so the cells it covers always come later
 * in cell order than the one it is saved in.
 *
 * @param grid The ops grid.
 * @param id The pick.
 * @param slot The cell it starts from.
 * @param cells Receives the cells it fills.
 * @return How many it fills, or 0 when it does not fit there.
 */
static int cells_for(const OpsGrid *grid, uint8_t id, int slot, int cells[OPS_CELLS_MAX])
{
    OpsShape shape = ops_shape(id);
    int cols = (shape == OPS_SHAPE_WIDE || shape == OPS_SHAPE_FULL) ? 2 : 1;
    int rows = (shape == OPS_SHAPE_TALL || shape == OPS_SHAPE_FULL) ? 2 : 1;
    int col0 = slot / grid->rows;
    int row0 = slot % grid->rows;

    // the tall block is drawn in the left column only
    if (shape == OPS_SHAPE_TALL && col0 != 0)
    {
        return 0;
    }

    if (col0 + cols > grid->cols || row0 + rows > grid->rows)
    {
        return 0;
    }

    int count = 0;

    for (int col = col0; col < col0 + cols; col++)
    {
        for (int row = row0; row < row0 + rows; row++)
        {
            cells[count++] = col * grid->rows + row;
        }
    }

    return count;
}

void ops_owners(const OpsGrid *grid, const uint8_t *picks, int8_t *owner)
{
    int cell_count = ops_cell_count(grid);

    for (int slot = 0; slot < cell_count; slot++)
    {
        owner[slot] = -1;
    }

    for (int slot = 0; slot < cell_count; slot++)
    {
        if (picks[slot] == OPS_EMPTY || picks[slot] >= OPS_COUNT || owner[slot] >= 0)
        {
            continue;
        }

        int cells[OPS_CELLS_MAX];
        int count = cells_for(grid, picks[slot], slot, cells);
        bool free = count > 0;

        for (int i = 0; i < count; i++)
        {
            if (owner[cells[i]] >= 0)
            {
                free = false;
            }
        }

        if (!free)
        {
            continue;
        }

        for (int i = 0; i < count; i++)
        {
            owner[cells[i]] = (int8_t)slot;
        }
    }
}
