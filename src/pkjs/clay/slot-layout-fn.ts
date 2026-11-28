/**
 * Tells the slot builder which layout the Layout picker holds, so it draws that layout's grid and
 * edits that layout's picks, and keeps each layout's Mirrored switch in step with it.
 *
 * Clay hands a component nothing but its own row, so the builder cannot read the picker itself. This
 * is the face's customClay, which Clay runs with the whole page as `this`. Once the page is built it
 * reads `APPEARANCE_LAYOUT` and the picked layout's Mirrored switch, and hands the builder the pick the
 * watch reads, the layout times two plus one when mirrored. It does that again whenever either changes.
 *
 * The picker saves the plain layouts. An odd pick reads on the watch as that layout mirrored, so one
 * found as the page opens is turned into its plain layout with that layout's Mirrored switch on, and
 * the picker's odd options come off the list.
 *
 * Clay serializes this and runs it again inside the config webview, so it reaches nothing outside
 * itself: no imports and no module-scope constants. The message keys are written out here.
 */

/** The builder's element, with the hook it takes the layout through. */
type HookedElement = HTMLElement & { _sbLayout?: (layout: string) => void };

/** The bits of a Clay row this reaches. */
interface ClayRow {
  $element: ArrayLike<HookedElement | undefined>;
  get(): unknown;
  set?(value: unknown): void;
  on(event: string, handler: () => void): void;
}

/** The bits of the Clay config page this reaches. */
interface ClayPage {
  EVENTS: { AFTER_BUILD: string };
  on(event: string, handler: () => void): void;
  getItemByMessageKey(messageKey: string): ClayRow | undefined;
}

/**
 * Turns an odd pick into its layout's Mirrored switch once the page is built, then hands the picked
 * layout to the slot builder, and again on every change. A page missing the picker is left as it is.
 */
export default function slotLayout(this: ClayPage): void {
  this.on(this.EVENTS.AFTER_BUILD, () => {
    const picker = this.getItemByMessageKey('APPEARANCE_LAYOUT');

    if (!picker) {
      return;
    }

    // each layout's Mirrored switch, by the layout's number. Transporter has a rail down each side
    // and no switch
    const switches = [
      this.getItemByMessageKey('APPEARANCE_MIRROR_STARDATE'),
      this.getItemByMessageKey('APPEARANCE_MIRROR_TACTICAL'),
      this.getItemByMessageKey('APPEARANCE_MIRROR_ENGINEERING'),
    ];

    const saved = parseInt(String(picker.get()), 10) || 0;
    const savedSwitch = switches[Math.floor(saved / 2)];

    if (saved % 2 === 1 && picker.set) {
      picker.set(saved - 1);

      if (savedSwitch && savedSwitch.set) {
        savedSwitch.set(true);
      }
    }

    // the mirrored picks are only listed so a saved one could be read above. off the list they go, so
    // the picker offers the plain layouts and the switches do the mirroring
    const row = picker.$element ? picker.$element[0] : undefined;
    const options = row && row.querySelectorAll ? row.querySelectorAll('option') : [];

    Array.prototype.slice.call(options).forEach((option: HTMLOptionElement) => {
      if ((parseInt(option.value, 10) || 0) % 2 === 1 && option.parentNode) {
        option.parentNode.removeChild(option);
      }
    });

    // the rest only matters with the builder on the page to tell
    const builder = this.getItemByMessageKey('APPEARANCE_SLOT_LEFT_TOP');
    const host = builder && builder.$element ? builder.$element[0] : undefined;

    if (!host || typeof host._sbLayout !== 'function') {
      return;
    }

    const push = (): void => {
      const layout = Math.floor((parseInt(String(picker.get()), 10) || 0) / 2);
      const mirror = switches[layout];
      const mirrored = Boolean(mirror) && String(mirror ? mirror.get() : '') === 'true';

      if (typeof host._sbLayout === 'function') {
        host._sbLayout(String(layout * 2 + (mirrored ? 1 : 0)));
      }
    };

    picker.on('change', push);
    switches.forEach((mirror) => {
      if (mirror) {
        mirror.on('change', push);
      }
    });
    push();
  });
}
