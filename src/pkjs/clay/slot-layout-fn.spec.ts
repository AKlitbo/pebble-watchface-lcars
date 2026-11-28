/**
 * Specs for the page code that tells the slot builder which layout is picked.
 *
 * The builder cannot read the Layout picker or the Mirrored switches itself, so this is the only thing
 * keeping its grid and its rail in step with them. Worth pinning are the moments it has to speak, once
 * the page is built and on every change of either, that it waits for the build, since Clay has not
 * listed the rows before then, and that an odd pick found on the page becomes its plain layout with
 * the layout's Mirrored switch on, whether or not the builder is there to tell.
 */

import { describe, test, expect } from 'vitest';
import slotLayout from './slot-layout-fn';

/** A Clay row that records changes and can be set the way Clay's manipulator sets one. */
function fakeRow(value: unknown) {
  const changes: Array<() => void> = [];

  return {
    value: value,
    $element: [],
    get: function () { return this.value; },
    set: function (next: unknown) {
      this.value = next;
      changes.forEach((handler) => handler());
    },
    on: function (event: string, handler: () => void) {
      if (event === 'change') {
        changes.push(handler);
      }
    },
  };
}

/** A Clay page with the Layout picker, the Mirrored switches, and the builder on it, recording what the builder is told. */
function fakePage(layout: string, hooked: boolean, mirrors = [false, false, false]) {
  const handlers: Record<string, () => void> = {};
  const told: string[] = [];
  const picker = fakeRow(layout);
  const switches = mirrors.map((on) => fakeRow(on));
  const host = hooked ? { _sbLayout: (value: string) => { told.push(value); } } : {};
  const builder = { $element: [host], get: () => '23', on: () => {} };
  const rows: Record<string, unknown> = {
    APPEARANCE_LAYOUT: picker,
    APPEARANCE_MIRROR_STARDATE: switches[0],
    APPEARANCE_MIRROR_TACTICAL: switches[1],
    APPEARANCE_MIRROR_ENGINEERING: switches[2],
    APPEARANCE_SLOT_LEFT_TOP: builder,
  };
  const page = {
    EVENTS: { AFTER_BUILD: 'AFTER_BUILD' },
    on: (event: string, handler: () => void) => { handlers[event] = handler; },
    getItemByMessageKey: (key: string) => rows[key],
  };

  return {
    page: page,
    told: told,
    picker: picker,
    switches: switches,
    build: () => handlers.AFTER_BUILD(),
  };
}

describe('slotLayout', () => {
  /** Clay has not listed the rows before the build, so speaking any earlier would find no builder and leave it on Stardate. */
  test('waits for the page to be built', () => {
    const fake = fakePage('2', true);

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
    fake.picker.set('2');

    expect(fake.told).toEqual(['0', '2']);
  });

  /** The sketch runs its rail down the right only when the picked layout's own switch is on, and the watch reads the pick the same way. */
  test('hands the builder a mirrored pick once that layout is switched to mirrored', () => {
    const fake = fakePage('4', true);

    slotLayout.call(fake.page as never);
    fake.build();
    fake.switches[2].set(true);

    expect(fake.told).toEqual(['4', '5']);
  });

  /** Another layout's switch says nothing about the one on show, so Stardate mirrored leaves Tactical's sketch alone. */
  test('reads only the picked layout\'s switch', () => {
    const fake = fakePage('2', true, [true, false, false]);

    slotLayout.call(fake.page as never);
    fake.build();

    expect(fake.told).toEqual(['2']);
  });

  /**
   * A wearer with a 3 saved has Tactical mirrored, and the picker lists no 3 once the page is built. It
   * has to open as Tactical with Tactical's switch on, or the page shows no layout picked and the next
   * save drops the mirror.
   */
  test('turns a saved mirrored pick into its layout with the switch on', () => {
    const fake = fakePage('3', true);

    slotLayout.call(fake.page as never);
    fake.build();

    const result = { layout: fake.picker.value, mirrored: fake.switches[1].value, told: fake.told };

    expect(result).toEqual({ layout: 2, mirrored: true, told: ['3'] });
  });

  /** The mirrored picks are only listed so a saved one can be read, so a picker still offering them would give two ways to mirror a layout. */
  test('takes the mirrored picks off the list once the page is built', () => {
    const fake = fakePage('0', true);
    const list = ['0', '1', '2', '3', '4', '5', '6'];
    const parent = { removeChild: (option: { value: string }) => { list.splice(list.indexOf(option.value), 1); } };
    const options = list.map((value) => ({ value: value, parentNode: parent }));

    (fake.picker as { $element: unknown[] }).$element = [{ querySelectorAll: () => options }];

    slotLayout.call(fake.page as never);
    fake.build();

    expect(list).toEqual(['0', '2', '4', '6']);
  });

  /** A page whose builder failed to hang its hook still has to turn an odd pick into the switch, or it shows two ways to mirror. */
  test('turns a saved mirrored pick into the switch without the builder', () => {
    const fake = fakePage('5', false);

    slotLayout.call(fake.page as never);
    fake.build();

    const result = { layout: fake.picker.value, mirrored: fake.switches[2].value };

    expect(result).toEqual({ layout: 4, mirrored: true });
  });

  /** A page without the builder's hook must be left alone rather than throw and stop the rest of the page code. */
  test('leaves a page whose builder has no hook alone', () => {
    const fake = fakePage('1', false);

    slotLayout.call(fake.page as never);

    const result = () => fake.build();

    expect(result).not.toThrow();
  });
});
