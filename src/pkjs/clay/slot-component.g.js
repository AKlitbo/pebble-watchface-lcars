// generated from src/pkjs/clay/builder/slots.manifest.ts by the Clay generator
// do not edit by hand: run `paf gen <face> clay` after changing the sources
/**
 * Clay custom component for the drag and drop ops slot builder.
 *
 * Shows a sketch of the watch face with its four panels as drop targets, and a
 * palette of every readout below it, by size and by group. Drag a readout onto a
 * panel, or drag one panel onto another to swap them.
 *
 * The value it round trips is the upper-left panel only, which is this
 * component's own message key. The other three ride hidden stores on the same
 * page, because a Clay component owns exactly one key.
 *
 * IMPORTANT: initialize and the manipulator run inside the Clay config webview,
 * a separate JS context. They must be self contained, which is why the generator
 * inlines every piece below into one initialize.
 */
module.exports = {
  name: "slotBuilder",

  template: "<div class=\"component sb\">  <div class=\"sb-header\">    <p class=\"sb-desc\">Drag a readout onto a panel.<br>Drag one panel onto another to swap them.<br>On Tactical, the clock and the date are panels too.</p>  </div>  <div class=\"sb-face-wrap\">    <div class=\"sb-face\">      <div class=\"sb-rail\">        <div class=\"sb-rail-block sb-rail-1\"></div>        <div class=\"sb-rail-block sb-rail-2\"></div>        <div class=\"sb-rail-block sb-rail-top\"></div>        <div class=\"sb-pill sb-pill-1\"></div>        <div class=\"sb-pill sb-pill-2\"></div>        <div class=\"sb-rail-block sb-rail-bottom\"></div>      </div>      <div class=\"sb-slots\"></div>    </div>    <div class=\"sb-actions\">      <button type=\"button\" class=\"sb-btn sb-preset\" data-preset=\"default\">Default</button>      <button type=\"button\" class=\"sb-btn sb-btn-clear\">Clear</button>    </div>  </div>  <div class=\"sb-palette\"></div>  <input type=\"hidden\" class=\"sb-value\"></div>",

  style: "@font-face{font-family:'LcarsAntonio';font-style:normal;font-weight:700;font-display:swap;src:url(data:font/woff2;base64,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) format('woff2')}.pf-palette{padding:0 var(--pf-palette-gutter,12px);margin-top:8px;width:100%;box-sizing:border-box}.pf-palette-sizes{display:flex;gap:4px;position:relative;z-index:1}.pf-palette-size{flex:1;min-width:0;padding:9px 4px;border:none;border-radius:0;font-size:10px;text-transform:uppercase;margin-bottom:-1px}.pf-palette-groups{display:flex;flex-wrap:wrap;gap:3px;margin-bottom:10px}.pf-palette-groups[hidden]{display:none}.pf-palette-group{flex:1 1 auto;display:flex;align-items:center;justify-content:center;gap:4px;min-width:0 !important;margin:0 !important;padding:5px 6px 3px !important;border:none;border-bottom:2px solid;border-radius:0;font-size:9px;text-transform:uppercase}.pf-palette-group-icon{font-size:12px;line-height:1}.pf-palette-group-name{white-space:nowrap}.pf-palette-box{background:var(--pf-palette-panel,transparent);border-radius:0;padding:10px 8px;border:1px solid var(--pf-palette-line,rgba(128,128,128,0.4))}.pf-palette-status{display:flex;align-items:center;justify-content:space-between;gap:8px;margin-top:10px;padding-top:8px;border-top:1px solid var(--pf-palette-line,rgba(128,128,128,0.4));font-size:11px;font-weight:700;text-transform:uppercase;letter-spacing:1px;font-family:ui-monospace,'Cascadia Mono',Menlo,Consolas,monospace}.pf-palette-all{flex-shrink:0;min-width:0 !important;margin:0 !important;padding:4px 8px !important;border:1px solid;border-radius:0;font:inherit}.pf-palette-tiles{display:flex;flex-wrap:wrap;justify-content:center;gap:8px}.pf-palette-tile{min-height:42px;flex-shrink:0;box-sizing:border-box;padding:2px;border-radius:0;display:flex;flex-direction:column;align-items:center;justify-content:center;cursor:grab;touch-action:none;position:relative;box-shadow:inset 0 0 0 1px rgba(255,255,255,0.1);transition:opacity 0.2s}.pf-palette-tile.narrow{width:calc((100% - 24px) / 4);max-width:94px}.pf-palette-tile.wide{width:calc((100% - 8px) / 2);max-width:192px}.pf-palette-tile:active{opacity:0.7}.pf-palette-tile.placed{opacity:0.3;pointer-events:none}.pf-palette-icon{font-size:20px;line-height:1;color:#fff}.pf-palette-shot{width:100%;height:auto;display:block}.sb{--sb-slot-w:100px;--sb-slot-h:44px;--sb-gap:4px;--sb-strip-h:6px;--sb-label:#ffaa00;--pf-palette-gutter:0px;--pf-palette-panel:#0a0a0a;--pf-palette-line:#3a3a44}.sb-header{margin:0 0 10px}.sb-desc{margin:0;font-size:13px;line-height:1.4;opacity:0.75}.sb-face-wrap{display:flex;justify-content:center;gap:8px;margin-bottom:12px}.sb-face{display:flex;gap:var(--sb-gap);padding:6px;background:#000;border-radius:6px;user-select:none;-webkit-user-select:none;touch-action:none;flex:0 1 auto;min-width:0;max-width:calc(var(--sb-slot-w) * 2 + var(--sb-gap) * 2 + 32px)}.sb-rail{display:flex;flex-direction:column;gap:2px;width:20px}.sb-rail-block{flex:1;border-radius:2px}.sb-rail-1{background:#ff5500}.sb-rail-2{background:#ffaa55}.sb-rail-top,.sb-rail-bottom,.sb-pill{display:none}.sb-face.sb-tactical .sb-rail{display:grid;grid-template-rows:var(--sb-slot-h) var(--sb-slot-h) var(--sb-strip-h) var(--sb-slot-h) var(--sb-slot-h) var(--sb-strip-h) var(--sb-slot-h) var(--sb-slot-h);gap:var(--sb-gap)}.sb-face.sb-tactical .sb-rail-1,.sb-face.sb-tactical .sb-rail-2{display:none}.sb-face.sb-tactical .sb-rail-top,.sb-face.sb-tactical .sb-rail-bottom,.sb-face.sb-tactical .sb-pill{display:block}.sb-face.sb-tactical .sb-rail-top{grid-row:1 / span 2;background:#55aaff}.sb-face.sb-tactical .sb-rail-bottom{grid-row:7 / span 2;background:#aa5555}.sb-face.sb-tactical .sb-pill{align-self:center;height:18px;border-radius:9px}.sb-face.sb-tactical .sb-pill-1{grid-row:4;background:#aa5555}.sb-face.sb-tactical .sb-pill-2{grid-row:5;background:#ff5500}.sb-slots{display:grid;grid-template-columns:minmax(0,var(--sb-slot-w)) minmax(0,var(--sb-slot-w));grid-template-rows:var(--sb-slot-h) var(--sb-slot-h);gap:var(--sb-gap);min-width:0;flex:0 1 auto}.sb-slot{position:relative;background:#111;border:1px dashed #444;border-radius:3px;overflow:hidden;cursor:grab;display:flex;align-items:center;justify-content:center}.sb-slot.filled{border-style:solid;border-color:#222}.sb-slot.over{border-color:var(--sb-label);border-style:solid;box-shadow:0 0 0 2px rgba(255,170,0,0.35)}.sb-slot.blocked{opacity:0.35}.sb-slot-img{width:100%;height:100%;object-fit:contain;pointer-events:none;image-rendering:pixelated}.sb-slot-empty{color:#555;font-size:11px;letter-spacing:0.5px;pointer-events:none}.sb-actions{display:flex;flex-direction:column;justify-content:flex-start;gap:5px;width:84px;box-sizing:border-box;padding-left:8px;border-left:1px solid #444}.sb-btn{width:100%;min-width:0;box-sizing:border-box;margin:0;padding:5px 4px;font-size:12px;line-height:1.2;border:1px solid #666;border-radius:9px;background:transparent;color:inherit;cursor:pointer}.sb-preset{border-color:#aa55ff;color:#ccaaff}.sb-btn-clear{border-color:#aa5555;color:#ffaaaa}.sb-fallback{color:#fff}.sb-fallback-icon{font-size:16px}.sb .pf-palette-size{background:#16161c;color:#ffcc88;font-family:'LcarsAntonio','Arial Narrow',sans-serif;font-size:13px;letter-spacing:1px;cursor:pointer}.sb .pf-palette-size.active{background:#ffaa55;color:#000}.sb .pf-palette-group{background:transparent;color:#8a8a9a;border-bottom-color:#3a3a44;cursor:pointer}.sb .pf-palette-group[aria-pressed='true']{color:#ffaa55;border-bottom-color:#ffaa55}.sb .pf-palette-status{color:#ffcc88}.sb .pf-palette-all{background:transparent;color:#ffaa55;border-color:#ffaa55;cursor:pointer}.sb .pf-palette-shot{image-rendering:pixelated}.sb-ghost{position:fixed;z-index:9999;width:var(--sb-slot-w);height:var(--sb-slot-h);pointer-events:none;opacity:0.85;border-radius:3px;overflow:hidden;background:#000;box-shadow:0 2px 8px rgba(0,0,0,0.5)}.sb-ghost.tall{height:calc(var(--sb-slot-h) * 2 + var(--sb-gap))}.sb-ghost.wide{width:calc(var(--sb-slot-w) * 2 + var(--sb-gap))}.sb-ghost.full{width:calc(var(--sb-slot-w) * 2 + var(--sb-gap));height:calc(var(--sb-slot-h) * 2 + var(--sb-gap))}.sb-ghost img{width:100%;height:100%;object-fit:contain;image-rendering:pixelated}html,html body{background:#000;color:#fff}:root{--pf-accent:#ffaa55;--pf-on-accent:#000;--pf-note-accent:#ffaa55;--pf-note-info-accent:#aaaaff;--pf-surface:#16161c;--pf-ink:#ffcc88;--pf-line:#3a3a44}body .section{position:relative;background:#0a0a0a;border:none;border-left:10px solid #aa55ff;border-radius:0 10px 10px 0;margin:0 6px 14px;padding:10px 12px 12px}body .section:nth-of-type(2){border-left-color:#ffaa55}body .section:nth-of-type(3){border-left-color:#55aaff}body .section:nth-of-type(4){border-left-color:#ff5500}body .section:nth-of-type(5){border-left-color:#aaaaff}body .section:nth-of-type(6){border-left-color:#aa5555}body .component.component-heading,body .section>.component.component-heading{position:relative;background:transparent;border:none;padding:0;margin:0 0 2px;overflow:visible}body .component.component-heading::after,body .section>.component.component-heading::after{content:'';display:block;position:absolute;left:0;right:0;top:7px;bottom:auto;height:11px;background:#aaaaff;border-radius:6px;z-index:0}body .inputs>.component.component-heading{margin-top:14px}body .inputs>.component.component-text{margin-bottom:16px}body .component.component-heading h4,body .component.component-heading h5{position:relative;display:inline-block;margin:0;padding:0 8px 0 0;background:#0a0a0a;color:#ffaa00;font-family:'LcarsAntonio','Arial Narrow',sans-serif;font-size:21px;font-weight:700;letter-spacing:1.5px;line-height:25px;text-transform:uppercase;z-index:1}body .component.component-heading h5{color:#ffcc99;font-size:16px}body .component.component-text p{color:#c8c8d8;font-size:13px;line-height:1.45}body .component .label{color:#ffaa55;font-family:'LcarsAntonio','Arial Narrow',sans-serif;font-size:16px;letter-spacing:0.8px;text-transform:uppercase}body .component .description{color:#8a8a9a;font-size:12px;line-height:1.4;text-transform:none}body .component select,body .component input[type='text'],body .component input[type='number']{background:#16161c;color:#ffcc88;border:1px solid #3a3a44;border-radius:4px;font-family:'LcarsAntonio','Arial Narrow',sans-serif;font-size:16px;letter-spacing:0.5px}body .component-submit button,body button[type='submit']{background:#ff5500;color:#000;border:none;border-radius:18px;padding:11px 24px;font-family:'LcarsAntonio','Arial Narrow',sans-serif;font-size:19px;font-weight:700;letter-spacing:1.4px;text-transform:uppercase}body .sb-btn{font-family:'LcarsAntonio','Arial Narrow',sans-serif;font-size:13px;letter-spacing:1px;text-transform:uppercase}body .pf-loc-search .pf-loc-note{background:#16161c !important;border-left:6px solid #ffaa55 !important;border-radius:0 8px 8px 0 !important;margin:10px 0 2px !important;padding:8px 12px 8px 38px !important;color:#ffcc88 !important;font-family:'LcarsAntonio','Arial Narrow',sans-serif !important;font-size:14px !important;letter-spacing:0.8px !important;line-height:1.35 !important;text-transform:uppercase !important}body .pf-loc-search .pf-loc-note:before{top:8px !important;left:14px !important;width:16px !important;height:16px !important;background:#ffaa55 !important;border:none !important;border-radius:2px !important;color:#000 !important;font-family:'LcarsAntonio','Arial Narrow',sans-serif !important;font-size:14px !important;font-weight:700 !important;line-height:16px !important}body .section>.component:last-child,body .section>.component:last-child .description{padding-bottom:0}body .section>.component:not(:has(~ .component:not(.hide))),body .section>.component:not(:has(~ .component:not(.hide))) .description{padding-bottom:0}body .section>.component:first-child:not(.component-heading){margin-top:0;padding-top:0}body .pf-theme{display:flex;flex-direction:column}body .pf-theme>label{order:1}body .pf-theme .pf-theme-desc{order:2;padding-bottom:0}body .pf-theme .pf-theme-preview{order:3;margin-top:0.5rem}",

  manipulator: {
    set: function (value) {
      this.$element[0]._sbSet(value || '');
    },
    get: function () {
      return this.$element[0]._sbGet();
    }
  },

  initialize: function () {
    var __clayComponent = (() => {
      var __defProp = Object.defineProperty;
      var __getOwnPropDesc = Object.getOwnPropertyDescriptor;
      var __getOwnPropNames = Object.getOwnPropertyNames;
      var __hasOwnProp = Object.prototype.hasOwnProperty;
      var __esm = (fn, res, err) => function __init() {
        if (err) throw err[0];
        try {
          return fn && (res = (0, fn[__getOwnPropNames(fn)[0]])(fn = 0)), res;
        } catch (e) {
          throw err = [e], e;
        }
      };
      var __commonJS = (cb, mod) => function __require() {
        try {
          return mod || (0, cb[__getOwnPropNames(cb)[0]])((mod = { exports: {} }).exports, mod), mod.exports;
        } catch (e) {
          throw mod = 0, e;
        }
      };
      var __export = (target, all) => {
        for (var name in all)
          __defProp(target, name, { get: all[name], enumerable: true });
      };
      var __copyProps = (to, from, except, desc) => {
        if (from && typeof from === "object" || typeof from === "function") {
          for (let key of __getOwnPropNames(from))
            if (!__hasOwnProp.call(to, key) && key !== except)
              __defProp(to, key, { get: () => from[key], enumerable: !(desc = __getOwnPropDesc(from, key)) || desc.enumerable });
        }
        return to;
      };
      var __toCommonJS = (mod) => __copyProps(__defProp({}, "__esModule", { value: true }), mod);

      // src/pkjs/clay/builder/ts/slots/geometry.ts
      var geometry_exports = {};
      __export(geometry_exports, {
        ID_BATTERY: () => ID_BATTERY,
        ID_CLOCK: () => ID_CLOCK,
        ID_EMPTY: () => ID_EMPTY,
        ID_FULL_AGENDA: () => ID_FULL_AGENDA,
        ID_FULL_FORECAST_DAILY: () => ID_FULL_FORECAST_DAILY,
        ID_FULL_FORECAST_HOURLY: () => ID_FULL_FORECAST_HOURLY,
        ID_FULL_HEART: () => ID_FULL_HEART,
        ID_FULL_MUSIC: () => ID_FULL_MUSIC,
        ID_FULL_STEPS: () => ID_FULL_STEPS,
        ID_HEART: () => ID_HEART,
        ID_MOON_PCT: () => ID_MOON_PCT,
        ID_SENSORS: () => ID_SENSORS,
        ID_STARDATE: () => ID_STARDATE,
        ID_STEPS: () => ID_STEPS,
        ID_SUNRISE: () => ID_SUNRISE,
        ID_SUNSET: () => ID_SUNSET,
        ID_TALL_ACTIVE_GOAL: () => ID_TALL_ACTIVE_GOAL,
        ID_TALL_ALARM: () => ID_TALL_ALARM,
        ID_TALL_BATTERY: () => ID_TALL_BATTERY,
        ID_TALL_BEATS: () => ID_TALL_BEATS,
        ID_TALL_CALORIE_GOAL: () => ID_TALL_CALORIE_GOAL,
        ID_TALL_DAYLIGHT: () => ID_TALL_DAYLIGHT,
        ID_TALL_DAY_PROGRESS: () => ID_TALL_DAY_PROGRESS,
        ID_TALL_DISTANCE_GOAL: () => ID_TALL_DISTANCE_GOAL,
        ID_TALL_EVENT: () => ID_TALL_EVENT,
        ID_TALL_HEART: () => ID_TALL_HEART,
        ID_TALL_HEART_HOURS: () => ID_TALL_HEART_HOURS,
        ID_TALL_HOUR_PROGRESS: () => ID_TALL_HOUR_PROGRESS,
        ID_TALL_MOON: () => ID_TALL_MOON,
        ID_TALL_SLEEP_GOAL: () => ID_TALL_SLEEP_GOAL,
        ID_TALL_STEPS_HOURS: () => ID_TALL_STEPS_HOURS,
        ID_TALL_STEP_GOAL: () => ID_TALL_STEP_GOAL,
        ID_TALL_SUN_NEXT: () => ID_TALL_SUN_NEXT,
        ID_TALL_UV: () => ID_TALL_UV,
        ID_TALL_WEEK_PROGRESS: () => ID_TALL_WEEK_PROGRESS,
        ID_TALL_YEAR_PROGRESS: () => ID_TALL_YEAR_PROGRESS,
        ID_WIDE_ACTIVE_GOAL: () => ID_WIDE_ACTIVE_GOAL,
        ID_WIDE_ALARM: () => ID_WIDE_ALARM,
        ID_WIDE_BEATS: () => ID_WIDE_BEATS,
        ID_WIDE_CALORIE_GOAL: () => ID_WIDE_CALORIE_GOAL,
        ID_WIDE_DAYLIGHT: () => ID_WIDE_DAYLIGHT,
        ID_WIDE_DAY_PROGRESS: () => ID_WIDE_DAY_PROGRESS,
        ID_WIDE_DISTANCE_GOAL: () => ID_WIDE_DISTANCE_GOAL,
        ID_WIDE_EVENT: () => ID_WIDE_EVENT,
        ID_WIDE_HEART: () => ID_WIDE_HEART,
        ID_WIDE_HOUR_PROGRESS: () => ID_WIDE_HOUR_PROGRESS,
        ID_WIDE_MOON: () => ID_WIDE_MOON,
        ID_WIDE_MUSIC: () => ID_WIDE_MUSIC,
        ID_WIDE_SLEEP_GOAL: () => ID_WIDE_SLEEP_GOAL,
        ID_WIDE_STEPS: () => ID_WIDE_STEPS,
        ID_WIDE_STEP_GOAL: () => ID_WIDE_STEP_GOAL,
        ID_WIDE_SUN_NEXT: () => ID_WIDE_SUN_NEXT,
        ID_WIDE_UV: () => ID_WIDE_UV,
        ID_WIDE_WEEK_PROGRESS: () => ID_WIDE_WEEK_PROGRESS,
        ID_WIDE_YEAR_PROGRESS: () => ID_WIDE_YEAR_PROGRESS,
        LAYOUT_STARDATE: () => LAYOUT_STARDATE,
        LAYOUT_TACTICAL: () => LAYOUT_TACTICAL,
        SHAPE_FULL: () => SHAPE_FULL,
        SHAPE_SINGLE: () => SHAPE_SINGLE,
        SHAPE_TALL: () => SHAPE_TALL,
        SHAPE_WIDE: () => SHAPE_WIDE,
        SLOT_COUNT: () => SLOT_COUNT,
        SLOT_KEYS: () => SLOT_KEYS,
        SLOT_LB: () => SLOT_LB,
        SLOT_LT: () => SLOT_LT,
        SLOT_RB: () => SLOT_RB,
        SLOT_RT: () => SLOT_RT,
        SLOT_STORE_CLASS: () => SLOT_STORE_CLASS,
        STARDATE_GRID: () => STARDATE_GRID,
        TACTICAL_GRID: () => TACTICAL_GRID,
        TACTICAL_STORE_CLASS: () => TACTICAL_STORE_CLASS,
        UNIQUE_IDS: () => UNIQUE_IDS,
        areaCellCount: () => areaCellCount,
        areaOf: () => areaOf,
        canPlace: () => canPlace,
        cellCount: () => cellCount,
        cellsFor: () => cellsFor,
        colOf: () => colOf,
        gridFor: () => gridFor,
        owners: () => owners,
        rowOf: () => rowOf,
        shapeOf: () => shapeOf,
        slotAt: () => slotAt,
        spanOf: () => spanOf
      });
      function gridFor(layout) {
        return layout === LAYOUT_TACTICAL ? TACTICAL_GRID : STARDATE_GRID;
      }
      function areaCellCount(grid) {
        return grid.cols * grid.rows;
      }
      function cellCount(grid) {
        return areaCellCount(grid) * grid.areas;
      }
      function areaOf(slot, grid) {
        return Math.floor(slot / areaCellCount(grid));
      }
      function colOf(slot, grid) {
        return Math.floor(slot % areaCellCount(grid) / grid.rows);
      }
      function rowOf(slot, grid) {
        return slot % areaCellCount(grid) % grid.rows;
      }
      function slotAt(col, row, grid, area = 0) {
        if (col < 0 || col >= grid.cols || row < 0 || row >= grid.rows || area < 0 || area >= grid.areas) {
          return -1;
        }
        return area * areaCellCount(grid) + col * grid.rows + row;
      }
      function shapeOf(id) {
        return SHAPES[id] || SHAPE_SINGLE;
      }
      function spanOf(shape) {
        return {
          cols: shape === SHAPE_WIDE || shape === SHAPE_FULL ? 2 : 1,
          rows: shape === SHAPE_TALL || shape === SHAPE_FULL ? 2 : 1
        };
      }
      function cellsFor(id, slot, grid) {
        if (!grid.placesClock && UNIQUE_IDS.indexOf(id) >= 0) {
          return null;
        }
        const span = spanOf(shapeOf(id));
        const area = areaOf(slot, grid);
        const col0 = colOf(slot, grid);
        const row0 = rowOf(slot, grid);
        const cells = [];
        for (let col = 0; col < span.cols; col++) {
          for (let row = 0; row < span.rows; row++) {
            const cell = slotAt(col0 + col, row0 + row, grid, area);
            if (cell < 0) {
              return null;
            }
            cells.push(cell);
          }
        }
        return cells.sort((a, b) => a - b);
      }
      function canPlace(id, slot, grid) {
        return cellsFor(id, slot, grid) !== null;
      }
      function owners(slots, grid) {
        const count = cellCount(grid);
        const owner = [];
        for (let slot = 0; slot < count; slot++) {
          owner.push(-1);
        }
        for (let slot = 0; slot < count; slot++) {
          const id = slots[slot];
          if (id === ID_EMPTY || owner[slot] >= 0) {
            continue;
          }
          const cells = cellsFor(id, slot, grid);
          if (!cells || cells.some((cell) => owner[cell] >= 0)) {
            continue;
          }
          cells.forEach((cell) => {
            owner[cell] = slot;
          });
        }
        return owner;
      }
      var LAYOUT_STARDATE, LAYOUT_TACTICAL, STARDATE_GRID, TACTICAL_GRID, SLOT_LT, SLOT_LB, SLOT_RT, SLOT_RB, SLOT_COUNT, ID_HEART, ID_STEPS, ID_BATTERY, ID_MOON_PCT, ID_SUNRISE, ID_SUNSET, ID_EMPTY, ID_SENSORS, ID_WIDE_DAYLIGHT, ID_WIDE_STEPS, ID_FULL_HEART, ID_FULL_STEPS, ID_WIDE_HEART, ID_WIDE_MUSIC, ID_FULL_MUSIC, ID_FULL_AGENDA, ID_WIDE_EVENT, ID_WIDE_HOUR_PROGRESS, ID_WIDE_DAY_PROGRESS, ID_WIDE_WEEK_PROGRESS, ID_WIDE_YEAR_PROGRESS, ID_FULL_FORECAST_HOURLY, ID_FULL_FORECAST_DAILY, ID_WIDE_UV, ID_WIDE_MOON, ID_WIDE_ALARM, ID_WIDE_BEATS, ID_WIDE_STEP_GOAL, ID_WIDE_DISTANCE_GOAL, ID_WIDE_CALORIE_GOAL, ID_WIDE_ACTIVE_GOAL, ID_WIDE_SLEEP_GOAL, ID_WIDE_SUN_NEXT, ID_TALL_STEP_GOAL, ID_TALL_DISTANCE_GOAL, ID_TALL_CALORIE_GOAL, ID_TALL_ACTIVE_GOAL, ID_TALL_SLEEP_GOAL, ID_TALL_HEART, ID_TALL_STEPS_HOURS, ID_TALL_HEART_HOURS, ID_TALL_BATTERY, ID_TALL_ALARM, ID_TALL_BEATS, ID_TALL_SUN_NEXT, ID_TALL_DAYLIGHT, ID_TALL_MOON, ID_TALL_UV, ID_TALL_HOUR_PROGRESS, ID_TALL_DAY_PROGRESS, ID_TALL_WEEK_PROGRESS, ID_TALL_YEAR_PROGRESS, ID_TALL_EVENT, ID_CLOCK, ID_STARDATE, UNIQUE_IDS, SHAPE_SINGLE, SHAPE_TALL, SHAPE_WIDE, SHAPE_FULL, SHAPES, SLOT_KEYS, SLOT_STORE_CLASS, TACTICAL_STORE_CLASS;
      var init_geometry = __esm({
        "src/pkjs/clay/builder/ts/slots/geometry.ts"() {
          LAYOUT_STARDATE = 0;
          LAYOUT_TACTICAL = 1;
          STARDATE_GRID = { cols: 2, rows: 2, areas: 1, placesClock: false };
          TACTICAL_GRID = { cols: 2, rows: 2, areas: 3, placesClock: true };
          SLOT_LT = 0;
          SLOT_LB = 1;
          SLOT_RT = 2;
          SLOT_RB = 3;
          SLOT_COUNT = 4;
          ID_HEART = 0;
          ID_STEPS = 1;
          ID_BATTERY = 2;
          ID_MOON_PCT = 6;
          ID_SUNRISE = 9;
          ID_SUNSET = 10;
          ID_EMPTY = 20;
          ID_SENSORS = 23;
          ID_WIDE_DAYLIGHT = 28;
          ID_WIDE_STEPS = 29;
          ID_FULL_HEART = 30;
          ID_FULL_STEPS = 31;
          ID_WIDE_HEART = 32;
          ID_WIDE_MUSIC = 33;
          ID_FULL_MUSIC = 34;
          ID_FULL_AGENDA = 35;
          ID_WIDE_EVENT = 36;
          ID_WIDE_HOUR_PROGRESS = 41;
          ID_WIDE_DAY_PROGRESS = 42;
          ID_WIDE_WEEK_PROGRESS = 43;
          ID_WIDE_YEAR_PROGRESS = 44;
          ID_FULL_FORECAST_HOURLY = 52;
          ID_FULL_FORECAST_DAILY = 53;
          ID_WIDE_UV = 54;
          ID_WIDE_MOON = 55;
          ID_WIDE_ALARM = 56;
          ID_WIDE_BEATS = 57;
          ID_WIDE_STEP_GOAL = 58;
          ID_WIDE_DISTANCE_GOAL = 59;
          ID_WIDE_CALORIE_GOAL = 60;
          ID_WIDE_ACTIVE_GOAL = 61;
          ID_WIDE_SLEEP_GOAL = 62;
          ID_WIDE_SUN_NEXT = 63;
          ID_TALL_STEP_GOAL = 64;
          ID_TALL_DISTANCE_GOAL = 65;
          ID_TALL_CALORIE_GOAL = 66;
          ID_TALL_ACTIVE_GOAL = 67;
          ID_TALL_SLEEP_GOAL = 68;
          ID_TALL_HEART = 69;
          ID_TALL_STEPS_HOURS = 70;
          ID_TALL_HEART_HOURS = 71;
          ID_TALL_BATTERY = 72;
          ID_TALL_ALARM = 73;
          ID_TALL_BEATS = 74;
          ID_TALL_SUN_NEXT = 75;
          ID_TALL_DAYLIGHT = 76;
          ID_TALL_MOON = 77;
          ID_TALL_UV = 78;
          ID_TALL_HOUR_PROGRESS = 79;
          ID_TALL_DAY_PROGRESS = 80;
          ID_TALL_WEEK_PROGRESS = 81;
          ID_TALL_YEAR_PROGRESS = 82;
          ID_TALL_EVENT = 83;
          ID_CLOCK = 84;
          ID_STARDATE = 85;
          UNIQUE_IDS = [ID_CLOCK, ID_STARDATE];
          SHAPE_SINGLE = "single";
          SHAPE_TALL = "tall";
          SHAPE_WIDE = "wide";
          SHAPE_FULL = "full";
          SHAPES = {
            [ID_SENSORS]: SHAPE_TALL,
            [ID_WIDE_DAYLIGHT]: SHAPE_WIDE,
            [ID_WIDE_STEPS]: SHAPE_WIDE,
            [ID_WIDE_HEART]: SHAPE_WIDE,
            [ID_WIDE_MUSIC]: SHAPE_WIDE,
            [ID_WIDE_EVENT]: SHAPE_WIDE,
            [ID_WIDE_HOUR_PROGRESS]: SHAPE_WIDE,
            [ID_WIDE_DAY_PROGRESS]: SHAPE_WIDE,
            [ID_WIDE_WEEK_PROGRESS]: SHAPE_WIDE,
            [ID_WIDE_YEAR_PROGRESS]: SHAPE_WIDE,
            [ID_FULL_MUSIC]: SHAPE_FULL,
            [ID_FULL_AGENDA]: SHAPE_FULL,
            [ID_FULL_HEART]: SHAPE_FULL,
            [ID_FULL_STEPS]: SHAPE_FULL,
            [ID_WIDE_UV]: SHAPE_WIDE,
            [ID_WIDE_MOON]: SHAPE_WIDE,
            [ID_WIDE_ALARM]: SHAPE_WIDE,
            [ID_WIDE_BEATS]: SHAPE_WIDE,
            [ID_WIDE_STEP_GOAL]: SHAPE_WIDE,
            [ID_WIDE_DISTANCE_GOAL]: SHAPE_WIDE,
            [ID_WIDE_CALORIE_GOAL]: SHAPE_WIDE,
            [ID_WIDE_ACTIVE_GOAL]: SHAPE_WIDE,
            [ID_WIDE_SLEEP_GOAL]: SHAPE_WIDE,
            [ID_WIDE_SUN_NEXT]: SHAPE_WIDE,
            [ID_FULL_FORECAST_HOURLY]: SHAPE_FULL,
            [ID_FULL_FORECAST_DAILY]: SHAPE_FULL,
            [ID_TALL_STEP_GOAL]: SHAPE_TALL,
            [ID_TALL_DISTANCE_GOAL]: SHAPE_TALL,
            [ID_TALL_CALORIE_GOAL]: SHAPE_TALL,
            [ID_TALL_ACTIVE_GOAL]: SHAPE_TALL,
            [ID_TALL_SLEEP_GOAL]: SHAPE_TALL,
            [ID_TALL_HEART]: SHAPE_TALL,
            [ID_TALL_STEPS_HOURS]: SHAPE_TALL,
            [ID_TALL_HEART_HOURS]: SHAPE_TALL,
            [ID_TALL_BATTERY]: SHAPE_TALL,
            [ID_TALL_ALARM]: SHAPE_TALL,
            [ID_TALL_BEATS]: SHAPE_TALL,
            [ID_TALL_SUN_NEXT]: SHAPE_TALL,
            [ID_TALL_DAYLIGHT]: SHAPE_TALL,
            [ID_TALL_MOON]: SHAPE_TALL,
            [ID_TALL_UV]: SHAPE_TALL,
            [ID_TALL_HOUR_PROGRESS]: SHAPE_TALL,
            [ID_TALL_DAY_PROGRESS]: SHAPE_TALL,
            [ID_TALL_WEEK_PROGRESS]: SHAPE_TALL,
            [ID_TALL_YEAR_PROGRESS]: SHAPE_TALL,
            [ID_TALL_EVENT]: SHAPE_TALL,
            [ID_CLOCK]: SHAPE_FULL,
            [ID_STARDATE]: SHAPE_FULL
          };
          SLOT_KEYS = [
            "APPEARANCE_SLOT_LEFT_TOP",
            "APPEARANCE_SLOT_LEFT_BOTTOM",
            "APPEARANCE_SLOT_RIGHT_TOP",
            "APPEARANCE_SLOT_RIGHT_BOTTOM"
          ];
          SLOT_STORE_CLASS = [null, "sb-lb", "sb-rt", "sb-rb"];
          TACTICAL_STORE_CLASS = "sb-tac";
        }
      });

      // src/pkjs/clay/builder/ts/slots/codec.ts
      var codec_exports = {};
      __export(codec_exports, {
        formatId: () => formatId,
        formatTactical: () => formatTactical,
        moveUnique: () => moveUnique,
        parseId: () => parseId,
        parseTactical: () => parseTactical,
        readSlots: () => readSlots,
        readTactical: () => readTactical,
        sanitize: () => sanitize,
        storeFor: () => storeFor,
        writeStores: () => writeStores,
        writeTactical: () => writeTactical
      });
      function parseId(text) {
        const n = parseInt(text || "", 10);
        return isNaN(n) || n < 0 ? ID_EMPTY : n;
      }
      function formatId(id) {
        return String(id);
      }
      function storeByClass(root, cls) {
        const doc = root.ownerDocument || document;
        return doc.querySelector("." + cls);
      }
      function storeFor(root, slot) {
        const cls = SLOT_STORE_CLASS[slot];
        return cls ? storeByClass(root, cls) : null;
      }
      function readSlots(root, ownValue) {
        const slots = [];
        for (let slot = 0; slot < SLOT_COUNT; slot++) {
          const store = storeFor(root, slot);
          slots.push(parseId(store ? store.value : ownValue));
        }
        return sanitize(slots, STARDATE_GRID);
      }
      function writeStore(store, next) {
        if (store && store.value !== next) {
          store.value = next;
          store.dispatchEvent(new Event("change"));
        }
      }
      function writeStores(root, slots) {
        for (let slot = 1; slot < SLOT_COUNT; slot++) {
          writeStore(storeFor(root, slot), formatId(slots[slot]));
        }
      }
      function parseTactical(text) {
        const source = text || "";
        const picks = [];
        let cursor = 0;
        let readable = true;
        for (let cell = 0; cell < cellCount(TACTICAL_GRID); cell++) {
          const digits = readable ? /^[0-9]+/.exec(source.slice(cursor)) : null;
          if (!digits) {
            readable = false;
            picks.push(ID_EMPTY);
            continue;
          }
          picks.push(parseInt(digits[0], 10));
          cursor += digits[0].length;
          if (source.charAt(cursor) !== ",") {
            readable = false;
          }
          cursor++;
        }
        return sanitize(picks, TACTICAL_GRID);
      }
      function formatTactical(picks) {
        return picks.map(formatId).join(",");
      }
      function readTactical(root) {
        const store = storeByClass(root, TACTICAL_STORE_CLASS);
        return parseTactical(store ? store.value : "");
      }
      function writeTactical(root, picks) {
        writeStore(storeByClass(root, TACTICAL_STORE_CLASS), formatTactical(picks));
      }
      function moveUnique(before, after) {
        const out = after.slice();
        UNIQUE_IDS.forEach(function(id) {
          const cells = out.map(function(pick, cell) {
            return pick === id ? cell : -1;
          }).filter(function(cell) {
            return cell >= 0;
          });
          if (cells.length < 2) {
            return;
          }
          const fresh = cells.filter(function(cell) {
            return before[cell] !== id;
          });
          const keep = fresh.length > 0 ? fresh[0] : cells[0];
          cells.forEach(function(cell) {
            if (cell !== keep) {
              out[cell] = ID_EMPTY;
            }
          });
        });
        return out;
      }
      function sanitize(slots, grid) {
        const count = cellCount(grid);
        const out = slots.slice(0, count);
        while (out.length < count) {
          out.push(ID_EMPTY);
        }
        for (let slot = 0; slot < count; slot++) {
          if (!grid.placesClock && UNIQUE_IDS.indexOf(out[slot]) >= 0) {
            out[slot] = ID_EMPTY;
          }
        }
        const kept = owners(out, grid);
        const seen = [];
        for (let slot = 0; slot < count; slot++) {
          if (kept[slot] === slot && UNIQUE_IDS.indexOf(out[slot]) >= 0) {
            if (seen.indexOf(out[slot]) >= 0) {
              out[slot] = ID_EMPTY;
            } else {
              seen.push(out[slot]);
            }
          }
        }
        const owner = owners(out, grid);
        for (let slot = 0; slot < count; slot++) {
          if (owner[slot] !== slot) {
            out[slot] = ID_EMPTY;
          }
        }
        return out;
      }
      var init_codec = __esm({
        "src/pkjs/clay/builder/ts/slots/codec.ts"() {
          init_geometry();
        }
      });

      // src/data/slot-presets.json
      var slot_presets_default;
      var init_slot_presets = __esm({
        "src/data/slot-presets.json"() {
          slot_presets_default = {
            default: [23, 20, 0, 1]
          };
        }
      });

      // src/data/tactical-presets.json
      var tactical_presets_default;
      var init_tactical_presets = __esm({
        "src/data/tactical-presets.json"() {
          tactical_presets_default = {
            default: [84, 20, 20, 20, 23, 20, 0, 1, 85, 20, 20, 20]
          };
        }
      });

      // src/pkjs/clay/builder/ts/slots/presets.ts
      var presets_exports = {};
      __export(presets_exports, {
        SLOT_PRESETS: () => SLOT_PRESETS,
        TACTICAL_PRESETS: () => TACTICAL_PRESETS,
        presetFor: () => presetFor
      });
      function presetFor(layout, id) {
        return (layout === LAYOUT_TACTICAL ? TACTICAL_PRESETS : SLOT_PRESETS)[id];
      }
      var SLOT_PRESETS, TACTICAL_PRESETS;
      var init_presets = __esm({
        "src/pkjs/clay/builder/ts/slots/presets.ts"() {
          init_slot_presets();
          init_tactical_presets();
          init_geometry();
          SLOT_PRESETS = slot_presets_default;
          TACTICAL_PRESETS = tactical_presets_default;
        }
      });

      // src/pkjs/clay/builder/ts/slots/panels.ts
      var panels_exports = {};
      __export(panels_exports, {
        PALETTE_GROUPS: () => PALETTE_GROUPS,
        PALETTE_SIZES: () => PALETTE_SIZES,
        sizeKeyOf: () => sizeKeyOf,
        warningName: () => warningName
      });
      function sizeKeyOf(shape) {
        for (let index = 0; index < PALETTE_SIZES.length; index++) {
          if (PALETTE_SIZES[index].shape === shape) {
            return PALETTE_SIZES[index].key;
          }
        }
        return PALETTE_SIZES[0].key;
      }
      function warningName(label) {
        return label.replace(/\s*\([^)]*\)\s*$/, "");
      }
      var SIZE_ORDER, PALETTE_SIZES, PALETTE_GROUPS;
      var init_panels = __esm({
        "src/pkjs/clay/builder/ts/slots/panels.ts"() {
          init_geometry();
          SIZE_ORDER = [SHAPE_SINGLE, SHAPE_WIDE, SHAPE_TALL, SHAPE_FULL];
          PALETTE_SIZES = SIZE_ORDER.map(function(shape) {
            const span = spanOf(shape);
            return { shape, key: span.cols + "x" + span.rows, wide: span.cols === 2 };
          });
          PALETTE_GROUPS = [
            { name: "Health", icon: "\u2764\uFE0F" },
            { name: "Weather", icon: "\u{1F324}" },
            { name: "Sky", icon: "\u{1F317}" },
            { name: "Clock", icon: "\u23F0" },
            { name: "Date", icon: "\u{1F4C5}" },
            { name: "Calendar", icon: "\u{1F5D3}" },
            { name: "Music", icon: "\u{1F3B5}" },
            { name: "Status", icon: "\u{1F50B}" }
          ];
        }
      });

      // paf/ts/clay/builder/ts/shared/thumbs.ts
      function thumbByLabel(thumbs, label, size) {
        const byModule = thumbs[label];
        return byModule && byModule[size] || null;
      }
      var init_thumbs = __esm({
        "paf/ts/clay/builder/ts/shared/thumbs.ts"() {
        }
      });

      // src/pkjs/clay/builder/ts/slots/visuals.ts
      var visuals_exports = {};
      __export(visuals_exports, {
        buildReadoutList: () => buildReadoutList,
        fillVisual: () => fillVisual,
        readoutById: () => readoutById,
        sizeFor: () => sizeFor,
        thumbFor: () => thumbFor
      });
      function buildReadoutList(options) {
        return (options || []).map(function(option) {
          return {
            value: typeof option.value === "string" ? parseInt(option.value, 10) : option.value,
            label: option.label,
            icon: option.icon || "\xB7",
            color: option.blockColor || "#999",
            group: option.group,
            plugin: option.plugin
          };
        }).filter(function(readout) {
          return readout.value !== ID_EMPTY;
        });
      }
      function sizeFor(id) {
        const shape = shapeOf(id);
        return shape === SHAPE_SINGLE ? "slot" : shape;
      }
      function thumbFor(thumbs, readout) {
        return readout ? thumbByLabel(thumbs, readout.label, sizeFor(readout.value)) : null;
      }
      function fillVisual(el, readout, thumbs) {
        el.innerHTML = "";
        el.classList.remove("sb-fallback");
        el.style.background = "";
        if (!readout) {
          const empty = el.ownerDocument.createElement("span");
          empty.className = "sb-slot-empty";
          empty.textContent = "Empty";
          el.appendChild(empty);
          return;
        }
        const thumb = thumbFor(thumbs, readout);
        if (thumb) {
          const img = el.ownerDocument.createElement("img");
          img.className = "sb-slot-img";
          img.src = thumb;
          img.alt = readout.label;
          el.appendChild(img);
          return;
        }
        el.classList.add("sb-fallback");
        el.style.background = readout.color;
        const icon = el.ownerDocument.createElement("span");
        icon.className = "sb-fallback-icon";
        icon.textContent = readout.icon;
        el.appendChild(icon);
      }
      function readoutById(readouts, id) {
        for (let i = 0; i < readouts.length; i++) {
          if (readouts[i].value === id) {
            return readouts[i];
          }
        }
        return null;
      }
      var init_visuals = __esm({
        "src/pkjs/clay/builder/ts/slots/visuals.ts"() {
          init_geometry();
          init_thumbs();
        }
      });

      // paf/ts/clay/builder/ts/drag.ts
      function createDrag(spec, doc = document) {
        const threshold = spec.threshold === void 0 ? DEFAULT_THRESHOLD : spec.threshold;
        let armed = null;
        let active = null;
        let owner;
        function place(ghost, x, y) {
          if (spec.anchor === "pointer") {
            ghost.style.left = x + "px";
            ghost.style.top = y + "px";
            return;
          }
          const box = ghost.getBoundingClientRect();
          ghost.style.left = x - box.width / 2 + "px";
          ghost.style.top = y - box.height / 2 + "px";
        }
        function foreign(event) {
          return (armed !== null || active !== null) && event.pointerId !== owner;
        }
        function begin(payload, x, y) {
          const ghost = spec.ghost(payload);
          doc.body.appendChild(ghost);
          active = { payload, ghost };
          place(ghost, x, y);
          track(x, y);
        }
        function track(x, y) {
          if (!active) {
            return;
          }
          place(active.ghost, x, y);
          const target = spec.hitTest(x, y);
          const allowed = target !== null && spec.allows(active.payload, target);
          spec.highlight(active.payload, target, allowed);
        }
        function end() {
          if (active) {
            active.ghost.remove();
            active = null;
          }
          armed = null;
          spec.highlight(null, null, false);
        }
        function abandon() {
          if (!armed && !active) {
            return;
          }
          const payload = active && active.payload;
          end();
          if (payload === null || payload === void 0) {
            return;
          }
          if (spec.cancel) {
            spec.cancel(payload);
            return;
          }
          spec.dropOutside(payload);
        }
        doc.addEventListener("pointermove", (event) => {
          if (foreign(event)) {
            return;
          }
          if (armed) {
            const moved = Math.abs(event.clientX - armed.x) > threshold || Math.abs(event.clientY - armed.y) > threshold;
            if (moved) {
              const payload = armed.payload;
              armed = null;
              if (spec.lift) {
                spec.lift(payload);
              }
              begin(payload, event.clientX, event.clientY);
            }
          }
          if (active) {
            track(event.clientX, event.clientY);
            event.preventDefault();
          }
        });
        doc.addEventListener("pointerup", (event) => {
          if (foreign(event)) {
            return;
          }
          if (!active) {
            armed = null;
            return;
          }
          const payload = active.payload;
          const target = spec.hitTest(event.clientX, event.clientY);
          end();
          if (target !== null && spec.allows(payload, target)) {
            spec.drop(payload, target);
            return;
          }
          spec.dropOutside(payload);
        });
        doc.addEventListener("pointercancel", (event) => {
          if (foreign(event)) {
            return;
          }
          abandon();
        });
        return {
          start(payload, event) {
            abandon();
            owner = event.pointerId;
            begin(payload, event.clientX, event.clientY);
            event.preventDefault();
          },
          arm(payload, event) {
            abandon();
            owner = event.pointerId;
            armed = { payload, x: event.clientX, y: event.clientY };
            event.preventDefault();
          }
        };
      }
      var DEFAULT_THRESHOLD;
      var init_drag = __esm({
        "paf/ts/clay/builder/ts/drag.ts"() {
          DEFAULT_THRESHOLD = 10;
        }
      });

      // src/pkjs/clay/builder/ts/slots/drag.ts
      var drag_exports = {};
      __export(drag_exports, {
        installDrag: () => installDrag,
        tryPlace: () => tryPlace
      });
      function tryPlace(slots, id, from, to, grid) {
        const landing = cellsFor(id, to, grid);
        if (!landing) {
          return null;
        }
        if (from === to) {
          return slots.slice();
        }
        const next = slots.slice();
        if (from >= 0) {
          clearPiece(next, from, grid);
        }
        const under = owners(next, grid);
        const hit = [];
        landing.forEach((cell) => {
          if (under[cell] >= 0 && hit.indexOf(under[cell]) < 0) {
            hit.push(under[cell]);
          }
        });
        const hitIds = hit.map((slot) => next[slot]);
        hit.forEach((slot) => {
          clearPiece(next, slot, grid);
        });
        next[to] = id;
        if (from < 0) {
          return next;
        }
        const stepArea = areaOf(from, grid) - areaOf(to, grid);
        const stepCol = colOf(from, grid) - colOf(to, grid);
        const stepRow = rowOf(from, grid) - rowOf(to, grid);
        for (let i = 0; i < hit.length; i++) {
          const dest = slotAt(colOf(hit[i], grid) + stepCol, rowOf(hit[i], grid) + stepRow, grid, areaOf(hit[i], grid) + stepArea);
          const cells = dest >= 0 ? cellsFor(hitIds[i], dest, grid) : null;
          const taken = owners(next, grid);
          if (!cells || cells.some((cell) => taken[cell] >= 0)) {
            return null;
          }
          next[dest] = hitIds[i];
        }
        return next;
      }
      function clearPiece(slots, slot, grid) {
        const cells = cellsFor(slots[slot], slot, grid) || [slot];
        cells.forEach((cell) => {
          slots[cell] = ID_EMPTY;
        });
      }
      function installDrag(env) {
        const doc = env.root.ownerDocument || document;
        function slotUnder(x, y) {
          for (let slot = 0; slot < env.slotEls.length; slot++) {
            const box = env.slotEls[slot].getBoundingClientRect();
            if (box.width && x >= box.left && x <= box.right && y >= box.top && y <= box.bottom) {
              return slot;
            }
          }
          return null;
        }
        const drag = createDrag({
          anchor: "center",
          ghost(dragged) {
            const ghost = doc.createElement("div");
            const shape = shapeOf(dragged.id);
            ghost.className = "sb-ghost" + (shape === SHAPE_SINGLE ? "" : " " + shape);
            fillVisual(ghost, readoutById(env.readouts, dragged.id), env.thumbs);
            return ghost;
          },
          hitTest: slotUnder,
          allows(dragged, slot) {
            return tryPlace(env.getSlots(), dragged.id, dragged.from, slot, env.getGrid()) !== null;
          },
          highlight(dragged, slot, allowed) {
            for (let i = 0; i < env.slotEls.length; i++) {
              env.slotEls[i].classList.remove("over", "blocked");
            }
            if (slot !== null) {
              env.slotEls[slot].classList.add(allowed ? "over" : "blocked");
            }
          },
          drop(dragged, slot) {
            const next = tryPlace(env.getSlots(), dragged.id, dragged.from, slot, env.getGrid());
            if (next) {
              env.setSlots(next);
            }
          },
          dropOutside(dragged) {
            if (dragged.from < 0) {
              return;
            }
            const cleared = env.getSlots().slice();
            cleared[dragged.from] = ID_EMPTY;
            env.setSlots(cleared);
          }
        }, doc);
        return {
          fromPalette(id, event) {
            if (readoutById(env.readouts, id)) {
              drag.start({ id, from: -1 }, event);
            }
          },
          // armed rather than started, so a tap on a panel is not mistaken for picking it up
          fromSlot(slot, event) {
            const id = env.getSlots()[slot];
            if (id !== ID_EMPTY) {
              drag.arm({ id, from: slot }, event);
            }
          }
        };
      }
      var init_drag2 = __esm({
        "src/pkjs/clay/builder/ts/slots/drag.ts"() {
          init_geometry();
          init_visuals();
          init_drag();
        }
      });

      // paf/ts/clay/builder/ts/shared/palette.ts
      function groupsForSize(modules, groups, size) {
        const known = {};
        const sections = [];
        const atSize = modules.filter(function(module) {
          return module.sizes.indexOf(size) !== -1;
        });
        for (let index = 0; index < groups.length; index++) {
          const group = groups[index];
          if (known[group.name]) {
            continue;
          }
          const members = atSize.filter(function(module) {
            return module.group === group.name;
          });
          known[group.name] = true;
          if (members.length > 0) {
            sections.push({ group, modules: members });
          }
        }
        const others = atSize.filter(function(module) {
          return !module.group || !known[module.group];
        });
        if (others.length > 0) {
          sections.push({ group: OTHER_GROUP, modules: others });
        }
        return sections;
      }
      function createPalette(env) {
        const chosen = {};
        let placed = [];
        let activeSize = "";
        let activeWide = false;
        const wrap = document.createElement("div");
        wrap.className = "pf-palette";
        const tabBar = document.createElement("div");
        tabBar.className = "pf-palette-sizes";
        const groupBar = document.createElement("div");
        groupBar.className = "pf-palette-groups";
        const box = document.createElement("div");
        box.className = "pf-palette-box";
        const pane = document.createElement("div");
        pane.className = "pf-palette-pane";
        box.appendChild(groupBar);
        box.appendChild(pane);
        function buildTile(module, size) {
          const tile = document.createElement("div");
          const shot = env.thumbFor(module.value, size);
          tile.className = "pf-palette-tile " + (activeWide ? "wide" : "narrow");
          tile.dataset.value = String(module.value);
          tile.title = module.label;
          if (shot) {
            const img = document.createElement("img");
            tile.style.background = "#000";
            img.className = "pf-palette-shot";
            img.src = shot;
            img.alt = module.label;
            tile.appendChild(img);
          } else {
            const icon = document.createElement("div");
            tile.style.background = module.color;
            icon.className = "pf-palette-icon";
            icon.textContent = module.icon;
            tile.appendChild(icon);
          }
          tile.addEventListener("pointerdown", function(event) {
            env.onPick(module, size, event);
          });
          return tile;
        }
        function buildGroupTab(group, pressed) {
          const tab = document.createElement("button");
          const icon = document.createElement("span");
          const name = document.createElement("span");
          tab.type = "button";
          tab.className = "pf-palette-group";
          tab.title = group ? group.name : "Every panel";
          tab.setAttribute("aria-pressed", String(pressed));
          icon.className = "pf-palette-group-icon";
          icon.textContent = group ? group.icon : "\u25A6";
          name.className = "pf-palette-group-name";
          name.textContent = group ? group.name : ALL;
          tab.appendChild(icon);
          tab.appendChild(name);
          tab.addEventListener("click", function() {
            chosen[activeSize] = group ? group.name : ALL;
            draw();
          });
          return tab;
        }
        function applyPlaced() {
          const tiles = pane.querySelectorAll(".pf-palette-tile");
          for (let index = 0; index < tiles.length; index++) {
            const value = parseInt(tiles[index].dataset.value || "", 10);
            tiles[index].classList.toggle("placed", placed.indexOf(value) !== -1);
          }
        }
        function draw() {
          const sections = groupsForSize(env.modules, env.groups, activeSize);
          const single = sections.length < 2;
          let pick = chosen[activeSize];
          let total = 0;
          for (let index = 0; index < sections.length; index++) {
            total += sections[index].modules.length;
          }
          if (single) {
            pick = ALL;
          } else if (pick !== ALL && !sections.some(function(section) {
            return section.group.name === pick;
          })) {
            pick = sections[0].group.name;
          }
          while (groupBar.firstChild) {
            groupBar.removeChild(groupBar.firstChild);
          }
          groupBar.hidden = single;
          for (let index = 0; index < sections.length && !single; index++) {
            groupBar.appendChild(buildGroupTab(sections[index].group, sections[index].group.name === pick));
          }
          if (!single) {
            groupBar.appendChild(buildGroupTab(null, pick === ALL));
          }
          while (pane.firstChild) {
            pane.removeChild(pane.firstChild);
          }
          const showing = pick === ALL ? null : sections.filter(function(section) {
            return section.group.name === pick;
          })[0];
          const tiles = document.createElement("div");
          tiles.className = "pf-palette-tiles";
          const listed = showing ? [showing] : sections;
          for (let index = 0; index < listed.length; index++) {
            for (let moduleIndex = 0; moduleIndex < listed[index].modules.length; moduleIndex++) {
              tiles.appendChild(buildTile(listed[index].modules[moduleIndex], activeSize));
            }
          }
          pane.appendChild(tiles);
          if (showing) {
            const status = document.createElement("div");
            const what = document.createElement("span");
            const all = document.createElement("button");
            status.className = "pf-palette-status";
            what.textContent = showing.group.name + " \xB7 " + showing.modules.length + " of " + total;
            all.type = "button";
            all.className = "pf-palette-all";
            all.textContent = "Show all";
            all.addEventListener("click", function() {
              chosen[activeSize] = ALL;
              draw();
            });
            status.appendChild(what);
            status.appendChild(all);
            pane.appendChild(status);
          }
          applyPlaced();
        }
        function selectSize(size) {
          const tabs = tabBar.querySelectorAll(".pf-palette-size");
          activeSize = size.key;
          activeWide = size.wide;
          for (let index = 0; index < tabs.length; index++) {
            tabs[index].classList.toggle("active", tabs[index].dataset.size === size.key);
          }
          draw();
        }
        let first = null;
        for (let index = 0; index < env.sizes.length; index++) {
          const size = env.sizes[index];
          if (groupsForSize(env.modules, env.groups, size.key).length === 0) {
            continue;
          }
          const tab = document.createElement("button");
          tab.type = "button";
          tab.className = "pf-palette-size";
          tab.dataset.size = size.key;
          tab.textContent = size.key;
          tab.addEventListener("click", function() {
            selectSize(size);
          });
          tabBar.appendChild(tab);
          if (!first) {
            first = size;
          }
        }
        wrap.appendChild(tabBar);
        wrap.appendChild(box);
        env.container.appendChild(wrap);
        if (first) {
          selectSize(first);
        }
        return {
          markPlaced: function(values) {
            placed = values.slice();
            applyPlaced();
          }
        };
      }
      var OTHER_GROUP, ALL;
      var init_palette = __esm({
        "paf/ts/clay/builder/ts/shared/palette.ts"() {
          OTHER_GROUP = { name: "Other", icon: "\xB7" };
          ALL = "All";
        }
      });

      // paf/ts/clay/callout-style.ts
      var CALLOUT_BOX, CALLOUT_BADGE;
      var init_callout_style = __esm({
        "paf/ts/clay/callout-style.ts"() {
          CALLOUT_BOX = "position: relative; margin: 0.7rem 0 0.15rem; padding: 0.5rem 0.6rem 0.5rem 2.1rem; background: rgba(255, 71, 0, 0.1); background: color-mix(in srgb, var(--pf-note-accent, #ff4700) 10%, transparent); border-left: 3px solid var(--pf-note-accent, #ff4700); font-size: 0.85em; line-height: 1.4;";
          CALLOUT_BADGE = 'content: "!"; position: absolute; left: 0.6rem; top: 0.65rem; width: 1.15em; height: 1.15em; border: 1px solid var(--pf-note-accent, #ff4700); border-radius: 50%; color: var(--pf-note-accent, #ff4700); font-size: 0.95em; font-weight: 700; line-height: 1.1em; text-align: center;';
        }
      });

      // paf/ts/clay/builder/ts/shared/plugin-warnings.ts
      function pluginWarnings(panels, panelKinds, states, picked) {
        const off = [];
        const missing = {};
        panels.forEach(function(panel) {
          const kind = panelKinds[panel];
          if (!kind || CHOSEN_KINDS.indexOf(kind) >= 0 && !picked[kind]) {
            return;
          }
          const state = states[kind];
          let named;
          if (state === "off") {
            named = off;
          } else if (state === "missing") {
            named = missing[kind] = missing[kind] || [];
          }
          if (named && named.indexOf(panel) < 0) {
            named.push(panel);
          }
        });
        const warnings = [];
        if (off.length) {
          warnings.push({ state: "off", kind: null, panels: off });
        }
        KIND_NEEDS.forEach(function(entry) {
          const named = missing[entry[0]];
          if (named) {
            warnings.push({ state: "missing", kind: entry[0], panels: named });
          }
        });
        return warnings;
      }
      function nameList(panels) {
        if (panels.length < 3) {
          return panels.join(" and ");
        }
        return panels.slice(0, -1).join(", ") + ", and " + panels[panels.length - 1];
      }
      function warningText(warning) {
        const subject = nameList(warning.panels) + (warning.panels.length > 1 ? " need " : " needs ");
        if (warning.state === "off") {
          return subject + "plugins turned on in the Pebble app, under Settings, Plugins.";
        }
        let needs = "a plugin";
        KIND_NEEDS.forEach(function(entry) {
          if (entry[0] === warning.kind) {
            needs = entry[1];
          }
        });
        return subject + needs + " in the Pebble app, and your phone has none yet.";
      }
      function readStates() {
        const store = document.querySelector("." + KINDS_ROW_CLASS);
        if (!store || !store.value) {
          return {};
        }
        try {
          const parsed = JSON.parse(store.value);
          return parsed && typeof parsed === "object" && !Array.isArray(parsed) ? parsed : {};
        } catch (e) {
          return {};
        }
      }
      function createPluginWarnings(env) {
        const states = readStates();
        const box = document.createElement("div");
        let panels = [];
        let picked = {};
        box.className = "pf-plugin-warnings";
        box.setAttribute("style", "padding: 0 var(--pf-palette-gutter, 12px);");
        if (env.after.parentNode) {
          env.after.parentNode.insertBefore(box, env.after.nextSibling);
        }
        function draw() {
          box.innerHTML = "";
          pluginWarnings(panels, env.panelKinds, states, picked).forEach(function(warning) {
            const note = document.createElement("div");
            const badge = document.createElement("span");
            const words = document.createElement("span");
            note.className = "pf-plugin-warning";
            note.setAttribute("style", CALLOUT_BOX);
            badge.setAttribute("style", CALLOUT_BADGE);
            badge.textContent = "!";
            words.textContent = warningText(warning);
            note.appendChild(badge);
            note.appendChild(words);
            box.appendChild(note);
          });
          box.hidden = !box.firstChild;
        }
        function setPicked(next) {
          picked = next;
          draw();
        }
        env.root._pfPluginSources = setPicked;
        draw();
        return {
          update: function(next) {
            panels = next;
            draw();
          },
          setPicked
        };
      }
      var KINDS_ROW_CLASS, CHOSEN_KINDS, KIND_NEEDS;
      var init_plugin_warnings = __esm({
        "paf/ts/clay/builder/ts/shared/plugin-warnings.ts"() {
          init_callout_style();
          KINDS_ROW_CLASS = "pf-plugin-kinds";
          CHOSEN_KINDS = ["weather", "stocks", "calendar"];
          KIND_NEEDS = [
            ["phone", "the Phone plugin"],
            ["music", "a music plugin"],
            ["weather", "a weather plugin"],
            ["stocks", "a stock plugin"],
            ["calendar", "a calendar plugin"]
          ];
        }
      });

      // src/pkjs/clay/builder/ts/slots/init.ts
      var init_exports = {};
      __export(init_exports, {
        init: () => init
      });
      function init() {
        const root = this.$element[0];
        const cfg = this.config || {};
        const fireChange = this.trigger.bind(this);
        const READOUTS = buildReadoutList(cfg.moduleOptions || []);
        const THUMBS = cfg.moduleThumbnails || {};
        const hidden = root.querySelector(".sb-value");
        const faceEl = root.querySelector(".sb-face");
        const slotsEl = root.querySelector(".sb-slots");
        const slotEls = [];
        let layout = LAYOUT_STARDATE;
        const picks = {};
        picks[LAYOUT_STARDATE] = readSlots(root, hidden.value);
        picks[LAYOUT_TACTICAL] = readTactical(root);
        function grid() {
          return gridFor(layout);
        }
        function current() {
          return picks[layout];
        }
        function sketchRow(slot) {
          return areaOf(slot, grid()) * (grid().rows + 1) + rowOf(slot, grid()) + 1;
        }
        function buildSlots() {
          const rows = [];
          for (let area = 0; area < grid().areas; area++) {
            if (area > 0) {
              rows.push("var(--sb-strip-h)");
            }
            for (let row = 0; row < grid().rows; row++) {
              rows.push("var(--sb-slot-h)");
            }
          }
          slotsEl.innerHTML = "";
          slotEls.length = 0;
          slotsEl.style.gridTemplateRows = rows.join(" ");
          faceEl.classList.toggle("sb-tactical", layout === LAYOUT_TACTICAL);
          for (let slot = 0; slot < cellCount(grid()); slot++) {
            const el = root.ownerDocument.createElement("div");
            el.className = "sb-slot";
            el.dataset.slot = String(slot);
            (function(index) {
              el.addEventListener("pointerdown", function(event) {
                drag.fromSlot(index, event);
              });
            })(slot);
            slotsEl.appendChild(el);
            slotEls.push(el);
          }
        }
        function render() {
          const slots = current();
          const owner = owners(slots, grid());
          const placed = [];
          for (let slot = 0; slot < slotEls.length; slot++) {
            const el = slotEls[slot];
            const covered = owner[slot] >= 0 && owner[slot] !== slot;
            const span = spanOf(shapeOf(slots[slot]));
            const readout = slots[slot] === ID_EMPTY ? null : readoutById(READOUTS, slots[slot]);
            el.style.display = covered ? "none" : "";
            el.style.gridArea = sketchRow(slot) + " / " + (colOf(slot, grid()) + 1) + " / span " + span.rows + " / span " + span.cols;
            fillVisual(el, readout, THUMBS);
            el.classList.toggle("filled", Boolean(readout));
            if (readout) {
              placed.push(readout.value);
            }
          }
          palette.markPlaced(grid().placesClock ? placed : placed.concat(UNIQUE_IDS));
          warnings.update(placed.map(function(value) {
            const readout = readoutById(READOUTS, value);
            return readout ? warningName(readout.label) : "";
          }));
          publish();
        }
        function publish() {
          const own = formatId(picks[LAYOUT_STARDATE][0]);
          if (hidden.value !== own) {
            hidden.value = own;
            fireChange("change");
          }
          writeStores(root, picks[LAYOUT_STARDATE]);
          writeTactical(root, picks[LAYOUT_TACTICAL]);
        }
        function setSlots(next) {
          picks[layout] = sanitize(moveUnique(current(), next), grid());
          render();
        }
        const drag = installDrag({
          root,
          readouts: READOUTS,
          thumbs: THUMBS,
          slotEls,
          getGrid: grid,
          getSlots: current,
          setSlots
        });
        const MODULES = READOUTS.map(function(readout) {
          return Object.assign({}, readout, { sizes: [sizeKeyOf(shapeOf(readout.value))] });
        });
        const palette = createPalette({
          container: root.querySelector(".sb-palette"),
          modules: MODULES,
          groups: PALETTE_GROUPS,
          sizes: PALETTE_SIZES.map(function(size) {
            return { key: size.key, wide: size.wide };
          }),
          thumbFor: function(value) {
            return thumbFor(THUMBS, readoutById(READOUTS, value));
          },
          onPick: function(module, size, event) {
            drag.fromPalette(module.value, event);
          }
        });
        const panelKinds = {};
        READOUTS.forEach(function(readout) {
          if (readout.plugin) {
            panelKinds[warningName(readout.label)] = readout.plugin;
          }
        });
        const warnings = createPluginWarnings({
          root,
          after: root.querySelector(".sb-face-wrap"),
          panelKinds
        });
        function showLayout(next) {
          layout = next === LAYOUT_TACTICAL ? LAYOUT_TACTICAL : LAYOUT_STARDATE;
          buildSlots();
          render();
        }
        const presetBtns = root.querySelectorAll(".sb-preset");
        for (let i = 0; i < presetBtns.length; i++) {
          presetBtns[i].addEventListener("click", function(event) {
            const id = event.currentTarget.getAttribute("data-preset") || "";
            const preset = presetFor(layout, id);
            if (preset) {
              setSlots(preset);
            }
          });
        }
        const clear = root.querySelector(".sb-btn-clear");
        if (clear) {
          clear.addEventListener("click", function() {
            const empty = [];
            while (empty.length < cellCount(grid())) {
              empty.push(ID_EMPTY);
            }
            setSlots(empty);
          });
        }
        const hooked = root;
        hooked._sbSet = function(value) {
          picks[LAYOUT_STARDATE] = readSlots(root, value);
          render();
        };
        hooked._sbGet = function() {
          return formatId(picks[LAYOUT_STARDATE][0]);
        };
        hooked._sbLayout = function(value) {
          const next = parseInt(value, 10);
          if (next !== layout) {
            showLayout(next);
          }
        };
        showLayout(LAYOUT_STARDATE);
        setTimeout(function() {
          const stardate = readSlots(root, hidden.value);
          const tactical = readTactical(root);
          const moved = function(late, kept) {
            return late.some(function(id, slot) {
              return id !== kept[slot];
            });
          };
          if (moved(stardate, picks[LAYOUT_STARDATE]) || moved(tactical, picks[LAYOUT_TACTICAL])) {
            picks[LAYOUT_STARDATE] = stardate;
            picks[LAYOUT_TACTICAL] = tactical;
            render();
          }
        }, 0);
      }
      var init_init = __esm({
        "src/pkjs/clay/builder/ts/slots/init.ts"() {
          init_geometry();
          init_codec();
          init_presets();
          init_panels();
          init_visuals();
          init_palette();
          init_plugin_warnings();
          init_drag2();
        }
      });

      // src/pkjs/clay/builder/component-entry.js
      var require_component_entry = __commonJS({
        "src/pkjs/clay/builder/component-entry.js"(exports, module) {
          init_geometry();
          init_codec();
          init_presets();
          init_panels();
          init_visuals();
          init_drag2();
          init_init();
          module.exports = (init_init(), __toCommonJS(init_exports));
        }
      });
      return require_component_entry();
    })();

    __clayComponent.init.call(this);
  }
};
