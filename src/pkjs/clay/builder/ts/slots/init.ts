/**
 * Wiring: find the page's pieces, paint the face and the palette, and hang the hooks Clay's
 * manipulator calls.
 *
 * This is the only piece that touches Clay. Everything above it is plain functions over the panel
 * values, which is what makes them testable outside the config webview.
 *
 * The builder keeps both layouts' picks and edits the one the Layout picker holds. The face's page
 * code hands it the layout through the `_sbLayout` hook once the page is built and on every change,
 * so switching the picker redraws the grid without touching the other layout's picks.
 */

import { ID_EMPTY, LAYOUT_TACTICAL, LAYOUT_STARDATE, UNIQUE_IDS, areaOf, cellCount, colOf, gridFor, owners, rowOf, shapeOf, spanOf } from './geometry';
import type { Grid } from './geometry';
import { formatId, moveUnique, readTactical, readSlots, sanitize, writeTactical, writeStores } from './codec';
import { presetFor } from './presets';
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
  const faceEl = root.querySelector('.sb-face') as HTMLElement;
  const slotsEl = root.querySelector('.sb-slots') as HTMLElement;

  // the panel boxes of the layout on show. the drag reads this same array, so a layout switch
  // refills it in place rather than handing out a new one
  const slotEls: HTMLElement[] = [];

  let layout = LAYOUT_STARDATE;
  // each layout's picks, kept apart so editing one never touches the other
  const picks: Record<number, number[]> = {};

  picks[LAYOUT_STARDATE] = readSlots(root, hidden.value);
  picks[LAYOUT_TACTICAL] = readTactical(root);

  function grid(): Grid {
    return gridFor(layout);
  }

  function current(): number[] {
    return picks[layout];
  }

  /**
   * The sketch's row for a cell. Between two areas sits a thin row standing in for the frame's
   * strips, so each area starts a row further down than the last one ended.
   */
  function sketchRow(slot: number): number {
    return areaOf(slot, grid()) * (grid().rows + 1) + rowOf(slot, grid()) + 1;
  }

  /** Builds one box a cell for the layout on show, in cell order. */
  function buildSlots(): void {
    const rows: string[] = [];

    for (let area = 0; area < grid().areas; area++) {
      if (area > 0) {
        rows.push('var(--sb-strip-h)');
      }

      for (let row = 0; row < grid().rows; row++) {
        rows.push('var(--sb-slot-h)');
      }
    }

    slotsEl.innerHTML = '';
    slotEls.length = 0;
    slotsEl.style.gridTemplateRows = rows.join(' ');
    faceEl.classList.toggle('sb-tactical', layout === LAYOUT_TACTICAL);

    for (let slot = 0; slot < cellCount(grid()); slot++) {
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
    const slots = current();
    const owner = owners(slots, grid());
    const placed: number[] = [];

    for (let slot = 0; slot < slotEls.length; slot++) {
      const el = slotEls[slot];
      const covered = owner[slot] >= 0 && owner[slot] !== slot;
      const span = spanOf(shapeOf(slots[slot]));
      const readout = slots[slot] === ID_EMPTY ? null : readoutById(READOUTS, slots[slot]);

      el.style.display = covered ? 'none' : '';
      el.style.gridArea = sketchRow(slot) + ' / ' + (colOf(slot, grid()) + 1) + ' / span ' + span.rows + ' / span ' + span.cols;
      fillVisual(el, readout, THUMBS);
      el.classList.toggle('filled', Boolean(readout));

      if (readout) {
        placed.push(readout.value);
      }
    }

    // the clock and the date dim on a layout that keeps them in their own places, since they cannot go there
    palette.markPlaced(grid().placesClock ? placed : placed.concat(UNIQUE_IDS));
    warnings.update(placed.map(function (value) {
      const readout = readoutById(READOUTS, value);

      return readout ? warningName(readout.label) : '';
    }));
    publish();
  }

  /**
   * Hand the values to Clay: the Stardate layout's upper left through this component's own value
   * and the rest through their stores, and Tactical's twelve through its one store. Both go out on
   * every render whichever layout is on show, and only a value that moved fires a change, so opening
   * the page does not mark the settings dirty on its own.
   */
  function publish(): void {
    const own = formatId(picks[LAYOUT_STARDATE][0]);

    if (hidden.value !== own) {
      hidden.value = own;
      fireChange('change');
    }

    writeStores(root, picks[LAYOUT_STARDATE]);
    writeTactical(root, picks[LAYOUT_TACTICAL]);
  }

  /** Takes new picks for the layout on show. A clock or date dropped somewhere moves there. */
  function setSlots(next: number[]): void {
    picks[layout] = sanitize(moveUnique(current(), next), grid());
    render();
  }

  const drag = installDrag({
    root: root,
    readouts: READOUTS,
    thumbs: THUMBS,
    slotEls: slotEls,
    getGrid: grid,
    getSlots: current,
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

  /** Puts a layout on show: its grid and its picks. */
  function showLayout(next: number): void {
    layout = next === LAYOUT_TACTICAL ? LAYOUT_TACTICAL : LAYOUT_STARDATE;
    buildSlots();
    render();
  }

  // --- actions ---

  const presetBtns = root.querySelectorAll('.sb-preset');

  for (let i = 0; i < presetBtns.length; i++) {
    presetBtns[i].addEventListener('click', function (event) {
      const id = (event.currentTarget as HTMLElement).getAttribute('data-preset') || '';
      const preset = presetFor(layout, id);

      if (preset) {
        setSlots(preset);
      }
    });
  }

  const clear = root.querySelector('.sb-btn-clear');

  if (clear) {
    clear.addEventListener('click', function () {
      const empty: number[] = [];

      while (empty.length < cellCount(grid())) {
        empty.push(ID_EMPTY);
      }

      setSlots(empty);
    });
  }

  // --- the hooks ---
  // the manipulator speaks the Stardate layout's upper-left slot only, which is this component's
  // message key. the other three and Tactical's list are stores it knows nothing about, so they
  // get read and written around it

  interface Hooked extends HTMLElement {
    _sbSet?: (value: string) => void;
    _sbGet?: () => string;
    _sbLayout?: (value: string) => void;
  }
  const hooked = root as Hooked;

  hooked._sbSet = function (value: string): void {
    picks[LAYOUT_STARDATE] = readSlots(root, value);
    render();
  };

  hooked._sbGet = function (): string {
    return formatId(picks[LAYOUT_STARDATE][0]);
  };

  // the face's page code calls this with the Layout picker's value, which Clay hands over as text
  hooked._sbLayout = function (value: string): void {
    const next = parseInt(value, 10);

    if (next !== layout) {
      showLayout(next);
    }
  };

  showLayout(LAYOUT_STARDATE);

  // the stores may be built after this component and then the reads above saw nothing. pick them up
  // once the page has finished. the mosaic builder keeps the same safety net
  setTimeout(function () {
    const stardate = readSlots(root, hidden.value);
    const tactical = readTactical(root);
    const moved = function (late: number[], kept: number[]): boolean {
      return late.some(function (id, slot) { return id !== kept[slot]; });
    };

    if (moved(stardate, picks[LAYOUT_STARDATE]) || moved(tactical, picks[LAYOUT_TACTICAL])) {
      picks[LAYOUT_STARDATE] = stardate;
      picks[LAYOUT_TACTICAL] = tactical;
      render();
    }
  }, 0);
}
