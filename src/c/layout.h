/**
 * @file layout.h
 * @brief Spatial map for the LCARS frame (200x228), kept in sync with the background image.
 *
 * @ingroup watchface-lcars
 */
#pragma once
#include <pebble.h>

#include "ui/engine/engine.h"

/**
 * @defgroup watchface-lcars LCARS Watchface
 * @brief The LCARS Stardate Emery watchface.
 * @{
 */

// --- Zone (text slot) ids ---
// face-owned slot vocabulary. layout.c declares a Zone table indexed by these and the engine loops
// it to build the text layers
//
// the ops cells are not in here. what they show comes from the catalogue in ops/, their bar word
// and glyph follow the pick, and layout.c works their zones out from the grid of the layout on show
enum
{
    ZONE_TIME,
    ZONE_MERIDIEM,
    ZONE_DATE,
    ZONE_LAT,
    ZONE_LON,
    ZONE_LINK,
    ZONE_QUIET,
    ZONE_COUNT
};

// --- Text Slots ---
#define SLOT_TIME GRect(53, 92, 142, 74)       // centred time box (Antonio 62)
#define SLOT_BANNER GRect(53, 17, 142, 44)     // date below STARDATE bar (Antonio 36)
#define SLOT_BANNER_SM GRect(53, 20, 142, 43)  // wide date fallback vertically centred (Antonio 32)
#define SLOT_BANNER_XS GRect(53, 22, 142, 42)  // wider date fallback vertically centred (Antonio 28)
#define SLOT_BANNER_XXS GRect(53, 24, 142, 40) // widest date fallback, fits the .beats formats (Antonio 24)
#define SLOT_MERIDIEM GRect(150, 95, 45, 14)   // AM/PM top-right of clock and right-aligned (Antonio 10)
// --- Ops slot geometry ---
// all four slots are the same shape so it gets described once here and stamped
// out per column and row. every offset is measured from the slot's bar origin
//
// the value box runs to the far edge of its column. catalogue readings get long so
// every pixel of room here is one that keeps a reading on the full-size font
// instead of stepping it down
//
// dy is the nudge a smaller font needs. text sits at the top of its box so a step
// down has to push the box down by the cap height it lost. otherwise the reading
// floats above the line the other rows sit on. same trick the date banner uses
#define OPS_GLYPH_W 3   // gap from the bar's left edge to the glyph
#define OPS_TEXT_GAP 3  // gap from the glyph to the reading so the two do not crowd

// where a reading starts: past the glyph and its gap
#define OPS_TEXT_X(x) ((x) + OPS_GLYPH_W + 14 + OPS_TEXT_GAP)

// a value box is handed its column's right edge rather than a width. that way the
// gap above it can change without dragging the right margin along with it
#define OPS_BAR(x, y)               GRect((x), (y), 69, 11)
#define OPS_LABEL(x, y)             GRect((x) + 8, (y), 54, 11)
#define OPS_GLYPH(x, y)             GRect((x) + OPS_GLYPH_W, (y) + 13, 14, 14)
#define OPS_VALUE(x, y, r, dy)      GRect(OPS_TEXT_X(x), (y) + 11 + (dy), (r) - OPS_TEXT_X(x), 18)
#define OPS_VALUE_WIDE(x, y, r, dy) GRect((x) + OPS_GLYPH_W, (y) + 11 + (dy), \
                                          (r) - ((x) + OPS_GLYPH_W), 18)

// the two columns and the two rows they each hold. the rail block's edge, the gap between the two
// bars, and the screen's edge each leave 5px either side of the bars
#define OPS_COL_L 52
#define OPS_COL_R 126
#define OPS_ROW_T 166
#define OPS_ROW_B 197

// how far right a reading may run. the right column goes to x=198 which is two
// off the screen edge. the left has to stop 3px short of the right column's bar so
// it is narrower and steps its font down a little sooner
#define OPS_R_L (OPS_COL_R - 3)
#define OPS_R_R 198

// the font step-down nudges in the order the Zone tiers try them
// Antonio 16 then 14 then 12
#define OPS_DY_FB 1
#define OPS_DY_FB2 3

