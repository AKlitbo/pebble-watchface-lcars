/**
 * @file panels.c
 * @brief The ops panels bigger than one slot, each painted whole under its bar.
 *
 * They are painted here rather than through the engine's text layers. The engine holds a fixed
 * count of those and the face already uses them all, and most of a chart is not text anyway.
 *
 * @ingroup watchface-lcars
 */
#include "widgets/panels.h"

#include "layout.h"
#include "draw/fonts.h"
#include "ops/ops.h"
#include "ops/ops_text.h"
#include "theme/theme.h"
#include "widgets/widgets.h"
#include "health/goals.h"
#include "health/hr_day.h"
#include "clock/progress.h"
#include "clock/solar.h"
#include "clock/weekday.h"
#include "ui/fonts.h"
#include "ui/readouts.h"
#include "io/stores/health_store.h"
#include "io/stores/time_store.h"
#include "io/stores/weather_store.h"
#include "io/stores/calendar_store.h"
#include "io/stores/music_store.h"
#include "io/stores/system_store.h"
#include "math/pct.h"
#include "system/settings/settings.h"
#include "system/settings/setting_values.h"
#include "system/units/units.h"
#include "text/text_case.h"

/**
 * @addtogroup watchface-lcars
 * @{
 */

#define PANEL_W 142      ///< Both columns, from the left column's bar to a pixel inside the bar's far end, the same inset the charts take on the left
#define PANEL_BAR_GAP 12 ///< From the top of a row's bar to the panel under it
#define TEXT_LIFT 1      ///< A value box starts a pixel above the body, the same line the single readouts use

#define SUN_SEGMENTS 12 ///< The daylight bar's segments
#define SUN_SEG_W 5     ///< One segment's width
#define SUN_BAR_H 9     ///< The daylight bar's height
#define SUN_TEXT_W 36   ///< Room for an HH:MM sun time at Antonio 16

#define PROGRESS_SPAN 96  ///< Room the progress bar's segments and the gaps between them share, which 24 and 12 both divide evenly
#define PROGRESS_BAR_H 9  ///< The progress bar's height, the same as the daylight bar's
#define PROGRESS_RADIUS 3 ///< The bar's rounded ends, for an end segment wide enough to take them

#define WEEKDAY_SEG_H 4 ///< A day's segment under its letter on the weekday readout
#define WEEKDAY_SEG_W 9 ///< A day's segment, seven of them with 1px between filling the 69px bar exactly

#define FORECAST_HOURLY_COLS 6 ///< The hours the hourly forecast across all four shows
#define FORECAST_DAILY_COLS 4  ///< The days the four day forecast across all four shows

#define CHART_HOURS 24 ///< One bar an hour from midnight
#define CHART_PITCH 3  ///< A bar and the gap after it
#define CHART_BAR_W 2  ///< One bar's width
#define CHART_LEFT 4   ///< A full panel's chart starts this far in from the panel's left edge
#define WIDE_RIGHT 2   ///< A wide panel's right hand reading or chart stops this far in from the panel's right edge
#define VALUE_RIGHT 3  ///< A full panel's numbers stop this far in from the panel's right edge

#define MUSIC_ART 48     ///< The album art's square, the size the phone is asked to send it at
#define WHEN_GAP 4       ///< Between when an event is and its title
#define AGENDA_ROWS 3    ///< The events the agenda lists
#define AGENDA_ROW_H 16  ///< One line of the agenda

#define STATS_X 78     ///< Where a full panel's numbers start, from the panel's left edge
#define STATS_ROW_H 12 ///< One line of numbers
#define STATS_LABEL_DROP 2 ///< Antonio 10 sits higher in its box than Antonio 12, so its label drops to share the line

// the charts' scales. each top has a floor, so a quiet day keeps a quiet shape rather than being
// stretched to fill the panel the way a busy one does
#define HR_FLOOR 40       ///< Few people's heart rate drops under this, so the bars start here
#define HR_TOP_MIN 100    ///< The heart rate chart's top never sits below this
#define STEPS_TOP_MIN 500 ///< The steps charts' top never sits below this

/**
 * @brief Where a panel's body sits, under its bar.
 *
 * It runs down to the next row's bar under the rows it fills, or to the grid's foot when it fills
 * the last row.
 *
 * @param grid The ops grid.
 * @param slot The slot the panel starts from.
 * @param id The OpsId, for how many rows it fills.
 * @return The body's box.
 */
static GRect body_for(const OpsGrid *grid, int slot, uint8_t id)
{
    int row = slot % grid->rows;
    int last = row + (ops_shape(id) == OPS_SHAPE_FULL ? 2 : 1);
    int top = grid->row_y[row] + PANEL_BAR_GAP;
    int bottom = last < grid->rows ? grid->row_y[last] : grid->bottom;

    return GRect(grid->col_x[0], top, PANEL_W, bottom - top);
}

/** @brief Draw text in a box in one colour. */
static void put_text(GContext *ctx, const char *text, FontId font, GRect box, GTextAlignment align, GColor color)
{
    graphics_context_set_text_color(ctx, color);
    graphics_draw_text(ctx, text, fonts_get(font), box, GTextOverflowModeFill, align, NULL);
}

/**
 * @brief The colour a lit segment takes, the bar's own body colour.
 *
 * Lower Decks Mono and PADD Mono paint the bar's body the same dark grey as an unlit segment, so on
 * those a lit one takes the panel accent rather than vanishing into the unlit ones.
 *
 * @param theme The active theme.
 * @return The lit colour.
 */
static GColor segment_lit(uint8_t theme)
{
    GColor body = bar_body_for_theme(theme);

    return gcolor_equal(body, GColorDarkGray) ? panel_accent_for_theme(theme) : body;
}

/**
 * @brief The biggest of Antonio 16, 14, and 12 a reading fits at.
 *
 * @param text The reading.
 * @param width The room it has.
 * @return The font to draw it in.
 */
static FontId fit_font(const char *text, int width)
{
    static const FontId sizes[] = {FONT_ANTONIO_16, FONT_ANTONIO_14};

    for (unsigned i = 0; i < ARRAY_LENGTH(sizes); i++)
    {
        GSize size = graphics_text_layout_get_content_size(text, fonts_get(sizes[i]), GRect(0, 0, 200, 20),
                                                           GTextOverflowModeFill, GTextAlignmentLeft);

        if (size.w <= width)
        {
            return sizes[i];
        }
    }

    return FONT_ANTONIO_12;
}

/** @brief How wide a line of text draws in a font. */
static int text_width(const char *text, FontId font)
{
    return graphics_text_layout_get_content_size(text, fonts_get(font), GRect(0, 0, 200, 20), GTextOverflowModeFill,
                                                 GTextAlignmentLeft).w;
}

