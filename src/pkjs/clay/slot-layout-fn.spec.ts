/**
 * Specs for the page code that tells the slot builder which layout is picked.
 *
 * The builder cannot read the Layout picker itself, so this is the only thing keeping its grid in
 * step with the picker. Worth pinning are the two moments it has to speak, once the page is built
 * and on every change, and that it waits for the build, since Clay has not listed the rows before
 * then.
 */

import { describe, test, expect } from 'vitest';
import slotLayout from './slot-layout-fn';

/** A Clay page with the Layout picker and the builder on it, recording what the builder is told. */
function fakePage(layout: string, hooked: boolean) {
  const handlers: Record<string, () => void> = {};
  const changes: Array<() => void> = [];
  const told: string[] = [];
  const picker = {
    value: layout,
    $element: [],
    get: function () { return this.value; },
    on: function (event: string, handler: () => void) {
      if (event === 'change') {
        changes.push(handler);
      }
    },
  };
  const host = hooked ? { _sbLayout: (value: string) => { told.push(value); } } : {};
  const builder = { $element: [host], get: () => '23', on: () => {} };
  const rows: Record<string, unknown> = { APPEARANCE_LAYOUT: picker, APPEARANCE_SLOT_LEFT_TOP: builder };
  const page = {
    EVENTS: { AFTER_BUILD: 'AFTER_BUILD' },
    on: (event: string, handler: () => void) => { handlers[event] = handler; },
    getItemByMessageKey: (key: string) => rows[key],
  };

  return {
    page: page,
    told: told,
    build: () => handlers.AFTER_BUILD(),
    pick: (value: string) => {
      picker.value = value;
      changes.forEach((handler) => handler());
    },
  };
}

describe('slotLayout', () => {
  /** Clay has not listed the rows before the build, so speaking any earlier would find no builder and leave it on Stardate. */
  test('waits for the page to be built', () => {
    const fake = fakePage('1', true);

    slotLayout.call(fake.page as never);

    expect(fake.told).toEqual([]);
  });

  /** A returning wearer on Tactical has to see its three grids as the page opens, not Stardate's one grid with eight of their twelve picks hidden. */
  test('hands the saved layout to the builder once the page is built', () => {
    const fake = fakePage('2', true);

    slotLayout.call(fake.page as never);
    fake.build();

    expect(fake.told).toEqual(['2']);
  });

  /** Switching the picker has to redraw the builder, or the wearer would edit the layout they just left. */
  test('hands each new pick to the builder', () => {
    const fake = fakePage('0', true);

    slotLayout.call(fake.page as never);
    fake.build();
    fake.pick('2');

    expect(fake.told).toEqual(['0', '2']);
  });

  /** A page without the builder's hook must be left alone rather than throw and stop the rest of the page code. */
  test('leaves a page whose builder has no hook alone', () => {
    const fake = fakePage('1', false);

    slotLayout.call(fake.page as never);

    const result = () => fake.build();

    expect(result).not.toThrow();
  });
});
