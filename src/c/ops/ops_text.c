/**
 * @file ops_text.c
 * @brief The value formatters behind the ops catalogue.
 *
 * Every reading here has a no-data state, and all of them answer "--" for it rather than
 * printing a bogus zero, matching the shared readouts.
 */
#include "ops/ops_text.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "clock/astro.h"
#include "clock/date.h"
#include "clock/duration.h"
#include "clock/moon.h"
#include "clock/progress.h"
#include "clock/solar.h"
#include "io/stores/health_store.h"
#include "io/stores/system_store.h"
#include "io/stores/time_store.h"
#include "io/stores/weather_store.h"
#include "settings_schema.h"
#include "system/settings/settings.h"
#include "system/units/units.h"
#include "text/text_case.h"
#include "weather/wx_cond.h"

// the eight phase glyphs in cycle order (0 new, 4 full, wrapping back round). the index comes
// straight from moon_glyph_index so this table only turns it into a resource id
static const uint32_t s_moon_res[8] = {
    RESOURCE_ID_ICON_MOON_0, RESOURCE_ID_ICON_MOON_1, RESOURCE_ID_ICON_MOON_2, RESOURCE_ID_ICON_MOON_3,
    RESOURCE_ID_ICON_MOON_4, RESOURCE_ID_ICON_MOON_5, RESOURCE_ID_ICON_MOON_6, RESOURCE_ID_ICON_MOON_7,
};

/**
 * @brief Write the no-reading marker.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
static void no_data(char *out, size_t n)
{
    snprintf(out, n, "--");
}

/**
 * @brief Write a length of time as "13H 43M".
 *
 * Every duration in the catalogue goes through here, so only a real clock reading ever comes out
 * as "HH:MM". A day length written 13:43 sitting under a sunset written 20:14 reads as a third
 * time of day rather than as a span.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 * @param minutes The span in minutes.
 */
static void write_duration(char *out, size_t n, int minutes)
{
    // the compact form writes its units in lower case, and this font is cut to capitals only
    duration_hm_compact(out, n, minutes);
    text_to_upper(out);
}

/**
 * @brief Today's sunrise and sunset as minutes past midnight.
 *
 * The phone sends them as minutes past the phone's midnight, which is the clock the watch keeps.
 *
 * @param rise Receives sunrise, or -1 when there is no reading.
 * @param set Receives sunset, or -1 when there is no reading.
 * @return True when both landed.
 */
static bool sun_minutes(int *rise, int *set)
{
    *rise = weather_store_sunrise();
    *set = weather_store_sunset();
    return *rise >= 0 && *set >= 0;
}

/** @brief The clock as minutes past midnight. */
static int now_minutes(void)
{
    const struct tm *tm = time_store_tm();
    return tm->tm_hour * 60 + tm->tm_min;
}

/**
 * @brief Whether the moon countdown is running to the full moon rather than the new one.
 *
 * Both the word and the number need the answer, so they ask the same question rather than each
 * deciding for itself and risking a "FULL IN" over a count to the new moon.
 */
static bool moon_next_is_full(void)
{
    time_t now = time(NULL);
    return moon_days_to_phase(now, true) <= moon_days_to_phase(now, false);
}

// --- system ---

void ops_text_battery(char *out, size_t n)
{
    snprintf(out, n, "%d%%", system_store_battery());
}

/**
 * @brief When the wearer's next alarm goes off.
 *
 * A smart alarm reports the end of its wake window, so what shows is the latest it can go off
 * rather than the start of the window.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 */
void ops_text_next_alarm(char *out, size_t n)
{
    time_t alarm = 0;
    if (!system_store_next_alarm(&alarm))
    {
        no_data(out, n);
        return;
    }

    // 24 hour, matching DAWN and DUSK. an AM or PM would not fit beside the glyph anyway
    struct tm lt = *localtime(&alarm);
    snprintf(out, n, "%02d:%02d", lt.tm_hour, lt.tm_min);
}