/**
 * @brief Draw a row of hourly bars growing up from a line.
 *
 * Hours from now on get a one pixel stub on the line, so the chart still reads as the whole day. An
 * hour with no reading gets the same stub, and the hour now is drawn in white.
 *
 * @param ctx The graphics context.
 * @param left The first bar's left edge.
 * @param base The line the bars stand on.
 * @param tall The tallest a bar may be.
 * @param heights Each hour's height, or -1 for an hour with no reading.
 * @param now The hour now, 0 to 23.
 * @param bar The past hours' colour.
 */
static void draw_hours(GContext *ctx, int left, int base, int tall, const int heights[CHART_HOURS], int now,
                       GColor bar)
{
    for (int hour = 0; hour < CHART_HOURS; hour++)
    {
        int x = left + hour * CHART_PITCH;
        int h = hour <= now ? heights[hour] : -1;

        if (h < 0)
        {
            graphics_context_set_fill_color(ctx, GColorDarkGray);
            graphics_fill_rect(ctx, GRect(x, base - 1, CHART_BAR_W, 1), 0, GCornerNone);
            continue;
        }

        if (h > tall)
        {
            h = tall;
        }

        // a reading too small to see still gets a pixel, so it is not mistaken for a gap
        if (h < 1)
        {
            h = 1;
        }

        graphics_context_set_fill_color(ctx, hour == now ? GColorWhite : bar);
        graphics_fill_rect(ctx, GRect(x, base - h, CHART_BAR_W, h), 0, GCornerNone);
    }
}

/**
 * @brief One line of a full panel's numbers: the word on the left, the number on the right.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param row Which line, from the top.
 * @param label The word.
 * @param value The number as text.
 * @param label_color The word's colour.
 */
static void stat_row(GContext *ctx, GRect body, int row, const char *label, const char *value, GColor label_color)
{
    int x = body.origin.x + STATS_X;
    int y = body.origin.y - TEXT_LIFT + row * STATS_ROW_H;
    int w = body.size.w - STATS_X;

    put_text(ctx, label, FONT_ANTONIO_10, GRect(x, y + STATS_LABEL_DROP, w, 14), GTextAlignmentLeft, label_color);
    // Antonio 12 rather than 14, since four lines of the bigger size touch at this spacing
    put_text(ctx, value, FONT_ANTONIO_12, GRect(x, y + 1, w - VALUE_RIGHT, 16), GTextAlignmentRight, GColorWhite);
}

/** @brief Write a number, or two dashes for no reading. */
static void write_number(char *out, size_t n, int value)
{
    if (value < 0)
    {
        snprintf(out, n, "--");
        return;
    }

    snprintf(out, n, "%d", value);
}

/** @brief The hour now, 0 to 23. */
static int hour_now(void)
{
    return time_store_tm()->tm_hour;
}

// --- the daylight bar ---

/**
 * @brief Sunrise on the left, sunset on the right, and a segmented bar between them.
 *
 * By day the bar fills with how far through the daylight it is. By night it fills in the bar's end
 * colour with how far through the night it is, so it counts towards the next sunrise. With no sun
 * times yet it stays empty.
 */
static void draw_daylight(GContext *ctx, GRect body, uint8_t theme)
{
    char rise[8];
    char set[8];

    ops_text_sunrise(rise, sizeof(rise));
    ops_text_sunset(set, sizeof(set));

    int text_y = body.origin.y - TEXT_LIFT;
    put_text(ctx, rise, FONT_ANTONIO_16, GRect(body.origin.x + 3, text_y, SUN_TEXT_W, 18), GTextAlignmentLeft,
             GColorWhite);
    put_text(ctx, set, FONT_ANTONIO_16, GRect(body.origin.x + body.size.w - WIDE_RIGHT - SUN_TEXT_W, text_y, SUN_TEXT_W, 18),
             GTextAlignmentRight, GColorWhite);

    int now = hour_now() * 60 + time_store_tm()->tm_min;
    int day = solar_day_progress(weather_store_sunrise(), weather_store_sunset(), now);
    int night = solar_night_progress(weather_store_sunrise(), weather_store_sunset(), now);
    int pct = day >= 0 ? day : night;
    GColor lit_color = day >= 0 ? bar_body_for_theme(theme) : bar_cap_for_theme(theme);
    int lit = pct < 0 ? 0 : (pct * SUN_SEGMENTS + 50) / 100;

    // the bar is centred in the room between the two times as drawn, not in the body, so it keeps an
    // even gap either side whichever of the two is the wider
    int bar_w = SUN_SEGMENTS * (SUN_SEG_W + 1) - 1;
    GRect measure = GRect(0, 0, 200, 20);
    int rise_w = graphics_text_layout_get_content_size(rise, fonts_get(FONT_ANTONIO_16), measure, GTextOverflowModeFill,
                                                       GTextAlignmentLeft).w;
    int set_w = graphics_text_layout_get_content_size(set, fonts_get(FONT_ANTONIO_16), measure, GTextOverflowModeFill,
                                                      GTextAlignmentLeft).w;
    int rise_right = body.origin.x + 3 + rise_w;
    int set_left = body.origin.x + body.size.w - WIDE_RIGHT - set_w;
    // the measured widths take in each glyph's side spacing, which leaves more empty room after the
    // sunrise than before the sunset, so the bar sits a pixel right of the measured middle
    int x0 = rise_right + (set_left - rise_right - bar_w) / 2 + 1;
    int y = body.origin.y + 4;

    for (int i = 0; i < SUN_SEGMENTS; i++)
    {
        GCornerMask corners = i == 0 ? GCornersLeft : (i == SUN_SEGMENTS - 1 ? GCornersRight : GCornerNone);

        graphics_context_set_fill_color(ctx, i < lit ? lit_color : GColorDarkGray);
        graphics_fill_rect(ctx, GRect(x0 + i * (SUN_SEG_W + 1), y, SUN_SEG_W, SUN_BAR_H), 3, corners);
    }
}

// --- the progress bars ---

/**
 * @brief A segmented bar lit as far as it is told.
 *
 * A segment that is only part way lit lights that part of its width, the way the bar would read
 * with no gaps in it.
 *
 * @param ctx The graphics context.
 * @param x0 The bar's left edge.
 * @param y The bar's top.
 * @param span Room the segments and the gaps between them share.
 * @param theme The active theme.
 * @param segments How many segments.
 * @param lit How far the bar is lit, in hundredths of a segment, so 250 lights two and a half.
 */
