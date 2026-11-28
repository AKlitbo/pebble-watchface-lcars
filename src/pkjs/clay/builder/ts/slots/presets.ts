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
import engineeringPresets from '../../../../../data/engineering-presets.json';
import transporterPresets from '../../../../../data/transporter-presets.json';
import { LAYOUT_ENGINEERING, LAYOUT_STARDATE, LAYOUT_TACTICAL, LAYOUT_TRANSPORTER } from './geometry';

/** preset id -> the Stardate layout's four slot ids, in slot order. */
export const SLOT_PRESETS: Record<string, number[]> = slotPresets;

/** preset id -> Tactical's twelve slot ids, in cell order. */
export const TACTICAL_PRESETS: Record<string, number[]> = tacticalPresets;

/** preset id -> Engineering's six slot ids, top to bottom. */
export const ENGINEERING_PRESETS: Record<string, number[]> = engineeringPresets;

/** preset id -> Transporter's eight slot ids, in cell order. */
export const TRANSPORTER_PRESETS: Record<string, number[]> = transporterPresets;

/**
 * A layout's preset.
 *
 * @param layout The layout.
 * @param id The preset id, such as "default".
 * @return Its slot ids, or undefined when the layout has no preset by that id.
 */
export function presetFor(layout: number, id: string): number[] | undefined {
  const presets: Record<number, Record<string, number[]>> = {
    [LAYOUT_STARDATE]: SLOT_PRESETS,
    [LAYOUT_TACTICAL]: TACTICAL_PRESETS,
    [LAYOUT_ENGINEERING]: ENGINEERING_PRESETS,
    [LAYOUT_TRANSPORTER]: TRANSPORTER_PRESETS,
  };

  return (presets[layout] || SLOT_PRESETS)[id];
}