// --- Rail Side ---
// with the rail down the right the frame is mirrored. what sits inside the frame keeps its order and
// moves left by the rail's width, so a panel stays in the column it was set in. what sits on the
// rail, such as the gauge, the coordinates, and the glyphs, flips across to the other side
#define RAIL_W 47  // the rail, plus the gap that keeps the panels clear of it

// the battery gauge on the top-left block
#define BATTERY_GAUGE GRect(6, 4, 32, 12)

// --- Coordinates ---
// right-aligned black text on the left rail blocks (Antonio 12)
#define COORD_TEXT_COLOR GColorBlack
#define SLOT_LAT GRect(0, 166, 43, 22)  // top block (coral)
#define SLOT_LON GRect(0, 203, 43, 22)  // bottom block (peach)

// --- Sensors Block ---
// the tall weather block, measured from its column's bar so it sits the same in either column. the
// glyphs are sized 1:1 to their bitmaps
#define SENSORS_WX_ICON(x, y) GRect((x) - 1, (y) + 10, 24, 24)  // the condition glyph, centred above the thermometer
#define SENSORS_COND(x, y)    GRect((x) + 25, (y) + 14, 42, 18) // the condition word beside it (Antonio 16)
#define SENSORS_THERMO(x, y)  GRect((x) + 4, (y) + 38, 13, 17)  // the thermometer, centred below the condition glyph
#define SENSORS_TEMP(x, y)    GRect((x) + 20, (y) + 36, 49, 24) // the temperature beside it (Antonio 20)
// the weather glyphs are not all cut to the same margin. any whose art starts
// higher than this inside its box gets nudged down to it. that keeps the whole
// set clear of the bar overhead
#define WX_MIN_TOP 2

// --- Icons ---
// sized 1:1 to match bitmap
#define BT_ICON GRect(30, 95, 14, 14)       // bluetooth status in the first left-rail block
#define QT_ICON GRect(15, 95, 14, 14)       // quiet-time status, just left of the bluetooth glyph

// --- LCARS Header Bars ---
// these get drawn rather than baked into the background art. each one is a pill
// an 8px rounded cap at either end with a square body between them
// a 1px notch of the black field sits between each cap and the body
// the label holder box is painted over the top afterwards
#define BAR_CAP_W 8
#define BAR_NOTCH 1

// the stardate bar spans the whole width so it gets its own rect. the four slot
// bars come from OPS_BAR. the label rides a pixel high inside its holder box and
// the spare row underneath is what squares that up
#define BAR_STARDATE GRect(52, 2, 143, 11)

// --- Tactical ---
// three 2x2 grids: one in the top frame, one in the band between the two elbows, and one in the
// bottom frame. every row here matches the Tactical frames, frame/<theme>-tactical~emery.html
#define TACTICAL_ROW_TOP    4    // the top grid's first bar. its rows run 4 to 66, clear of the strip on row 69
#define TACTICAL_ROW_MID    84   // the band's grid, rows 84 to 146 between the two strips
#define TACTICAL_ROW_BOTTOM 166  // the bottom frame's grid, rows 166 to 228, where the Stardate layout's sits
#define TACTICAL_ROW_H      31   // one grid row, its bar and its reading

// the clock and the date as panels, measured from the first bar of the grid they sit in. the
// clock's digits sit 8 rows into their box, so these put them on rows 2 to 57 of the grid. in the
// top grid that leaves room under them for Voyager's frost line on rows 64 to 66. the date keeps the
// Stardate layout's place 15 rows under its bar, and its smaller fallbacks step down with it
#define TACTICAL_TIME(y)      GRect(53, (y) - 6, 142, 74)
#define TACTICAL_MERIDIEM(y)  GRect(150, (y) - 3, 45, 14)
#define TACTICAL_DATE(y)      GRect(53, (y) + 15, 142, 44)
#define TACTICAL_DATE_SM(y)   GRect(53, (y) + 18, 142, 43)
#define TACTICAL_DATE_XS(y)   GRect(53, (y) + 20, 142, 42)
#define TACTICAL_DATE_XXS(y)  GRect(53, (y) + 22, 142, 40)