static void draw_segments(GContext *ctx, int x0, int y, int span, uint8_t theme, int segments, int lit)
{
    GColor lit_color = segment_lit(theme);

    for (int i = 0; i < segments; i++)
    {
        // the edges are spread over the span, so a count that does not divide it evenly, like the
        // week's seven, gets segments a pixel apart in width rather than a bar that falls short
        int left = x0 + i * span / segments;
        int width = x0 + (i + 1) * span / segments - 1 - left;
        int share = lit - i * 100;

        if (share < 0)
        {
            share = 0;
        }

        if (share > 100)
        {
            share = 100;
        }

        int lit_w = width * share / 100;

        // a three pixel wide end segment on the day bar only has room for a one pixel round. the goal
        // bar's five pixel segments take the full round, the way the daylight bar's do
        int radius = width >= SUN_SEG_W ? PROGRESS_RADIUS : 1;
        GCornerMask corners = i == 0 ? GCornersLeft : (i == segments - 1 ? GCornersRight : GCornerNone);

        graphics_context_set_fill_color(ctx, GColorDarkGray);
        graphics_fill_rect(ctx, GRect(left, y, width, PROGRESS_BAR_H), radius, corners);

        if (lit_w <= 0)
        {
            continue;
        }

        // a part lit segment keeps only the round on its left, and only once it is wide enough to
        // hold it. its right edge sits partway along, where a round would read as the bar's end
        GCornerMask lit_corners = corners;
        if (lit_w < width)
        {
            lit_corners = lit_w >= radius * 2 ? (corners & GCornersLeft) : GCornerNone;
        }

        graphics_context_set_fill_color(ctx, lit_color);
        graphics_fill_rect(ctx, GRect(left, y, lit_w, PROGRESS_BAR_H), radius, lit_corners);
    }
}

/**
 * @brief A reading on the left, and a segmented bar to its right lit as far as it is told.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 * @param text The reading on the left.
 * @param segments How many segments.
 * @param lit How far the bar is lit, in hundredths of a segment, so 250 lights two and a half.
 */
static void draw_segment_bar(GContext *ctx, GRect body, uint8_t theme, const char *text, int segments, int lit)
{
    int x0 = body.origin.x + body.size.w - WIDE_RIGHT - (PROGRESS_SPAN - 1);
    int text_w = x0 - body.origin.x - 3 - 4;
    put_text(ctx, text, fit_font(text, text_w), GRect(body.origin.x + 3, body.origin.y - TEXT_LIFT, text_w, 18),
             GTextAlignmentLeft, GColorWhite);

    draw_segments(ctx, x0, body.origin.y + 4, PROGRESS_SPAN, theme, segments, lit);
}

/**
 * @brief The percent on the left, and a segmented bar with a segment for each of the period's own
 * units.
 *
 * The bar lights as far as the percent beside it, so the two always agree. That puts a segment edge
 * a little off the real month boundary on the year bar, since the months are not all one length,
 * but a day there is about a quarter of a pixel.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 * @param period Which period.
 * @param segments How many segments, such as 12 for the five minute steps of an hour.
 */
static void draw_progress(GContext *ctx, GRect body, uint8_t theme, OpsPeriod period, int segments)
{
    int pct = ops_progress_pct(period);

    char text[8];
    snprintf(text, sizeof(text), "%d%%", pct);
    draw_segment_bar(ctx, body, theme, text, segments, pct * segments);
}

// --- the uv bar and the beat bar ---

#define UV_SEGMENTS 11    ///< The UV bar's segments, one for each step of the scale up to 11+
#define BEATS_SEGMENTS 10 ///< The beat bar's segments, a hundred beats each

/**
 * @brief The UV index and its risk on the left, and a bar with a segment for each step up to 11+.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 */
static void draw_uv(GContext *ctx, GRect body, uint8_t theme)
{
    char text[16];
    ops_text_uv(text, sizeof(text));

    // no reading yet lights nothing, and anything past the scale fills it
    int uv = weather_store_uv();
    int steps = uv < 0 ? 0 : (uv > UV_SEGMENTS ? UV_SEGMENTS : uv);
    draw_segment_bar(ctx, body, theme, text, UV_SEGMENTS, steps * 100);
}

/**
 * @brief The Swatch beat on the left, and a bar of the beat day, a segment for each hundred beats.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 */
static void draw_beats(GContext *ctx, GRect body, uint8_t theme)
{
    char text[8];
    ops_text_beats(text, sizeof(text));

    // a thousand beats over ten segments is a hundred a segment, so the beat is the hundredths lit
    draw_segment_bar(ctx, body, theme, text, BEATS_SEGMENTS, units_swatch_beats());
}

// --- the daily goals ---

#define GOAL_SEGMENTS 10 ///< A goal bar's segments, ten percent of the goal each
#define GOAL_SPAN 60     ///< Room the goal bar's segments and gaps share, five pixels a segment like the daylight bar's, which leaves the goal room beside the reading
#define GOAL_GAP 3       ///< Between today's reading and the goal after it
#define GOAL_DROP 3      ///< Antonio 12 sits higher in its box than Antonio 16, so the goal drops to share the reading's line

/**
 * @brief Today's reading, the goal after it, and a bar lit as far towards the goal as it has come.
 *
 * The goal is written smaller and in the label colour, so the reading stays the biggest thing on the
 * row. The reading gets the room the goal leaves, and steps down a size if that runs short.
 *
 * A met goal fills the bar and stops there. No reading yet writes two dashes and lights nothing.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 * @param text The reading, already written the way the panel shows it.
 * @param value The reading in the goal's own unit, below zero for none.
 * @param goal_text The goal, already written, or "" to leave it off.
 * @param goal The goal in the reading's unit.
 */
static void draw_goal(GContext *ctx, GRect body, uint8_t theme, const char *text, int value, const char *goal_text,
                      int goal)
{
    int x0 = body.origin.x + body.size.w - WIDE_RIGHT - (GOAL_SPAN - 1);
    int text_x = body.origin.x + 3;
    int text_y = body.origin.y - TEXT_LIFT;
    int room = x0 - 4 - text_x;

    char of[20] = "";
    int of_w = 0;
    if (goal_text[0] != '\0')
    {
        snprintf(of, sizeof(of), "/%s", goal_text);
        of_w = text_width(of, FONT_ANTONIO_12);
    }

    int text_w = of_w > 0 ? room - of_w - GOAL_GAP : room;
    FontId font = fit_font(text, text_w);
    put_text(ctx, text, font, GRect(text_x, text_y, text_w, 18), GTextAlignmentLeft, GColorWhite);

    if (of_w > 0)
    {
        // the goal follows the reading as drawn, so a short reading keeps the two together
        int drawn_w = text_width(text, font);
        int of_x = text_x + (drawn_w < text_w ? drawn_w : text_w) + GOAL_GAP;
        put_text(ctx, of, FONT_ANTONIO_12, GRect(of_x, text_y + GOAL_DROP, of_w + 2, 16), GTextAlignmentLeft,
                 label_color_for_theme(theme));
    }

    draw_segments(ctx, x0, body.origin.y + 4, GOAL_SPAN, theme, GOAL_SEGMENTS, pct_of(value, goal) * GOAL_SEGMENTS);
}

