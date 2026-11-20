/**
 * The Clay settings page for this face.
 *
 * Built from the shared template (see paf/ts/pkjs/config-builder.ts), tuned with the LCARS theme
 * list and the slot builder for the ops panels.
 */
import buildConfig from '../../paf/ts/pkjs/config-builder';
import moduleThumbnails from './clay/module-thumbnails.g';
import moduleMeta from './clay/module-meta';
import themePreviews from './clay/theme-previews.g';

// what one panel can show. the value is the C OpsId, so these numbers are the
// contract with src/c/ops/ops.h and are only ever appended to, never reordered
const OPS_OPTIONS = [
  { label: 'Heart Rate (VITALS)', value: 0 },
  { label: 'Steps / Distance (TRAVERSAL)', value: 1 },
  { label: 'Battery (POWER)', value: 2 },
  { label: 'Calories (METABOLIC)', value: 3 },
  { label: 'Sleep (REGEN)', value: 4 },
  { label: 'Active Minutes (EXERTION)', value: 5 },
  { label: 'Moon Phase % (LUNAR)', value: 6 },
  { label: 'Moon Phase Name (LUNAR)', value: 7 },
  { label: 'Next Full / New Moon', value: 8 },
  { label: 'Sunrise (DAWN)', value: 9 },
  { label: 'Sunset (DUSK)', value: 10 },
  { label: 'Length of Day (DAYLIGHT)', value: 11 },
  { label: 'Countdown to Sunrise / Sunset', value: 12 },
  { label: 'Humidity (ATMOS)', value: 13 },
  { label: 'Wind (AIRFLOW)', value: 14 },
  { label: 'UV Index', value: 15 },
  { label: 'High / Low Temperature (RANGE)', value: 16 },
  { label: 'Julian Date', value: 17 },
  { label: 'Day of Year (SOL)', value: 18 },
  { label: 'Week Number (CYCLE)', value: 19 },
  { label: 'Epoch Clock', value: 24 },
  { label: 'Swatch Beats', value: 25 },
  { label: 'Alternate Time Zone (ZONE 1)', value: 26 },
  { label: 'Next Alarm (ALARM)', value: 27 },
  { label: 'Temperature (THERMAL)', value: 21 },
  { label: 'Conditions (SKY)', value: 22 },
  { label: 'Hour Progress', value: 37 },
  { label: 'Day Progress', value: 38 },
  { label: 'Week Progress', value: 39 },
  { label: 'Year Progress', value: 40 },
  { label: 'Precipitation (PRECIP)', value: 45 },
  { label: 'Feels Like', value: 46 },
  { label: 'Pressure', value: 47 },
  { label: 'Dew Point', value: 48 },
  { label: 'Days Left', value: 49 },
  { label: 'Weeks Left', value: 50 },
  { label: 'Weekday Dots', value: 51 },
  { label: 'Empty', value: 20 },
];

// the bigger panels. the weather block fills two cells of the left column, a wide panel fills a row,
// and a full panel fills a 2x2 block. each is only offered where it starts, and the watch reads it as
// Empty anywhere else
const BIG_PANELS = [
  { label: 'Sensors Block (weather + temperature)', value: 23 },
  { label: 'Daylight Bar (DAYLIGHT)', value: 28 },
  { label: 'Steps by the Hour (TRAVERSAL)', value: 29 },
  { label: 'Heart Rate by the Hour (VITALS)', value: 32 },
  { label: 'Now Playing (AUDIO)', value: 33 },
  { label: 'Next Event (SCHEDULE)', value: 36 },
  { label: 'Hour Progress Bar', value: 41 },
  { label: 'Day Progress Bar', value: 42 },
  { label: 'Week Progress Bar', value: 43 },
  { label: 'Year Progress Bar', value: 44 },
  { label: 'UV Index Bar', value: 54 },
  { label: 'Moon Phase and Countdown', value: 55 },
  { label: 'Next Alarm and Countdown', value: 56 },
  { label: 'Swatch Beats Bar', value: 57 },
  { label: 'Now Playing with Art (AUDIO)', value: 34 },
  { label: 'Agenda (SCHEDULE)', value: 35 },
  { label: 'Hourly Forecast Board', value: 52 },
  { label: '4-Day Forecast Board', value: 53 },
  { label: 'Heart Rate Today (VITALS TODAY)', value: 30 },
  { label: 'Steps Today (TRAVERSAL TODAY)', value: 31 },
];

// what the builder drags. every readout plus the block with the emoji and tint
// from module-meta riding along
// the builder cannot import that file itself. Clay turns its initialize into text
// and runs it in a webview with no module scope. so the merge happens here and
// rides on the page item
const BUILDER_OPTIONS = [...BIG_PANELS, ...OPS_OPTIONS].map(function (option) {
  const merged: Record<string, unknown> = {};

  Object.assign(merged, option, moduleMeta[option.label]);
  return merged;
});

