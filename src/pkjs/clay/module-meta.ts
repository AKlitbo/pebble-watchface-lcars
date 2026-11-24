/**
 * Per-readout presentation metadata for the Clay slot builder, keyed by label (matching
 * config.ts OPS_OPTIONS and module-thumbnails.g.js) so a lookup survives a numeric id drift.
 *
 * config.ts merges these onto OPS_OPTIONS so the builder reads them straight from its config. It
 * cannot import this file directly, because Clay turns the builder's initialize into text and
 * runs it in a webview of its own where module scope (and this import) does not exist.
 *
 * Fields per row:
 *   icon        emoji fallback, shown when a readout has no real panel thumbnail.
 *   blockColor  the tint the builder paints behind the icon.
 *   slug        the resources/thumbnails/<slug>-<size>.png filename stem. embed-thumbnails.ts
 *               reads it from here so the thumbnail list has one home, not a second copy.
 *   group       the palette group tab it sits under, one of the builder's PALETTE_GROUPS.
 *   plugin      the Pebble app plugin kind it reads, for one whose readings a plugin can bring,
 *               so the builder warns when that plugin cannot fill it.
 *
 * Empty is deliberately absent. It has no panel to photograph, and the builder offers it as a
 * clear action rather than as something to drag.
 */

import type { ModuleMeta } from '../../../paf/ts/clay/types';

/** One readout's row: the framework's fields, plus where the palette files it and what it reads. */
export interface LcarsModuleMeta extends ModuleMeta {
  group: string;
  plugin?: 'weather' | 'calendar' | 'music';
}