/**
 * @brief The goal bar a pick stands for, with its reading and the wearer's goal.
 *
 * Each goal is written in its reading's unit, so distance follows the Distance Unit, sleep reads in
 * hours, and active time in minutes.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 * @param id The OpsId, one of the goal bars.
 */
static void draw_goal_for(GContext *ctx, GRect body, uint8_t theme, uint8_t id)
{
    char text[16];
    char goal_text[16];
    int value;
    int goal;

    switch (id)
    {
        case OPS_WIDE_STEP_GOAL:
            value = health_store_steps();
            goal = goals_steps(settings_u8(SETTING_GOAL_STEPS));
            write_number(text, sizeof(text), value);
            snprintf(goal_text, sizeof(goal_text), "%d", goal);
            break;
        case OPS_WIDE_DISTANCE_GOAL:
        {
            bool miles = settings_u8(SETTING_DISTANCE_UNIT) == DISTANCE_UNIT_MILES;

            // the distance reads 0 rather than -1 with no reading, so the step count tells the two
            // apart, the way the steps readout does
            value = health_store_steps() < 0 ? -1 : health_store_distance_m();
            goal = goals_distance_m(settings_u8(SETTING_GOAL_DISTANCE));
            if (value < 0)
            {
                snprintf(text, sizeof(text), "--");
            }
            else
            {
                // the goal after it carries the unit for both, the way the active minutes' does. with
                // the unit on each, a 25.3 KM walk beside a 20.0 KM goal has no room left at any size
                units_format_distance_value(text, sizeof(text), value, miles);
            }

            units_format_distance(goal_text, sizeof(goal_text), goal, miles);
            break;
        }
        case OPS_WIDE_CALORIE_GOAL:
            value = health_store_calories();
            goal = goals_calories(settings_u8(SETTING_GOAL_CALORIES));
            ops_text_calories(text, sizeof(text));
            snprintf(goal_text, sizeof(goal_text), "%d", goal);
            break;
        case OPS_WIDE_ACTIVE_GOAL:
            value = health_store_active_min();
            goal = goals_active_min(settings_u8(SETTING_GOAL_ACTIVE));
            ops_text_active(text, sizeof(text));
            // the reading is a bare count, so the goal carries the unit for both
            snprintf(goal_text, sizeof(goal_text), "%d MIN", goal);
            break;
        case OPS_WIDE_SLEEP_GOAL:
            value = health_store_sleep_min();
            goal = goals_sleep_min(settings_u8(SETTING_GOAL_SLEEP));
            ops_text_sleep(text, sizeof(text));
            // the settings page only offers whole hours
            snprintf(goal_text, sizeof(goal_text), "%dH", goal / 60);
            break;
        default:
            return;
    }

    draw_goal(ctx, body, theme, text, value, goal > 0 ? goal_text : "", goal);
}

// --- the days of the week ---

/**
 * @brief The week's seven days in one slot, each a letter over a segment, with today lit.
 *
 * The days run from the same Monday the week's progress counts from.
 *
 * @param ctx The graphics context.
 * @param grid The ops grid.
 * @param slot The cell it sits in.
 * @param theme The active theme.
 */
static void draw_weekdays(GContext *ctx, const OpsGrid *grid, int slot, uint8_t theme)
{
    int col = slot / grid->rows;
    GRect box = OPS_VALUE_WIDE(grid->col_x[col], grid->row_y[slot % grid->rows], grid->col_right[col], 0);
    int start = ops_week_start();
    int today = progress_week_day(time_store_tm()->tm_wday, start);
    GColor lit = segment_lit(theme);
    GColor label = label_color_for_theme(theme);

    for (int wday = 0; wday < 7; wday++)
    {
        // the framework numbers each weekday by its place in the week, so the letters follow the
        // week start without this working the order out again
        int place = progress_week_day(wday, start);
        // the segments run from the bar's left end to its right end, each letter centred over its own
        int left = grid->col_x[col] + (place - 1) * (WEEKDAY_SEG_W + 1);
        bool is_today = place == today;

        char letter[2] = {weekday_short(wday)[0], '\0'};
        put_text(ctx, letter, FONT_ANTONIO_10, GRect(left, box.origin.y, WEEKDAY_SEG_W, 12), GTextAlignmentCenter,
                 is_today ? GColorWhite : label);

        graphics_context_set_fill_color(ctx, is_today ? lit : GColorDarkGray);
        graphics_fill_rect(ctx, GRect(left, box.origin.y + 13, WEEKDAY_SEG_W, WEEKDAY_SEG_H), 1, GCornersAll);
    }
}

// --- the forecasts ---

/** @brief A forecast temperature as a bare number, or two dashes for a column with none. */
static void write_forecast_temp(char *out, size_t n, int temp)
{
    if (temp == WEATHER_NO_TEMP)
    {
        snprintf(out, n, "--");
        return;
    }

    snprintf(out, n, "%d", temp);
}

/** @brief A forecast column's sky glyph, by day or after dark from the bit the phone set on its code. */
static uint32_t forecast_glyph(uint8_t code)
{
    return ops_wx_icon_for((WxCond)(code & ~WX_FORECAST_NIGHT_BIT), (code & WX_FORECAST_NIGHT_BIT) != 0);
}

/**
 * @brief What one forecast column shows: when it is, its sky, and its temperature.
 *
 * The temperature is a bare number, like the high and low readout, since a unit letter would not
 * fit beside the glyph across a row.
 */
typedef struct
{
    char     when[4];   ///< The hour as two digits, or the weekday as two letters
    uint32_t glyph;     ///< The sky glyph
    char     temp[12];  ///< The temperature, or the high and low as "18/9"
} ForecastCol;

/**
 * @brief The hourly forecast's columns, from the hour now.
 *
 * @param cols Receives up to max of them.
 * @param max How many to take.
 * @return How many it took, 0 with no forecast yet.
 */
static int hourly_cols(ForecastCol *cols, int max)
{
    const WeatherHourly *strip = weather_store_forecast_hourly();
    int count = strip->count < max ? strip->count : max;

    for (int i = 0; i < count; i++)
    {
        int hour = (strip->base_hour + i * strip->step_hours) % 24;

        snprintf(cols[i].when, sizeof(cols[i].when), "%02d", hour);
        cols[i].glyph = forecast_glyph(strip->col[i].code);
        write_forecast_temp(cols[i].temp, sizeof(cols[i].temp), strip->col[i].temp);
    }

    return count;
}

