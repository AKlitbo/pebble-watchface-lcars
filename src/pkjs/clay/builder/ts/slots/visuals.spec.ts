// @vitest-environment jsdom
/**
 * Specs for turning a readout into something to look at.
 *
 * A filled panel in the builder is meant to be a picture of what the watch will
 * draw, so the cases worth pinning are the ones where the picture is missing:
 * a readout the thumbnail pass never shot falls back to its emoji and tint, and
 * the fallback is what stops an unphotographed readout rendering as a broken
 * image or an empty box.
 */

import { describe, test, expect, beforeEach } from 'vitest';
import { buildReadoutList, sizeFor, thumbFor, fillVisual, readoutById } from './visuals';
import type { Readout } from './visuals';
import { ID_HEART, ID_EMPTY, ID_SENSORS, ID_WIDE_DAYLIGHT, ID_FULL_HEART } from './geometry';

const THUMBS = {
  'Heart Rate': { slot: 'data:heart-slot' },
  'Sensors': { tall: 'data:sensors-tall' },
  'Steps': { slot: '' },
};

const HEART: Readout = { value: ID_HEART, label: 'Heart Rate', icon: '♥', color: '#f00' };
const SENSORS: Readout = { value: ID_SENSORS, label: 'Sensors', icon: '⚙', color: '#0f0' };
const UNSHOT: Readout = { value: 5, label: 'Stocks', icon: '$', color: '#00f' };

beforeEach(() => {
  document.body.innerHTML = '';
});

describe('buildReadoutList', () => {
  /**
   * Clay hands option values back as strings.
   *
   * Leaving them as text makes every later === against a catalogue id fail, so the palette would
   * drag readouts that never match the slot they land in.
   */
  test('reads a string option value as a number', () => {
    const result = buildReadoutList([{ value: '1', label: 'Steps' }]);

    expect(result[0].value).toBe(1);
  });

  /** Clearing a panel is what dragging one off does, so an Empty row in the palette would be a second way to do it that looks like a readout. */
  test('drops the Empty row from the palette', () => {
    const result = buildReadoutList([
      { value: 0, label: 'Heart Rate' },
      { value: ID_EMPTY, label: 'Empty' },
      { value: 1, label: 'Steps' },
    ]);

    expect(result.map((readout) => readout.label)).toEqual(['Heart Rate', 'Steps']);
  });

  /** A row with no icon or colour still has to draw something, or its palette tile renders as a blank square. */
  test('fills in an icon and tint for a row that carries neither', () => {
    const result = buildReadoutList([{ value: 1, label: 'Steps' }]);

    expect(result[0].icon).toBe('·');
    expect(result[0].color).toBe('#999');
  });

  /** The palette files a readout under its group and the warnings read its plugin, so dropping either off the merged row hides it under Other or silences its warning. */
  test('carries the group and the plugin from the merged row', () => {
    const result = buildReadoutList([{ value: 33, label: 'Now Playing (AUDIO)', group: 'Music', plugin: 'music' }]);

    expect(result[0].group).toBe('Music');
    expect(result[0].plugin).toBe('music');
  });

  /** Clay can hand over nothing at all before the config settles, and mapping that would throw before the palette draws. */
  test('reads a missing option list as an empty palette', () => {
    const result = buildReadoutList(null);

    expect(result).toEqual([]);
  });
});

describe('sizeFor', () => {
  /** A big panel is shot at its own size, so filing it under the panel size fetches a crop of the wrong shape. */
  test('files each big panel apart from the ordinary readouts', () => {
    const result = [sizeFor(ID_SENSORS), sizeFor(ID_WIDE_DAYLIGHT), sizeFor(ID_FULL_HEART), sizeFor(ID_HEART)];

    expect(result).toEqual(['tall', 'wide', 'full', 'slot']);
  });
});

describe('thumbFor', () => {
  /** Reading the wrong size hands the panel another shot, so the tall block would render a panel-sized crop. */
  test('returns the shot filed under the label at its own size', () => {
    const result = [thumbFor(THUMBS, HEART), thumbFor(THUMBS, SENSORS)];

    expect(result).toEqual(['data:heart-slot', 'data:sensors-tall']);
  });

  /** An empty panel has no readout to photograph, and asking anyway would throw on the label lookup. */
  test('returns null for no readout', () => {
    const result = thumbFor(THUMBS, null);

    expect(result).toBeNull();
  });
});

describe('readoutById', () => {
  /** A drag carries a catalogue id, so a failed lookup is a ghost with nothing drawn in it. */
  test('finds a readout by its catalog id', () => {
    const result = readoutById([HEART, SENSORS], ID_SENSORS);

    expect(result).toBe(SENSORS);
  });

  /** An id the palette does not carry must read as nothing rather than as the first readout in the list. */
  test('returns null for an id that is not in the list', () => {
    const result = readoutById([HEART, SENSORS], 99);

    expect(result).toBeNull();
  });
});

describe('fillVisual', () => {
  /** A panel is repainted every time the slots change, and leaving the old contents stacks a second image on top of the first. */
  test('clears whatever the box held before', () => {
    const box = document.createElement('div');

    fillVisual(box, HEART, THUMBS);

    fillVisual(box, SENSORS, THUMBS);

    expect(box.querySelectorAll('img')).toHaveLength(1);
    expect(box.querySelector('img').src).toBe('data:sensors-tall');
  });

  /** An empty panel has to say so, or a cleared slot looks like one the builder failed to draw. */
  test('writes an empty marker for no readout', () => {
    const box = document.createElement('div');

    fillVisual(box, null, THUMBS);

    expect(box.querySelector('.sb-slot-empty').textContent).toBe('Empty');
  });

  /** Without the fallback an unphotographed readout draws nothing at all, which reads as a builder that failed rather than a readout with no shot. */
  test('falls back to the tint and emoji for a readout with no shot', () => {
    const box = document.createElement('div');

    fillVisual(box, UNSHOT, THUMBS);

    expect(box.querySelector('img')).toBeNull();
    expect(box.classList.contains('sb-fallback')).toBe(true);
    expect(box.querySelector('.sb-fallback-icon').textContent).toBe('$');
  });

  /** A box that fell back once and is later given a shot would keep the tint behind its image. */
  test('drops the fallback styling when a later readout has a shot', () => {
    const box = document.createElement('div');

    fillVisual(box, UNSHOT, THUMBS);

    fillVisual(box, HEART, THUMBS);

    expect(box.classList.contains('sb-fallback')).toBe(false);
    expect(box.style.background).toBe('');
  });
});