const moduleMeta: Record<string, LcarsModuleMeta> = {
  'Clock':                          { icon: '🕐', blockColor: '#ff9c00', slug: 'clock', group: 'Clock' },
  'Date (STARDATE)':                { icon: '📅', blockColor: '#cc88ff', slug: 'stardate', group: 'Date' },
  'Compact Clock':                  { icon: '🕐', blockColor: '#ff9c00', slug: 'clock-compact', group: 'Clock' },
  'Compact Date (STARDATE)':        { icon: '📅', blockColor: '#cc88ff', slug: 'stardate-compact', group: 'Date' },

  'Heart Rate (VITALS)':            { icon: '❤️', blockColor: '#e74c3c', slug: 'heart', group: 'Health' },
  'Steps / Distance (TRAVERSAL)':   { icon: '👟', blockColor: '#2ecc71', slug: 'steps', group: 'Health' },
  'Battery (POWER)':                { icon: '🔋', blockColor: '#f0a500', slug: 'battery', group: 'Status' },
  'Next Alarm (ALARM)':             { icon: '🔔', blockColor: '#ef5350', slug: 'alarm', group: 'Status' },
  'Calories (METABOLIC)':           { icon: '🔥', blockColor: '#e67e22', slug: 'calories', group: 'Health' },
  'Sleep (REGEN)':                  { icon: '😴', blockColor: '#4a5568', slug: 'sleep', group: 'Health' },
  'Active Minutes (EXERTION)':      { icon: '🏃', blockColor: '#1abc9c', slug: 'active', group: 'Health' },

  'Moon Phase % (LUNAR)':           { icon: '🌘', blockColor: '#7e57c2', slug: 'moon-pct', group: 'Sky' },
  'Moon Phase Name (LUNAR)':        { icon: '🌗', blockColor: '#7e57c2', slug: 'moon-phase', group: 'Sky' },
  'Next Full / New Moon':           { icon: '🌑', blockColor: '#673ab7', slug: 'moon-next', group: 'Sky' },

  'Sunrise (DAWN)':                 { icon: '🌅', blockColor: '#ffa726', slug: 'sunrise', group: 'Sky', plugin: 'weather' },
  'Sunset (DUSK)':                  { icon: '🌇', blockColor: '#ef6c00', slug: 'sunset', group: 'Sky', plugin: 'weather' },
  'Length of Day (DAYLIGHT)':       { icon: '☀️', blockColor: '#fbc02d', slug: 'daylight', group: 'Sky', plugin: 'weather' },
  'Countdown to Sunrise / Sunset':  { icon: '⏳', blockColor: '#ff8f00', slug: 'sun-next', group: 'Sky', plugin: 'weather' },

  'Humidity (ATMOS)':               { icon: '💧', blockColor: '#29b6f6', slug: 'humidity', group: 'Weather', plugin: 'weather' },
  'Wind (AIRFLOW)':                 { icon: '🌬', blockColor: '#4dd0e1', slug: 'wind', group: 'Weather', plugin: 'weather' },
  'UV Index':                       { icon: '😎', blockColor: '#ab47bc', slug: 'uv', group: 'Weather', plugin: 'weather' },
  'High / Low Temperature (RANGE)': { icon: '🌡', blockColor: '#42a5f5', slug: 'hilo', group: 'Weather', plugin: 'weather' },
  'Temperature (THERMAL)':          { icon: '🌡', blockColor: '#42a5f5', slug: 'temp', group: 'Weather', plugin: 'weather' },
  'Precipitation (PRECIP)':         { icon: '☔', blockColor: '#1e88e5', slug: 'precip', group: 'Weather', plugin: 'weather' },
  'Feels Like':         { icon: '🌡', blockColor: '#42a5f5', slug: 'feels-like', group: 'Weather', plugin: 'weather' },
  'Pressure':                       { icon: '🧭', blockColor: '#5c6bc0', slug: 'pressure', group: 'Weather', plugin: 'weather' },
  'Dew Point':           { icon: '💧', blockColor: '#29b6f6', slug: 'dew-point', group: 'Weather', plugin: 'weather' },
  'Conditions (SKY)':               { icon: '🌤', blockColor: '#2196f3', slug: 'conditions', group: 'Weather', plugin: 'weather' },

  'Julian Date':                    { icon: '🗓', blockColor: '#90a4ae', slug: 'julian', group: 'Date' },
  'Day of Year (SOL)':              { icon: '📅', blockColor: '#90a4ae', slug: 'day-of-year', group: 'Date' },
  'Week Number (CYCLE)':            { icon: '🗓', blockColor: '#78909c', slug: 'week', group: 'Date' },

  'Epoch Clock':                    { icon: '🕗', blockColor: '#aed581', slug: 'epoch', group: 'Clock' },
  'Swatch Beats':                   { icon: '🌐', blockColor: '#e91e63', slug: 'beats', group: 'Clock' },
  'Alternate Time Zone (ZONE 1)':   { icon: '🌍', blockColor: '#8e44ad', slug: 'zone1', group: 'Clock' },

  'Hour Progress':         { icon: '⏱', blockColor: '#26a69a', slug: 'hour-progress', group: 'Date' },
  'Day Progress':         { icon: '🕛', blockColor: '#26a69a', slug: 'day-progress', group: 'Date' },
  'Week Progress':          { icon: '📆', blockColor: '#00897b', slug: 'week-progress', group: 'Date' },
  'Year Progress':         { icon: '📆', blockColor: '#00796b', slug: 'year-progress', group: 'Date' },
  'Days Left':          { icon: '📅', blockColor: '#90a4ae', slug: 'days-left', group: 'Date' },
  'Weeks Left':                     { icon: '🗓', blockColor: '#78909c', slug: 'weeks-left', group: 'Date' },
  'Weekday Dots':       { icon: '📅', blockColor: '#78909c', slug: 'weekday-dots', group: 'Date' },

  // the blocks that fill two cells of a column, in either column
  'Sensors Block (weather + temperature)': { icon: '🌤', blockColor: '#1e88e5', slug: 'sensors', group: 'Weather', plugin: 'weather' },
  'Step Goal Block (TRAVERSAL)': { icon: '👟', blockColor: '#2ecc71', slug: 'step-goal-block', group: 'Health' },
  'Distance Goal Block (DISTANCE)': { icon: '📏', blockColor: '#27ae60', slug: 'distance-goal-block', group: 'Health' },
  'Calorie Goal Block (METABOLIC)': { icon: '🔥', blockColor: '#e67e22', slug: 'calorie-goal-block', group: 'Health' },
  'Active Minutes Goal Block (EXERTION)': { icon: '🏃', blockColor: '#1abc9c', slug: 'active-goal-block', group: 'Health' },
  'Sleep Goal Block (REGEN)': { icon: '😴', blockColor: '#4a5568', slug: 'sleep-goal-block', group: 'Health' },
  'Heart Rate Block (VITALS)': { icon: '❤️', blockColor: '#e74c3c', slug: 'heart-block', group: 'Health' },
  'Steps by the Hour Block (TRAVERSAL)': { icon: '👟', blockColor: '#2ecc71', slug: 'steps-hours-block', group: 'Health' },
  'Heart Rate by the Hour Block (VITALS)': { icon: '❤️', blockColor: '#e74c3c', slug: 'heart-hours-block', group: 'Health' },
  'Battery Block (POWER)': { icon: '🔋', blockColor: '#f0a500', slug: 'battery-block', group: 'Status' },
  'Next Alarm Block (ALARM)': { icon: '🔔', blockColor: '#ef5350', slug: 'alarm-block', group: 'Status' },
  'Swatch Beats Block': { icon: '🌐', blockColor: '#e91e63', slug: 'beats-block', group: 'Clock' },
  'Countdown to Sunrise / Sunset Block': { icon: '⏳', blockColor: '#ff8f00', slug: 'sun-next-block', group: 'Sky', plugin: 'weather' },
  'Daylight Block (DAYLIGHT)': { icon: '☀️', blockColor: '#fbc02d', slug: 'daylight-block', group: 'Sky', plugin: 'weather' },
  'Moon Block (LUNAR)': { icon: '🌗', blockColor: '#7e57c2', slug: 'moon-block', group: 'Sky' },
  'UV Index Block': { icon: '😎', blockColor: '#ab47bc', slug: 'uv-block', group: 'Weather', plugin: 'weather' },
  'Hour Progress Block': { icon: '⏱', blockColor: '#26a69a', slug: 'hour-progress-block', group: 'Date' },
  'Day Progress Block': { icon: '🕛', blockColor: '#26a69a', slug: 'day-progress-block', group: 'Date' },
  'Week Progress Block': { icon: '📆', blockColor: '#00897b', slug: 'week-progress-block', group: 'Date' },
  'Year Progress Block': { icon: '📆', blockColor: '#00796b', slug: 'year-progress-block', group: 'Date' },
  'Next Event Block (SCHEDULE)': { icon: '📅', blockColor: '#3498db', slug: 'event-block', group: 'Calendar', plugin: 'calendar' },

  // the wide ones that fill a row, and the full ones that fill a 2x2 block
  'Daylight Bar (DAYLIGHT)':        { icon: '🌅', blockColor: '#fbc02d', slug: 'daylight-bar', group: 'Sky', plugin: 'weather' },
  'Steps by the Hour (TRAVERSAL)':  { icon: '👟', blockColor: '#2ecc71', slug: 'steps-hours', group: 'Health' },
  'Heart Rate by the Hour (VITALS)': { icon: '❤️', blockColor: '#e74c3c', slug: 'heart-hours', group: 'Health' },
  'Now Playing (AUDIO)':            { icon: '🎵', blockColor: '#8e44ad', slug: 'music', group: 'Music', plugin: 'music' },
  'Next Event (SCHEDULE)':          { icon: '📅', blockColor: '#3498db', slug: 'event', group: 'Calendar', plugin: 'calendar' },
  'Now Playing with Art (AUDIO)':   { icon: '🎵', blockColor: '#8e44ad', slug: 'music-art', group: 'Music', plugin: 'music' },
  'Agenda (SCHEDULE)':              { icon: '📅', blockColor: '#3498db', slug: 'agenda', group: 'Calendar', plugin: 'calendar' },
  'Heart Rate Today (VITALS TODAY)': { icon: '❤️', blockColor: '#e74c3c', slug: 'heart-today', group: 'Health' },
  'Steps Today (TRAVERSAL TODAY)': { icon: '👟', blockColor: '#27ae60', slug: 'steps-today', group: 'Health' },
  'Hour Progress Bar':     { icon: '⏱', blockColor: '#26a69a', slug: 'hour-progress-bar', group: 'Date' },
  'Day Progress Bar':     { icon: '🕛', blockColor: '#26a69a', slug: 'day-progress-bar', group: 'Date' },
  'Week Progress Bar':      { icon: '📆', blockColor: '#00897b', slug: 'week-progress-bar', group: 'Date' },
  'Year Progress Bar':     { icon: '📆', blockColor: '#00796b', slug: 'year-progress-bar', group: 'Date' },
  'UV Index Bar':             { icon: '😎', blockColor: '#ab47bc', slug: 'uv-bar', group: 'Weather', plugin: 'weather' },
  'Moon Phase and Countdown': { icon: '🌗', blockColor: '#7e57c2', slug: 'moon-wide', group: 'Sky' },
  'Next Alarm and Countdown': { icon: '🔔', blockColor: '#ef5350', slug: 'alarm-wide', group: 'Status' },
  'Swatch Beats Bar':         { icon: '🌐', blockColor: '#e91e63', slug: 'beats-bar', group: 'Clock' },
  'Step Goal Bar (TRAVERSAL GOAL)':          { icon: '👟', blockColor: '#2ecc71', slug: 'step-goal-bar', group: 'Health' },
  'Distance Goal Bar':                       { icon: '📏', blockColor: '#27ae60', slug: 'distance-goal-bar', group: 'Health' },
  'Calorie Goal Bar (METABOLIC GOAL)':       { icon: '🔥', blockColor: '#e67e22', slug: 'calorie-goal-bar', group: 'Health' },
  'Active Minutes Goal Bar (EXERTION GOAL)': { icon: '🏃', blockColor: '#1abc9c', slug: 'active-goal-bar', group: 'Health' },
  'Sleep Goal Bar (REGEN GOAL)':             { icon: '😴', blockColor: '#4a5568', slug: 'sleep-goal-bar', group: 'Health' },
  'Next Sunrise / Sunset and Countdown': { icon: '⏳', blockColor: '#ff8f00', slug: 'sun-next-wide', group: 'Sky', plugin: 'weather' },
  'Hourly Forecast Board': { icon: '⛅', blockColor: '#2196f3', slug: 'forecast-hourly', group: 'Weather', plugin: 'weather' },
  '4-Day Forecast Board':   { icon: '📅', blockColor: '#1976d2', slug: 'forecast-daily', group: 'Weather', plugin: 'weather' },
};

// the panel shapes this face's thumbnails come in: one ops slot, a block filling two cells of a
// column, a wide panel filling a row, or a full one filling a 2x2 block. embed-thumbnails.ts refuses a
// png in any other size
export const thumbnailSizes = ['slot', 'tall', 'wide', 'full'];

export default moduleMeta;
