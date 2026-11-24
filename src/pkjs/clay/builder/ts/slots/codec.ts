/**
 * Reading and writing each layout's panel values.
 *
 * The Stardate layout keeps its four panels on four message keys. A Clay component owns exactly one
 * key, so the builder's own key carries the upper-left panel and the other three ride hidden stores
 * on the same page, each found by its own class. Each value is the catalogue id written as text,
 * which is what the watch's enum settings already expect off the wire.
 *
 * Tactical keeps its twelve picks in one hidden store as a list of ids with commas between, such
 * as "84,20,20,20,23,20,0,1,85,20,20,20", in cell order. The watch reads that list one cell at a
 * time from the front, and the parser here makes the same calls so the builder never shows a
 * different face from the watch.
 */

import { TACTICAL_GRID, TACTICAL_STORE_CLASS, ID_EMPTY, SLOT_COUNT, SLOT_STORE_CLASS, STARDATE_GRID, UNIQUE_IDS, cellCount, owners } from './geometry';
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

  return sanitize(slots, STARDATE_GRID);
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
 * Tactical's picks out of the text the watch reads.
 *
 * The picks are read in order, the way the watch reads them. A list that runs out, or holds anything
 * but numbers and commas, leaves the rest of the cells empty rather than guessing at them. A list
 * longer than the twelve cells gives its first twelve picks, and the rest are never read.
 *
 * @param text The saved list, such as "84,20,20,20,23,20,0,1,85,20,20,20".
 * @return The twelve picks, corrected.
 */
export function parseTactical(text: string | null | undefined): number[] {
  const source = text || '';
  const picks: number[] = [];
  let cursor = 0;
  let readable = true;

  for (let cell = 0; cell < cellCount(TACTICAL_GRID); cell++) {
    const digits = readable ? /^[0-9]+/.exec(source.slice(cursor)) : null;

    if (!digits) {
      readable = false;
      picks.push(ID_EMPTY);
      continue;
    }

    picks.push(parseInt(digits[0], 10));
    cursor += digits[0].length;

    if (source.charAt(cursor) !== ',') {
      readable = false;
    }

    cursor++;
  }

  return sanitize(picks, TACTICAL_GRID);
}

/**
 * Tactical's picks as the text the watch reads, every cell written out.
 *
 * @param picks The twelve picks.
 * @return The list, such as "84,20,20,20,23,20,0,1,85,20,20,20".
 */
export function formatTactical(picks: number[]): string {
  return picks.map(formatId).join(',');
}

/**
 * Tactical's current picks, read from their store. A page without the store reads as empty.
 *
 * @param root The component element.
 * @return The twelve picks, corrected.
 */
export function readTactical(root: HTMLElement): number[] {
  const store = storeByClass(root, TACTICAL_STORE_CLASS);

  return parseTactical(store ? store.value : '');
}

/**
 * Push Tactical's picks out to their store.
 *
 * @param root The component element.
 * @param picks The twelve picks.
 */
export function writeTactical(root: HTMLElement, picks: number[]): void {
  writeStore(storeByClass(root, TACTICAL_STORE_CLASS), formatTactical(picks));
}

/**
 * Keep only the newest copy of the clock and the date, after a drop.
 *
 * Each goes on the face once, so dropping one somewhere moves it there. The copy in a cell that did
 * not hold it before the drop is the one kept, and any other copy is cleared. A drop that left a
 * copy only where it already was changes nothing.
 *
 * @param before The picks before the drop.
 * @param after The picks after it.
 * @return The picks after it, with one copy of each at most.
 */
export function moveUnique(before: number[], after: number[]): number[] {
  const out = after.slice();

  UNIQUE_IDS.forEach(function (id) {
    const cells = out.map(function (pick, cell) { return pick === id ? cell : -1; }).filter(function (cell) { return cell >= 0; });

    if (cells.length < 2) {
      return;
    }

    const fresh = cells.filter(function (cell) { return before[cell] !== id; });
    const keep = fresh.length > 0 ? fresh[0] : cells[0];

    cells.forEach(function (cell) {
      if (cell !== keep) {
        out[cell] = ID_EMPTY;
      }
    });
  });

  return out;
}

/**
 * Fix up a set of panel values that came from somewhere untrusted.
 *
 * A stored value or a hand-edited store can hold a big pick in a cell that cannot show one, which
 * would render as a labelled panel with nothing in it, or a pick in a cell a bigger one covers. The
 * firmware makes the same correction, so this is only about the builder showing the truth. Each
 * cell keeps a value only when a readout starts there, and every other cell is saved as empty. The
 * clock and the date are kept once each, the first copy in cell order the shape rules keep, and only
 * where the grid places them.
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

  for (let slot = 0; slot < count; slot++) {
    if (!grid.placesClock && UNIQUE_IDS.indexOf(out[slot]) >= 0) {
      out[slot] = ID_EMPTY;
    }
  }

  // a copy only counts once the shape rules keep it, so a first copy that does not fit leaves the
  // next one standing, the same call the watch makes
  const kept = owners(out, grid);
  const seen: number[] = [];

  for (let slot = 0; slot < count; slot++) {
    if (kept[slot] === slot && UNIQUE_IDS.indexOf(out[slot]) >= 0) {
      if (seen.indexOf(out[slot]) >= 0) {
        out[slot] = ID_EMPTY;
      } else {
        seen.push(out[slot]);
      }
    }
  }

  const owner = owners(out, grid);

  for (let slot = 0; slot < count; slot++) {
    if (owner[slot] !== slot) {
      out[slot] = ID_EMPTY;
    }
  }

  return out;
}
