/**
 * Every fact about the shape of this face's panel layouts lives here, so pointing the builder at a
 * different arrangement is a matter of editing this file rather than chasing constants through the
 * drag and render code.
 *
 * Each layout has its panels in areas, each area a grid two columns wide and two rows tall. The
 * Stardate layout has one area under the clock. Tactical has three, one in the top frame, one
 * between the two elbows, and one in the bottom frame. Transporter has two, one under the other
 * between its rails. The cells are numbered down each column of an area, and the areas follow each
 * other from the top, so on Tactical the cells run 0 to 11.
 * Most readouts fill one cell. The bigger ones fill two cells of a column, a row, or a whole area,
 * and each is saved in the cell it starts from, with the cells it covers saved as empty. A panel
 * never runs from one area into the next. The watch reads the values back the same way.
 */

/** A layout's panels: its areas, each a grid of the same columns and rows. */
export interface Grid {
  cols: number;
  rows: number;
  areas: number;
  /** Whether the clock and the date are panels here, which go on the face once each. */
  placesClock: boolean;
}

/**
 * The four layouts, as layoutOfPick returns them. The Layout picker saves the layout times two, and
 * the builder is handed that plus one while the layout's Mirrored switch is on, so read a pick through
 * layoutOfPick rather than comparing it with these.
 */
export const LAYOUT_STARDATE = 0;
export const LAYOUT_TACTICAL = 1;
export const LAYOUT_ENGINEERING = 2;
export const LAYOUT_TRANSPORTER = 3;

/**
 * How many layouts the Layout picker offers. Each has a Mirrored switch but Transporter, whose rails
 * run down both sides.
 */
export const LAYOUT_COUNT = 4;

/** The Stardate layout's panels: two columns of two under the clock. */
export const STARDATE_GRID: Grid = { cols: 2, rows: 2, areas: 1, placesClock: false };

/** Tactical's panels: three areas of two columns of two, with the clock and the date among them. */
export const TACTICAL_GRID: Grid = { cols: 2, rows: 2, areas: 3, placesClock: true };

/**
 * The layout a Layout pick draws. Each layout saves as twice its number and its mirrored pick as one
 * more, and a mirrored pick's panels are the same as the layout's own.
 *
 * @param pick The saved Layout pick, 0 to 6.
 * @return One of the LAYOUT_ numbers.
 */
export function layoutOfPick(pick: number): number {
  // a pick past the ones the picker offers reads as Stardate, the same as the watch
  return pick >= 0 && pick < LAYOUT_COUNT * 2 ? Math.floor(pick / 2) : LAYOUT_STARDATE;
}

/**
 * Whether a Layout pick runs the rail down the right.
 *
 * @param pick The saved Layout pick, 0 to 6.
 * @return True for a mirrored pick. Transporter is never mirrored, the same as on the watch.
 */
export function railRightOfPick(pick: number): boolean {
  return pick >= 0 && pick < LAYOUT_COUNT * 2 && pick % 2 === 1 && layoutOfPick(pick) !== LAYOUT_TRANSPORTER;
}

/** Engineering's panels: one column of six down the right of the screen. */
export const ENGINEERING_GRID: Grid = { cols: 1, rows: 6, areas: 1, placesClock: false };

/** Transporter's panels: two areas of two columns of two, between its rails and under the clock. */
export const TRANSPORTER_GRID: Grid = { cols: 2, rows: 2, areas: 2, placesClock: false };

/**
 * A layout's grid. A layout past the known ones reads as the Stardate layout, the same as on the watch.
 *
 * @param layout The saved layout.
 * @return Its grid.
 */
export function gridFor(layout: number): Grid {
  const grids = [STARDATE_GRID, TACTICAL_GRID, ENGINEERING_GRID, TRANSPORTER_GRID];

  return grids[layout] || STARDATE_GRID;
}

/**
 * How many cells one area of a grid holds.
 *
 * @param grid The grid.
 * @return Its columns times its rows.
 */
export function areaCellCount(grid: Grid): number {
  return grid.cols * grid.rows;
}

/**
 * How many cells a grid holds, across all its areas.
 *
 * @param grid The grid.
 * @return Its columns times its rows times its areas.
 */
export function cellCount(grid: Grid): number {
  return areaCellCount(grid) * grid.areas;
}

/** The Stardate layout's four panels, in the order the watch numbers them. */
export const SLOT_LT = 0;
export const SLOT_LB = 1;
export const SLOT_RT = 2;
export const SLOT_RB = 3;
export const SLOT_COUNT = 4;

