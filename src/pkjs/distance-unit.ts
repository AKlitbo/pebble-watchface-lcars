/**
 * Opens the Distance Unit on the unit Steps Readout uses, until the wearer saves one of their own.
 *
 * The framework's Distance Unit row opens on kilometres. A wearer whose Steps Readout already
 * shows miles would then see the Distance Goal Bar in kilometres beside their steps in miles, so
 * while the phone has no Distance Unit saved, the page opens on miles for them and the watch is sent
 * miles. Once the wearer saves the page, their own pick rules and this does nothing.
 */

import type { Feature, FeatureContext, FeatureHooks } from '../../paf/ts/pkjs/feature';
import type { ClayConfigItem } from '../../paf/ts/clay/types';
import { getConfig, readChoice, readValue } from '../../paf/ts/pkjs/settings-store';

/** Steps Readout's pick for the distance walked in miles, as `STEPS_MODE_MILES` on the watch. */
const STEPS_MODE_MILES = 1;

/** The Distance Unit's two picks, as `DISTANCE_UNIT_KM` and `DISTANCE_UNIT_MILES` on the watch. */
const DISTANCE_UNIT_KM = 0;
const DISTANCE_UNIT_MILES = 1;

/**
 * The Distance Unit a wearer who never saved one gets.
 *
 * @param saved The settings the phone keeps, by name.
 * @param defaults The page's defaults, by name.
 * @return Miles when Steps Readout is on miles, kilometres otherwise, or null once a Distance Unit
 *   is saved.
 */
export function unsavedDistanceUnit(saved: Record<string, unknown>, defaults: Record<string, unknown>): number | null {
  if (readValue(saved.HEALTH_DISTANCE_UNIT, undefined) !== undefined) {
    return null;
  }

  const stepsMode = readChoice(saved.HEALTH_STEPS_MODE, readChoice(defaults.HEALTH_STEPS_MODE, 0));

  return stepsMode === STEPS_MODE_MILES ? DISTANCE_UNIT_MILES : DISTANCE_UNIT_KM;
}

/** Sets the Distance Unit row's opening value, wherever on the page it sits. */
function openOn(items: ClayConfigItem[], unit: number): void {
  items.forEach((item) => {
    if (item.messageKey === 'HEALTH_DISTANCE_UNIT') {
      item.defaultValue = unit;
    }

    if (item.items) {
      openOn(item.items, unit);
    }
  });
}

/**
 * The page hook and the send hook that keep an unsaved Distance Unit on the Steps Readout's unit.
 *
 * @param context What the app shares with every feature.
 * @return The hooks.
 */
const distanceUnit: Feature = (context: FeatureContext): FeatureHooks => ({
  page(config: ClayConfigItem[]): ClayConfigItem[] {
    const unit = unsavedDistanceUnit(getConfig(), context.defaults);

    if (unit !== null) {
      openOn(config, unit);
    }

    return config;
  },

  // a restore sends only what the phone has saved, so a wearer updating from a release with no
  // Distance Unit would leave the watch on kilometres. a save from the page always carries the row
  settingsOut(dict: AppMessageDict): void {
    const key = context.messageKeys.HEALTH_DISTANCE_UNIT;
    const unit = unsavedDistanceUnit(getConfig(), context.defaults);

    if (key === undefined || unit === null || key in dict) {
      return;
    }

    dict[key] = unit;
  },
});

export default distanceUnit;
