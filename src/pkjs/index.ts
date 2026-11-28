/**
 * PebbleKit JS entry point.
 *
 * Thin wrapper over the shared bootstrap (see paf/ts/pkjs/app.ts). This face
 * opts into weather with coordinates, formatted in LCARS dash style into separate
 * latitude/longitude keys. Everything else is shared.
 */
import app from '../../paf/ts/pkjs/app';
import weather from '../../paf/ts/weather/feature';
import calendar from '../../paf/ts/calendar/feature';
import music from '../../paf/ts/music/feature';
import phone from '../../paf/ts/phone/feature';
import type { WeatherResult } from '../../paf/ts/weather/util';
import hiddenStoreComponent from '../../paf/ts/clay/hidden-store-component';
import clayConfig from './config';
import slotComponent from './clay/slot-component.g';
import slotLayout from './clay/slot-layout-fn';
import distanceUnit from './distance-unit';

/**
 * Formats a decimal coordinate in LCARS dash style.
 * E.g. 33.448376 -> "33-448", -112.074036 -> "-112-074". Missing -> "".
 */
function fmtCoord(v: number | undefined): string {
  if (typeof v !== 'number' || Number.isNaN(v)) {
    return '';
  }

  const prefix = v < 0 ? '-' : '';

  return prefix + Math.abs(v).toFixed(3).replace('.', '-');
}

app.startPebbleApp({
  clayConfig,
  // the slot builder plus the hidden stores holding the panels it does not own
  components: [slotComponent, hiddenStoreComponent],
  // tells the slot builder which layout the Layout picker holds
  customClay: slotLayout,
  // dash style into two keys. fmtCoord yields '' for a missing coordinate
  features: [
    weather.withCoords((messageKeys: Record<string, number>, result: WeatherResult) => ({
      [messageKeys.LOCATION_LATITUDE]: fmtCoord(result.lat),
      [messageKeys.LOCATION_LONGITUDE]: fmtCoord(result.lon),
    })),
    calendar,
    // the art fills the full Now Playing panel's square
    music.withArtwork({ w: 48, h: 48 }),
    // the phone's battery, for its panel and the rail gauges
    phone,
    // a Distance Unit never saved starts on the Steps Readout unit, so the Distance Goal Bar matches the steps
    distanceUnit,
  ],
});
