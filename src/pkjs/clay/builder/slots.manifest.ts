/**
 * Build manifest for the slotBuilder component. The clay builder generator reads this to stitch
 * the pieces below into the self contained slot-component.g.js Clay ships into its config webview.
 *
 * Piece order only groups the banners for reading. Function declarations hoist across the
 * assembled initialize, so order never changes behaviour.
 *
 * The pieces under ts/slots are face-local. This face belongs to no family, so the generator's core
 * overlay never fires, and a piece reaching for a family core would not resolve. The palette and the
 * plugin warnings come from the framework, which the pieces import, and css/palette.css is found
 * there too since the face has none of its own.
 */

import type { Manifest } from '../../../../paf/tools/clay-components/generate-components.ts';

export default {
  name: 'slotBuilder',
  hookPrefix: '_sb',
  template: 'html/slots.html',
  // the font first so the page theme can name it
  // then the framework palette's layout, and the builder's own chrome with the palette's colours
  // then the LCARS dressing this face puts over the whole settings page
  styles: ['css/antonio.g.css', 'css/palette.css', 'css/slots.css', 'css/page.css'],
  pieces: [
    'ts/slots/geometry',
    'ts/slots/codec',
    'ts/slots/presets',
    'ts/slots/panels',
    'ts/slots/visuals',
    'ts/slots/drag',
    'ts/slots/init',
  ],
  output: 'src/pkjs/clay/slot-component.g.js',
  doc: [
    'Clay custom component for the drag and drop ops slot builder.',
    '',
    'Shows a sketch of the watch face with its four panels as drop targets, and a',
    'palette of every readout below it, by size and by group. Drag a readout onto a',
    'panel, or drag one panel onto another to swap them.',
    '',
    'The value it round trips is the upper-left panel only, which is this',
    'component\'s own message key. The other three ride hidden stores on the same',
    'page, because a Clay component owns exactly one key.',
    '',
    'IMPORTANT: initialize and the manipulator run inside the Clay config webview,',
    'a separate JS context. They must be self contained, which is why the generator',
    'inlines every piece below into one initialize.',
  ],
} satisfies Manifest;
