/**
 * Tells the slot builder which layout the Layout picker holds, so it draws that layout's grid and
 * edits that layout's picks.
 *
 * Clay hands a component nothing but its own row, so the builder cannot read the picker itself. This
 * is the face's customClay, which Clay runs with the whole page as `this`. Once the page is built it
 * reads `APPEARANCE_LAYOUT` and hands the value to the `_sbLayout` hook the builder hangs on its own
 * element, then again whenever the picker changes.
 *
 * Clay serializes this and runs it again inside the config webview, so it reaches nothing outside
 * itself: no imports and no module-scope constants. The two message keys are written out here.
 */

/** The builder's element, with the hook it takes the layout through. */
type HookedElement = HTMLElement & { _sbLayout?: (layout: string) => void };

/** The bits of a Clay row this reaches. */
interface ClayRow {
  $element: ArrayLike<HookedElement | undefined>;
  get(): unknown;
  on(event: string, handler: () => void): void;
}

/** The bits of the Clay config page this reaches. */
interface ClayPage {
  EVENTS: { AFTER_BUILD: string };
  on(event: string, handler: () => void): void;
  getItemByMessageKey(messageKey: string): ClayRow | undefined;
}

/**
 * Hands the picked layout to the slot builder once the page is built, and again on every change.
 * A page missing the picker or the builder is left as it is, and the builder stays on Stardate.
 */
export default function slotLayout(this: ClayPage): void {
  this.on(this.EVENTS.AFTER_BUILD, () => {
    const picker = this.getItemByMessageKey('APPEARANCE_LAYOUT');
    const builder = this.getItemByMessageKey('APPEARANCE_SLOT_LEFT_TOP');
    const host = builder && builder.$element ? builder.$element[0] : undefined;

    if (!picker || !host || typeof host._sbLayout !== 'function') {
      return;
    }

    const push = (): void => {
      if (typeof host._sbLayout === 'function') {
        host._sbLayout(String(picker.get()));
      }
    };

    picker.on('change', push);
    push();
  });
}
