/**
 * This face's answers for the shared drag engine: what a panel drag carries, where it may land,
 * and what happens when it gets there.
 *
 * The pointer handling itself lives in the framework. All that is here is the panel grid's own
 * shape: hit testing the panel boxes rather than mapping a pointer onto a grid pitch, and swapping
 * panels rather than placing a block into free space.
 */

import { ID_EMPTY, SHAPE_SINGLE, areaOf, cellsFor, colOf, owners, rowOf, shapeOf, slotAt } from './geometry';
import type { Grid } from './geometry';
import { fillVisual, readoutById } from './visuals';
import type { Readout } from './visuals';
import type { Thumbs } from '../../../../../../paf/ts/clay/builder/ts/types';
import { createDrag } from '../../../../../../paf/ts/clay/builder/ts/drag';

/** What the drag needs from the component around it. */
export interface DragEnv {
  root: HTMLElement;
  readouts: Readout[];
  thumbs: Thumbs;
  /** The panel boxes, in cell order. A hidden one reports an empty rect and is skipped. */
  slotEls: HTMLElement[];
  /** The grid of the layout on show. */
  getGrid: () => Grid;
  getSlots: () => number[];
  setSlots: (slots: number[]) => void;
}

/** A readout in flight, and the panel it was lifted from (-1 straight off the palette). */
interface Dragged {
  id: number;
  from: number;
}

/**
 * Work out what the panels would become, or null when the move is not allowed.
 *
 * A drop off the palette replaces whatever it lands on, all of it, so a single readout dropped on
 * part of a big panel takes the whole panel out. A drag from one panel to another trades places
 * instead. Each readout it lands on moves by the same step the other way, so swapping two singles,
 * or a wide panel with the row under it, both come out as the eye expects, and so does a trade
 * between two areas, which moves each readout by the same areas too. A readout that would not
 * fit where the trade sends it refuses the whole move rather than being dropped or placed badly, so
 * the whole candidate arrangement is checked rather than just the panel under the pointer.
 *
 * @param slots The current values, one a cell.
 * @param id What is being dragged.
 * @param from The cell it came from, or -1 from the palette.
 * @param to The cell it is being dropped on.
 * @param grid The grid.
 * @return The new values, or null to refuse the drop.
 */
export function tryPlace(slots: number[], id: number, from: number, to: number, grid: Grid): number[] | null {
  const landing = cellsFor(id, to, grid);

  if (!landing) {
    return null;
  }

  if (from === to) {
    return slots.slice();
  }

  const next = slots.slice();

  if (from >= 0) {
    clearPiece(next, from, grid);
  }

  // what sits under the landing cells, by the cell each one starts from
  const under = owners(next, grid);
  const hit: number[] = [];

  landing.forEach((cell) => {
    if (under[cell] >= 0 && hit.indexOf(under[cell]) < 0) {
      hit.push(under[cell]);
    }
  });

  const hitIds = hit.map((slot) => next[slot]);

  hit.forEach((slot) => {
    clearPiece(next, slot, grid);
  });

  next[to] = id;

  if (from < 0) {
    return next;
  }

  const stepArea = areaOf(from, grid) - areaOf(to, grid);
  const stepCol = colOf(from, grid) - colOf(to, grid);
  const stepRow = rowOf(from, grid) - rowOf(to, grid);

  for (let i = 0; i < hit.length; i++) {
    const dest = slotAt(colOf(hit[i], grid) + stepCol, rowOf(hit[i], grid) + stepRow, grid, areaOf(hit[i], grid) + stepArea);
    const cells = dest >= 0 ? cellsFor(hitIds[i], dest, grid) : null;
    const taken = owners(next, grid);

    if (!cells || cells.some((cell) => taken[cell] >= 0)) {
      return null;
    }

    next[dest] = hitIds[i];
  }

  return next;
}

/** Empties every cell the readout starting at a cell fills. */
function clearPiece(slots: number[], slot: number, grid: Grid): void {
  const cells = cellsFor(slots[slot], slot, grid) || [slot];

  cells.forEach((cell) => {
    slots[cell] = ID_EMPTY;
  });
}

/**
 * Wire up dragging.
 *
 * @param env The component's handles.
 * @return Starters for the two places a drag can begin.
 */
export function installDrag(env: DragEnv) {
  const doc = env.root.ownerDocument || document;

  /** Which panel is under a point, or null. A hidden panel has no box and cannot win. */
  function slotUnder(x: number, y: number): number | null {
    for (let slot = 0; slot < env.slotEls.length; slot++) {
      const box = env.slotEls[slot].getBoundingClientRect();

      if (box.width && x >= box.left && x <= box.right && y >= box.top && y <= box.bottom) {
        return slot;
      }
    }

    return null;
  }

  const drag = createDrag<Dragged, number>({
    anchor: 'center',

    ghost(dragged) {
      const ghost = doc.createElement('div');

      const shape = shapeOf(dragged.id);

      ghost.className = 'sb-ghost' + (shape === SHAPE_SINGLE ? '' : ' ' + shape);
      fillVisual(ghost, readoutById(env.readouts, dragged.id), env.thumbs);
      return ghost;
    },

    hitTest: slotUnder,

    allows(dragged, slot) {
      return tryPlace(env.getSlots(), dragged.id, dragged.from, slot, env.getGrid()) !== null;
    },

    highlight(dragged, slot, allowed) {
      for (let i = 0; i < env.slotEls.length; i++) {
        env.slotEls[i].classList.remove('over', 'blocked');
      }

      if (slot !== null) {
        env.slotEls[slot].classList.add(allowed ? 'over' : 'blocked');
      }
    },

    drop(dragged, slot) {
      const next = tryPlace(env.getSlots(), dragged.id, dragged.from, slot, env.getGrid());

      if (next) {
        env.setSlots(next);
      }
    },

    dropOutside(dragged) {
      // a panel dragged away is emptied. a palette drag that lands nowhere is just abandoned
      if (dragged.from < 0) {
        return;
      }

      const cleared = env.getSlots().slice();

      cleared[dragged.from] = ID_EMPTY;
      env.setSlots(cleared);
    },
  }, doc);

  return {
    fromPalette(id: number, event: PointerEvent) {
      if (readoutById(env.readouts, id)) {
        drag.start({ id: id, from: -1 }, event);
      }
    },

    // armed rather than started, so a tap on a panel is not mistaken for picking it up
    fromSlot(slot: number, event: PointerEvent) {
      const id = env.getSlots()[slot];

      if (id !== ID_EMPTY) {
        drag.arm({ id: id, from: slot }, event);
      }
    },
  };
}
