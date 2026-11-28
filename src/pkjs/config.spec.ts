/**
 * Specs for the face's settings page layout.
 *
 * Each layout's own settings sit in a section that shows only while that layout is picked. Worth
 * pinning is that the rows the framework builds elsewhere, which only one layout uses, end up in that
 * layout's section, since nothing else notices a row left behind where it does nothing.
 */

import { describe, test, expect } from 'vitest';
import config from './config';
import { listHolding } from '../../paf/ts/clay/rows';

describe('the settings page', () => {
  /** Only the Stardate layout draws the connection and Quiet Time glyphs, so their switches left in Bluetooth and Appearance would sit there doing nothing on the other layouts' pages. */
  test('puts the two glyph switches in Stardate Settings', () => {
    const stardate = listHolding(config, (item) => item.messageKey === 'APPEARANCE_MIRROR_STARDATE');

    const result = (stardate || []).map((item) => item.messageKey || item.type);

    expect(result).toEqual(['heading', 'APPEARANCE_MIRROR_STARDATE', 'CONNECTION_BLUETOOTH_ICON', 'APPEARANCE_QUIET_TIME_ICON']);
  });
});
