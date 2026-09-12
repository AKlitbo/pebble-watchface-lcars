# Third-party notices

LCARS Stardate is under the PolyForm Noncommercial License, see [LICENSE](LICENSE). The shared
engine it builds on (`lib/`) is used under the PolyForm option of its dual licence. The face bundles
the third-party work below, each of which keeps its own licence. This file is how those licences are
passed on to anyone the watchface or this repository is given to.

For who made these and what they are used for, see the Credits section of the [README](README.md).

## Code

### ical.js

The phone-side calendar reader in the shared PebbleKit JS bundle. LCARS Stardate surfaces no iCal
panel, but the shared bundle carries the reader, so it ships in the built `.pbw`.

- **Source:** <https://github.com/kewisch/ical.js>
- **Licence:** Mozilla Public License 2.0, published at <https://www.mozilla.org/en-US/MPL/2.0/>.
  The full text also ships in the package at `node_modules/ical.js/LICENSE`
- **What ships:** the package's own prebuilt `dist/ical.es5.min.cjs`, copied in unchanged by the
  pkjs build. It is not modified, patched or re-bundled, so the source that produced it is the
  upstream repository above, and its licence header travels inside the bundle

The MPL covers ical.js and nothing else here. Section 1.10 of that licence puts it plainly: a file
carrying none of its code is not a modification of it, so the rest of this project stays under the
PolyForm terms. Keeping ical.js in a file of its own rather than mixing it into ours is deliberate,
and is what section 1.7 asks of a larger work.

## Fonts

The face bundles its `.ttf` files under `watchfaces/lcars-stardate/resources/fonts/`, and the build
converts them into the watch's own font format, so both this repository and the built watchface
carry them.

- **[Antonio](https://fonts.google.com/specimen/Antonio)**: Copyright The Antonio Project Authors,
  with Reserved Font Name "Antonio". SIL Open Font License 1.1, published at
  <https://openfontlicense.org>. Licence text in
  [OFL.txt](watchfaces/lcars-stardate/resources/fonts/OFL.txt)

## Icons

The SVG sources for the weather and glyph icons are not in this repository. They are fetched to
regenerate the icons, and only the rendered PNGs are bundled. See `resources/icons.json`.

- **[Weather Icons by Erik Flowers](https://github.com/erikflowers/weather-icons)**: SIL Open Font
  License 1.1 for the font, MIT for the code. Rendered PNGs bundled
- **[UXWing](https://uxwing.com)** (the health, weather, time and system glyphs): the
  [UXWing licence](https://uxwing.com/license/), which allows use without attribution but does not
  allow redistributing the icons themselves. SVG sources fetched separately, rendered PNGs bundled
- **[SVG Repo](https://www.svgrepo.com)** (the bluetooth glyphs):
  [CC Attribution](https://www.svgrepo.com/page/licensing/#CC%20Attribution). Rendered PNGs bundled

## Template

- **LCARS Inspired Website Template by [TheLCARS.com](https://www.thelcars.com)**: the HTML/CSS the
  LCARS Stardate frame backgrounds are baked from, used with modifications. Fetched separately and
  not bundled. Please visit the site to download and support the creator.

## Trademarks

*Star Trek*, LCARS, and related marks are trademarks of CBS / Paramount Global. This face is an
unaffiliated, noncommercial homage and is not endorsed by or associated with those rights holders.
No trademark licence is granted or implied.

Refer to each source above for the full terms.