/** The catalogue ids the builder has to reason about by name. Everything else is just a number. */
export const ID_HEART = 0;
export const ID_STEPS = 1;
export const ID_BATTERY = 2;
export const ID_MOON_PCT = 6;
export const ID_SUNRISE = 9;
export const ID_SUNSET = 10;
export const ID_EMPTY = 20;
export const ID_SENSORS = 23;
export const ID_WIDE_DAYLIGHT = 28;
export const ID_WIDE_STEPS = 29;
export const ID_FULL_HEART = 30;
export const ID_FULL_STEPS = 31;
export const ID_WIDE_HEART = 32;
export const ID_WIDE_MUSIC = 33;
export const ID_FULL_MUSIC = 34;
export const ID_FULL_AGENDA = 35;
export const ID_WIDE_EVENT = 36;
export const ID_WIDE_HOUR_PROGRESS = 41;
export const ID_WIDE_DAY_PROGRESS = 42;
export const ID_WIDE_WEEK_PROGRESS = 43;
export const ID_WIDE_YEAR_PROGRESS = 44;
export const ID_FULL_FORECAST_HOURLY = 52;
export const ID_FULL_FORECAST_DAILY = 53;
export const ID_WIDE_UV = 54;
export const ID_WIDE_MOON = 55;
export const ID_WIDE_ALARM = 56;
export const ID_WIDE_BEATS = 57;
export const ID_WIDE_STEP_GOAL = 58;
export const ID_WIDE_DISTANCE_GOAL = 59;
export const ID_WIDE_CALORIE_GOAL = 60;
export const ID_WIDE_ACTIVE_GOAL = 61;
export const ID_WIDE_SLEEP_GOAL = 62;
export const ID_WIDE_SUN_NEXT = 63;
export const ID_TALL_STEP_GOAL = 64;
export const ID_TALL_DISTANCE_GOAL = 65;
export const ID_TALL_CALORIE_GOAL = 66;
export const ID_TALL_ACTIVE_GOAL = 67;
export const ID_TALL_SLEEP_GOAL = 68;
export const ID_TALL_HEART = 69;
export const ID_TALL_STEPS_HOURS = 70;
export const ID_TALL_HEART_HOURS = 71;
export const ID_TALL_BATTERY = 72;
export const ID_TALL_ALARM = 73;
export const ID_TALL_BEATS = 74;
export const ID_TALL_SUN_NEXT = 75;
export const ID_TALL_DAYLIGHT = 76;
export const ID_TALL_MOON = 77;
export const ID_TALL_UV = 78;
export const ID_TALL_HOUR_PROGRESS = 79;
export const ID_TALL_DAY_PROGRESS = 80;
export const ID_TALL_WEEK_PROGRESS = 81;
export const ID_TALL_YEAR_PROGRESS = 82;
export const ID_TALL_EVENT = 83;
export const ID_CLOCK = 84;
export const ID_STARDATE = 85;
export const ID_CLOCK_COMPACT = 86;
export const ID_STARDATE_COMPACT = 87;

/** How many readouts the catalogue has, the watch's OPS_COUNT. A pick at or past it reads as empty. */
export const ID_COUNT = 89;

/**
 * The clock and the date, each in its two sizes. A group goes on the face once at most, so a compact
 * clock and a full one are the same clock, and all four only go where the grid places the clock.
 */
export const ONCE_GROUPS = [[ID_CLOCK, ID_CLOCK_COMPACT], [ID_STARDATE, ID_STARDATE_COMPACT]];

/** Every panel in ONCE_GROUPS. */
export const UNIQUE_IDS = ONCE_GROUPS.reduce(function (all, group) {
  return all.concat(group);
}, [] as number[]);

/**
 * Which of the clock or the date a readout is, in either size.
 *
 * @param id The readout.
 * @return Its index in ONCE_GROUPS, or -1 for any other readout.
 */
export function onceGroup(id: number): number {
  for (let group = 0; group < ONCE_GROUPS.length; group++) {
    if (ONCE_GROUPS[group].indexOf(id) >= 0) {
      return group;
    }
  }

  return -1;
}

/** How much of the grid a readout fills. */
export const SHAPE_SINGLE = 'single';
export const SHAPE_TALL = 'tall';
export const SHAPE_WIDE = 'wide';
export const SHAPE_FULL = 'full';
export type Shape = typeof SHAPE_SINGLE | typeof SHAPE_TALL | typeof SHAPE_WIDE | typeof SHAPE_FULL;

/**
 * The readouts bigger than one cell. A readout missing here fills one.
 *
 * It is not a flag on the option list because the watch decides the same thing from its own
 * catalogue, and two sources would be one too many.
 */