int ops_alarm_minutes(void)
{
    time_t alarm = 0;
    if (!system_store_next_alarm(&alarm))
    {
        return -1;
    }

    long minutes = (long)(alarm - time(NULL)) / 60;
    return minutes < 0 ? 0 : (int)minutes;
}

void ops_text_alarm_left(char *out, size_t n)
{
    int minutes = ops_alarm_minutes();
    if (minutes < 0)
    {
        out[0] = '\0';
        return;
    }

    if (minutes >= 60)
    {
        snprintf(out, n, "%dH %02dM", minutes / 60, minutes % 60);
        return;
    }

    snprintf(out, n, "%dM", minutes);
}

void ops_text_alarm_until(char *out, size_t n)
{
    char left[16];
    ops_text_alarm_left(left, sizeof(left));

    if (left[0] == '\0')
    {
        out[0] = '\0';
        return;
    }

    snprintf(out, n, "IN %s", left);
}

// --- health ---

void ops_text_calories(char *out, size_t n)
{
    int kcal = health_store_calories();
    if (kcal < 0)
    {
        no_data(out, n);
        return;
    }

    snprintf(out, n, "%d", kcal);
}

void ops_text_sleep(char *out, size_t n)
{
    int minutes = health_store_sleep_min();
    if (minutes < 0)
    {
        no_data(out, n);
        return;
    }

    write_duration(out, n, minutes);
}

void ops_text_active(char *out, size_t n)
{
    int minutes = health_store_active_min();
    if (minutes < 0)
    {
        no_data(out, n);
        return;
    }

    snprintf(out, n, "%d", minutes);
}

// --- moon ---

void ops_text_moon_pct(char *out, size_t n)
{
    snprintf(out, n, "%d%%", moon_illumination_pct(time(NULL)));
}

void ops_text_moon_phase(char *out, size_t n)
{
    snprintf(out, n, "%s", moon_phase_name(time(NULL)));
}

void ops_text_moon_next(char *out, size_t n)
{
    int days = moon_days_to_phase(time(NULL), moon_next_is_full());
    if (days == 0)
    {
        snprintf(out, n, "NOW");
        return;
    }

    snprintf(out, n, "%d %s", days, days == 1 ? "DAY" : "DAYS");
}

uint32_t ops_moon_icon(void)
{
    return s_moon_res[moon_glyph_index(time(NULL), 8)];
}

const char *ops_moon_next_label(void)
{
    return moon_next_is_full() ? "FULL IN" : "NEW IN";
}

/** @brief FULL or NEW, whichever the countdown is running to. */
static const char *moon_next_word(void)
{
    return moon_next_is_full() ? "FULL" : "NEW";
}

void ops_text_moon_countdown(char *out, size_t n)
{
    char days[12];
    ops_text_moon_next(days, sizeof(days));

    // on the day itself the word loses its IN, so it reads FULL NOW or NEW NOW
    if (strcmp(days, "NOW") == 0)
    {
        snprintf(out, n, "%s NOW", moon_next_word());
        return;
    }

    snprintf(out, n, "%s %s", ops_moon_next_label(), days);
}

void ops_text_moon_countdown_short(char *out, size_t n)
{
    int days = moon_days_to_phase(time(NULL), moon_next_is_full());
    if (days == 0)
    {
        snprintf(out, n, "%s NOW", moon_next_word());
        return;
    }

    snprintf(out, n, "%s %dD", moon_next_word(), days);
}

// --- sun ---

/**
 * @brief A time of day as 24 hour HH:MM, matching the other sun readouts.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 * @param minutes Minutes past midnight, or -1 for none.
 */
static void sun_time(char *out, size_t n, int minutes)
{
    if (minutes < 0)
    {
        no_data(out, n);
        return;
    }

    snprintf(out, n, "%02d:%02d", minutes / 60, minutes % 60);
}

