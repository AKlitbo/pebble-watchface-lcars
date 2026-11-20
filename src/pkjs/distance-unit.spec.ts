/**
 * Specs for opening an unsaved Distance Unit on the Steps Readout's unit.
 *
 * The Distance Goal Bar reads the Distance Unit, which opens on kilometres. A wearer on miles for
 * their steps would see the two readouts disagree. Worth pinning are that a saved pick always wins,
 * that the page and the watch both get miles while nothing is saved, and that a save from the page
 * is never changed.
 */

import { afterEach, describe, expect, test, vi } from 'vitest';
import type { ClayConfigItem } from '../../paf/ts/clay/types';
import distanceUnit, { unsavedDistanceUnit } from './distance-unit';

const KEYS = { HEALTH_STEPS_MODE: 10, HEALTH_DISTANCE_UNIT: 11 };

/** Starts the feature on a phone holding these saved settings. */
function startWith(saved: Record<string, unknown>) {
  vi.stubGlobal('localStorage', { getItem: () => JSON.stringify(saved) });

  return distanceUnit({
    messageKeys: KEYS,
    defaults: { HEALTH_STEPS_MODE: 0, HEALTH_DISTANCE_UNIT: 0 },
    queueSend: () => {},
    refetchDelayMs: 0,
    providers: null,
  });
}

/** A page with the Distance Unit row inside a section, as buildConfig lays it out. */
function page(): ClayConfigItem[] {
  return [{ type: 'section', items: [{ type: 'select', messageKey: 'HEALTH_DISTANCE_UNIT', defaultValue: 0 }] }];
}

afterEach(() => {
  vi.unstubAllGlobals();
});

describe('unsavedDistanceUnit', () => {
  /** The case this exists for. Steps in miles with the goal bar in kilometres reads as a bug on the wrist. */
  test('gives miles when Steps Readout is on miles', () => {
    const result = unsavedDistanceUnit({ HEALTH_STEPS_MODE: '1' }, {});

    expect(result).toBe(1);
  });

  /** Steps Readout's km pick is 2 and the Distance Unit's is 0, so a straight copy would land on no choice. */
  test('gives kilometres when Steps Readout is on kilometres', () => {
    const result = unsavedDistanceUnit({ HEALTH_STEPS_MODE: '2' }, {});

    expect(result).toBe(0);
  });

  /** A wearer who picked kilometres on purpose must keep it, whatever their steps show. */
  test('gives nothing once a Distance Unit is saved, even kilometres', () => {
    const result = unsavedDistanceUnit({ HEALTH_STEPS_MODE: '1', HEALTH_DISTANCE_UNIT: '0' }, {});

    expect(result).toBeNull();
  });
});

describe('page', () => {
  /** The row sits inside a section, so a walk of the top level alone would leave it on kilometres. */
  test('opens the row on miles for a wearer on miles', () => {
    const hooks = startWith({ HEALTH_STEPS_MODE: '1' });

    const result = hooks.page?.(page());

    expect(result?.[0].items?.[0].defaultValue).toBe(1);
  });
});

describe('settingsOut', () => {
  /** An update restores the watch from the phone's saved settings, which hold no Distance Unit yet. */
  test('adds miles to a restore that carries no Distance Unit', () => {
    const hooks = startWith({ HEALTH_STEPS_MODE: '1' });
    const dict: AppMessageDict = { [KEYS.HEALTH_STEPS_MODE]: '1' };

    hooks.settingsOut?.(dict);

    expect(dict[KEYS.HEALTH_DISTANCE_UNIT]).toBe(1);
  });

  /** The page always sends the row, so what the wearer picked there goes to the watch unchanged. */
  test('leaves a Distance Unit the dict already carries', () => {
    const hooks = startWith({ HEALTH_STEPS_MODE: '1' });
    const dict: AppMessageDict = { [KEYS.HEALTH_STEPS_MODE]: '1', [KEYS.HEALTH_DISTANCE_UNIT]: '0' };

    hooks.settingsOut?.(dict);

    expect(dict[KEYS.HEALTH_DISTANCE_UNIT]).toBe('0');
  });
});
