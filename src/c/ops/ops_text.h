/**
 * @file ops_text.h
 * @brief The value formatters behind the ops catalogue, plus the two hooks whose word or glyph
 * moves with the reading. Each formatter matches the engine's text-slot signature, so the
 * catalogue table binds straight to one.
 *
 * The framework already formats the heart rate and the step count, so those are not repeated
 * here. Everything below is a reading no other face on this frame shows.
 *
 * @ingroup watchface-lcars
 */
#pragma once
#include <pebble.h>

#include "weather/wx_cond.h"

/**
 * @addtogroup watchface-lcars
 * @{
 */

// --- system ---
void ops_text_battery(char *out, size_t n);
void ops_text_next_alarm(char *out, size_t n);

// --- health ---
void ops_text_calories(char *out, size_t n);
void ops_text_sleep(char *out, size_t n);
void ops_text_active(char *out, size_t n);

// --- moon ---
void ops_text_moon_pct(char *out, size_t n);
void ops_text_moon_phase(char *out, size_t n);
void ops_text_moon_next(char *out, size_t n);

/** @brief The phase glyph for right now, one of the eight in the ring. */
uint32_t ops_moon_icon(void);

/** @brief FULL IN or NEW IN, whichever the countdown is running to. */
const char *ops_moon_next_label(void);

// --- sun ---
// these read the sunrise and sunset the phone sent, so they need the five extra weather keys
// declared in the appinfo or they sit on "--" forever
void ops_text_sunrise(char *out, size_t n);
void ops_text_sunset(char *out, size_t n);
void ops_text_daylight(char *out, size_t n);
void ops_text_sun_next(char *out, size_t n);

/** @brief DAWN IN or DUSK IN, whichever event comes next. */
const char *ops_sun_next_label(void);

/** @brief The sunrise or sunset glyph, matching whichever event comes next. */
uint32_t ops_sun_next_icon(void);

// --- weather ---
/** @brief The current condition's glyph at ops size, since the SENSORS one is cut for a 24px box. */
uint32_t ops_wx_icon(void);

/**
 * @brief A condition's glyph at ops size, for a forecast column as well as the sky now.
 *
 * @param cond The condition, or anything past the list for the not available glyph.
 * @param night True for the after dark glyph.
 * @return The resource id.
 */
uint32_t ops_wx_icon_for(WxCond cond, bool night);

void ops_text_humidity(char *out, size_t n);
void ops_text_wind(char *out, size_t n);
void ops_text_uv(char *out, size_t n);
void ops_text_hilo(char *out, size_t n);

/**
 * @brief Today's chance of rain, such as "40%".
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_precip(char *out, size_t n);

/**
 * @brief How warm it feels, with its unit letter.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_feels_like(char *out, size_t n);

/**
 * @brief The sea level pressure in hPa, the unit the phone sends it in.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_pressure(char *out, size_t n);

/**
 * @brief The dew point, with its unit letter.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_dew_point(char *out, size_t n);

// --- calendar ---
void ops_text_julian(char *out, size_t n);
void ops_text_day_of_year(char *out, size_t n);
void ops_text_week(char *out, size_t n);

/**
 * @brief How many days are left in the year, so December 31st reads 0.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_days_left(char *out, size_t n);

/**
 * @brief How many whole weeks are left in the year.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_weeks_left(char *out, size_t n);

// --- progress ---

/** @brief The stretches of time the progress readouts measure. */
typedef enum
{
    OPS_PERIOD_HOUR,
    OPS_PERIOD_DAY,
    OPS_PERIOD_WEEK,
    OPS_PERIOD_YEAR,
} OpsPeriod;

/**
 * @brief How far through a period the clock is, for the readouts and for the bars a row wide.
 *
 * The week starts on Monday, the same week the CYCLE readout numbers.
 *
 * @param period Which period.
 * @return Whole percent from 0 to 99.
 */
int ops_progress_pct(OpsPeriod period);

/**
 * @brief The weekday the week starts on, for the readouts that lay the week out.
 *
 * @return 1 for Monday, counted the way tm_wday counts.
 */
int ops_week_start(void);

/**
 * @brief The hour's progress as a whole percent, such as "87%".
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_hour_progress(char *out, size_t n);

/**
 * @brief The day's progress as a whole percent.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_day_progress(char *out, size_t n);

/**
 * @brief The week's progress as a whole percent, counted from Monday.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_week_progress(char *out, size_t n);

/**
 * @brief The year's progress as a whole percent.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_year_progress(char *out, size_t n);

// --- other clocks ---
void ops_text_epoch(char *out, size_t n);
void ops_text_beats(char *out, size_t n);
void ops_text_zone_1(char *out, size_t n);

/** @brief The configured zone's own name, so the holder box reads LONDON rather than ZONE 1. */
const char *ops_zone_1_label(void);

/** @} */