void ops_text_sunrise(char *out, size_t n)
{
    sun_time(out, n, weather_store_sunrise());
}

void ops_text_sunset(char *out, size_t n)
{
    sun_time(out, n, weather_store_sunset());
}

void ops_text_daylight(char *out, size_t n)
{
    int rise, set;
    if (!sun_minutes(&rise, &set))
    {
        no_data(out, n);
        return;
    }

    // a sunset before the sunrise means the span crosses midnight, which happens near the poles
    int span = set - rise;
    if (span < 0)
    {
        span += 1440;
    }

    write_duration(out, n, span);
}

/**
 * @brief How long until the next sunrise or sunset, which one it is, and when.
 *
 * The countdown, its word, its glyph, and the time on the wide panel all ask here, so they can never
 * disagree about which event comes next.
 *
 * @param is_sunrise Receives whether the next event is a sunrise. Untouched when there is no reading.
 * @param at Receives the event's time, minutes past midnight. Untouched when there is no reading.
 * @return Minutes until it, or -1 when there are no sun times.
 */
static int sun_next(bool *is_sunrise, int *at)
{
    int rise, set;
    if (!sun_minutes(&rise, &set))
    {
        return -1;
    }

    bool sunrise = false;
    int minutes = solar_next_event(rise, set, now_minutes(), &sunrise);
    if (minutes < 0)
    {
        return -1;
    }

    *is_sunrise = sunrise;
    *at = sunrise ? rise : set;
    return minutes;
}

void ops_text_sun_next(char *out, size_t n)
{
    bool is_sunrise = false;
    int at = -1;
    int minutes = sun_next(&is_sunrise, &at);
    if (minutes < 0)
    {
        no_data(out, n);
        return;
    }

    write_duration(out, n, minutes);
}

void ops_text_sun_next_at(char *out, size_t n)
{
    bool is_sunrise = false;
    int at = -1;
    sun_next(&is_sunrise, &at);
    sun_time(out, n, at);
}

const char *ops_sun_next_label(void)
{
    bool is_sunrise = false;
    int at = -1;
    if (sun_next(&is_sunrise, &at) < 0)
    {
        return "SOLAR";
    }

    return is_sunrise ? "DAWN IN" : "DUSK IN";
}

uint32_t ops_sun_next_icon(void)
{
    bool is_sunrise = false;
    int at = -1;
    if (sun_next(&is_sunrise, &at) < 0)
    {
        return RESOURCE_ID_ICON_SUN;
    }

    return is_sunrise ? RESOURCE_ID_ICON_SUNRISE : RESOURCE_ID_ICON_SUNSET;
}

// --- weather ---

// the SENSORS glyphs are cut for a 24px box and an ops slot only has 14. so the
// small set is a second cut of the same art, by day and after dark
//
// the rows are by the framework's numbering of the conditions, which is also the code a forecast
// column carries. freezing drizzle and freezing rain share the sleet glyph just like the 24px set
static const uint32_t s_wx_day[WX_COND_COUNT] = {
    [WX_COND_CLEAR] = RESOURCE_ID_ICON_WX_SM_CLEAR,
    [WX_COND_PCLDY] = RESOURCE_ID_ICON_WX_SM_PARTLY_CLOUDY,
    [WX_COND_CLDY]  = RESOURCE_ID_ICON_WX_SM_CLOUDY,
    [WX_COND_FOGGY] = RESOURCE_ID_ICON_WX_SM_FOG,
    [WX_COND_DRZL]  = RESOURCE_ID_ICON_WX_SM_DRIZZLE,
    [WX_COND_FZDZ]  = RESOURCE_ID_ICON_WX_SM_SLEET,
    [WX_COND_RAIN]  = RESOURCE_ID_ICON_WX_SM_RAIN,
    [WX_COND_FZRN]  = RESOURCE_ID_ICON_WX_SM_SLEET,
    [WX_COND_SNOW]  = RESOURCE_ID_ICON_WX_SM_SNOW,
    [WX_COND_SHWR]  = RESOURCE_ID_ICON_WX_SM_SHOWERS,
    [WX_COND_SNSH]  = RESOURCE_ID_ICON_WX_SM_SNOW_WIND,
    [WX_COND_STRM]  = RESOURCE_ID_ICON_WX_SM_THUNDERSTORM,
};

