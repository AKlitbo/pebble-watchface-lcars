/**
 * Specs for the slot placement rules.
 *
 * The panels are a grid two columns wide and two rows tall, and a readout fills one cell, two cells
 * of a column, a row, or the whole grid. The rules worth pinning are where each shape may start,
 * which cells it then fills, and who owns a cell when two readouts claim it, since the watch makes
 * the same calls from its own copy of the rules and the two must agree. The rest is the lookup
 * tables the builder indexes by slot, where a table one entry short fails silently rather than
 * throwing.
 */

import { describe, test, expect } from 'vitest';
import {
  canPlace, cellsFor, owners, shapeOf, slotAt, GRID,
  SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RB, SLOT_COUNT,
  SLOT_KEYS, SLOT_STORE_CLASS,
  ID_HEART, ID_STEPS, ID_EMPTY, ID_SENSORS,
  ID_WIDE_DAYLIGHT, ID_FULL_HEART,
  ID_WIDE_HOUR_PROGRESS, ID_WIDE_DAY_PROGRESS, ID_WIDE_WEEK_PROGRESS, ID_WIDE_YEAR_PROGRESS,
  ID_FULL_FORECAST_HOURLY, ID_FULL_FORECAST_DAILY,
  ID_WIDE_UV, ID_WIDE_MOON, ID_WIDE_ALARM, ID_WIDE_BEATS,
  ID_WIDE_STEP_GOAL, ID_WIDE_DISTANCE_GOAL, ID_WIDE_CALORIE_GOAL, ID_WIDE_ACTIVE_GOAL, ID_WIDE_SLEEP_GOAL,
  ID_WIDE_SUN_NEXT,
  ID_TALL_STEP_GOAL, ID_TALL_DISTANCE_GOAL, ID_TALL_CALORIE_GOAL, ID_TALL_ACTIVE_GOAL, ID_TALL_SLEEP_GOAL,
  ID_TALL_HEART, ID_TALL_STEPS_HOURS, ID_TALL_HEART_HOURS, ID_TALL_BATTERY, ID_TALL_ALARM, ID_TALL_BEATS,
  ID_TALL_SUN_NEXT, ID_TALL_DAYLIGHT, ID_TALL_MOON, ID_TALL_UV,
  ID_TALL_HOUR_PROGRESS, ID_TALL_DAY_PROGRESS, ID_TALL_WEEK_PROGRESS, ID_TALL_YEAR_PROGRESS, ID_TALL_EVENT,
  SHAPE_SINGLE, SHAPE_TALL, SHAPE_WIDE, SHAPE_FULL,
} from './geometry';

// every block the watch's catalogue draws down a column, by its OpsId there
const TALL_BLOCKS: Array<[string, number]> = [
  ['the sensors block', ID_SENSORS],
  ['the step goal block', ID_TALL_STEP_GOAL],
  ['the distance goal block', ID_TALL_DISTANCE_GOAL],
  ['the calorie goal block', ID_TALL_CALORIE_GOAL],
  ['the active minutes goal block', ID_TALL_ACTIVE_GOAL],
  ['the sleep goal block', ID_TALL_SLEEP_GOAL],
  ['the heart rate block', ID_TALL_HEART],
  ['the steps by the hour block', ID_TALL_STEPS_HOURS],
  ['the heart rate by the hour block', ID_TALL_HEART_HOURS],
  ['the battery block', ID_TALL_BATTERY],
  ['the alarm block', ID_TALL_ALARM],
  ['the beats block', ID_TALL_BEATS],
  ['the sun countdown block', ID_TALL_SUN_NEXT],
  ['the daylight block', ID_TALL_DAYLIGHT],
  ['the moon block', ID_TALL_MOON],
  ['the uv block', ID_TALL_UV],
  ['the hour progress block', ID_TALL_HOUR_PROGRESS],
  ['the day progress block', ID_TALL_DAY_PROGRESS],
  ['the week progress block', ID_TALL_WEEK_PROGRESS],
  ['the year progress block', ID_TALL_YEAR_PROGRESS],
  ['the next event block', ID_TALL_EVENT],
];

describe('slotAt', () => {
  /** The slots are numbered down each column, and reading them across instead would send a trade to the wrong cell. */
  test('numbers the slots down each column', () => {
    const result = [slotAt(0, 0, GRID), slotAt(0, 1, GRID), slotAt(1, 0, GRID), slotAt(1, 1, GRID)];

    expect(result).toEqual([SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RB]);
  });

  /** A trade that steps off the grid has to come back as nowhere, not wrap round to a cell on the other side. */
  test('answers -1 off the grid', () => {
    const result = [slotAt(-1, 0, GRID), slotAt(2, 0, GRID), slotAt(0, -1, GRID), slotAt(0, 2, GRID)];

    expect(result).toEqual([-1, -1, -1, -1]);
  });
});