/**
 * @brief The daily forecast's columns, from today.
 *
 * @param cols Receives up to max of them.
 * @param max How many to take.
 * @return How many it took, 0 with no forecast yet.
 */
static int daily_cols(ForecastCol *cols, int max)
{
    const WeatherDaily *strip = weather_store_forecast_daily();
    int count = strip->count < max ? strip->count : max;

    for (int i = 0; i < count; i++)
    {
        const WeatherDayCol *day = &strip->col[i];

        snprintf(cols[i].when, sizeof(cols[i].when), "%s", weekday_short((strip->base_weekday + i) % 7));
        cols[i].glyph = forecast_glyph(day->code);
        write_forecast_temp(cols[i].temp, sizeof(cols[i].temp), day->temp_max);

        // each day shows its high over its low as one reading, such as 18/9
        char min[6];
        write_forecast_temp(min, sizeof(min), day->temp_min);

        size_t used = strlen(cols[i].temp);
        snprintf(cols[i].temp + used, sizeof(cols[i].temp) - used, "/%s", min);
    }

    return count;
}

/** @brief The words a forecast panel shows before the first forecast lands. */
static void no_forecast(GContext *ctx, GRect body, FontId font, GColor label)
{
    put_text(ctx, "NO FORECAST", font, GRect(body.origin.x + 3, body.origin.y - TEXT_LIFT + 1, body.size.w - 3, 18),
             GTextAlignmentLeft, label);
}

/**
 * @brief A forecast across all four: a column each, with when it is over its glyph over its temperature.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 * @param cols The columns.
 * @param count How many there are.
 * @param slots How many columns the panel is split into, so a short forecast keeps its spacing.
 * @param temp_font The temperature's font, smaller for a high and a low.
 */
static void draw_forecast_full(GContext *ctx, GRect body, uint8_t theme, const ForecastCol *cols, int count, int slots,
                               FontId temp_font)
{
    GColor label = label_color_for_theme(theme);

    if (count == 0)
    {
        no_forecast(ctx, body, FONT_ANTONIO_14, label);
        return;
    }

    // the three lines sit 2px down the body, so the column reads centred between its bar and the foot
    int top = body.origin.y + 2;

    for (int i = 0; i < count; i++)
    {
        int x = body.origin.x + i * body.size.w / slots;
        int col_w = body.origin.x + (i + 1) * body.size.w / slots - x;

        put_text(ctx, cols[i].when, FONT_ANTONIO_12, GRect(x, top - TEXT_LIFT, col_w, 14), GTextAlignmentCenter, label);
        widgets_draw_glyph(ctx, cols[i].glyph, GRect(x, top + 15, col_w, 14));
        put_text(ctx, cols[i].temp, temp_font, GRect(x, top + 30, col_w, 18), GTextAlignmentCenter, GColorWhite);
    }
}

// --- steps by the hour ---

/**
 * @brief Each hour's steps as bar heights, scaled so the busiest hour fills the room.
 *
 * @param heights Receives each hour's height.
 * @param tall The tallest a bar may be.
 * @param peak Receives the busiest hour's steps, or -1 with no hours yet.
 * @return How many hours hold a reading.
 */
static int step_heights(int heights[CHART_HOURS], int tall, int *peak)
{
    const uint16_t *hourly = health_store_step_hourly();
    int hours = hourly ? health_store_step_hours() : 0;
    int top = STEPS_TOP_MIN;

    *peak = -1;

    for (int hour = 0; hour < hours && hour < CHART_HOURS; hour++)
    {
        if (hourly[hour] > *peak)
        {
            *peak = hourly[hour];
        }
    }

    if (*peak > top)
    {
        top = *peak;
    }

    for (int hour = 0; hour < CHART_HOURS; hour++)
    {
        heights[hour] = hour < hours ? hourly[hour] * tall / top : -1;
    }

    return hours;
}

/** @brief The steps or distance beside the step glyph, with a small bar a hour to its right. */
static void draw_steps_wide(GContext *ctx, GRect body, uint8_t theme)
{
    widgets_draw_glyph(ctx, RESOURCE_ID_ICON_FEET, GRect(body.origin.x + 3, body.origin.y + 1, 14, 14));

    char steps[16];
    readout_steps(steps, sizeof(steps));

    int text_x = body.origin.x + 20;
    int chart_w = CHART_HOURS * CHART_PITCH;
    int text_w = body.size.w - chart_w - 22;
    put_text(ctx, steps, fit_font(steps, text_w), GRect(text_x, body.origin.y - TEXT_LIFT, text_w, 18),
             GTextAlignmentLeft, GColorWhite);

    int tall = body.size.h - 4;
    int heights[CHART_HOURS];
    int peak;
    int hours = step_heights(heights, tall, &peak);

    draw_hours(ctx, body.origin.x + body.size.w - WIDE_RIGHT - chart_w, body.origin.y + 1 + tall, tall, heights, hours - 1,
               bar_body_for_theme(theme));
}

/** @brief Today's steps by the hour, with the total, the busiest hour, this hour, and the active minutes. */
static void draw_steps_full(GContext *ctx, GRect body, uint8_t theme)
{
    int base = body.origin.y + body.size.h - 3;
    int tall = base - body.origin.y - 2;
    int heights[CHART_HOURS];
    int peak;
    int hours = step_heights(heights, tall, &peak);

    draw_hours(ctx, body.origin.x + CHART_LEFT, base, tall, heights, hours - 1, bar_body_for_theme(theme));

    const uint16_t *hourly = health_store_step_hourly();
    GColor label = label_color_for_theme(theme);
    char text[12];

    write_number(text, sizeof(text), health_store_steps());
    stat_row(ctx, body, 0, "TOTAL", text, label);

    write_number(text, sizeof(text), peak);
    stat_row(ctx, body, 1, "PEAK", text, label);

    write_number(text, sizeof(text), hours > 0 ? hourly[hours - 1] : -1);
    stat_row(ctx, body, 2, "THIS HR", text, label);

    ops_text_active(text, sizeof(text));
    stat_row(ctx, body, 3, "ACTIVE", text, label);
}

// --- heart rate by the hour ---

/**
 * @brief Each hour's average heart rate as bar heights, scaled from HR_FLOOR up to the day's high.
 *
 * @param heights Receives each hour's height, or -1 for an hour with no reading.
 * @param tall The tallest a bar may be.
 * @param low Receives the day's lowest reading, or -1 with none yet.
 * @param avg Receives the day's average, or -1 with none yet.
 * @param high Receives the day's highest reading, or -1 with none yet.
 */