static const uint32_t s_wx_night[WX_COND_COUNT] = {
    [WX_COND_CLEAR] = RESOURCE_ID_ICON_WX_SM_NIGHT_CLEAR,
    [WX_COND_PCLDY] = RESOURCE_ID_ICON_WX_SM_NIGHT_PARTLY_CLOUDY,
    [WX_COND_CLDY]  = RESOURCE_ID_ICON_WX_SM_NIGHT_CLOUDY,
    [WX_COND_FOGGY] = RESOURCE_ID_ICON_WX_SM_NIGHT_FOG,
    [WX_COND_DRZL]  = RESOURCE_ID_ICON_WX_SM_NIGHT_DRIZZLE,
    [WX_COND_FZDZ]  = RESOURCE_ID_ICON_WX_SM_NIGHT_SLEET,
    [WX_COND_RAIN]  = RESOURCE_ID_ICON_WX_SM_NIGHT_RAIN,
    [WX_COND_FZRN]  = RESOURCE_ID_ICON_WX_SM_NIGHT_SLEET,
    [WX_COND_SNOW]  = RESOURCE_ID_ICON_WX_SM_NIGHT_SNOW,
    [WX_COND_SHWR]  = RESOURCE_ID_ICON_WX_SM_NIGHT_SHOWERS,
    [WX_COND_SNSH]  = RESOURCE_ID_ICON_WX_SM_NIGHT_SNOW_WIND,
    [WX_COND_STRM]  = RESOURCE_ID_ICON_WX_SM_NIGHT_THUNDERSTORM,
};

// a condition added to the framework's list needs a row in both tables above
// this stops the build until it has one, rather than letting it draw no glyph at all
_Static_assert(WX_COND_COUNT == 12, "a new weather condition needs a small glyph in s_wx_day and s_wx_night");

uint32_t ops_wx_icon_for(WxCond cond, bool night)
{
    if (cond >= WX_COND_COUNT)
    {
        return RESOURCE_ID_ICON_WX_SM_NA;
    }

    return night ? s_wx_night[cond] : s_wx_day[cond];
}

uint32_t ops_wx_icon(void)
{
    bool night = false;
    WxCond cond = wx_cond_parse(weather_store_cond(), &night);

    return ops_wx_icon_for(cond, night);
}

/**
 * @brief Write a temperature with its unit letter, or "--" for no reading.
 *
 * The face declares the temperature unit key, so the setting always says which unit the phone
 * sent it in, plugin weather included.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 * @param temp The temperature, or WEATHER_NO_TEMP for none.
 */
static void write_temp(char *out, size_t n, int temp)
{
    if (temp == WEATHER_NO_TEMP)
    {
        no_data(out, n);
        return;
    }

    snprintf(out, n, "%d%c", temp, settings_u8(SETTING_TEMPERATURE_UNIT) ? 'F' : 'C');
}

void ops_text_humidity(char *out, size_t n)
{
    int pct = weather_store_humidity();
    if (pct < 0)
    {
        no_data(out, n);
        return;
    }

    snprintf(out, n, "%d%%", pct);
}

void ops_text_wind(char *out, size_t n)
{
    int kmh = weather_store_wind_kmh();
    if (kmh < 0)
    {
        no_data(out, n);
        return;
    }

    // the compass point rides along when the phone sent one, so this reads "12 NW"
    const char *dir = weather_store_wind_dir();
    if (dir[0])
    {
        snprintf(out, n, "%d %s", kmh, dir);
        return;
    }

    snprintf(out, n, "%d", kmh);
}

