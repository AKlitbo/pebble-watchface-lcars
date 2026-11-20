/**
 * Reading and writing the four panel values.
 *
 * The four panels sit on four message keys. A Clay component owns exactly one key, so the builder's
 * own key carries the upper-left panel and the other three ride hidden stores on the same page, each
 * found by its own class. Each value is the catalogue id written as text, which is what the watch's
 * enum settings already expect off the wire.
 */

import { GRID, ID_EMPTY, SLOT_COUNT, SLOT_STORE_CLASS, cellCount, owners } from './geometry';
import type { Grid } from './geometry';

/** Read a slot id out of a hidden input's text, falling back to empty. */
export function parseId(text: string | null | undefined): number {
  const n = parseInt(text || '', 10);

  return isNaN(n) || n < 0 ? ID_EMPTY : n;
}

/** An id as text, ready for the input it lives in. */
export function formatId(id: number): string {
  return String(id);
}

/**
 * Find a hidden store on the page by its class.
 *
 * Every hiddenStore carries .pf-hidden-store, so each one on the page is given its own extra class
 * and that is what this looks for.
 *
 * @param root The component element, used only to reach the document.
 * @param cls The store's own class.
 * @return The store, or null when it is not on the page.
 */
function storeByClass(root: HTMLElement, cls: string): HTMLInputElement | null {
  const doc = root.ownerDocument || document;

  return doc.querySelector('.' + cls) as HTMLInputElement | null;
}

/**
 * Find the hidden input backing a slot.
 *
 * Returns null for the upper-left slot, which is the component's own value rather than a store, and
 * for a store that is not on the page.
 *
 * @param root The component element, used only to reach the document.
 * @param slot The slot to find.
 * @return The store, or null.
 */
export function storeFor(root: HTMLElement, slot: number): HTMLInputElement | null {
  const cls = SLOT_STORE_CLASS[slot];

  return cls ? storeByClass(root, cls) : null;
}

/**
 * The four current slot values.
 *
 * @param root The component element.
 * @param ownValue The component's own value, which is the upper-left slot.
 * @return The four values, corrected.
 */
export function readSlots(root: HTMLElement, ownValue: string): number[] {
  const slots = [];

  for (let slot = 0; slot < SLOT_COUNT; slot++) {
    const store = storeFor(root, slot);

    slots.push(parseId(store ? store.value : ownValue));
  }

  return sanitize(slots, GRID);
}

/**
 * Push a store's new text out, with a change event so Clay notices. A store that already holds the
 * text is left alone, since a change Clay answers would only write the same thing back.
 */
function writeStore(store: HTMLInputElement | null, next: string): void {
  if (store && store.value !== next) {
    store.value = next;
    store.dispatchEvent(new Event('change'));
  }
}

/**
 * Push the three store-backed slots out to their inputs.
 *
 * The upper-left slot is not written here. It is the component's own value and goes back through
 * the manipulator.
 *
 * @param root The component element.
 * @param slots The four values.
 */
export function writeStores(root: HTMLElement, slots: number[]): void {
  for (let slot = 1; slot < SLOT_COUNT; slot++) {
    writeStore(storeFor(root, slot), formatId(slots[slot]));
  }
}

/**
 * Fix up a set of panel values that came from somewhere untrusted.
 *
 * A stored value or a hand-edited store can hold a big pick in a cell that cannot show one, which
 * would render as a labelled panel with nothing in it, or a pick in a cell a bigger one covers. The
 * firmware makes the same correction, so this is only about the builder showing the truth. Each
 * cell keeps a value only when a readout starts there, and every other cell is saved as empty.
 *
 * @param slots The values, one a cell.
 * @param grid The grid.
 * @return One value a cell, corrected.
 */
export function sanitize(slots: number[], grid: Grid): number[] {
  const count = cellCount(grid);
  const out = slots.slice(0, count);

  while (out.length < count) {
    out.push(ID_EMPTY);
  }

  const owner = owners(out, grid);

  for (let slot = 0; slot < count; slot++) {
    if (owner[slot] !== slot) {
      out[slot] = ID_EMPTY;
    }
  }

  return out;
}
