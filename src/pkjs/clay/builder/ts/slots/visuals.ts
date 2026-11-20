/**
 * Turning a readout into something to look at: its screenshot when there is one, its emoji and
 * tint when there is not.
 *
 * The screenshots are real crops of the rendered panel, so a filled slot in the builder is a
 * picture of what the watch will actually show.
 */

import { ID_EMPTY, SHAPE_SINGLE, shapeOf } from './geometry';
import { thumbByLabel } from '../../../../../../paf/ts/clay/builder/ts/shared/thumbs';
import type { Thumbs } from '../../../../../../paf/ts/clay/builder/ts/types';
import type { PluginKind } from '../../../../../../paf/ts/clay/builder/ts/shared/plugin-warnings';

/** One readout as the builder needs it, merged from the option list and module-meta. */
export interface Readout {
  value: number;
  label: string;
  icon: string;
  color: string;
  /** Its palette group, by name. */
  group?: string;
  /** The plugin kind it reads, when it reads one. */
  plugin?: PluginKind;
}

/** The raw option rows Clay hands over on the component's config. */
interface RawOption {
  value: number | string;
  label: string;
  icon?: string;
  blockColor?: string;
  group?: string;
  plugin?: PluginKind;
}

/**
 * The palette's readouts, in config order, with Empty dropped.
 *
 * Empty has nothing to photograph and nothing to drag. Clearing a panel is what dragging one off
 * the face does, so it does not need to be in the list as well.
 *
 * @param options The option rows off the component config.
 * @return The readouts to show.
 */
export function buildReadoutList(options: RawOption[]): Readout[] {
  return (options || [])
    .map(function (option) {
      return {
        value: typeof option.value === 'string' ? parseInt(option.value, 10) : option.value,
        label: option.label,
        icon: option.icon || '·',
        color: option.blockColor || '#999',
        group: option.group,
        plugin: option.plugin,
      };
    })
    .filter(function (readout) {
      return readout.value !== ID_EMPTY;
    });
}

/** The size token a readout's thumbnail is filed under: slot for one cell, else its shape. */
export function sizeFor(id: number): string {
  const shape = shapeOf(id);

  return shape === SHAPE_SINGLE ? 'slot' : shape;
}

/**
 * A readout's screenshot, or null when it has none.
 *
 * @param thumbs The thumbnail asset.
 * @param readout The readout.
 * @return A data URI, or null.
 */
export function thumbFor(thumbs: Thumbs, readout: Readout | null): string | null {
  return readout ? thumbByLabel(thumbs, readout.label, sizeFor(readout.value)) : null;
}

/**
 * Paint a readout into a panel or a drag ghost: the screenshot if there is one, otherwise its tint
 * behind its emoji so an unphotographed readout is still recognizable.
 *
 * @param el The box to fill. Its contents are replaced.
 * @param readout The readout, or null to leave the box empty.
 * @param thumbs The thumbnail asset.
 */
export function fillVisual(el: HTMLElement, readout: Readout | null, thumbs: Thumbs): void {
  el.innerHTML = '';
  el.classList.remove('sb-fallback');
  el.style.background = '';

  if (!readout) {
    const empty = el.ownerDocument.createElement('span');

    empty.className = 'sb-slot-empty';
    empty.textContent = 'Empty';
    el.appendChild(empty);
    return;
  }

  const thumb = thumbFor(thumbs, readout);

  if (thumb) {
    const img = el.ownerDocument.createElement('img');

    img.className = 'sb-slot-img';
    img.src = thumb;
    img.alt = readout.label;
    el.appendChild(img);
    return;
  }

  el.classList.add('sb-fallback');
  el.style.background = readout.color;

  const icon = el.ownerDocument.createElement('span');

  icon.className = 'sb-fallback-icon';
  icon.textContent = readout.icon;
  el.appendChild(icon);
}

/** Look a readout up by its catalogue id. */
export function readoutById(readouts: Readout[], id: number): Readout | null {
  for (let i = 0; i < readouts.length; i++) {
    if (readouts[i].value === id) {
      return readouts[i];
    }
  }

  return null;
}
