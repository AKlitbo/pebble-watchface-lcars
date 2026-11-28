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
    ZONE_MINUTES,
    ZONE_POWER,
    ZONE_CORE_WORD,
    ZONE_COUNT
};

// --- Text Slots ---
#define SLOT_TIME GRect(53, 92, 142, 74)       // centred time box (Antonio 62)
// the date and its fallbacks under the STARDATE bar, the same places a Tactical date takes under its bar
#define SLOT_BANNER TACTICAL_DATE(STARDATE_BAR_Y)         // Antonio 36
#define SLOT_BANNER_SM TACTICAL_DATE_SM(STARDATE_BAR_Y)   // a wide date, Antonio 32
#define SLOT_BANNER_XS TACTICAL_DATE_XS(STARDATE_BAR_Y)   // a wider date, Antonio 28
#define SLOT_BANNER_XXS TACTICAL_DATE_XXS(STARDATE_BAR_Y) // the widest, fits the .beats formats, Antonio 24
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
#define STARDATE_BAR_Y 2
#define BAR_STARDATE GRect(52, STARDATE_BAR_Y, 143, 11)

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

// --- Engineering ---
// one rail down the left that is the power core, sweeping into the strip across the top and the strip
// along the foot. every place here matches frame/<theme>-engineering~emery.html, whose rail is 30 wide
//
// the clock stands hours over minutes between the rail and the panels in Antonio 100, with AM or PM
// between them. Antonio's digits sit 13 rows into their box at 100, so the hours land on rows 19 to
// 106 and the minutes on rows 125 to 212, the middle of the open rows 14 to 217. Antonio 12's capitals
// sit 2 rows into theirs, which puts the AM or PM on rows 111 to 120, halfway between
#define ENGINEERING_HOURS GRect(30, 6, 97, 110)
#define ENGINEERING_MERIDIEM GRect(30, 109, 97, 16)
#define ENGINEERING_MINUTES GRect(30, 112, 97, 110)

// a .beats reading has three digits, too wide for Antonio 100 in the clock's width, so it stands
// alone in Antonio 62 in the middle of the open rows with .BEATS under it
#define ENGINEERING_BEATS GRect(30, 79, 97, 76)
#define ENGINEERING_BEATS_WORD GRect(30, 144, 97, 16)

// six panels down one column at the right, the bars 69 wide to x=198, 33 rows apart from row 20, a
// little looser than a row on the other layouts. the column's first bar to its last reading runs 193
// rows, which leaves it the middle of the open rows 14 to 217
#define ENGINEERING_COL 129
#define ENGINEERING_ROW_0 20
#define ENGINEERING_ROW_PITCH 33

// the date's word on the top strip, rows 0 to 13, from just past the rail's sweep. Antonio 12's
// capitals sit 2 rows into their box, so they land on rows 2 to 11, the middle of the strip
#define ENGINEERING_DATE GRect(40, 0, 158, 14)

// the core: thirteen cells six rows tall, eight rows apart from row 61, across the rail. its reading
// sits in a block of its own just above them, rows 43 to 58, centred like the words on Transporter's
// rails. Antonio 12's capitals sit 2 rows into their box, so they land on rows 46 to 55, 3 rows clear
// of each edge
#define ENGINEERING_CELL GRect(0, 61, 30, 6)
#define ENGINEERING_CELL_PITCH 8
#define ENGINEERING_CELLS 13
#define ENGINEERING_POWER GRect(0, 44, 30, 16)

// the phone link and Quiet Time are two blocks in the rail's tail under the cells, cut out by seams
// in the frame. each is 16 rows. Antonio 12's capitals are 10 rows and sit 2 rows into their box, so
// lowering the word a row leaves 3 rows above them and 3 below
#define ENGINEERING_LINK GRect(0, 165, 30, 16)
#define ENGINEERING_QUIET GRect(0, 183, 30, 16)
#define ENGINEERING_RAIL_WORD_DROP 1

// the core's word sits on the rail over the reading's block, saying what the core shows. Antonio 12's
// capitals sit 2 rows into their box, which lands them on rows 29 to 38, clear of the seam on row 41
#define ENGINEERING_CORE_WORD GRect(0, 27, 30, 14)

// --- Transporter ---
// a rail down each side, 24 wide, joined by a strip across the top and one along the foot, with the
// clock in a frame of its own between them. every place here matches frame/<theme>-transporter~emery.html.
// the frame has no mirrored version, so nothing here turns with the rail
#define TRANSPORTER_RAIL_W 24
#define TRANSPORTER_RAIL_RIGHT 176

