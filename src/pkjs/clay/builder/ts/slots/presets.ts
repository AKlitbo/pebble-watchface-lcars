/**
 * The starter arrangements behind the preset buttons, one map for each layout, which for now is just
 * the one each layout ships with.
 *
 * Kept as data next to the face rather than in here, so a new arrangement is a JSON edit. esbuild
 * inlines the import when the component is bundled, which is what lets the config webview read it
 * without reaching for a file it cannot see.
 */

import slotPresets from '../../../../../data/slot-presets.json';
import tacticalPresets from '../../../../../data/tactical-presets.json';
import { LAYOUT_TACTICAL } from './geometry';

/** preset id -> the Stardate layout's four slot ids, in slot order. */
export const SLOT_PRESETS: Record<string, number[]> = slotPresets;

/** preset id -> Tactical's twelve slot ids, in cell order. */
export const TACTICAL_PRESETS: Record<string, number[]> = tacticalPresets;

/**
 * A layout's preset.
 *
 * @param layout The layout.
 * @param id The preset id, such as "default".
 * @return Its slot ids, or undefined when the layout has no preset by that id.
 */
export function presetFor(layout: number, id: string): number[] | undefined {
  return (layout === LAYOUT_TACTICAL ? TACTICAL_PRESETS : SLOT_PRESETS)[id];
}
