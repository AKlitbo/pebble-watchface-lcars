/**
 * Specs for what the palette and the plugin warnings are told about the readouts.
 *
 * The size keys are how the palette files each readout under a tab, so a wrong key lists a panel
 * under a size it does not fill. The groups and plugin kinds ride on module-meta's rows by name, and a
 * name the builder does not list fails quietly: the readout lands under Other, or a panel that needs
 * a plugin never warns.
 */

import { describe, test, expect } from 'vitest';
import { PALETTE_GROUPS, PALETTE_SIZES, sizeKeyOf, warningName } from './panels';
import { SHAPE_FULL, SHAPE_SINGLE, SHAPE_TALL, SHAPE_WIDE } from './geometry';
import type { Shape } from './geometry';
import moduleMeta from '../../../module-meta';

describe('PALETTE_SIZES', () => {
  /** The tabs read width by height, so a wide panel labelled 1x2 would send the wearer to the tall block's tab. */
  test('names each size width by height, smallest first', () => {
    const result = PALETTE_SIZES.map((size) => size.key);

    expect(result).toEqual(['1x1', '2x1', '1x2', '2x2']);
  });

  /** A shape two columns wide spans the grid, and a narrow tile would squash its shot to half its width. */
  test('gives the shapes two columns wide the wide tiles', () => {
    const result = PALETTE_SIZES.filter((size) => size.wide).map((size) => size.shape);

    expect(result).toEqual([SHAPE_WIDE, SHAPE_FULL]);
  });
});

describe('sizeKeyOf', () => {
  /** The palette files each readout under the tab its key names, so a shape filed under the wrong key would list a panel under a size it does not fill. */
  test('files each shape under its own size tab', () => {
    const shapes: Shape[] = [SHAPE_SINGLE, SHAPE_TALL, SHAPE_WIDE, SHAPE_FULL];

    const result = shapes.map((shape) => sizeKeyOf(shape));

    expect(result).toEqual(['1x1', '1x2', '2x1', '2x2']);
  });
});

describe('warningName', () => {
  /** The warning reads as a sentence, so the bar's own word in brackets would land in the middle of it. */
  test('drops the bracketed word off a label', () => {
    const result = [warningName('Now Playing (AUDIO)'), warningName('Sensors Block (weather + temperature)'), warningName('Swatch Beats')];

    expect(result).toEqual(['Now Playing', 'Sensors Block', 'Swatch Beats']);
  });

  /** Two readouts sharing a warning name would merge into one line, and the second would never be named on its own. */
  test('keeps every readout that reads a plugin apart', () => {
    const names = Object.keys(moduleMeta).filter((label) => moduleMeta[label].plugin).map(warningName);

    const result = new Set(names).size;

    expect(result).toBe(names.length);
  });
});

describe('the module-meta rows', () => {
  /** A group the palette does not list puts the readout under Other, away from the readouts it belongs with. */
  test('name only groups the palette lists', () => {
    const known = PALETTE_GROUPS.map((group) => group.name);

    const result = Object.keys(moduleMeta).filter((label) => known.indexOf(moduleMeta[label].group) < 0);

    expect(result).toEqual([]);
  });

  /** The music and calendar panels only fill from a plugin or an iCal link, so one missing its kind sits empty on the watch with no warning on the page. */
  test('mark the panels that read music or the calendar', () => {
    const result = Object.keys(moduleMeta)
      .filter((label) => moduleMeta[label].plugin === 'music' || moduleMeta[label].plugin === 'calendar')
      .sort();

    expect(result).toEqual(['Agenda (SCHEDULE)', 'Next Event (SCHEDULE)', 'Now Playing (AUDIO)', 'Now Playing with Art (AUDIO)']);
  });
});
