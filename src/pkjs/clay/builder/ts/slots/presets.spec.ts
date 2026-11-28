/**
 * Specs for the preset arrangements.
 *
 * The presets are hand written slot lists in a JSON file, so nothing else
 * guarantees they are actually placeable. A preset the rules have to correct
 * would hand the user a different face than the button promised, and the
 * correction is silent.
 */

import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { describe, test, expect } from 'vitest';
import { ENGINEERING_PRESETS, SLOT_PRESETS, TACTICAL_PRESETS, TRANSPORTER_PRESETS } from './presets';
import { sanitize } from './codec';
import { ENGINEERING_GRID, STARDATE_GRID, SLOT_COUNT, TACTICAL_GRID, TRANSPORTER_GRID, canPlace, cellCount } from './geometry';

describe('SLOT_PRESETS', () => {
  /** esbuild inlines the JSON at bundle time, so a table that arrived empty leaves the preset buttons doing nothing. */
  test('ships the default arrangement the reset button puts back', () => {
    const result = Object.keys(SLOT_PRESETS);

    expect(result).toContain('default');
  });

  Object.keys(SLOT_PRESETS).forEach((presetId) => {
    /** A preset with the wrong number of slots would leave a panel undefined for the builder to draw. */
    test(`preset "${presetId}" names every slot`, () => {
      const result = SLOT_PRESETS[presetId].length;

      expect(result).toBe(SLOT_COUNT);
    });

    /** A preset the rules reject would blank a panel the moment it loads, so the button would not do what it says. */
    test(`preset "${presetId}" places every readout where the watch can draw it`, () => {
      const result = SLOT_PRESETS[presetId].map((id, slot) => canPlace(id, slot, STARDATE_GRID));

      expect(result).toEqual(new Array(SLOT_COUNT).fill(true));
    });

    /** And it has to survive the read path untouched, or the arrangement shown is not the one stored. */
    test(`preset "${presetId}" comes back from sanitize unchanged`, () => {
      const result = sanitize(SLOT_PRESETS[presetId], STARDATE_GRID);

      expect(result).toEqual(SLOT_PRESETS[presetId]);
    });
  });
});

describe('the watch defaults', () => {
  const schema = readFileSync(fileURLToPath(new URL('../../../../../c/settings_schema.c', import.meta.url)), 'utf8');

  /**
   * A fresh watch draws the default the C code spells out, and the settings page offers the preset
   * JSON. Editing one without the other leaves a new install showing panels the Default button would
   * not put back.
   */
  test.each([
    ['TACTICAL_SLOTS_DEFAULT', TACTICAL_PRESETS],
    ['ENGINEERING_SLOTS_DEFAULT', ENGINEERING_PRESETS],
    ['TRANSPORTER_SLOTS_DEFAULT', TRANSPORTER_PRESETS],
  ])('%s matches the default preset', (name, presets) => {
    const match = new RegExp('#define ' + name + ' "([0-9,]*)"').exec(schema);

    const result = match ? match[1] : null;

    expect(result).toBe(presets.default.join(','));
  });
});

describe('the presets of every layout', () => {
  const layouts: [string, Record<string, number[]>, typeof STARDATE_GRID][] = [
    ['Stardate', SLOT_PRESETS, STARDATE_GRID],
    ['Tactical', TACTICAL_PRESETS, TACTICAL_GRID],
    ['Engineering', ENGINEERING_PRESETS, ENGINEERING_GRID],
    ['Transporter', TRANSPORTER_PRESETS, TRANSPORTER_GRID],
  ];

  /** Tactical, Engineering, and Transporter show the same preset buttons, so one missing a preset hides a button the others offer. */
  test.each([
    ['Engineering', ENGINEERING_PRESETS],
    ['Transporter', TRANSPORTER_PRESETS],
  ])('%s offers the same presets as Tactical', (name, presets) => {
    const result = Object.keys(presets).sort();

    expect(result).toEqual(Object.keys(TACTICAL_PRESETS).sort());
  });

  /** A preset the shape rules correct would hand the wearer a different face from the one the button promised. */
  test.each(layouts)('%s presets fill every cell and need no correcting', (name, presets, grid) => {
    const result = Object.keys(presets).filter(function (presetId) {
      const slots = presets[presetId];

      return slots.length !== cellCount(grid) || JSON.stringify(sanitize(slots, grid)) !== JSON.stringify(slots);
    });

    expect(result).toEqual([]);
  });
});
