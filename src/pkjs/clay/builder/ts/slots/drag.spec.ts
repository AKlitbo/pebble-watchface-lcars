/**
 * Specs for what a panel drag is allowed to do.
 *
 * The pointer handling lives in the framework and jsdom cannot exercise it, since every
 * rect is zeros and no drop would ever hit a panel. What is this face's own is
 * tryPlace, which decides the whole arrangement rather than just the panel
 * under the pointer, because a swap can push a big panel somewhere it does
 * not fit. Every case below is one of those knock-on moves.
 */

import { describe, test, expect } from 'vitest';
import { tryPlace } from './drag';
import {
  STARDATE_GRID,
  SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RB,
  ID_HEART, ID_STEPS, ID_EMPTY, ID_SENSORS, ID_WIDE_DAYLIGHT, ID_FULL_HEART,
  ID_TALL_HEART, ID_TALL_BATTERY,
} from './geometry';

/** Dragged straight off the palette rather than out of a panel. */
const FROM_PALETTE = -1;

// what the face ships as: the tall block over the left column, heart and steps down the right
const SHIPPED = [ID_SENSORS, ID_EMPTY, ID_HEART, ID_STEPS];

describe('tryPlace', () => {
  /** A palette drop is the ordinary case, and refusing it would make the builder look broken on first use. */
  test('drops a palette pick into the panel under the pointer', () => {
    const result = tryPlace(SHIPPED, ID_STEPS, FROM_PALETTE, SLOT_RT, STARDATE_GRID);

    expect(result).toEqual([ID_SENSORS, ID_EMPTY, ID_STEPS, ID_STEPS]);
  });

  /** Dragging between panels trades them, and writing only the target would leave the dragged readout in both places. */
  test('swaps the two panels when a pick is dragged from one to another', () => {
    const result = tryPlace(SHIPPED, ID_HEART, SLOT_RT, SLOT_RB, STARDATE_GRID);

    expect(result).toEqual([ID_SENSORS, ID_EMPTY, ID_STEPS, ID_HEART]);
  });

  /** The tall block fills its column, so whatever sat below it has to go rather than be pushed aside. */
  test('clears the panel below when the tall block lands upper left', () => {
    const slots = [ID_EMPTY, ID_HEART, ID_STEPS, ID_EMPTY];

    const result = tryPlace(slots, ID_SENSORS, FROM_PALETTE, SLOT_LT, STARDATE_GRID);

    expect(result).toEqual([ID_SENSORS, ID_EMPTY, ID_STEPS, ID_EMPTY]);
  });

  /** A tall block started on the bottom row would run off the screen, so a drop there has to be refused rather than shown and then corrected. */
  test('refuses the tall block on the bottom row', () => {
    const slots = [ID_EMPTY, ID_HEART, ID_STEPS, ID_EMPTY];

    const result = [
      tryPlace(slots, ID_SENSORS, FROM_PALETTE, SLOT_LB, STARDATE_GRID),
      tryPlace(slots, ID_SENSORS, FROM_PALETTE, SLOT_RB, STARDATE_GRID),
    ];

    expect(result).toEqual([null, null]);
  });

  /**
   * A swap into the upper left would push the tall block onto the steps still sitting in the right
   * column.
   *
   * This is why the whole arrangement is checked rather than the target panel alone. Looking only
   * at where the heart lands says yes, and the move quietly drops the steps under the sensors block.
   */
  test('refuses a swap that would push the tall block onto a panel it cannot trade with', () => {
    const result = tryPlace(SHIPPED, ID_HEART, SLOT_RT, SLOT_LT, STARDATE_GRID);

    expect(result).toBeNull();
  });

  /** A tall block started on the bottom row would run off the screen, so dragging it down its own column is refused. */
  test('refuses moving the tall block into the panel below it', () => {
    const result = tryPlace(SHIPPED, ID_SENSORS, SLOT_LT, SLOT_LB, STARDATE_GRID);

    expect(result).toBeNull();
  });

  /** A block off the palette goes in the right column too, and takes both of its panels. */
  test('drops a block into the right column', () => {
    const result = tryPlace(SHIPPED, ID_TALL_BATTERY, FROM_PALETTE, SLOT_RT, STARDATE_GRID);

    expect(result).toEqual([ID_SENSORS, ID_EMPTY, ID_TALL_BATTERY, ID_EMPTY]);
  });

  /** Dragging a block across trades whole columns, so the column it lands on moves over rather than vanishing. */
  test('trades two columns when a block is dragged across', () => {
    const slots = [ID_TALL_HEART, ID_EMPTY, ID_HEART, ID_STEPS];

    const result = tryPlace(slots, ID_TALL_HEART, SLOT_LT, SLOT_RT, STARDATE_GRID);

    expect(result).toEqual([ID_HEART, ID_STEPS, ID_TALL_HEART, ID_EMPTY]);
  });

  /** Two blocks side by side swap places as whole columns, with neither one refused on the right. */
  test('swaps two blocks side by side', () => {
    const slots = [ID_TALL_HEART, ID_EMPTY, ID_TALL_BATTERY, ID_EMPTY];

    const result = tryPlace(slots, ID_TALL_BATTERY, SLOT_RT, SLOT_LT, STARDATE_GRID);

    expect(result).toEqual([ID_TALL_BATTERY, ID_EMPTY, ID_TALL_HEART, ID_EMPTY]);
  });

  /** The sensors block goes in either column like the other blocks, so a swap carries it right rather than being refused. */
  test('swaps the sensors block into the right column', () => {
    const slots = [ID_SENSORS, ID_EMPTY, ID_TALL_BATTERY, ID_EMPTY];

    const result = tryPlace(slots, ID_TALL_BATTERY, SLOT_RT, SLOT_LT, STARDATE_GRID);

    expect(result).toEqual([ID_TALL_BATTERY, ID_EMPTY, ID_SENSORS, ID_EMPTY]);
  });

  /** A sensors block off the palette lands on the right too, and takes both of that column's panels. */
  test('drops the sensors block into the right column', () => {
    const slots = [ID_TALL_HEART, ID_EMPTY, ID_HEART, ID_STEPS];

    const result = tryPlace(slots, ID_SENSORS, FROM_PALETTE, SLOT_RT, STARDATE_GRID);

    expect(result).toEqual([ID_TALL_HEART, ID_EMPTY, ID_SENSORS, ID_EMPTY]);
  });

  /** A pick let go on the panel it came from must land back as it was, not swap the panel with itself into something else. */
  test('leaves the arrangement alone when a pick is dropped on its own panel', () => {
    const result = tryPlace(SHIPPED, ID_HEART, SLOT_RT, SLOT_RT, STARDATE_GRID);

    expect(result).toEqual(SHIPPED);
  });

  /** A wide panel off the palette takes its whole row, so both readouts it lands on go rather than one hiding under it. */
  test('clears both panels of the row a wide panel lands on', () => {
    const result = tryPlace(SHIPPED, ID_WIDE_DAYLIGHT, FROM_PALETTE, SLOT_LB, STARDATE_GRID);

    expect(result).toEqual([ID_EMPTY, ID_WIDE_DAYLIGHT, ID_HEART, ID_EMPTY]);
  });

  /** A single readout dropped on a wide panel replaces all of it, or the other half of the panel would stay behind as a stray bar. */
  test('replaces a whole wide panel when a single lands on it', () => {
    const slots = [ID_WIDE_DAYLIGHT, ID_HEART, ID_EMPTY, ID_STEPS];

    const result = tryPlace(slots, ID_STEPS, FROM_PALETTE, SLOT_LT, STARDATE_GRID);

    expect(result).toEqual([ID_STEPS, ID_HEART, ID_EMPTY, ID_STEPS]);
  });

  /** The watch cannot draw a wide panel in the right column, so the drop is refused rather than shown and then corrected. */
  test('refuses a wide panel in the right column', () => {
    const result = tryPlace(SHIPPED, ID_WIDE_DAYLIGHT, FROM_PALETTE, SLOT_RT, STARDATE_GRID);

    expect(result).toBeNull();
  });

  /** Dragging a wide panel down trades it with the row below, so both of that row's readouts move up rather than vanish. */
  test('trades a wide panel with the row it is dragged onto', () => {
    const slots = [ID_WIDE_DAYLIGHT, ID_HEART, ID_EMPTY, ID_STEPS];

    const result = tryPlace(slots, ID_WIDE_DAYLIGHT, SLOT_LT, SLOT_LB, STARDATE_GRID);

    expect(result).toEqual([ID_HEART, ID_WIDE_DAYLIGHT, ID_STEPS, ID_EMPTY]);
  });

  /** Trading a single with a wide panel would push the panel into the right column, so the move is refused rather than losing it. */
  test('refuses a trade that would push a wide panel off the grid', () => {
    const slots = [ID_WIDE_DAYLIGHT, ID_HEART, ID_EMPTY, ID_STEPS];

    const result = tryPlace(slots, ID_STEPS, SLOT_RB, SLOT_LT, STARDATE_GRID);

    expect(result).toBeNull();
  });

  /** A full panel off the palette fills the grid, so every readout under it goes. */
  test('clears every panel when a full panel lands', () => {
    const result = tryPlace(SHIPPED, ID_FULL_HEART, FROM_PALETTE, SLOT_LT, STARDATE_GRID);

    expect(result).toEqual([ID_FULL_HEART, ID_EMPTY, ID_EMPTY, ID_EMPTY]);
  });

  /** A full panel only fits from the upper left, so a drop anywhere else is refused. */
  test('refuses a full panel anywhere but the upper left', () => {
    const result = [SLOT_LB, SLOT_RT, SLOT_RB].map((slot) => tryPlace(SHIPPED, ID_FULL_HEART, FROM_PALETTE, slot, STARDATE_GRID));

    expect(result).toEqual([null, null, null]);
  });

  /** The arrangement handed in is the live one, so mutating it would move panels the user only dragged over. */
  test('does not touch the arrangement it was given', () => {
    const slots = SHIPPED.slice();

    tryPlace(slots, ID_STEPS, FROM_PALETTE, SLOT_RT, STARDATE_GRID);

    expect(slots).toEqual(SHIPPED);
  });
});