/**
 * @brief EPA-style risk band for a uv index. A bare number means little without it.
 *
 * The same wording the mosaic faces use, so a 7 reads HIGH wherever you see it.
 *
 * @param uv The index.
 * @return The band name.
 */
static const char *uv_risk(int uv)
{
    if (uv <= 2)  { return "LOW"; }
    if (uv <= 5)  { return "MOD"; }
    if (uv <= 7)  { return "HIGH"; }
    if (uv <= 10) { return "V.HIGH"; }
    // gridlock spells this one out, but it has a panel to do it in. here the number rides in
    // front of it in a 55px slot
    return "EXTRM";
}

void ops_text_uv(char *out, size_t n)
{
    int uv = weather_store_uv();
    if (uv < 0)
    {
        no_data(out, n);
        return;
    }

    // the top two bands run long enough to drop the readout onto its smaller font, which is
    // what that fallback is for. they are also the two worth reading properly
    snprintf(out, n, "%d %s", uv, uv_risk(uv));
}

void ops_text_hilo(char *out, size_t n)
{
    int hi = weather_store_temp_max();
    int lo = weather_store_temp_min();
    if (hi == WEATHER_NO_TEMP || lo == WEATHER_NO_TEMP)
    {
        no_data(out, n);
        return;
    }

    snprintf(out, n, "%d/%d", hi, lo);
}

void ops_text_precip(char *out, size_t n)
{
    int pct = weather_store_precip_chance();
    if (pct < 0)
    {
        no_data(out, n);
        return;
    }

    snprintf(out, n, "%d%%", pct);
}

void ops_text_feels_like(char *out, size_t n)
{
    write_temp(out, n, weather_store_feels_like());
}

void ops_text_pressure(char *out, size_t n)
{
    int hpa = weather_store_pressure();
    if (hpa < 0)
    {
        no_data(out, n);
        return;
    }

    snprintf(out, n, "%d HPA", hpa);
}

void ops_text_dew_point(char *out, size_t n)
{
    write_temp(out, n, weather_store_dew_point());
}

// --- calendar ---

void ops_text_julian(char *out, size_t n)
{
    // the shared helper keeps the hundredths so it can stay in 32 bits, and the whole day is
    // what a stardate-style readout wants
    snprintf(out, n, "%d", (int)(astro_jd_centi(time(NULL)) / 100));
}

void ops_text_day_of_year(char *out, size_t n)
{
    snprintf(out, n, "%d", date_day_of_year(time_store_tm()->tm_yday));
}

void ops_text_week(char *out, size_t n)
{
    const struct tm *tm = time_store_tm();
    snprintf(out, n, "%d", date_iso_week(tm->tm_year + 1900, tm->tm_yday, tm->tm_wday));
}

void ops_text_days_left(char *out, size_t n)
{
    const struct tm *tm = time_store_tm();
    snprintf(out, n, "%d", date_days_left_in_year(tm->tm_year + 1900, tm->tm_yday));
}

void ops_text_weeks_left(char *out, size_t n)
{
    const struct tm *tm = time_store_tm();
    snprintf(out, n, "%d", date_weeks_left_in_year(tm->tm_year + 1900, tm->tm_yday));
}

// --- progress ---

// the face has no week start setting, so the week runs from Monday like the ISO week CYCLE counts
#define WEEK_START_MONDAY 1

int ops_week_start(void)
{
    return WEEK_START_MONDAY;
}

