/**
 * @file main.c
 * @brief stardate-emery entry point: wires services to their stores and drives the slot
 * engine directly (no shell).
 */
#include <pebble.h>

#include "io/appmessage/appmessage.h"
#include "ui/engine/engine.h"
#include "ui/readouts.h"
#include "io/stores/calendar_store.h"
#include "io/stores/health_store.h"
#include "io/stores/music_store.h"
#include "io/stores/location_store.h"
#include "io/stores/system_store.h"
#include "io/stores/time_store.h"
#include "io/stores/weather_store.h"
#include "dev/dev.h"
#include "layout.h"
#include "ops/ops.h"
#include "persist_keys.h"
#include "system/settings/settings.h"
#include "system/settings/setting_values.h"
#include "settings_schema.h"
#include "system/vibe/vibe.h"

// weather polls the phone this often
#define WEATHER_POLL_MIN 30

// the calendar polls the phone this often while a calendar panel shows it
#define CALENDAR_POLL_MIN 30

static Window *s_window;

// the health store's rules as last started, so a settings change only restarts it when a chart
// panel came or went. false while the dev fixture seeds the stores, which a restart would undo
static HealthConfig s_health;
static bool s_health_live;

// whether the calendar and music stores were last started live
static bool s_calendar_live;
static bool s_music_live;

/** @brief Whether a panel that shows the calendar is placed. */
static bool calendar_shown(void)
{
    return stardate_ops_shows(OPS_FULL_AGENDA) || stardate_ops_shows(OPS_WIDE_EVENT);
}

/** @brief Whether a panel that shows what is playing is placed. */
static bool music_shown(void)
{
    return stardate_ops_shows(OPS_WIDE_MUSIC) || stardate_ops_shows(OPS_FULL_MUSIC);
}

/**
 * @brief Starts the calendar and music stores live only while a panel shows them, and again when a
 * save adds the first one or takes the last one off.
 *
 * A live calendar store asks the phone for the agenda every half hour, and a live music store has
 * the phone send every change of track. With no panel showing either, both would only wake the
 * watch for nothing. A music store started not live tells the phone so, and the phone stops sending.
 *
 * @param first True at launch, when both start whatever they were before.
 */
static void apply_phone_feeds(bool first)
{
    if (!s_health_live)
    {
        return;
    }

    bool calendar_live = calendar_shown();
    if (first || calendar_live != s_calendar_live)
    {
        s_calendar_live = calendar_live;
        calendar_store_init((CalendarConfig){.live = calendar_live, .poll_min = CALENDAR_POLL_MIN,
                                             .persist_key = CALENDAR_STORE_KEY}, NULL);
    }

    bool music_live = music_shown();
    if (first || music_live != s_music_live)
    {
        s_music_live = music_live;
        music_store_init((MusicConfig){.live = music_live}, NULL);
    }
}

/**
 * @brief The health store's rules from what the ops panels show.
 *
 * An ops slot can show sleep, active minutes or calories, and those three each cost a read the
 * watch does not cache, so they are always on. The two hourly parts only run while a chart panel
 * draws them. The heart rate one reads the watch's minute log at launch, which is real work for a
 * panel nobody placed.
 */
static HealthConfig health_cfg(void)
{
    bool step_chart = stardate_ops_shows(OPS_WIDE_STEPS) || stardate_ops_shows(OPS_FULL_STEPS);
    bool heart_chart = stardate_ops_shows(OPS_WIDE_HEART) || stardate_ops_shows(OPS_FULL_HEART);

    return (HealthConfig){
        .live = true,
        .sleep = true,
        .active = true,
        .calories = true,
        .distance = true,
        .step_history = step_chart ? HEALTH_STEP_HISTORY : NULL,
        .hr_day = heart_chart ? HEALTH_HR_DAY : NULL,
        .persist_key = HEALTH_STORE_KEY,
    };
}

/**
 * @brief The time-store rules from the current settings. The cadence follows what is on screen.
 * On a .beats clock the hourly buzz also asks for a wake at the top of each hour.
 *
 * Three things can want the .beats ticker and they are independent: the clock itself, any ops
 * slot picked as BEATS, and a date format ending in a reading. A beat is 86.4 seconds, so
 * anything left on the minute tick sits on a stale number and reports every rollover up to a
 * minute late. The store runs both cadences at once, so the .beats ticker can drive an ops slot
 * or the date line while the clock keeps its minute tick.
 */