// the clock in Antonio 62 inside its frame, between the frame's left side and the right rail. its
// digits sit 8 rows into their box, so on rows 24 to 79, between the frame's top bar and its foot
#define TRANSPORTER_TIME GRect(38, 16, 138, 74)

// a 12 hour time with a leading zero is about 121 wide, which leaves no room beside it for the AM/PM.
// so the AM/PM goes in black on the nub on the frame's left side, rows 46 to 57 from x=28 to x=43, the
// way the rails' words sit on their blocks. Antonio 10's capitals are 8 rows and sit a row into their
// box, which puts them on rows 48 to 55, 2 rows clear of each edge. on a 24 hour clock the nub is bare
#define TRANSPORTER_MERIDIEM GRect(28, 47, 16, 12)

// the part of the nub that stands out past the frame's side, which a clock with no AM/PM paints over so
// the nub sits flush with the side
#define TRANSPORTER_NUB_OVERHANG GRect(38, 46, 6, 12)

// the date is the frame's title, in a black box at the right end of its top bar, rows 9 to 20, just
// before the round cap at x=167. a long date grows the box to the left along the bar. the word is
// drawn from the box's top, and Antonio 12's capitals sit 2 rows into it, so the box starts a row
// above the bar to leave the capitals on rows 10 to 19, a row clear of each edge
#define TRANSPORTER_DATE GRect(46, 8, 120, 13)

// two grids of two columns of two, one straight under the other from row 92, 4 rows under the clock
// frame's foot. the four rows sit 32 apart, a row looser than the other layouts', so the last row's
// body ends on row 219, just above the foot's strip. the columns are 69 wide bars from x=28 and x=102,
// which leaves 4 or 5 pixels to each rail
#define TRANSPORTER_COL_L 28
#define TRANSPORTER_COL_R 102
#define TRANSPORTER_ROW_TOP 92
#define TRANSPORTER_ROW_PITCH 32
#define TRANSPORTER_ROW_BOTTOM (TRANSPORTER_ROW_TOP + 2 * TRANSPORTER_ROW_PITCH)

// a reading in the left column stops 3 short of the right column's bar, and one in the right column
// 3 short of the right rail
#define TRANSPORTER_R_L (TRANSPORTER_COL_R - 3)
#define TRANSPORTER_R_R (TRANSPORTER_RAIL_RIGHT - 3)

// each rail's gauge: its word on the rail's head between the top sweep and the cells, thirteen cells
// eight rows tall and ten apart from row 32, then the reading in its block on rows 162 to 177. all of
// it is set in Antonio 10, the size that fits the widest word, MOON, and a full 100 in the 24 wide
// rail. the drops lower the words to sit in the middle of their blocks
#define TRANSPORTER_WORD GRect(0, 17, 24, 14)
#define TRANSPORTER_CELL GRect(0, 32, 24, 8)
#define TRANSPORTER_CELL_PITCH 10
#define TRANSPORTER_CELLS 13
#define TRANSPORTER_READING GRect(0, 162, 24, 16)

// the phone link on the left rail's last block and Quiet Time on the right's, rows 180 to 195, lit or
// dark the way Engineering's are
#define TRANSPORTER_STATE GRect(0, 180, 24, 16)
#define TRANSPORTER_BLOCK_DROP 2

/**
 * @brief Where something on Transporter's right rail sits, given its place on the left rail.
 *
 * @param box Its place on the left rail.
 * @return The same place on the right rail.
 */
static inline GRect transporter_right(GRect box)
{
    box.origin.x += TRANSPORTER_RAIL_RIGHT;
    return box;
}

// the pills run rows 88 to 110 and 119 to 141, from x=2 to the rail's edge, and light for the phone
// link and Quiet Time in place of the glyphs the Stardate layout draws. Antonio 12's capitals are 10
// rows and sit 2 rows into their box, so lowering the word 4 rows sits them in the middle of the pill
#define TACTICAL_LINK GRect(2, 88, 47, 23)
#define TACTICAL_QUIET GRect(2, 119, 47, 23)
#define TACTICAL_PILL_RADIUS 11
#define TACTICAL_PILL_WORD_DROP 4

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
 * @brief Whether a rail gauge of the layout on show is set to a pick, such as Engineering's core or
 * either of Transporter's rails.
 *
 * main asks this, beside stardate_ops_shows, to start only the stores a gauge on screen reads.
 *
 * @param id The OpsId.
 * @return True when a gauge shows it.
 */
bool stardate_gauge_shows(uint8_t id);

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
