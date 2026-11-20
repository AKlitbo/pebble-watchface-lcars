/**
 * Every fact about the shape of this face's panel layout lives here, so pointing the builder at a
 * different arrangement is a matter of editing this file rather than chasing constants through the
 * drag and render code.
 *
 * The panels sit in a grid two columns wide and two rows tall. The cells are numbered down each
 * column, so a cell's column is its number divided by the rows and its row is what is left over.
 * Most readouts fill one cell. The bigger ones fill two cells of a column, a row, or the whole grid,
 * and each is saved in the cell it starts from, with the cells it covers saved as empty. The watch
 * reads the values back the same way.
 */

/** A grid of panels. */
export interface Grid {
  cols: number;
  rows: number;
}

/** The panels: two columns of two under the clock. */
export const GRID: Grid = { cols: 2, rows: 2 };

/**
 * How many cells a grid holds.
 *
 * @param grid The grid.
 * @return Its columns times its rows.
 */
export function cellCount(grid: Grid): number {
  return grid.cols * grid.rows;
}

/** The four panels, in the order the watch numbers them. */
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
};

/** A slot's message key, in slot order. The first is the component's own. */
export const SLOT_KEYS = [
  'APPEARANCE_SLOT_LEFT_TOP',
  'APPEARANCE_SLOT_LEFT_BOTTOM',
  'APPEARANCE_SLOT_RIGHT_TOP',
  'APPEARANCE_SLOT_RIGHT_BOTTOM',
];

/** The hidden-store class each of the other three slots is found by. */
export const SLOT_STORE_CLASS = [null, 'sb-lb', 'sb-rt', 'sb-rb'];

/** The grid column a cell sits in, 0 for the left. */
export function colOf(slot: number, grid: Grid): number {
  return Math.floor(slot / grid.rows);
}

/** The grid row a cell sits in, 0 for the top. */
export function rowOf(slot: number, grid: Grid): number {
  return slot % grid.rows;
}

/** The cell at a column and row, or -1 off the grid. */
export function slotAt(col: number, row: number, grid: Grid): number {
  if (col < 0 || col >= grid.cols || row < 0 || row >= grid.rows) {
    return -1;
  }

  return col * grid.rows + row;
}

/** How much of the grid a readout fills. */
export function shapeOf(id: number): Shape {
  return SHAPES[id] || SHAPE_SINGLE;
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
 * and a wide panel from the left of its row. The firmware makes the same check, so a hand-edited
 * setting cannot put one anywhere else.
 *
 * @param id The readout.
 * @param slot The cell it starts from.
 * @param grid The grid.
 * @return The cells it fills, or null.
 */
export function cellsFor(id: number, slot: number, grid: Grid): number[] | null {
  const span = spanOf(shapeOf(id));
  const col0 = colOf(slot, grid);
  const row0 = rowOf(slot, grid);
  const cells = [];

  for (let col = 0; col < span.cols; col++) {
    for (let row = 0; row < span.rows; row++) {
      const cell = slotAt(col0 + col, row0 + row, grid);

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