static void heart_heights(int heights[CHART_HOURS], int tall, int *low, int *avg, int *high)
{
    const HrDay *day = health_store_hr_day();

    *low = -1;
    *avg = -1;
    *high = -1;

    if (day)
    {
        hr_day_stats(day, low, avg, high);
    }

    int top = *high > HR_TOP_MIN ? *high : HR_TOP_MIN;

    for (int hour = 0; hour < CHART_HOURS; hour++)
    {
        int bpm = day ? hr_day_hour_avg(day, hour) : -1;

        heights[hour] = bpm < 0 ? -1 : (bpm > HR_FLOOR ? bpm - HR_FLOOR : 0) * tall / (top - HR_FLOOR);
    }
}

/** @brief The heart rate now beside the heart glyph, with a small bar an hour to its right. */
static void draw_heart_wide(GContext *ctx, GRect body, uint8_t theme)
{
    // the heart sits a pixel lower than the step glyph, since its point leaves it looking high
    widgets_draw_glyph(ctx, RESOURCE_ID_ICON_HEART, GRect(body.origin.x + 3, body.origin.y + 2, 14, 14));

    char hr[8];
    readout_hr(hr, sizeof(hr));

    int chart_w = CHART_HOURS * CHART_PITCH;
    int text_w = body.size.w - chart_w - 22;
    put_text(ctx, hr, FONT_ANTONIO_16, GRect(body.origin.x + 20, body.origin.y - TEXT_LIFT + 1, text_w, 18),
             GTextAlignmentLeft, GColorWhite);

    int tall = body.size.h - 4;
    int heights[CHART_HOURS];
    int low;
    int avg;
    int high;
    heart_heights(heights, tall, &low, &avg, &high);

    draw_hours(ctx, body.origin.x + body.size.w - WIDE_RIGHT - chart_w, body.origin.y + 1 + tall, tall, heights, hour_now(),
               bar_body_for_theme(theme));
}

/** @brief Today's heart rate by the hour, with the day's high, average, and low, and the reading now. */
static void draw_heart_full(GContext *ctx, GRect body, uint8_t theme)
{
    int base = body.origin.y + body.size.h - 3;
    int tall = base - body.origin.y - 2;
    int heights[CHART_HOURS];
    int low;
    int avg;
    int high;
    heart_heights(heights, tall, &low, &avg, &high);

    draw_hours(ctx, body.origin.x + CHART_LEFT, base, tall, heights, hour_now(), bar_body_for_theme(theme));

    GColor label = label_color_for_theme(theme);
    char text[8];

    write_number(text, sizeof(text), high);
    stat_row(ctx, body, 0, "HIGH", text, label);

    write_number(text, sizeof(text), avg);
    stat_row(ctx, body, 1, "AVG", text, label);

    write_number(text, sizeof(text), low);
    stat_row(ctx, body, 2, "LOW", text, label);

    readout_hr(text, sizeof(text));
    stat_row(ctx, body, 3, "NOW", text, label);
}

// --- now playing ---

/**
 * @brief Copies a title in capitals, the only letters the LCARS fonts carry.
 *
 * The phone already folds a title to plain ASCII, so capitals are all that is left to do.
 */
static void shout(char *out, size_t n, const char *in)
{
    snprintf(out, n, "%s", in ? in : "");
    text_to_upper(out);
}

/**
 * @brief Draws a play triangle, or two pause bars, in a box h pixels tall.
 *
 * @param ctx The graphics context.
 * @param at The box's top left.
 * @param h Its height.
 * @param color The mark's colour.
 * @param playing True for the triangle.
 */
static void draw_play_mark(GContext *ctx, GPoint at, int h, GColor color, bool playing)
{
    graphics_context_set_stroke_color(ctx, color);
    graphics_context_set_fill_color(ctx, color);

    if (playing)
    {
        // columns that lose a pixel top and bottom toward the tip
        for (int column = 0; column <= h / 2; column++)
        {
            graphics_draw_line(ctx, GPoint(at.x + column, at.y + column), GPoint(at.x + column, at.y + h - 1 - column));
        }
        return;
    }

    graphics_fill_rect(ctx, GRect(at.x, at.y, 2, h), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(at.x + 4, at.y, 2, h), 0, GCornerNone);
}

/**
 * @brief The word a music panel shows in place of a track, or NULL when there is a track.
 *
 * Plugins turned off is said apart from nothing playing, so the wearer knows which one to fix.
 */
static const char *music_gap(void)
{
    if (music_store_no_plugin())
    {
        return "PLUGINS OFF";
    }

    return music_store_has_track() ? NULL : "NO AUDIO";
}

/** @brief A play or pause mark and the song's title, across a row. */
static void draw_music_wide(GContext *ctx, GRect body, uint8_t theme)
{
    GColor label = label_color_for_theme(theme);
    int text_y = body.origin.y - TEXT_LIFT + 3;
    const char *gap = music_gap();

    if (gap)
    {
        put_text(ctx, gap, FONT_ANTONIO_16, GRect(body.origin.x + 3, text_y, body.size.w - 3, 18), GTextAlignmentLeft,
                 label);
        return;
    }

    // the mark is the height of the title's capitals and sits level with them
    draw_play_mark(ctx, GPoint(body.origin.x + 4, body.origin.y + 4), 10, label, music_store_playing());

    char title[48];
    shout(title, sizeof(title), music_store_title());

    int text_x = body.origin.x + 16;
    int text_w = body.size.w - 16 - WIDE_RIGHT;
    put_text(ctx, title, fit_font(title, text_w), GRect(text_x, text_y, text_w, 18), GTextAlignmentLeft,
             GColorWhite);
}

/**
 * @brief The album art in its square, or a plain grey square with none.
 *
 * The phone sends the art at the size this face asked for, and a plugin can send it smaller, so it
 * is centred rather than stretched.
 */
static void draw_art(GContext *ctx, GRect square)
{
    const GBitmap *art = music_store_artwork();

    if (!art)
    {
        graphics_context_set_fill_color(ctx, GColorDarkGray);
        graphics_fill_rect(ctx, square, 3, GCornersAll);
        return;
    }

    GSize size = gbitmap_get_bounds(art).size;
    GRect frame = GRect(square.origin.x + (square.size.w - size.w) / 2, square.origin.y + (square.size.h - size.h) / 2,
                        size.w, size.h);

    // colours the phone left out of the palette are see-through
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, art, frame);
}

