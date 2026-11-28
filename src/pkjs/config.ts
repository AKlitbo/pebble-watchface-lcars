/**
 * The Clay settings page for this face.
 *
 * Built from the shared template (see paf/ts/pkjs/config-builder.ts), tuned with the layout and
 * LCARS theme pickers and the slot builder for the ops panels.
 */
import buildConfig from '../../paf/ts/pkjs/config-builder';
import type { ClayConfigItem } from '../../paf/ts/clay/types';
import { allItems, findRow, listHolding } from '../../paf/ts/clay/rows';
import tacticalPresets from '../data/tactical-presets.json';
import engineeringPresets from '../data/engineering-presets.json';
import transporterPresets from '../data/transporter-presets.json';
import moduleThumbnails from './clay/module-thumbnails.g';
import moduleMeta from './clay/module-meta';
import themePreviews from './clay/theme-previews-layout-theme.g';

// what one panel can show. the value is the C OpsId, so these numbers are the
// contract with src/c/ops/ops.h and are only ever appended to, never reordered
const OPS_OPTIONS = [
  { label: 'Heart Rate (VITALS)', value: 0 },
  { label: 'Steps / Distance (TRAVERSAL)', value: 1 },
  { label: 'Battery (POWER)', value: 2 },
  { label: 'Phone Battery (PHONE)', value: 88 },
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

// the bigger panels. a block fills two cells of a column, a wide panel fills a row, and a full panel
// fills a 2x2 block. each is only offered where it starts, and the watch reads it as Empty anywhere
// else. a block goes in either column
const BIG_PANELS = [
  { label: 'Sensors Block (weather + temperature)', value: 23 },
  { label: 'Step Goal Block (TRAVERSAL)', value: 64 },
  { label: 'Distance Goal Block (DISTANCE)', value: 65 },
  { label: 'Calorie Goal Block (METABOLIC)', value: 66 },
  { label: 'Active Minutes Goal Block (EXERTION)', value: 67 },
  { label: 'Sleep Goal Block (REGEN)', value: 68 },
  { label: 'Heart Rate Block (VITALS)', value: 69 },
  { label: 'Steps by the Hour Block (TRAVERSAL)', value: 70 },
  { label: 'Heart Rate by the Hour Block (VITALS)', value: 71 },
  { label: 'Battery Block (POWER)', value: 72 },
  { label: 'Next Alarm Block (ALARM)', value: 73 },
  { label: 'Swatch Beats Block', value: 74 },
  { label: 'Countdown to Sunrise / Sunset Block', value: 75 },
  { label: 'Daylight Block (DAYLIGHT)', value: 76 },
  { label: 'Moon Block (LUNAR)', value: 77 },
  { label: 'UV Index Block', value: 78 },
  { label: 'Hour Progress Block', value: 79 },
  { label: 'Day Progress Block', value: 80 },
  { label: 'Week Progress Block', value: 81 },
  { label: 'Year Progress Block', value: 82 },
  { label: 'Next Event Block (SCHEDULE)', value: 83 },
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
  { label: 'Step Goal Bar (TRAVERSAL GOAL)', value: 58 },
  { label: 'Distance Goal Bar', value: 59 },
  { label: 'Calorie Goal Bar (METABOLIC GOAL)', value: 60 },
  { label: 'Active Minutes Goal Bar (EXERTION GOAL)', value: 61 },
  { label: 'Sleep Goal Bar (REGEN GOAL)', value: 62 },
  { label: 'Next Sunrise / Sunset and Countdown', value: 63 },
  { label: 'Now Playing with Art (AUDIO)', value: 34 },
  { label: 'Agenda (SCHEDULE)', value: 35 },
  { label: 'Hourly Forecast Board', value: 52 },
  { label: '4-Day Forecast Board', value: 53 },
  { label: 'Heart Rate Today (VITALS TODAY)', value: 30 },
  { label: 'Steps Today (TRAVERSAL TODAY)', value: 31 },
  // the clock and the date as panels, which only Tactical places, once each
  { label: 'Clock', value: 84 },
  { label: 'Date (STARDATE)', value: 85 },
  { label: 'Compact Clock', value: 86 },
  { label: 'Compact Date (STARDATE)', value: 87 },
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

// what a rail gauge can show. each value is the OpsId of the tall panel the gauge stands for, and
// the watch fills the gauge the way that panel fills its bar. the word in brackets is the one the
// rail shows over it
const GAUGE_OPTIONS = [
  { label: 'Battery (PWR)', value: 72 },
  { label: 'Phone Battery (PHN)', value: 88 },
  { label: 'Step Goal (STEP)', value: 64 },
  { label: 'Distance Goal (DIST)', value: 65 },
  { label: 'Calorie Goal (KCAL)', value: 66 },
  { label: 'Active Minutes Goal (ACT)', value: 67 },
  { label: 'Sleep Goal (SLP)', value: 68 },
  { label: 'Hour Progress (HOUR)', value: 79 },
  { label: 'Day Progress (DAY)', value: 80 },
  { label: 'Week Progress (WEEK)', value: 81 },
  { label: 'Year Progress (YEAR)', value: 82 },
  { label: 'Daylight (SUN)', value: 76 },
  { label: 'Moon (MOON)', value: 77 },
  { label: 'UV Index (UV)', value: 78 },
  { label: 'Swatch Beats (BEAT)', value: 74 },
  { label: 'Next Alarm (ALRM)', value: 73 },
];

/**
 * A section of settings that only one layout has, titled and shown while one of the Layout picks
 * given is picked, and hidden whole otherwise.
 */
function layoutSettings(title: string, picks: number[], items: ClayConfigItem[]): ClayConfigItem {
  const shown = { 'key': 'APPEARANCE_LAYOUT', 'in': picks };
  const heading: ClayConfigItem = { 'type': 'heading', 'defaultValue': title };
  const rows = [heading].concat(items).map(function (item) {
    return Object.assign({}, item, { 'visibleWhen': shown });
  });

  return {
    'type': 'section',
    // the framework hides a marked section's card once every row in it is hidden
    'hideWhenEmpty': true,
    'items': rows,
  };
}

/** A layout's Mirrored switch, which runs its rail down the right while its panels keep their places. */
function mirrorRow(messageKey: string): ClayConfigItem {
  return {
    'type': 'toggle',
    'messageKey': messageKey,
    'label': 'Mirrored',
    'description': 'Runs the rail down the right. Your panels keep their places.',
    'defaultValue': false,
  };
}

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

const config = buildConfig({
  heading: 'Watchface Configuration',
  // the framework's default intro leaves weather out, and this face has it
  intro: 'Personalize your layout, dial in your weather preferences, and make this watchface your own.',
  // the Layout picker leads, then the frame theme with a shot of each. the theme picker is spelled
  // out here rather than handed to buildConfig's theme shorthand, since that puts it first
  appearanceItems: [
    // a plain list, since the Frame Theme picker under it already shows the picked layout in each theme.
    // each layout saves as twice its number, and the watch reads an odd pick as that layout mirrored.
    // the mirrored picks stay listed so Clay can fill in one a watch already has saved, and
    // slot-layout-fn turns it into the layout's own Mirrored switch and then takes them off the list
    {
      'type': 'select',
      'messageKey': 'APPEARANCE_LAYOUT',
      'label': 'Layout',
      'description': 'Tactical has panels in all three frames, with the clock and date as panels you can move. Engineering stacks the clock beside six panels. Transporter puts the clock over eight panels between two gauge rails.',
      'defaultValue': 0,
      'options': [
        { 'label': 'Stardate (Default)', 'value': 0 },
        { 'label': 'Stardate Mirrored', 'value': 1 },
        { 'label': 'Tactical', 'value': 2 },
        { 'label': 'Tactical Mirrored', 'value': 3 },
        { 'label': 'Engineering', 'value': 4 },
        { 'label': 'Engineering Mirrored', 'value': 5 },
        { 'label': 'Transporter', 'value': 6 },
      ],
    },
    {
      'type': 'themePreview',
      'messageKey': 'APPEARANCE_THEME',
      'label': 'Frame Theme',
      'description': 'LCARS colour scheme for the watch frame.',
      'defaultValue': 0,
      'options': THEME_OPTIONS,
      // each layout has its own shot of every theme, so the picture follows the Layout picked above
      'previews': themePreviews,
      'previewsByKey': 'APPEARANCE_LAYOUT',
    },
    {
      'type': 'select',
      'messageKey': 'APPEARANCE_BAR_LABELS',
      'label': 'Bar Labels',
      'description': 'Which end of each bar its word sits at, such as VITALS or STARDATE.',
      'defaultValue': 0,
      'options': [
        { 'label': 'Left (Default)', 'value': 0 },
        { 'label': 'Right', 'value': 1 },
      ],
    },
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
    layoutSettings('Stardate Settings', [0, 1], [mirrorRow('APPEARANCE_MIRROR_STARDATE')]),
    layoutSettings('Tactical Settings', [2, 3], [mirrorRow('APPEARANCE_MIRROR_TACTICAL')]),
    layoutSettings('Engineering Settings', [4, 5], [
      mirrorRow('APPEARANCE_MIRROR_ENGINEERING'),
      {
        'type': 'select',
        'messageKey': 'APPEARANCE_ENGINEERING_CORE',
        'label': 'Power Core',
        'description': 'What the cells down the rail fill with. The word over its reading says which.',
        'defaultValue': 72,
        'options': GAUGE_OPTIONS,
      },
    ]),
    layoutSettings('Transporter Settings', [6, 7], [
      {
        'type': 'select',
        'messageKey': 'APPEARANCE_GAUGE_LEFT',
        'label': 'Left Gauge',
        'description': 'What the cells down the left rail fill with. The word on top of the rail says which.',
        'defaultValue': 72,
        'options': GAUGE_OPTIONS,
      },
      {
        'type': 'select',
        'messageKey': 'APPEARANCE_GAUGE_RIGHT',
        'label': 'Right Gauge',
        'description': 'What the cells down the right rail fill with.',
        'defaultValue': 64,
        'options': GAUGE_OPTIONS,
      },
    ]),
    {
      'type': 'section',
      'items': [
        {
          'type': 'heading',
          'defaultValue': 'Ops Panels',
        },
        {
          'type': 'text',
          'defaultValue': 'The panels of the layout picked above, four on Stardate, up to twelve on Tactical, six on Engineering, and eight on Transporter. Each layout keeps its own. The sun and weather readouts need a weather source set up below.',
        },
        // the panels the builder does not own outright. a Clay component carries one message key,
        // so the Stardate layout's other three ride hidden stores it writes to by class, and the watch
        // reads all four as plain enum settings. Tactical's twelve, Engineering's six, and Transporter's
        // eight each ride one more store as a list the watch reads a cell at a time. the stores sit above the builder, since Clay has not built a
        // row below it yet when the builder reads them
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
          'type': 'hiddenStore',
          'messageKey': 'APPEARANCE_SLOTS_ENGINEERING',
          'storeClass': 'sb-eng',
          'defaultValue': engineeringPresets.default.join(','),
        },
        {
          'type': 'hiddenStore',
          'messageKey': 'APPEARANCE_SLOTS_TRANSPORTER',
          'storeClass': 'sb-trn',
          'defaultValue': transporterPresets.default.join(','),
        },
        {
          'type': 'hiddenStore',
          'messageKey': 'APPEARANCE_SLOTS_TACTICAL',
          'storeClass': 'sb-tac',
          'defaultValue': tacticalPresets.default.join(','),
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
  // the date line runs through readout_date, so it can end in a .beats reading. it opens on the
  // numeric stardate-style date
  date: { beats: true, default: '%Y.%m%d' },
  location: { gpsDefault: false, placeholder: 'Search a city, e.g. Phoenix' },
  clock: { timeZone: true, timeZonePlaceholder: 'e.g. Phoenix, UTC, or Europe/London' },
  weather: {},
  calendar: {},
  temperature: {},
  quietTime: {},
  hourlyVibe: {},
  steps: {
    label: 'Steps Readout',
    description: 'Whether a panel set to Steps counts steps or shows the distance walked.',
  },
  // no panel draws a heart rate bar, so the Heart Rate Limit stays off
  goals: {
    distanceUnit: { description: 'The unit the Distance Goal Bar reads in. It starts on your Steps Readout unit.' },
    alerts: { custom: true },
  },
});

// the two glyph toggles only drive the Stardate layout's glyphs. Tactical reads the phone link and
// Quiet Time in its pills, and Engineering and Transporter in their rails, whatever they say, so they
// hide while any of those is picked
const STARDATE_ONLY = ['CONNECTION_BLUETOOTH_ICON', 'APPEARANCE_QUIET_TIME_ICON'];

/**
 * Hides the Stardate-only toggles while any other layout is picked. The framework builds those rows, so the
 * rule is added to them here once the page is built.
 *
 * @param items The page's rows, searched down through every section.
 */
function hideOffStardate(items: ClayConfigItem[]): void {
  allItems(items).forEach(function (item) {
    if (item.messageKey && STARDATE_ONLY.indexOf(item.messageKey) >= 0) {
      item.visibleWhen = { 'key': 'APPEARANCE_LAYOUT', 'in': [0, 1] };
    }
  });
}

/**
 * Moves the Stardate-only toggles into Stardate Settings, under Mirrored, in the order STARDATE_ONLY
 * lists them. The framework builds them in Bluetooth and Appearance, so they are moved here once the
 * page is built, and they keep the rule above that hides them off Stardate.
 *
 * @param items The page's rows, changed in place.
 */
function glyphTogglesToStardate(items: ClayConfigItem[]): void {
  const stardate = listHolding(items, (item) => item.messageKey === 'APPEARANCE_MIRROR_STARDATE');

  STARDATE_ONLY.forEach(function (key) {
    const found = findRow(items, (item) => item.messageKey === key);

    if (!found || !stardate) {
      return;
    }

    found.list.splice(found.list.indexOf(found.row), 1);
    stardate.push(found.row);
  });
}

hideOffStardate(config);
glyphTogglesToStardate(config);

export default config;