const SHAPES: Record<number, Shape> = {
  [ID_SENSORS]: SHAPE_TALL,
  [ID_WIDE_DAYLIGHT]: SHAPE_WIDE,
  [ID_WIDE_STEPS]: SHAPE_WIDE,
  [ID_WIDE_HEART]: SHAPE_WIDE,
  [ID_WIDE_MUSIC]: SHAPE_WIDE,
  [ID_WIDE_EVENT]: SHAPE_WIDE,
  [ID_WIDE_HOUR_PROGRESS]: SHAPE_WIDE,
  [ID_WIDE_DAY_PROGRESS]: SHAPE_WIDE,
  [ID_WIDE_WEEK_PROGRESS]: SHAPE_WIDE,
  [ID_WIDE_YEAR_PROGRESS]: SHAPE_WIDE,
  [ID_FULL_MUSIC]: SHAPE_FULL,
  [ID_FULL_AGENDA]: SHAPE_FULL,
  [ID_FULL_HEART]: SHAPE_FULL,
  [ID_FULL_STEPS]: SHAPE_FULL,
  [ID_WIDE_UV]: SHAPE_WIDE,
  [ID_WIDE_MOON]: SHAPE_WIDE,
  [ID_WIDE_ALARM]: SHAPE_WIDE,
  [ID_WIDE_BEATS]: SHAPE_WIDE,
  [ID_WIDE_STEP_GOAL]: SHAPE_WIDE,
  [ID_WIDE_DISTANCE_GOAL]: SHAPE_WIDE,
  [ID_WIDE_CALORIE_GOAL]: SHAPE_WIDE,
  [ID_WIDE_ACTIVE_GOAL]: SHAPE_WIDE,
  [ID_WIDE_SLEEP_GOAL]: SHAPE_WIDE,
  [ID_WIDE_SUN_NEXT]: SHAPE_WIDE,
  [ID_FULL_FORECAST_HOURLY]: SHAPE_FULL,
  [ID_FULL_FORECAST_DAILY]: SHAPE_FULL,
  [ID_TALL_STEP_GOAL]: SHAPE_TALL,
  [ID_TALL_DISTANCE_GOAL]: SHAPE_TALL,
  [ID_TALL_CALORIE_GOAL]: SHAPE_TALL,
  [ID_TALL_ACTIVE_GOAL]: SHAPE_TALL,
  [ID_TALL_SLEEP_GOAL]: SHAPE_TALL,
  [ID_TALL_HEART]: SHAPE_TALL,
  [ID_TALL_STEPS_HOURS]: SHAPE_TALL,
  [ID_TALL_HEART_HOURS]: SHAPE_TALL,
  [ID_TALL_BATTERY]: SHAPE_TALL,
  [ID_TALL_ALARM]: SHAPE_TALL,
  [ID_TALL_BEATS]: SHAPE_TALL,
  [ID_TALL_SUN_NEXT]: SHAPE_TALL,
  [ID_TALL_DAYLIGHT]: SHAPE_TALL,
  [ID_TALL_MOON]: SHAPE_TALL,
  [ID_TALL_UV]: SHAPE_TALL,
  [ID_TALL_HOUR_PROGRESS]: SHAPE_TALL,
  [ID_TALL_DAY_PROGRESS]: SHAPE_TALL,
  [ID_TALL_WEEK_PROGRESS]: SHAPE_TALL,
  [ID_TALL_YEAR_PROGRESS]: SHAPE_TALL,
  [ID_TALL_EVENT]: SHAPE_TALL,
  [ID_CLOCK]: SHAPE_FULL,
  [ID_STARDATE]: SHAPE_FULL,
  [ID_CLOCK_COMPACT]: SHAPE_WIDE,
  [ID_STARDATE_COMPACT]: SHAPE_WIDE,
};

/** A slot's message key, in slot order. The first is the component's own. */
export const SLOT_KEYS = [
  'APPEARANCE_SLOT_LEFT_TOP',
  'APPEARANCE_SLOT_LEFT_BOTTOM',
  'APPEARANCE_SLOT_RIGHT_TOP',
  'APPEARANCE_SLOT_RIGHT_BOTTOM',
];

/** The hidden-store class each of the other three Stardate slots is found by. */
export const SLOT_STORE_CLASS = [null, 'sb-lb', 'sb-rt', 'sb-rb'];

/** The hidden-store class Tactical's twelve picks are found by, all in one text value. */
export const TACTICAL_STORE_CLASS = 'sb-tac';

/** The hidden-store class Engineering's six picks are found by, all in one text value. */
export const ENGINEERING_STORE_CLASS = 'sb-eng';

/** The hidden-store class Transporter's eight picks are found by, all in one text value. */
export const TRANSPORTER_STORE_CLASS = 'sb-trn';

/** The layouts that save their picks as one list, each with the hidden-store class its list rides in. */
export const LIST_STORES: Record<number, string> = {
  [LAYOUT_TACTICAL]: TACTICAL_STORE_CLASS,
  [LAYOUT_ENGINEERING]: ENGINEERING_STORE_CLASS,
  [LAYOUT_TRANSPORTER]: TRANSPORTER_STORE_CLASS,
};