static TimeConfig time_cfg(void)
{
    bool clock_beats = settings_u8(SETTING_TIME_FORMAT) == TIME_FORMAT_BEATS;
    bool hourly = settings_u8(SETTING_HOURLY_VIBE) != VIBE_NONE;

    return (TimeConfig){
        .live = true,
        .minute_tick = !clock_beats,  // .beats replaces the clock, and only the clock
        .hourly = hourly,             // a .beats clock still wakes on the hour for the buzz
        .beats = clock_beats || stardate_ops_shows_beats() || readout_date_shows_beats(),
    };
}

/**
 * @brief Redraw after a settings change: re-theme, rebuild the slots, re-arm the ticker.
 *
 * @param time_or_date_changed True if a time or date format changed (unused: a rebuild re-pulls
 * every readout anyway, and the ticker is re-armed on every change, since Hourly Vibration picks it
 * too).
 */
static void on_settings_changed(bool time_or_date_changed)
{
    // swap the frame bitmap then rebuild so the slots repaint. the rebuild re-pulls every
    // readout covering a steps-mode / unit / format change
    stardate_apply_theme();
    engine_rebuild();

    time_store_reconfigure(time_cfg());  // on every change, since the formats and the buzz both pick it

    // a chart panel placed or taken away starts or stops the hourly part behind it
    HealthConfig health = health_cfg();
    if (s_health_live && (health.step_history != s_health.step_history || health.hr_day != s_health.hr_day))
    {
        s_health = health;
        health_store_init(s_health, NULL);
    }

    // a calendar or Now Playing panel placed or taken away starts or stops the phone's feed for it
    apply_phone_feeds(false);
}

/**
 * @brief Tick callback: repaint the slots and fire the hourly vibe.
 *
 * vibe_hourly buzzes once per hour however many tickers land in minute 0, and stays silent during
 * quiet time and for VIBE_NONE. On a .beats clock the hour tick time_cfg asks for is what lands it
 * there. The SDK sends no hour tick for the hour the clocks repeat in autumn, so on a .beats clock
 * that hour buzzes only when a beat lands in its minute 0. Catching the rest would take the minute
 * tick, sixty wakes an hour all year for one buzz a year.
 */
static void on_time_tick(void)
{
    // in Day / Night / Quiet mode the frame theme can change with the hour, so reload it if it moved
    stardate_refresh_theme();
    engine_mark_dirty();
    vibe_hourly(time_store_tm(), settings_u8(SETTING_HOURLY_VIBE));
}

static void init(void)
{
    settings_init(lcars_settings_schema());

    // each store owns its own source. init starts it (live) or seeds it. dev mode seeds
    // every store from a fixed fixture (live=false) for deterministic screenshots, and this
    // face's own walks seed their readings over it
    if (!dev_seed_stores())
    {
        system_store_init((SystemConfig){.live = true, .vibe = vibe_bt_transition}, NULL);
        s_health = health_cfg();
        s_health_live = true;
        health_store_init(s_health, NULL);
        apply_phone_feeds(true);
        time_store_init(time_cfg(), NULL);
        weather_store_init((WeatherConfig){.live = true, .poll_min = WEATHER_POLL_MIN,
                                           .persist_key = WEATHER_STORE_KEY}, NULL);
        location_store_init((LocationConfig){.live = true,
                                             .persist_key = LOCATION_STORE_KEY}, NULL);
    }
    lcars_dev_seed_stores();

    s_window = window_create();
    stardate_setup(s_window);              // fonts + frame + overlays
    engine_init(s_window, stardate_build); // build the slot layers over the frame
    window_stack_push(s_window, true);

    // any store change repaints all slots. the time tick also drives the hourly vibe
    time_store_subscribe(on_time_tick);
    health_store_subscribe(engine_mark_dirty);
    calendar_store_subscribe(engine_mark_dirty);
    music_store_subscribe(engine_mark_dirty);
    weather_store_subscribe(engine_mark_dirty);
    location_store_subscribe(engine_mark_dirty);
    system_store_subscribe(engine_mark_dirty);

    // weather_store + location_store own their channels. main only wires settings
    appmessage_on_settings_changed(on_settings_changed);
    appmessage_open(4096);

    // dev mode only: force the theme then paint the fixture then subscribe the tap walk
    dev_start(stardate_apply_theme);
    lcars_dev_start(stardate_apply_theme);
}

static void deinit(void)
{
    dev_stop();
    lcars_dev_stop();
    system_store_deinit();
    music_store_deinit();
    engine_deinit();
    stardate_teardown();
    window_destroy(s_window);
}

int main(void)
{
    init();
    app_event_loop();
    deinit();
}