// the coordinates sit on the bottom frame's two lower rail blocks, rows 176 to 200 and 203 to 226,
// right-aligned like the Stardate layout's. Antonio 12's digits sit 2 rows into their box, so these
// centre them in each block
#define TACTICAL_LAT GRect(0, 181, 43, 22)
#define TACTICAL_LON GRect(0, 208, 43, 22)

// the compact clock and date fill a row a grid wide, measured from that row's bar. the clock has no
// bar, so Antonio 32 has the whole row, and its AM/PM sits top right as on the full one. the date
// sits under its STARDATE bar like any reading, a row's width and centred, stepping down through
// Antonio 14 and 12 for a long format
#define TACTICAL_TIME_COMPACT(y)      GRect(53, (y) - 3, 142, 36)
#define TACTICAL_MERIDIEM_COMPACT(y)  GRect(150, (y) - 1, 45, 14)
#define TACTICAL_DATE_COMPACT(y, dy)  GRect(55, (y) + 11 + (dy), 140, 18)

// the pills run rows 88 to 110 and 119 to 141, from x=2 to the rail's edge, and carry the phone link
// and Quiet Time as words in place of the glyphs the Stardate layout draws. each word sits in the
// middle of its pill, so it reads the same with the rail on either side
#define TACTICAL_LINK GRect(2, 92, 45, 22)
#define TACTICAL_QUIET GRect(2, 123, 45, 22)

/**
 * @brief Register fonts, build the baked frame + overlays, and set the background.
 *
 * Call after the window exists and before engine_init, so the frame sits under the slots.
 *
 * @param window The main window.
 */
void stardate_setup(Window *window);

/**
 * @brief Fill the engine's slot list: the text readouts plus the overlays draw-slot.
 *
 * @param out The slot array to fill.
 * @param max How many slots out can hold.
 * @param bounds The window's root bounds (for the full-window overlays slot).
 * @return How many slots were written.
 */
uint8_t stardate_build(EngineSlot *out, uint8_t max, GRect bounds);

/**
 * @brief Whether any ops slot is showing a pick, after the shape rules.
 *
 * main asks this to start only the health parts a panel on screen needs.
 *
 * @param id The OpsId.
 * @return True when a slot shows it.
 */
bool stardate_ops_shows(uint8_t id);

/**
 * @brief Whether the clock is on the face.
 *
 * The Stardate layout always draws it. On Tactical it is a panel, and a wearer can leave it off.
 *
 * @return True when the clock is drawn.
 */
bool stardate_shows_clock(void);

/**
 * @brief Whether the date is on the face, the same way as the clock.
 *
 * @return True when the date is drawn.
 */
bool stardate_shows_date(void);

/**
 * @brief Whether any drawn ops slot is showing the .beats reading.
 *
 * A beat is 86.4 seconds, so a slot showing one needs its own ticker rather than the minute
 * tick. The pick is independent of the clock's own time format, so this asks the slots.
 *
 * @return True if at least one slot on screen shows .beats.
 */
bool stardate_ops_shows_beats(void);

/**
 * @brief The theme showing right now.
 *
 * In Fixed mode this is the picked theme. In Day / Night / Quiet mode it is the night or Quiet Time
 * theme when those are on and in effect, and the picked one otherwise. Every paint reads its colours
 * from here so the whole face stays on one theme.
 *
 * @return A theme setting value.
 */
uint8_t stardate_active_theme(void);

/**
 * @brief Reload the frame if the active theme has changed since the last paint.
 *
 * Call it on the clock tick so Day / Night / Quiet mode swaps the frame when dark falls or Quiet
 * Time starts. It does nothing when the theme has not moved.
 */
void stardate_refresh_theme(void);

/**
 * @brief Re-apply the theme by swapping the frame bitmap.
 */
void stardate_apply_theme(void);

/**
 * @brief Tear down the frame, overlays, and fonts.
 */
void stardate_teardown(void);

/** @} */