/** @brief The album art, the title over two lines, and the artist beside a play or pause mark. */
static void draw_music_full(GContext *ctx, GRect body, uint8_t theme)
{
    GColor label = label_color_for_theme(theme);

    draw_art(ctx, GRect(body.origin.x + CHART_LEFT, body.origin.y + 1, MUSIC_ART, MUSIC_ART));

    int text_x = body.origin.x + CHART_LEFT + MUSIC_ART + 6;
    int text_w = body.origin.x + body.size.w - VALUE_RIGHT - text_x;
    const char *gap = music_gap();

    // with no track the word sits level with the middle of the empty square. Antonio 14's capitals
    // sit about 8px down their box, so the box starts that far above the middle
    if (gap)
    {
        int art_mid = body.origin.y + 1 + MUSIC_ART / 2;
        put_text(ctx, gap, FONT_ANTONIO_14, GRect(text_x, art_mid - 8, text_w, 18), GTextAlignmentLeft, label);
        return;
    }

    char text[48];
    shout(text, sizeof(text), music_store_title());
    put_text(ctx, text, FONT_ANTONIO_14, GRect(text_x, body.origin.y - TEXT_LIFT + 1, text_w, 34), GTextAlignmentLeft,
             GColorWhite);

    draw_play_mark(ctx, GPoint(text_x, body.origin.y + 37), 9, label, music_store_playing());

    shout(text, sizeof(text), music_store_artist());
    put_text(ctx, text, FONT_ANTONIO_12, GRect(text_x + 9, body.origin.y + 34, text_w - 9, 16), GTextAlignmentLeft,
             label);
}

// --- the calendar ---

/** @brief Whether two moments fall on the same local day. */
static bool same_day(time_t a, time_t b)
{
    struct tm first = *localtime(&a);
    struct tm second = *localtime(&b);

    return first.tm_year == second.tm_year && first.tm_yday == second.tm_yday;
}

/**
 * @brief When an event is, the way the agenda writes it.
 *
 * NOW while it is on, the start time for one later today, and TODAY for an all-day one today. One on
 * a later day gets its weekday as two letters, and its start time beside it unless it runs all day,
 * so a meeting tomorrow still says when. Times are 24-hour like the face's other readouts.
 */
static void event_when(const CalendarEvent *event, time_t now, char *out, size_t n)
{
    if (!event->all_day && event->start <= now)
    {
        snprintf(out, n, "NOW");
        return;
    }

    if (same_day(event->start, now))
    {
        if (event->all_day)
        {
            snprintf(out, n, "TODAY");
            return;
        }

        strftime(out, n, "%H:%M", localtime(&event->start));
        return;
    }

    // the day goes in as two letters, MO to SU, which leaves the title more room and still never
    // repeats inside the week the phone sends
    struct tm start = *localtime(&event->start);
    char day[4];
    strftime(day, sizeof(day), "%a", &start);
    day[2] = 0;
    text_to_upper(day);

    if (event->all_day)
    {
        snprintf(out, n, "%s", day);
        return;
    }

    snprintf(out, n, "%s %02d:%02d", day, start.tm_hour, start.tm_min);
}

/**
 * @brief The events still to come or still on, in the order the phone sent them.
 *
 * @param events Receives up to max of them.
 * @param max How many to take.
 * @param now The clock.
 * @return How many it took.
 */
static int upcoming(const CalendarEvent **events, int max, time_t now)
{
    const CalendarStrip *strip = calendar_store_strip();
    int count = 0;

    for (uint8_t index = 0; index < strip->count && count < max; index++)
    {
        const CalendarEvent *event = &strip->event[index];

        if (event->end > now)
        {
            events[count++] = event;
        }
    }

    return count;
}

/** @brief The next event across a row: when it is, then its title. */
static void draw_event_wide(GContext *ctx, GRect body, uint8_t theme)
{
    GColor label = label_color_for_theme(theme);
    int text_y = body.origin.y - TEXT_LIFT + 1;
    time_t now = time(NULL);
    const CalendarEvent *next[1];

    if (upcoming(next, 1, now) == 0)
    {
        put_text(ctx, "NO EVENTS", FONT_ANTONIO_16, GRect(body.origin.x + 3, text_y, body.size.w - 3, 18),
                 GTextAlignmentLeft, label);
        return;
    }

    char when[12];
    event_when(next[0], now, when, sizeof(when));
    int when_w = text_width(when, FONT_ANTONIO_16);
    put_text(ctx, when, FONT_ANTONIO_16, GRect(body.origin.x + 3, text_y, when_w + 2, 18), GTextAlignmentLeft, label);

    char title[48];
    shout(title, sizeof(title), next[0]->title);

    int text_x = body.origin.x + 3 + when_w + WHEN_GAP;
    int text_w = body.size.w - (text_x - body.origin.x) - WIDE_RIGHT;
    put_text(ctx, title, fit_font(title, text_w), GRect(text_x, text_y, text_w, 18), GTextAlignmentLeft, GColorWhite);
}

/** @brief The next few events, a line each: when it is, then its title. */
static void draw_agenda_full(GContext *ctx, GRect body, uint8_t theme)
{
    GColor label = label_color_for_theme(theme);
    time_t now = time(NULL);
    const CalendarEvent *events[AGENDA_ROWS];
    int count = upcoming(events, AGENDA_ROWS, now);

    if (count == 0)
    {
        put_text(ctx, "NO EVENTS", FONT_ANTONIO_14, GRect(body.origin.x + CHART_LEFT, body.origin.y - TEXT_LIFT + 1,
                                                          body.size.w - CHART_LEFT, 18), GTextAlignmentLeft, label);
        return;
    }

    // the titles line up after the widest when, so a row with a weekday and a time pushes them all
    char when[AGENDA_ROWS][12];
    int when_w = 0;

    for (int row = 0; row < count; row++)
    {
        event_when(events[row], now, when[row], sizeof(when[row]));

        int width = text_width(when[row], FONT_ANTONIO_14);
        if (width > when_w)
        {
            when_w = width;
        }
    }

    int when_x = body.origin.x + CHART_LEFT;
    int title_x = when_x + when_w + WHEN_GAP;
    int title_w = body.origin.x + body.size.w - VALUE_RIGHT - title_x;

    for (int row = 0; row < count; row++)
    {
        int y = body.origin.y - TEXT_LIFT + 1 + row * AGENDA_ROW_H;
        char text[48];

        put_text(ctx, when[row], FONT_ANTONIO_14, GRect(when_x, y, when_w + 2, 18), GTextAlignmentLeft, label);

        shout(text, sizeof(text), events[row]->title);
        put_text(ctx, text, FONT_ANTONIO_14, GRect(title_x, y, title_w, 18), GTextAlignmentLeft, GColorWhite);
    }
}

// --- the moon, the next alarm, and the next sunrise or sunset ---

/**
 * @brief A glyph and a reading on the left, and a second reading set against the right edge.
 *
 * The right reading keeps Antonio 16, and the left one steps down a size when it would run into it,
 * as a long phase name like WAXING GIBBOUS does.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 * @param glyph The glyph's resource.
 * @param left The reading beside the glyph.
 * @param right The reading against the right edge, in the theme's label colour.
 */
