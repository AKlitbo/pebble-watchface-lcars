// @vitest-environment jsdom
/**
 * Specs for Tactical's twelve cells.
 *
 * Tactical has three areas of four cells, the clock and the date are panels among them, and its
 * picks travel as one comma list the watch reads a cell at a time. The rules worth pinning are the
 * ones the watch makes from its own copy: a panel never runs from one area into the next, the clock
 * and the date go on the face once each and only on this layout, and a list the watch would read
 * differently is read the same way here. A drop moving the clock or the date is pinned too, since
 * keeping the old copy would make the drop look like it did nothing.
 */

import { describe, test, expect, beforeEach } from 'vitest';
import { formatTactical, moveUnique, parseTactical, readTactical, sanitize, writeTactical } from './codec';
import { tryPlace } from './drag';
import { TACTICAL_PRESETS } from './presets';
import {
  TACTICAL_GRID, TACTICAL_STORE_CLASS, STARDATE_GRID,
  ID_CLOCK, ID_EMPTY, ID_FULL_HEART, ID_HEART, ID_SENSORS, ID_STARDATE, ID_STEPS, ID_TALL_HEART, ID_WIDE_DAYLIGHT,
  cellsFor, slotAt,
} from './geometry';

const E = ID_EMPTY;

/** The arrangement Tactical ships with: the clock on top, the Stardate layout's panels in the middle, and the date at the bottom. */
const SHIPPED = [ID_CLOCK, E, E, E, ID_SENSORS, E, ID_HEART, ID_STEPS, ID_STARDATE, E, E, E];

beforeEach(() => {
  document.body.innerHTML = '';
});

describe('Tactical geometry', () => {
  /** The watch numbers the areas from the top, so a cell numbered any other way would show the pick in a different area. */
  test('numbers each area after the one above it', () => {
    const result = [slotAt(0, 0, TACTICAL_GRID, 0), slotAt(0, 0, TACTICAL_GRID, 1), slotAt(1, 1, TACTICAL_GRID, 2)];

    expect(result).toEqual([0, 4, 11]);
  });

  /** A tall panel at the foot of one area would run through the frame's strip into the next, which the watch never draws. */
  test('keeps a tall panel inside its area', () => {
    const result = [cellsFor(ID_TALL_HEART, 4, TACTICAL_GRID), cellsFor(ID_TALL_HEART, 5, TACTICAL_GRID)];

    expect(result).toEqual([[4, 5], null]);
  });

  /** The clock and the date have fixed places on the Stardate layout, so as panels there they would draw a second clock. */
  test('refuses the clock and the date on the Stardate layout', () => {
    const result = [cellsFor(ID_CLOCK, 0, STARDATE_GRID), cellsFor(ID_STARDATE, 0, STARDATE_GRID)];

    expect(result).toEqual([null, null]);
  });

  /** Each area takes a full panel from its own upper left, so the clock can go top, middle, or bottom. */
  test('places the clock at the start of any area', () => {
    const result = [0, 4, 8].map((cell) => cellsFor(ID_CLOCK, cell, TACTICAL_GRID));

    expect(result).toEqual([[0, 1, 2, 3], [4, 5, 6, 7], [8, 9, 10, 11]]);
  });
});

describe('the Tactical list', () => {
  /** The watch reads the saved text a cell at a time, so the builder has to write exactly what it parses. */
  test('reads back what it writes', () => {
    const result = parseTactical(formatTactical(SHIPPED));

    expect(result).toEqual(SHIPPED);
  });

  /** The watch stops at the first thing that is not a number and leaves the rest empty, so the builder has to stop there too. */
  test('empties every cell after a broken entry', () => {
    const result = parseTactical('84,20,20,20,x,20,0,1,85');

    expect(result).toEqual([ID_CLOCK, E, E, E, E, E, E, E, E, E, E, E]);
  });

  /** A fresh page has no saved list, and the builder must show empty cells rather than reading off the end. */
  test('reads an empty list as twelve empty cells', () => {
    const result = parseTactical('');

    expect(result).toEqual(new Array(12).fill(E));
  });

  /** The twelve picks ride one hidden store, so a store written without its change event never reaches Clay. */
  test('writes the list to its store with a change event', () => {
    const store = document.createElement('input');
    const root = document.createElement('div');
    let changes = 0;

    store.className = 'pf-hidden-store ' + TACTICAL_STORE_CLASS;
    store.addEventListener('change', () => { changes++; });
    document.body.append(store, root);

    writeTactical(root, SHIPPED);
    const result = { value: store.value, changes: changes, read: readTactical(root) };

    expect(result).toEqual({ value: '84,20,20,20,23,20,0,1,85,20,20,20', changes: 1, read: SHIPPED });
  });
});

