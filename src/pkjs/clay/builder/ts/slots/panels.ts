/**
 * What the framework's palette and plugin warnings are told about this face's readouts: the size
 * tabs, the group tabs, and the name a warning calls a panel by.
 *
 * Which group a readout sits in and which plugin it reads ride on its row in module-meta, beside its
 * icon and tint, so each readout is described in one place.
 */

import { SHAPE_FULL, SHAPE_SINGLE, SHAPE_TALL, SHAPE_WIDE, spanOf } from './geometry';
import type { Shape } from './geometry';
import type { PaletteGroup } from '../../../../../../paf/ts/clay/builder/ts/shared/palette';

// the size tabs' order, smallest first, with the wide panel ahead of the tall block
const SIZE_ORDER: Shape[] = [SHAPE_SINGLE, SHAPE_WIDE, SHAPE_TALL, SHAPE_FULL];

/**
 * The palette's size tabs, in tab order, each named width by height, so a panel filling a row is 2x1
 * and the tall block filling two cells of a column is 1x2. A size two columns wide spans the grid, so
 * the palette gives its tiles twice the room.
 */
export const PALETTE_SIZES: Array<{ shape: Shape; key: string; wide: boolean }> = SIZE_ORDER.map(function (shape) {
  const span = spanOf(shape);

  return { shape: shape, key: span.cols + 'x' + span.rows, wide: span.cols === 2 };
});

/**
 * The palette's group tabs, in tab order. A readout's group is the name here that its module-meta row
 * carries, and one with a name not listed shows under the palette's own Other.
 */
export const PALETTE_GROUPS: PaletteGroup[] = [
  { name: 'Health', icon: '❤️' },
  { name: 'Weather', icon: '🌤' },
  { name: 'Sky', icon: '🌗' },
  { name: 'Clock', icon: '⏰' },
  { name: 'Date', icon: '📅' },
  { name: 'Calendar', icon: '🗓' },
  { name: 'Music', icon: '🎵' },
  { name: 'Status', icon: '🔋' },
];

/**
 * The palette size key a shape is filed under.
 *
 * @param shape The shape.
 * @return Its key, such as "2x1" for a wide panel.
 */
export function sizeKeyOf(shape: Shape): string {
  for (let index = 0; index < PALETTE_SIZES.length; index++) {
    if (PALETTE_SIZES[index].shape === shape) {
      return PALETTE_SIZES[index].key;
    }
  }

  return PALETTE_SIZES[0].key;
}

/**
 * The name a plugin warning calls a readout by: its label without the LCARS word in brackets, so
 * the warning reads "Now Playing needs a music plugin" rather than naming the bar's label too.
 *
 * @param label The readout's label.
 * @return The label up to its bracketed word.
 */
export function warningName(label: string): string {
  return label.replace(/\s*\([^)]*\)\s*$/, '');
}
