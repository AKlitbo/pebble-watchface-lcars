/**
 * Wiring: find the page's pieces, paint the face and the palette, and hang the hooks Clay's
 * manipulator calls.
 *
 * This is the only piece that touches Clay. Everything above it is plain functions over the panel
 * values, which is what makes them testable outside the config webview.
 */

import { GRID, ID_EMPTY, cellCount, colOf, owners, rowOf, shapeOf, spanOf } from './geometry';
import { formatId, readSlots, sanitize, writeStores } from './codec';
import { SLOT_PRESETS } from './presets';
import { PALETTE_GROUPS, PALETTE_SIZES, sizeKeyOf, warningName } from './panels';
import { buildReadoutList, fillVisual, readoutById, thumbFor } from './visuals';
import type { Readout } from './visuals';
import type { Thumbs } from '../../../../../../paf/ts/clay/builder/ts/types';
import { createPalette } from '../../../../../../paf/ts/clay/builder/ts/shared/palette';
import type { PaletteModule } from '../../../../../../paf/ts/clay/builder/ts/shared/palette';
import { createPluginWarnings } from '../../../../../../paf/ts/clay/builder/ts/shared/plugin-warnings';
import type { PluginKind } from '../../../../../../paf/ts/clay/builder/ts/shared/plugin-warnings';
import { installDrag } from './drag';

/** What Clay binds initialize to. */
interface ClayContext {
  $element: HTMLElement[];
  config?: { moduleOptions?: unknown[]; moduleThumbnails?: Thumbs };
  trigger: (event: string) => void;
}

/** A readout as the palette lists it, under the one size its shape comes in. */
type PaletteReadout = Readout & PaletteModule;

export function init(this: ClayContext): void {
  const root = this.$element[0];
  const cfg = this.config || {};
  // the change is fired through the bound trigger rather than through `this`
  // the nested callbacks below do not carry it
  const fireChange = this.trigger.bind(this);

  const READOUTS: Readout[] = buildReadoutList((cfg.moduleOptions || []) as never[]);
  const THUMBS: Thumbs = cfg.moduleThumbnails || {};

  const hidden = root.querySelector('.sb-value') as HTMLInputElement;
  const slotsEl = root.querySelector('.sb-slots') as HTMLElement;

  // the panel boxes, one a cell in cell order
  const slotEls: HTMLElement[] = [];
  let slots = readSlots(root, hidden.value);

  /** Builds one box a cell, in cell order. */
  function buildSlots(): void {
    for (let slot = 0; slot < cellCount(GRID); slot++) {
      const el = root.ownerDocument.createElement('div');

      el.className = 'sb-slot';
      el.dataset.slot = String(slot);

      (function (index) {
        el.addEventListener('pointerdown', function (event) {
          drag.fromSlot(index, event as PointerEvent);
        });
      })(slot);

      slotsEl.appendChild(el);
      slotEls.push(el);
    }
  }

  /**
   * Paint the panels and push the values back out.
   *
   * A big readout's box grows over the cells it fills, and the boxes it covers go away, so the
   * sketch has one box per readout the way the watch draws it.
   */
  function render(): void {
    const owner = owners(slots, GRID);
    const placed: number[] = [];

    for (let slot = 0; slot < slotEls.length; slot++) {
      const el = slotEls[slot];
      const covered = owner[slot] >= 0 && owner[slot] !== slot;
      const span = spanOf(shapeOf(slots[slot]));
      const readout = slots[slot] === ID_EMPTY ? null : readoutById(READOUTS, slots[slot]);

      el.style.display = covered ? 'none' : '';
      el.style.gridArea = (rowOf(slot, GRID) + 1) + ' / ' + (colOf(slot, GRID) + 1) + ' / span ' + span.rows + ' / span ' + span.cols;
      fillVisual(el, readout, THUMBS);
      el.classList.toggle('filled', Boolean(readout));

      if (readout) {
        placed.push(readout.value);
      }
    }

    palette.markPlaced(placed);
    warnings.update(placed.map(function (value) {
      const readout = readoutById(READOUTS, value);

      return readout ? warningName(readout.label) : '';
    }));
    publish();
  }

  /**
   * Hand the values to Clay: the upper left through this component's own value and the rest through
   * their stores. Only a value that moved fires a change, so opening the page does not mark the
   * settings dirty on its own.
   */
  function publish(): void {
    const own = formatId(slots[0]);

    if (hidden.value !== own) {
      hidden.value = own;
      fireChange('change');
    }

    writeStores(root, slots);
  }

  function setSlots(next: number[]): void {
    slots = sanitize(next, GRID);
    render();
  }

  const drag = installDrag({
    root: root,
    readouts: READOUTS,
    thumbs: THUMBS,
    slotEls: slotEls,
    getSlots: function () { return slots; },
    setSlots: setSlots,
  });

  // --- the palette and the warnings under the face ---

  const MODULES: PaletteReadout[] = READOUTS.map(function (readout) {
    return Object.assign({}, readout, { sizes: [sizeKeyOf(shapeOf(readout.value))] });
  });

  const palette = createPalette({
    container: root.querySelector('.sb-palette') as HTMLElement,
    modules: MODULES,
    groups: PALETTE_GROUPS,
    sizes: PALETTE_SIZES.map(function (size) {
      return { key: size.key, wide: size.wide };
    }),
    thumbFor: function (value) {
      return thumbFor(THUMBS, readoutById(READOUTS, value));
    },
    onPick: function (module, size, event) {
      drag.fromPalette(module.value, event);
    },
  });

  const panelKinds: Partial<Record<string, PluginKind>> = {};

  READOUTS.forEach(function (readout) {
    if (readout.plugin) {
      panelKinds[warningName(readout.label)] = readout.plugin;
    }
  });

  const warnings = createPluginWarnings({
    root: root,
    after: root.querySelector('.sb-face-wrap') as HTMLElement,
    panelKinds: panelKinds,
  });

  // --- actions ---

  const presetBtns = root.querySelectorAll('.sb-preset');

  for (let i = 0; i < presetBtns.length; i++) {
    presetBtns[i].addEventListener('click', function (event) {
      const id = (event.currentTarget as HTMLElement).getAttribute('data-preset') || '';

      if (SLOT_PRESETS[id]) {
        setSlots(SLOT_PRESETS[id]);
      }
    });
  }

  const clear = root.querySelector('.sb-btn-clear');

  if (clear) {
    clear.addEventListener('click', function () {
      const empty: number[] = [];

      while (empty.length < cellCount(GRID)) {
        empty.push(ID_EMPTY);
      }

      setSlots(empty);
    });
  }

  // --- the hooks ---
  // the manipulator speaks the upper-left slot only, which is this component's message key. the
  // other three are stores it knows nothing about, so they get read and written around it

  interface Hooked extends HTMLElement { _sbSet?: (value: string) => void; _sbGet?: () => string }
  const hooked = root as Hooked;

  hooked._sbSet = function (value: string): void {
    slots = readSlots(root, value);
    render();
  };

  hooked._sbGet = function (): string {
    return formatId(slots[0]);
  };

  buildSlots();
  render();

  // the stores may be built after this component and then the read above saw nothing. pick them up
  // once the page has finished. the mosaic builder keeps the same safety net
  setTimeout(function () {
    const late = readSlots(root, hidden.value);
    const moved = late.some(function (id, slot) { return id !== slots[slot]; });

    if (moved) {
      slots = late;
      render();
    }
  }, 0);
}