int ops_progress_pct(OpsPeriod period)
{
    const struct tm *tm = time_store_tm();

    switch (period)
    {
        case OPS_PERIOD_HOUR:
            return progress_hour_pct(tm->tm_min, tm->tm_sec);
        case OPS_PERIOD_DAY:
            return progress_day_pct(tm->tm_hour, tm->tm_min, tm->tm_sec);
        case OPS_PERIOD_WEEK:
            return progress_week_pct(tm->tm_wday, WEEK_START_MONDAY, tm->tm_hour, tm->tm_min, tm->tm_sec);
        case OPS_PERIOD_YEAR:
        default:
            return progress_year_pct(tm->tm_year + 1900, tm->tm_yday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    }
}

void ops_text_period_count(OpsPeriod period, char *out, size_t n)
{
    const struct tm *tm = time_store_tm();
    int year = tm->tm_year + 1900;

    switch (period)
    {
        case OPS_PERIOD_HOUR:
            snprintf(out, n, "MIN %d/60", tm->tm_min + 1);
            break;
        case OPS_PERIOD_DAY:
            snprintf(out, n, "HOUR %d/24", tm->tm_hour + 1);
            break;
        case OPS_PERIOD_WEEK:
            snprintf(out, n, "DAY %d/7", progress_week_day(tm->tm_wday, WEEK_START_MONDAY));
            break;
        case OPS_PERIOD_YEAR:
        default:
        {
            // the day under way plus the days still to come is the year's length, leap years included
            int day = date_day_of_year(tm->tm_yday);
            snprintf(out, n, "DAY %d/%d", day, day + date_days_left_in_year(year, tm->tm_yday));
            break;
        }
    }
}

/**
 * @brief Write a period's progress as a whole percent.
 *
 * @param out Output buffer.
 * @param n Buffer size.
 * @param period Which period.
 */
static void write_progress(char *out, size_t n, OpsPeriod period)
{
    snprintf(out, n, "%d%%", ops_progress_pct(period));
}

void ops_text_hour_progress(char *out, size_t n)
{
    write_progress(out, n, OPS_PERIOD_HOUR);
}

void ops_text_day_progress(char *out, size_t n)
{
    write_progress(out, n, OPS_PERIOD_DAY);
}

void ops_text_week_progress(char *out, size_t n)
{
    write_progress(out, n, OPS_PERIOD_WEEK);
}

void ops_text_year_progress(char *out, size_t n)
{
    write_progress(out, n, OPS_PERIOD_YEAR);
}

// --- other clocks ---

void ops_text_epoch(char *out, size_t n)
{
    // off the time store rather than time(NULL) so it tracks the same tick as everything else and
    // freezes with the dev clock. mktime wants a writable tm so it gets a copy
    struct tm now = *time_store_tm();
    snprintf(out, n, "%ld", (long)mktime(&now));
}

void ops_text_beats(char *out, size_t n)
{
    // three digits always, so the row does not jump width as the count rolls over
    snprintf(out, n, "@%03d", units_swatch_beats());
}

void ops_text_zone_1(char *out, size_t n)
{
    // no zone picked yet, or the picker was cleared, so there is no clock to show
    if (!lcars_zone_1_is_set())
    {
        no_data(out, n);
        return;
    }

    // off the time store like the rest of the face, so it tracks the same tick and freezes with
    // the dev clock. mktime turns the local reading back into UTC, then the offset and gmtime
    // give the other zone's wall clock rather than this one's
    struct tm here = *time_store_tm();
    time_t at = mktime(&here) + (lcars_zone_1_offset_minutes() * 60);
    struct tm *there = gmtime(&at);
    if (!there)
    {
        no_data(out, n);
        return;
    }

    // 24 hour, matching DAWN and DUSK. an AM or PM would not fit beside the glyph anyway
    snprintf(out, n, "%02d:%02d", there->tm_hour, there->tm_min);
}

const char *ops_zone_1_label(void)
{
    static char label[10];

    // the setting holds "offset,City, Region, CC" so the word stops at the first comma, which is
    // the city on its own. nine characters is all the holder box can hold
    const char *name = lcars_zone_1_name();
    size_t i = 0;
    while (i < sizeof(label) - 1 && name[i] && name[i] != ',')
    {
        label[i] = name[i];
        i++;
    }
    label[i] = '\0';

    text_to_upper(label);
    return label[0] ? label : "ZONE 1";
}