// the nine frame themes, shared by the Frame Theme picker and the night and Quiet Time pickers
const THEME_OPTIONS = [
  { label: 'Classic', value: 0 },
  { label: 'Nemesis Blue', value: 3 },
  { label: 'Classic Mono', value: 4 },
  { label: 'Voyager', value: 7 },
  { label: 'Voyager Mono', value: 8 },
  { label: 'Lower Decks', value: 1 },
  { label: 'Lower Decks Mono', value: 5 },
  { label: 'Lower Decks PADD', value: 2 },
  { label: 'Lower Decks PADD Mono', value: 6 },
];

export default buildConfig({
  // the framework's default intro leaves weather out, and this face has it
  intro: 'Personalize your layout, dial in your weather preferences, and make this watchface your own.',
  theme: {
    label: 'Frame Theme',
    description: 'LCARS colour scheme for the watch frame.',
    options: THEME_OPTIONS,
    previews: themePreviews,
  },
  appearanceItems: [
    {
      'type': 'select',
      'messageKey': 'APPEARANCE_THEME_MODE',
      'label': 'Theme Mode',
      'description': 'How the frame theme above is chosen. Fixed keeps it. Day / Night / Quiet keeps it by day and swaps to another after dark or while Quiet Time is on.',
      'defaultValue': 0,
      'options': [
        { 'label': 'Fixed', 'value': 0 },
        { 'label': 'Day / Night / Quiet', 'value': 1 },
      ],
    },
    {
      'type': 'toggle',
      'messageKey': 'APPEARANCE_NIGHT_THEME_ON',
      'visibleWhen': { 'key': 'APPEARANCE_THEME_MODE', 'in': [1] },
      'label': 'Use Night Theme',
      'description': 'In Day / Night / Quiet mode, use a different frame theme after dark. Off keeps the daytime one all night. Night follows your sunrise and sunset.',
      'defaultValue': false,
    },
    {
      'type': 'select',
      'messageKey': 'APPEARANCE_NIGHT_THEME',
      'visibleWhen': [{ 'key': 'APPEARANCE_THEME_MODE', 'in': [1] }, { 'key': 'APPEARANCE_NIGHT_THEME_ON', 'in': [true] }],
      'label': 'Night Theme',
      'defaultValue': 0,
      'options': THEME_OPTIONS,
    },
    {
      'type': 'toggle',
      'messageKey': 'APPEARANCE_QUIET_THEME_ON',
      'visibleWhen': { 'key': 'APPEARANCE_THEME_MODE', 'in': [1] },
      'label': 'Use Quiet Time Theme',
      'description': 'In Day / Night / Quiet mode, use a different frame theme while Quiet Time is on. Quiet Time wins over night.',
      'defaultValue': false,
    },
    {
      'type': 'select',
      'messageKey': 'APPEARANCE_QUIET_THEME',
      'visibleWhen': [{ 'key': 'APPEARANCE_THEME_MODE', 'in': [1] }, { 'key': 'APPEARANCE_QUIET_THEME_ON', 'in': [true] }],
      'label': 'Quiet Time Theme',
      'defaultValue': 0,
      'options': THEME_OPTIONS,
    },
  ],
  layoutSections: [
    {
      'type': 'section',
      'items': [
        {
          'type': 'heading',
          'defaultValue': 'Ops Panels',
        },
        {
          'type': 'text',
          'defaultValue': 'The four panels across the bottom of the face. The sun and weather readouts need a weather source set up below.',
        },
        // the three panels the builder does not own outright. a Clay component
        // carries one message key so the other three ride hidden stores it writes
        // to by class. the watch reads all four the same way as plain enum settings
        {
          'type': 'hiddenStore',
          'messageKey': 'APPEARANCE_SLOT_LEFT_BOTTOM',
          'storeClass': 'sb-lb',
          'defaultValue': '20',
        },
        {
          'type': 'hiddenStore',
          'messageKey': 'APPEARANCE_SLOT_RIGHT_TOP',
          'storeClass': 'sb-rt',
          'defaultValue': '0',
        },
        {
          'type': 'hiddenStore',
          'messageKey': 'APPEARANCE_SLOT_RIGHT_BOTTOM',
          'storeClass': 'sb-rb',
          'defaultValue': '1',
        },
        {
          'type': 'slotBuilder',
          'messageKey': 'APPEARANCE_SLOT_LEFT_TOP',
          'defaultValue': '23',
          'moduleOptions': BUILDER_OPTIONS,
          'moduleThumbnails': moduleThumbnails,
        },
      ],
    },
  ],
  // the date line runs through readout_date, so it can end in a .beats reading
  date: { beats: true },
  location: { gpsDefault: false },
  clock: { timeZone: true },
  weather: {},
  calendar: {},
  temperature: {},
  quietTime: {},
  hourlyVibe: {},
  steps: {
    label: 'Steps Readout',
    description: 'Whether a panel set to Steps counts steps or shows the distance walked.',
  },
});