describe('shapeOf', () => {
  /** Treating an ordinary readout as big would blank the panels beside it. */
  test('reports each readout at its own size', () => {
    const result = [shapeOf(ID_HEART), shapeOf(ID_SENSORS), shapeOf(ID_WIDE_DAYLIGHT), shapeOf(ID_FULL_HEART)];

    expect(result).toEqual([SHAPE_SINGLE, SHAPE_TALL, SHAPE_WIDE, SHAPE_FULL]);
  });
});

describe('cellsFor', () => {
  /** A big readout fills the cells after the one it starts from, and naming the wrong ones clears a panel the user kept. */
  test('fills the column, the row, or the grid from the upper left', () => {
    const result = [cellsFor(ID_SENSORS, SLOT_LT, GRID), cellsFor(ID_WIDE_DAYLIGHT, SLOT_LT, GRID), cellsFor(ID_FULL_HEART, SLOT_LT, GRID)];

    expect(result).toEqual([[SLOT_LT, SLOT_LB], [SLOT_LT, SLOT_RT], [SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RB]]);
  });

  /**
   * The watch's catalogue makes these bars and forecasts a row or the whole grid. Missing one here lets
   * the builder drop it in one cell, where the watch reads the save as a different face from the one drawn.
   */
  test.each([
    ['the hour progress bar', ID_WIDE_HOUR_PROGRESS, [SLOT_LT, SLOT_RT]],
    ['the day progress bar', ID_WIDE_DAY_PROGRESS, [SLOT_LT, SLOT_RT]],
    ['the week progress bar', ID_WIDE_WEEK_PROGRESS, [SLOT_LT, SLOT_RT]],
    ['the year progress bar', ID_WIDE_YEAR_PROGRESS, [SLOT_LT, SLOT_RT]],
    ['the uv bar', ID_WIDE_UV, [SLOT_LT, SLOT_RT]],
    ['the moon strip', ID_WIDE_MOON, [SLOT_LT, SLOT_RT]],
    ['the alarm strip', ID_WIDE_ALARM, [SLOT_LT, SLOT_RT]],
    ['the beats bar', ID_WIDE_BEATS, [SLOT_LT, SLOT_RT]],
    ['the step goal bar', ID_WIDE_STEP_GOAL, [SLOT_LT, SLOT_RT]],
    ['the distance goal bar', ID_WIDE_DISTANCE_GOAL, [SLOT_LT, SLOT_RT]],
    ['the calorie goal bar', ID_WIDE_CALORIE_GOAL, [SLOT_LT, SLOT_RT]],
    ['the active minutes goal bar', ID_WIDE_ACTIVE_GOAL, [SLOT_LT, SLOT_RT]],
    ['the sleep goal bar', ID_WIDE_SLEEP_GOAL, [SLOT_LT, SLOT_RT]],
    ['the sun countdown strip', ID_WIDE_SUN_NEXT, [SLOT_LT, SLOT_RT]],
    ['the hourly forecast board', ID_FULL_FORECAST_HOURLY, [SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RB]],
    ['the four day forecast board', ID_FULL_FORECAST_DAILY, [SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RB]],
  ])('fills the cells the watch draws %s across', (_name, id, cells) => {
    const result = cellsFor(id, SLOT_LT, GRID);

    expect(result).toEqual(cells);
  });

  /** A wide panel on the bottom row is a real place the watch draws it, so refusing it would cost half the spots it fits. */
  test('fits a wide panel along the bottom row', () => {
    const result = cellsFor(ID_WIDE_DAYLIGHT, SLOT_LB, GRID);

    expect(result).toEqual([SLOT_LB, SLOT_RB]);
  });

  /** A wide panel started in the right column would run off the screen, so it has to be refused rather than drawn half off. */
  test('refuses a wide panel in the right column', () => {
    const result = [cellsFor(ID_WIDE_DAYLIGHT, SLOT_RT, GRID), cellsFor(ID_WIDE_DAYLIGHT, SLOT_RB, GRID)];

    expect(result).toEqual([null, null]);
  });

  /**
   * The watch paints these blocks under their own bar in either column. Missing one here drops it in
   * one cell, and refusing the right column would turn away a place the watch draws it.
   */
  test.each(TALL_BLOCKS)('fills either column with %s', (_name, id) => {
    const result = [cellsFor(id, SLOT_LT, GRID), cellsFor(id, SLOT_RT, GRID)];

    expect(result).toEqual([[SLOT_LT, SLOT_LB], [SLOT_RT, SLOT_RB]]);
  });

  /** A block started on the bottom row would run off the screen, so it fits from the top of a column only. */
  test('refuses a block on the bottom row', () => {
    const result = [cellsFor(ID_TALL_STEP_GOAL, SLOT_LB, GRID), cellsFor(ID_TALL_STEP_GOAL, SLOT_RB, GRID)];

    expect(result).toEqual([null, null]);
  });

  /** A full panel anywhere but the upper left runs off the 2x2, so it fits one place only. */
  test('fits a full panel in the upper left only', () => {
    const result = [SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RB].map((slot) => canPlace(ID_FULL_HEART, slot, GRID));

    expect(result).toEqual([true, false, false, false]);
  });

  /** An ordinary readout fits any panel, and refusing one would make the builder reject a drop the watch accepts. */
  test('fits an ordinary readout in every slot', () => {
    const result = [SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RB].map((slot) => cellsFor(ID_HEART, slot, GRID));

    expect(result).toEqual([[SLOT_LT], [SLOT_LB], [SLOT_RT], [SLOT_RB]]);
  });

});