/** The layouts in LIST_STORES, in order. */
export const LIST_LAYOUTS = [LAYOUT_TACTICAL, LAYOUT_ENGINEERING, LAYOUT_TRANSPORTER];

/** The area a cell sits in, 0 for the top. */
export function areaOf(slot: number, grid: Grid): number {
  return Math.floor(slot / areaCellCount(grid));
}

/** The column a cell sits in within its area, 0 for the left. */
export function colOf(slot: number, grid: Grid): number {
  return Math.floor((slot % areaCellCount(grid)) / grid.rows);
}

/** The row a cell sits in within its area, 0 for the top. */
export function rowOf(slot: number, grid: Grid): number {
  return (slot % areaCellCount(grid)) % grid.rows;
}

/** The cell at a column and row of an area, or -1 off the area. */
export function slotAt(col: number, row: number, grid: Grid, area = 0): number {
  if (col < 0 || col >= grid.cols || row < 0 || row >= grid.rows || area < 0 || area >= grid.areas) {
    return -1;
  }

  return area * areaCellCount(grid) + col * grid.rows + row;
}

/** How much of the grid a readout fills. */
export function shapeOf(id: number): Shape {
  return SHAPES[id] || SHAPE_SINGLE;
}

/**
 * Whether a readout fits a grid at all, so the palette can dim one that cannot go anywhere on it,
 * such as a wide panel on Engineering's single column.
 *
 * @param id The readout.
 * @param grid The layout's grid.
 * @return True when the grid has the columns and rows its shape spans.
 */
export function fitsGrid(id: number, grid: Grid): boolean {
  const span = spanOf(shapeOf(id));

  return span.cols <= grid.cols && span.rows <= grid.rows;
}

/** How many columns and rows a shape spans. */
export function spanOf(shape: Shape): { cols: number; rows: number } {
  return {
    cols: shape === SHAPE_WIDE || shape === SHAPE_FULL ? 2 : 1,
    rows: shape === SHAPE_TALL || shape === SHAPE_FULL ? 2 : 1,
  };
}

/**
 * The cells a readout fills when it starts from a cell, or null when it does not fit there.
 *
 * The first cell is always the one it starts from. A big readout only starts from a cell where the
 * watch draws it: a tall panel from the top of either column, a full panel from the left column,
 * and a wide panel from the left of its row, all within one area. The clock and the date only go
 * where the grid places them. The firmware makes the same check, so a hand-edited setting cannot
 * put one anywhere else.
 *
 * @param id The readout.
 * @param slot The cell it starts from.
 * @param grid The grid.
 * @return The cells it fills, or null.
 */
export function cellsFor(id: number, slot: number, grid: Grid): number[] | null {
  // the clock and the date are panels only where the grid places them
  if (!grid.placesClock && UNIQUE_IDS.indexOf(id) >= 0) {
    return null;
  }

  const span = spanOf(shapeOf(id));
  const area = areaOf(slot, grid);
  const col0 = colOf(slot, grid);
  const row0 = rowOf(slot, grid);
  const cells = [];

  for (let col = 0; col < span.cols; col++) {
    for (let row = 0; row < span.rows; row++) {
      const cell = slotAt(col0 + col, row0 + row, grid, area);

      if (cell < 0) {
        return null;
      }

      cells.push(cell);
    }
  }

  return cells.sort((a, b) => a - b);
}

/** Whether a readout may start from a cell. */
export function canPlace(id: number, slot: number, grid: Grid): boolean {
  return cellsFor(id, slot, grid) !== null;
}

/**
 * Which cell fills each cell: the cell a readout starts from, or -1 for a cell nothing fills.
 *
 * Read in cell order, so when two readouts claim the same cell the earlier one keeps it. A big
 * readout only ever covers cells after the one it starts from, so that order is also the order
 * the watch draws them in.
 *
 * @param slots The values, one a cell.
 * @param grid The grid.
 * @return The owning cell of each cell.
 */
export function owners(slots: number[], grid: Grid): number[] {
  const count = cellCount(grid);
  const owner: number[] = [];

  for (let slot = 0; slot < count; slot++) {
    owner.push(-1);
  }

  for (let slot = 0; slot < count; slot++) {
    const id = slots[slot];

    if (id === ID_EMPTY || owner[slot] >= 0) {
      continue;
    }

    const cells = cellsFor(id, slot, grid);

    if (!cells || cells.some((cell) => owner[cell] >= 0)) {
      continue;
    }

    cells.forEach((cell) => {
      owner[cell] = slot;
    });
  }

  return owner;
}