describe('the clock and the date', () => {
  /** A hand-edited list can hold the clock twice, and the watch draws the first copy only, so the builder must too. */
  test('keeps only the first copy of the clock', () => {
    const result = sanitize([ID_CLOCK, E, E, E, ID_CLOCK, E, E, E, E, E, E, E], TACTICAL_GRID);

    expect(result).toEqual([ID_CLOCK, E, E, E, E, E, E, E, E, E, E, E]);
  });

  /** A first clock a full panel covers is not drawn, so the next copy has to stand rather than leaving the face with no clock. */
  test('keeps a later clock when the first one is covered', () => {
    const result = sanitize([ID_FULL_HEART, ID_CLOCK, E, E, ID_CLOCK, E, E, E, E, E, E, E], TACTICAL_GRID);

    expect(result).toEqual([ID_FULL_HEART, E, E, E, ID_CLOCK, E, E, E, E, E, E, E]);
  });

  /** A Stardate slot saved as the clock would read as a labelled panel with nothing in it, so it is cleared like the watch clears it. */
  test('clears the clock from the Stardate layout', () => {
    const result = sanitize([ID_CLOCK, E, E, E], STARDATE_GRID);

    expect(result).toEqual([E, E, E, E]);
  });

  /** Dropping the clock in the bottom area has to move it there, or the drop would look like it did nothing. */
  test('moves the clock to where it was dropped', () => {
    const before = [ID_CLOCK, E, E, E, ID_HEART, E, E, E, E, E, E, E];
    const after = [ID_CLOCK, E, E, E, ID_HEART, E, E, E, ID_CLOCK, E, E, E];

    const result = moveUnique(before, after);

    expect(result).toEqual([E, E, E, E, ID_HEART, E, E, E, ID_CLOCK, E, E, E]);
  });

  /** A drop that leaves the clock where it was must not clear it. */
  test('leaves a clock that did not move', () => {
    const result = moveUnique(SHIPPED, SHIPPED);

    expect(result).toEqual(SHIPPED);
  });
});

describe('trading between areas', () => {
  /** Swapping the clock with the date has to land each in the other's area, not drop both into the top one. */
  test('swaps the clock and the date across areas', () => {
    const result = tryPlace(SHIPPED, ID_CLOCK, 0, 8, TACTICAL_GRID);

    expect(result).toEqual([ID_STARDATE, E, E, E, ID_SENSORS, E, ID_HEART, ID_STEPS, ID_CLOCK, E, E, E]);
  });

  /** A wide panel traded with a full one in another area has to fit where the trade sends it, or the whole move is refused. */
  test('refuses a trade that sends a panel where it cannot fit', () => {
    const slots = [ID_FULL_HEART, E, E, E, E, ID_WIDE_DAYLIGHT, E, E, E, E, E, E];

    const result = tryPlace(slots, ID_WIDE_DAYLIGHT, 5, 0, TACTICAL_GRID);

    expect(result).toBeNull();
  });
});

describe('TACTICAL_PRESETS', () => {
  /** The Default button on Tactical puts back this list, so one the rules correct would not be the face the button promised. */
  test('ships a default that comes back from sanitize unchanged', () => {
    const result = sanitize(TACTICAL_PRESETS.default, TACTICAL_GRID);

    expect(result).toEqual(SHIPPED);
  });
});