describe('owners', () => {
  /** Every cell a wide panel covers belongs to it, so the builder hides those boxes and the watch skips their chrome. */
  test('gives a wide panel both cells of its row', () => {
    const result = owners([ID_WIDE_DAYLIGHT, ID_HEART, ID_EMPTY, ID_STEPS], GRID);

    expect(result).toEqual([SLOT_LT, SLOT_LB, SLOT_LT, SLOT_RB]);
  });

  /** The earlier slot keeps a cell two readouts claim, and the watch reads it the same way, so the two never disagree on what shows. */
  test('lets the earlier slot keep a cell two readouts claim', () => {
    const result = owners([ID_SENSORS, ID_WIDE_DAYLIGHT, ID_HEART, ID_EMPTY], GRID);

    expect(result).toEqual([SLOT_LT, SLOT_LT, SLOT_RT, -1]);
  });

  /** Two blocks side by side each own their own column, so neither hides the other. */
  test('gives two blocks a column each', () => {
    const result = owners([ID_TALL_HEART, ID_EMPTY, ID_TALL_BATTERY, ID_EMPTY], GRID);

    expect(result).toEqual([SLOT_LT, SLOT_LT, SLOT_RT, SLOT_RT]);
  });

  /** The sensors block on the right owns its whole column, so whatever was saved under it is hidden rather than drawn over. */
  test('gives a sensors block on the right its whole column', () => {
    const result = owners([ID_HEART, ID_STEPS, ID_SENSORS, ID_HEART], GRID);

    expect(result).toEqual([SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RT]);
  });

  /** The layout the face ships with, the sensors block on the left, reads as it always has. */
  test('gives the shipped sensors block the left column', () => {
    const result = owners([ID_SENSORS, ID_EMPTY, ID_HEART, ID_STEPS], GRID);

    expect(result).toEqual([SLOT_LT, SLOT_LT, SLOT_RT, SLOT_RB]);
  });

  /** A big pick where it does not fit owns nothing, rather than spilling over cells it was never meant to cover. */
  test('leaves a big pick that does not fit owning nothing', () => {
    const result = owners([ID_HEART, ID_EMPTY, ID_WIDE_DAYLIGHT, ID_STEPS], GRID);

    expect(result).toEqual([SLOT_LT, -1, -1, SLOT_RB]);
  });

});

describe('the slot tables', () => {
  /**
   * A table one entry short is the quiet failure here.
   *
   * readSlots walks to SLOT_COUNT and indexes both tables by slot, and a missing class reads as
   * "no store" rather than as an error, so the slot silently falls back to the component's own
   * value and two panels end up mirroring each other.
   */
  test('carry an entry for every slot', () => {
    const result = [SLOT_KEYS.length, SLOT_STORE_CLASS.length];

    expect(result).toEqual([SLOT_COUNT, SLOT_COUNT]);
  });

  /** The upper left is the component's own value rather than a hidden store, and giving it a class would send it looking for an input that is not on the page. */
  test('leave the upper left without a store class', () => {
    const result = SLOT_STORE_CLASS[SLOT_LT];

    expect(result).toBeNull();
  });

  /** Each store needs its own class to be found by, since every hiddenStore already shares .pf-hidden-store. */
  test('give the other three slots distinct store classes', () => {
    const classes = SLOT_STORE_CLASS.slice(1);

    const result = new Set(classes).size;

    expect(result).toBe(SLOT_COUNT - 1);
    expect(classes).not.toContain(null);
  });
});