static void draw_pair(GContext *ctx, GRect body, uint8_t theme, uint32_t glyph, const char *left, const char *right)
{
    int right_w = text_width(right, FONT_ANTONIO_16);
    int right_x = body.origin.x + body.size.w - WIDE_RIGHT - right_w;
    int text_x = OPS_TEXT_X(body.origin.x);
    int left_w = right_x - 4 - text_x;

    widgets_draw_glyph(ctx, glyph, GRect(body.origin.x + OPS_GLYPH_W, body.origin.y + 2, 14, 14));
    put_text(ctx, left, fit_font(left, left_w), GRect(text_x, body.origin.y - TEXT_LIFT, left_w, 18),
             GTextAlignmentLeft, GColorWhite);
    put_text(ctx, right, FONT_ANTONIO_16, GRect(right_x, body.origin.y - TEXT_LIFT, right_w + 2, 18),
             GTextAlignmentLeft, label_color_for_theme(theme));
}

/**
 * @brief The moon's phase beside its glyph, and how long until the next full or new moon.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 */
static void draw_moon(GContext *ctx, GRect body, uint8_t theme)
{
    char phase[24];
    char days[12];
    char next[24];

    ops_text_moon_phase(phase, sizeof(phase));
    ops_text_moon_next(days, sizeof(days));

    // the countdown reads FULL IN 6 DAYS, or FULL NOW on the day itself
    const char *label = ops_moon_next_label();
    if (strcmp(days, "NOW") == 0)
    {
        snprintf(next, sizeof(next), "%.4s NOW", label);
    }
    else
    {
        snprintf(next, sizeof(next), "%s %s", label, days);
    }

    draw_pair(ctx, body, theme, ops_moon_icon(), phase, next);
}

/**
 * @brief The next alarm's time beside the alarm glyph, and how long until it rings.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 */
static void draw_alarm(GContext *ctx, GRect body, uint8_t theme)
{
    char when[12];
    char until[16] = "";

    ops_text_next_alarm(when, sizeof(when));

    time_t alarm = 0;
    if (system_store_next_alarm(&alarm))
    {
        long minutes = (long)(alarm - time(NULL)) / 60;
        if (minutes < 0)
        {
            minutes = 0;
        }

        if (minutes >= 60)
        {
            snprintf(until, sizeof(until), "IN %ldH %02ldM", minutes / 60, minutes % 60);
        }
        else
        {
            snprintf(until, sizeof(until), "IN %ldM", minutes);
        }
    }

    draw_pair(ctx, body, theme, RESOURCE_ID_ICON_ALARM, when, until);
}

/**
 * @brief The countdown to the next sunrise or sunset beside its glyph, and the time it comes at.
 *
 * The bar's word says which of the two it is, so the row only needs how long and when.
 *
 * @param ctx The graphics context.
 * @param body The panel's body.
 * @param theme The active theme.
 */
static void draw_sun_next(GContext *ctx, GRect body, uint8_t theme)
{
    char until[16];
    char at[8];

    ops_text_sun_next(until, sizeof(until));
    ops_text_sun_next_at(at, sizeof(at));
    draw_pair(ctx, body, theme, ops_sun_next_icon(), until, at);
}

bool panels_paints(uint8_t id)
{
    OpsShape shape = ops_shape(id);

    return shape == OPS_SHAPE_WIDE || shape == OPS_SHAPE_FULL || id == OPS_WEEKDAYS;
}

void panels_draw(GContext *ctx, const OpsGrid *grid, uint8_t id, int slot)
{
    uint8_t theme = stardate_active_theme();

    // the weekday readout fills one slot rather than a body under a bar that spans the row
    if (id == OPS_WEEKDAYS)
    {
        draw_weekdays(ctx, grid, slot, theme);
        return;
    }

    GRect body = body_for(grid, slot, id);
    ForecastCol cols[FORECAST_HOURLY_COLS];

    switch (id)
    {
        case OPS_WIDE_DAYLIGHT:
            draw_daylight(ctx, body, theme);
            break;
        case OPS_WIDE_STEPS:
            draw_steps_wide(ctx, body, theme);
            break;
        case OPS_WIDE_HEART:
            draw_heart_wide(ctx, body, theme);
            break;
        case OPS_FULL_HEART:
            draw_heart_full(ctx, body, theme);
            break;
        case OPS_FULL_STEPS:
            draw_steps_full(ctx, body, theme);
            break;
        case OPS_WIDE_MUSIC:
            draw_music_wide(ctx, body, theme);
            break;
        case OPS_FULL_MUSIC:
            draw_music_full(ctx, body, theme);
            break;
        case OPS_WIDE_EVENT:
            draw_event_wide(ctx, body, theme);
            break;
        case OPS_FULL_AGENDA:
            draw_agenda_full(ctx, body, theme);
            break;
        // each bar takes its period's own units: five minutes, an hour, a day, and a month
        case OPS_WIDE_HOUR_PROGRESS:
            draw_progress(ctx, body, theme, OPS_PERIOD_HOUR, 12);
            break;
        case OPS_WIDE_DAY_PROGRESS:
            draw_progress(ctx, body, theme, OPS_PERIOD_DAY, 24);
            break;
        case OPS_WIDE_WEEK_PROGRESS:
            draw_progress(ctx, body, theme, OPS_PERIOD_WEEK, 7);
            break;
        case OPS_WIDE_YEAR_PROGRESS:
            draw_progress(ctx, body, theme, OPS_PERIOD_YEAR, 12);
            break;
        case OPS_FULL_FORECAST_HOURLY:
            draw_forecast_full(ctx, body, theme, cols, hourly_cols(cols, FORECAST_HOURLY_COLS), FORECAST_HOURLY_COLS,
                               FONT_ANTONIO_14);
            break;
        case OPS_FULL_FORECAST_DAILY:
            draw_forecast_full(ctx, body, theme, cols, daily_cols(cols, FORECAST_DAILY_COLS), FORECAST_DAILY_COLS,
                               FONT_ANTONIO_12);
            break;
        case OPS_WIDE_UV:
            draw_uv(ctx, body, theme);
            break;
        case OPS_WIDE_MOON:
            draw_moon(ctx, body, theme);
            break;
        case OPS_WIDE_ALARM:
            draw_alarm(ctx, body, theme);
            break;
        case OPS_WIDE_BEATS:
            draw_beats(ctx, body, theme);
            break;
        case OPS_WIDE_STEP_GOAL:
        case OPS_WIDE_DISTANCE_GOAL:
        case OPS_WIDE_CALORIE_GOAL:
        case OPS_WIDE_ACTIVE_GOAL:
        case OPS_WIDE_SLEEP_GOAL:
            draw_goal_for(ctx, body, theme, id);
            break;
        case OPS_WIDE_SUN_NEXT:
            draw_sun_next(ctx, body, theme);
            break;
        default:
            break;
    }
}

/** @} */
