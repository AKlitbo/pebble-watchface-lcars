/**
 * @file layout.c
 * @brief stardate-emery face: the zone table bound to shared readouts, the baked LCARS
 * frame, and the overlays draw-slot (battery + labels + glyphs). No shell. The face drives
 * the engine directly from main.
 */
#include "layout.h"

#include <string.h>

#include "ui/fonts.h"
#include "ui/engine/engine.h"
#include "ui/zone.h"
#include "dev/dev.h"
#include "draw/fonts.h"
#include "ops/ops.h"
#include "ops/ops_text.h"
#include "settings_schema.h"
#include "ui/readouts.h"
#include "ui/icon_cache.h"
#include "theme/theme.h"
#include "widgets/widgets.h"
#include "widgets/panels.h"
#include "io/stores/system_store.h"
#include "io/stores/weather_store.h"
#include "io/stores/time_store.h"
#include "clock/solar.h"
#include "role/role.h"
#include "system/settings/settings.h"
#include "system/settings/setting_values.h"

// --- Zone Table ---
// the clock, the date, the coordinates, and Engineering's minutes and battery level. readouts are
// white on the black field, and the words on the rail are black like its numerals
//
// only what every layout shares is set here. each layout's place function below puts the rest, its
// places, sizes, and alignments, on every build
static Zone s_zones[ZONE_COUNT] = {
    [ZONE_TIME]     = {.align = GTextAlignmentCenter, .color = GColorWhite},
    [ZONE_MERIDIEM] = {.color = GColorWhite},
    [ZONE_DATE]     = {.align = GTextAlignmentCenter, .color = GColorWhite},
    [ZONE_LAT]      = {.font_id = FONT_ANTONIO_12, .color = COORD_TEXT_COLOR},
    [ZONE_LON]      = {.font_id = FONT_ANTONIO_12, .color = COORD_TEXT_COLOR},
    [ZONE_MINUTES]  = {.font_id = FONT_ANTONIO_100, .align = GTextAlignmentCenter},
    [ZONE_POWER]    = {.font_id = FONT_ANTONIO_12, .color = COORD_TEXT_COLOR},
};

/**
 * @brief The clock's hours, or the whole reading when it has no colon to split at, as .beats does.
 *
 * @param out The buffer.
 * @param size Its size.
 */
static void hours_text(char *out, size_t size)
{
    readout_time(out, size);

    // Engineering's big digits have no @, and its .BEATS word says what the number is
    if (out[0] == '@')
    {
        memmove(out, out + 1, strlen(out));
    }

    char *colon = strchr(out, ':');

    if (colon)
    {
        *colon = '\0';
    }
}

/**
 * @brief The clock's minutes, or nothing for a reading with no colon.
 *
 * @param out The buffer.
 * @param size Its size.
 */
static void minutes_text(char *out, size_t size)
{
    char time[16];
    readout_time(time, sizeof(time));

    const char *colon = strchr(time, ':');
    snprintf(out, size, "%s", colon ? colon + 1 : "");
}

/**
 * @brief Whether the clock reads .beats rather than the time.
 *
 * @return True when Time Format is .beats.
 */
static bool clock_is_beats(void)
{
    return settings_u8(SETTING_TIME_FORMAT) == TIME_FORMAT_BEATS;
}

/**
 * @brief Engineering's word between the hours and the minutes, AM or PM, or .BEATS under a .beats reading.
 *
 * @param out The buffer.
 * @param size Its size.
 */
static void engineering_meridiem_text(char *out, size_t size)
{
    if (clock_is_beats())
    {
        snprintf(out, size, ".BEATS");
        return;
    }

    readout_meridiem(out, size);
}

// --- Ops cells ---
// the pick is read fresh on every paint rather than cached, so a settings change lands on the
// next rebuild with nothing to invalidate. dev_ops_pick hands back the saved value in any build
// that is not running the screenshot walk
//
// the Stardate layout has one grid of four cells, Tactical three, and Engineering one column of six.
// each grid's cells run down each column in turn, so left top then left bottom then right top then
// right bottom, and Tactical's grids follow each other from the top, so its cells run 0 to 11
#define CELLS_MAX LCARS_TACTICAL_CELLS
#define AREAS_MAX 3

_Static_assert(LCARS_ENGINEERING_CELLS <= CELLS_MAX, "the cell arrays have to hold Engineering's column");

// a grid of two columns of two rows from its first bar, as the Stardate layout and each of
// Tactical's frames hold. the columns are placed on each build, since they follow the rail
#define TACTICAL_GRID(y)                                                       \
    {.cols = 2, .rows = 2, .col_x = {OPS_COL_L, OPS_COL_R},                  \
     .col_right = {OPS_R_L, OPS_R_R}, .row_y = {(y), (y) + TACTICAL_ROW_H},    \
     .bottom = (y) + 2 * TACTICAL_ROW_H}

// the Stardate layout's grid sits under the clock where Tactical's bottom frame grid does
static OpsGrid s_stardate_grid = TACTICAL_GRID(TACTICAL_ROW_BOTTOM);

// Engineering's panels: one column of six rows at the right of the screen, a row apart
#define ENGINEERING_ROW(n) (ENGINEERING_ROW_0 + (n) * ENGINEERING_ROW_PITCH)

static OpsGrid s_engineering_grid = {
    .cols = 1,
    .rows = 6,
    .col_x = {ENGINEERING_COL},
    .col_right = {OPS_R_R},
    .row_y = {ENGINEERING_ROW(0), ENGINEERING_ROW(1), ENGINEERING_ROW(2), ENGINEERING_ROW(3), ENGINEERING_ROW(4), ENGINEERING_ROW(5)},
    .bottom = ENGINEERING_ROW(5) + TACTICAL_ROW_H,
};

static OpsGrid s_tactical_grids[AREAS_MAX] = {
    TACTICAL_GRID(TACTICAL_ROW_TOP),
    TACTICAL_GRID(TACTICAL_ROW_MID),
    TACTICAL_GRID(TACTICAL_ROW_BOTTOM),
};

/** @brief How far what sits inside the frame moves left: the rail's width with the rail down the right. */
static int inside_shift(void)
{
    return lcars_rail_right() ? RAIL_W : 0;
}

/**
 * @brief Where something inside the frame sits, given its place with the rail on the left.
 *
 * @param box Its place with the rail on the left.
 * @return Its place on the screen.
 */
static GRect inside(GRect box)
{
    box.origin.x -= inside_shift();
    return box;
}

/**
 * @brief Where something on the rail sits, given its place with the rail on the left. With the rail
 * down the right it flips across the screen.
 *
 * @param box Its place with the rail on the left.
 * @return Its place on the screen.
 */
static GRect on_rail(GRect box)
{
    if (lcars_rail_right())
    {
        box.origin.x = PBL_DISPLAY_WIDTH - box.origin.x - box.size.w;
    }

    return box;
}

/**
 * @brief Puts every grid's columns inside the frame, so a panel stays in the column it was set in.
 *
 * Engineering's column turns with the rail instead, so with the rail down the right its panels run down
 * the left edge and the clock sits beside the rail, the same way round as its frame.
 */
static void place_grids(void)
{
    int shift = inside_shift();
    GRect engineering_col = on_rail(GRect(ENGINEERING_COL, 0, OPS_R_R - ENGINEERING_COL, 0));

    s_engineering_grid.col_x[0] = engineering_col.origin.x;
    s_engineering_grid.col_right[0] = engineering_col.origin.x + engineering_col.size.w;

    for (int index = 0; index <= AREAS_MAX; index++)
    {
        // the Stardate layout's grid first, then each of Tactical's
        OpsGrid *grid = index == 0 ? &s_stardate_grid : &s_tactical_grids[index - 1];

        grid->col_x[0] = OPS_COL_L - shift;
        grid->col_x[1] = OPS_COL_R - shift;
        grid->col_right[0] = OPS_R_L - shift;
        grid->col_right[1] = OPS_R_R - shift;
    }
}

// --- Layouts ---
// what sets each layout apart, read through layout() rather than tested for by name. a layout is a
// row of s_layouts at the foot of this file: its grids, where its picks are saved, and its own
// functions for the chrome it draws, where its text goes, and which text slots it builds

/** @brief One layout: its panels, and what it draws and places around them. */
typedef struct
{
    OpsGrid *grids;                ///< its panel grids, one an area, from the top
    uint8_t areas;                 ///< how many grids it has
    uint8_t (*pick)(int cell);     ///< where a cell's saved pick is read from
    bool places_clock;             ///< the clock and the date are panels the wearer places once each
    bool big_clock;                ///< it draws the clock in Antonio 100, which only loads while it is on show
    bool theme_text;               ///< a text zone takes the theme's colour, so a theme switch rebuilds
    void (*chrome)(GContext *ctx); ///< draws what it adds over its frame besides the panels
    void (*place)(int clock_cell, int date_cell); ///< puts its clock, date, and rail words
    uint8_t (*build)(EngineSlot *out, uint8_t i, bool clock, bool date); ///< adds its text slots after the overlays
} LayoutSpec;

static const LayoutSpec *layout(void);

/** @brief How many grids the layout on show has. */
static int area_count(void)
{
    return layout()->areas;
}

/**
 * @brief How many cells each of the layout's grids holds. Every grid of a layout is the same shape.
 *
 * @return The cells in one grid.
 */
static int per_area(void);

/** @brief How many cells the layout on show has, across all its grids. */
static int cell_count(void)
{
    return area_count() * per_area();
}

/**
 * @brief One of the layout's grids.
 *
 * @param area 0 for the first, and on Tactical 1 for the band and 2 for the bottom frame.
 * @return The grid.
 */
static const OpsGrid *area_grid(int area)
{
    return &layout()->grids[area];
}

static int per_area(void)
{
    return ops_cell_count(area_grid(0));
}

/**
 * @brief The grid a cell sits in.
 *
 * @param cell The cell, across all the layout's grids.
 * @return The grid.
 */
static const OpsGrid *cell_grid(int cell)
{
    return area_grid(cell / per_area());
}

/**
 * @brief The row of the screen a cell's bar starts on.
 *
 * @param cell The cell, across all the layout's grids.
 * @return The bar's top row.
 */
static int cell_bar_y(int cell)
{
    const OpsGrid *grid = cell_grid(cell);

    return grid->row_y[ops_cell_row(grid, cell % per_area())];
}

/**
 * @brief What a cell is saved as showing, before the shape rules.
 *
 * @param cell The cell, across all the layout's grids.
 * @return An OpsId.
 */
static uint8_t stored_pick(int cell)
{
    return layout()->pick(cell);
}

/**
 * @brief What a Stardate cell is saved as showing. Its four cells each have their own setting.
 *
 * @param cell The cell, 0 to 3.
 * @return An OpsId.
 */
static uint8_t stardate_pick(int cell)
{
    static uint8_t (*const slots[])(void) = {
        lcars_slot_lt, lcars_slot_lb, lcars_slot_rt, lcars_slot_rb,
    };

    return slots[cell]();
}

/**
 * @brief Work out which cell of its own grid fills each cell, from the picks.
 *
 * @param picks The picks, one a cell.
 * @param owner Receives the cell of its own grid that fills each cell, or -1 for one nothing fills.
 */
static void own_cells(const uint8_t picks[CELLS_MAX], int8_t owner[CELLS_MAX])
{
    for (int area = 0; area < area_count(); area++)
    {
        int first = area * per_area();
        ops_owners(area_grid(area), &picks[first], &owner[first]);
    }
}

// the clock and the date in either size: a full one and a compact one are the same clock or date,
// so the face shows one of each at most
#define ONCE_GROUPS 2
enum { ONCE_CLOCK, ONCE_DATE };

/**
 * @brief Which of the clock or the date a pick is, in either size.
 *
 * @param id The OpsId.
 * @return ONCE_CLOCK, ONCE_DATE, or -1 for any other pick.
 */
static int once_group(uint8_t id)
{
    if (id == OPS_CLOCK || id == OPS_CLOCK_COMPACT)
    {
        return ONCE_CLOCK;
    }

    if (id == OPS_STARDATE || id == OPS_STARDATE_COMPACT)
    {
        return ONCE_DATE;
    }

    return -1;
}

/**
 * @brief What every cell is showing, after the shape rules.
 *
 * A cell keeps its pick only when the pick starts there and fits its grid. A cell a bigger panel
 * covers, or one holding a pick that does not fit, reads as empty rather than as a labelled panel
 * with nothing under it.
 *
 * The clock and the date only go on Tactical, and only once each in either size. The Stardate layout
 * draws them in their own places, so there they read as empty, and so does a second copy of either.
 * A copy only counts once the shape rules have kept it, so a first copy that does not fit leaves the
 * next one to be drawn rather than leaving the face without a clock.
 *
 * @param ids Receives each cell's OpsId.
 * @param owner Receives the cell of its own grid that fills each cell, or -1 for one nothing fills.
 */
static void resolve(uint8_t ids[CELLS_MAX], int8_t owner[CELLS_MAX])
{
    uint8_t picks[CELLS_MAX];
    int count = cell_count();

    for (int cell = 0; cell < count; cell++)
    {
        uint8_t id = dev_ops_pick(cell, stored_pick(cell));

        if (once_group(id) >= 0 && !layout()->places_clock)
        {
            id = OPS_EMPTY;
        }

        picks[cell] = id;
    }

    own_cells(picks, owner);

    bool placed_once[ONCE_GROUPS] = {false, false};
    bool cleared = false;

    for (int cell = 0; cell < count; cell++)
    {
        bool kept = owner[cell] == cell % per_area();
        int group = once_group(picks[cell]);

        if (!kept || group < 0)
        {
            continue;
        }

        bool *placed = &placed_once[group];

        if (*placed)
        {
            picks[cell] = OPS_EMPTY;
            cleared = true;
        }
        else
        {
            *placed = true;
        }
    }

    // a second copy cleared frees the cells it filled, so the owners are worked out again
    if (cleared)
    {
        own_cells(picks, owner);
    }

    for (int cell = 0; cell < count; cell++)
    {
        ids[cell] = owner[cell] == cell % per_area() ? picks[cell] : OPS_EMPTY;
    }
}

// what every cell shows and which cell fills it, worked out once a build. a pick only changes on a
// settings save or a dev walk tap, and both rebuild, so the paint path reads these rather than
// parsing the saved lists again for every cell on every repaint
static uint8_t s_ids[CELLS_MAX];
static int8_t s_owner[CELLS_MAX];

/**
 * @brief What a cell is showing, as of the last build.
 *
 * @param cell The cell, across all the layout's grids.
 * @return An OpsId.
 */
static uint8_t pick(int cell)
{
    return s_ids[cell];
}

/**
 * @brief The cell showing a pick.
 *
 * @param ids What every cell shows.
 * @param id The OpsId.
 * @return The cell, or -1 when no cell shows it.
 */
static int cell_in(const uint8_t ids[CELLS_MAX], uint8_t id)
{
    for (int cell = 0; cell < cell_count(); cell++)
    {
        if (ids[cell] == id)
        {
            return cell;
        }
    }

    return -1;
}

/**
 * @brief The cell showing the clock or the date in either size.
 *
 * @param ids What every cell shows.
 * @param group ONCE_CLOCK or ONCE_DATE.
 * @return The cell, or -1 when neither size is on the face.
 */
static int once_cell_in(const uint8_t ids[CELLS_MAX], int group)
{
    int full = cell_in(ids, group == ONCE_CLOCK ? OPS_CLOCK : OPS_STARDATE);

    return full >= 0 ? full : cell_in(ids, group == ONCE_CLOCK ? OPS_CLOCK_COMPACT : OPS_STARDATE_COMPACT);
}

// main.c asks the three below whenever it sets up the clock, which can be before a build, so each
// works the picks out fresh rather than reading the ones the last build kept

bool stardate_ops_shows(uint8_t id)
{
    uint8_t ids[CELLS_MAX];
    int8_t owner[CELLS_MAX];

    resolve(ids, owner);
    return cell_in(ids, id) >= 0;
}

/**
 * @brief Whether the layout on show draws the clock or the date. Only Tactical can leave either off.
 *
 * @param group ONCE_CLOCK or ONCE_DATE.
 * @return True when it is on the face.
 */
static bool shows_once(int group)
{
    if (!layout()->places_clock)
    {
        return true;
    }

    uint8_t ids[CELLS_MAX];
    int8_t owner[CELLS_MAX];

    resolve(ids, owner);
    return once_cell_in(ids, group) >= 0;
}

bool stardate_shows_clock(void)
{
    return shows_once(ONCE_CLOCK);
}

bool stardate_shows_date(void)
{
    return shows_once(ONCE_DATE);
}

bool stardate_ops_shows_beats(void)
{
    // the single panel and the wide and tall bars all count beats, and a slot another panel covers
    // reads as empty, so a covered pick never turns the ticker on
    return stardate_ops_shows(OPS_BEATS) || stardate_ops_shows(OPS_WIDE_BEATS) || stardate_ops_shows(OPS_TALL_BEATS);
}

// one ops cell's Zone in its normal and no-glyph widths. a catalogue entry can hand a cell a
// seven-digit julian day or a phase name like WAN CRES. so each one steps down through Antonio 14
// to 12 before it would clip
#define OPS_ZONE(x, y, r)                                                    \
    {.rect = OPS_VALUE((x), (y), (r), 0), .font_id = FONT_ANTONIO_16,        \
     .align = GTextAlignmentLeft, .color = GColorWhite,                      \
     .font_id_fallback = FONT_ANTONIO_14,                                    \
     .rect_fallback = OPS_VALUE((x), (y), (r), OPS_DY_FB),                   \
     .font_id_fallback2 = FONT_ANTONIO_12,                                   \
     .rect_fallback2 = OPS_VALUE((x), (y), (r), OPS_DY_FB2)}

#define OPS_ZONE_WIDE(x, y, r)                                               \
    {.rect = OPS_VALUE_WIDE((x), (y), (r), 0), .font_id = FONT_ANTONIO_16,   \
     .align = GTextAlignmentLeft, .color = GColorWhite,                      \
     .font_id_fallback = FONT_ANTONIO_14,                                    \
     .rect_fallback = OPS_VALUE_WIDE((x), (y), (r), OPS_DY_FB),              \
     .font_id_fallback2 = FONT_ANTONIO_12,                                   \
     .rect_fallback2 = OPS_VALUE_WIDE((x), (y), (r), OPS_DY_FB2)}

// each cell's zone, filled in on each build from the grid the cell sits in
static Zone s_cell_zones[CELLS_MAX];

/**
 * @brief Fill in a cell's zone: the narrow one beside a glyph, or the wide one that takes the
 * glyph's space back when the readout has none.
 *
 * stardate_build runs again on every engine_rebuild, and a cell only changes on a settings change,
 * which rebuilds. So working the zone out there is enough and nothing has to swap it later.
 *
 * @param cell The cell, across all the layout's grids.
 * @return The zone to bind.
 */
static const Zone *cell_zone(int cell)
{
    const OpsGrid *grid = cell_grid(cell);
    int slot = cell % per_area();
    int x = grid->col_x[ops_cell_col(grid, slot)];
    int y = cell_bar_y(cell);
    int right = grid->col_right[ops_cell_col(grid, slot)];
    bool has_icon = ops_icon(ops_entry(pick(cell))) != 0;

    s_cell_zones[cell] = has_icon ? (Zone)OPS_ZONE(x, y, right) : (Zone)OPS_ZONE_WIDE(x, y, right);
    return &s_cell_zones[cell];
}

// one text formatter per cell. the engine's text-slot signature carries no argument, so the cell
// each one reads is baked in rather than passed
#define CELL_TEXT(n) \
    static void cell_text_##n(char *out, size_t size) { ops_text(ops_entry(pick(n)), out, size); }

CELL_TEXT(0)
CELL_TEXT(1)
CELL_TEXT(2)
CELL_TEXT(3)
CELL_TEXT(4)
CELL_TEXT(5)
CELL_TEXT(6)
CELL_TEXT(7)
CELL_TEXT(8)
CELL_TEXT(9)
CELL_TEXT(10)
CELL_TEXT(11)

static void (*const s_cell_text[CELLS_MAX])(char *out, size_t size) = {
    cell_text_0, cell_text_1, cell_text_2, cell_text_3, cell_text_4, cell_text_5,
    cell_text_6, cell_text_7, cell_text_8, cell_text_9, cell_text_10, cell_text_11,
};

/**
 * @brief Antonio at the sizes each slot needs, registered under their category ids.
 */
static void load_fonts(void)
{
    fonts_register(FONT_ANTONIO_62, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_62)));
    fonts_register(FONT_ANTONIO_36, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_36)));
    fonts_register(FONT_ANTONIO_32, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_32)));
    fonts_register(FONT_ANTONIO_28, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_28)));
    fonts_register(FONT_ANTONIO_24, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_24)));
    fonts_register(FONT_ANTONIO_20, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_20)));
    fonts_register(FONT_ANTONIO_16, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_16)));
    fonts_register(FONT_ANTONIO_14, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_14)));
    fonts_register(FONT_ANTONIO_12, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_12)));
    fonts_register(FONT_ANTONIO_10, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_10)));
}

// --- Baked LCARS frame ---
static BitmapLayer *s_frame_layer;
static GBitmap *s_frame_bitmap;
static uint8_t s_loaded_frame = 0xFF;  // 0xFF = nothing loaded yet (forces first load)

/**
 * @brief (Re)load the baked frame for a theme, in the layout on show.
 *
 * @param theme The theme setting value.
 * @return True when it loaded a frame, false when the one on show was already right.
 */
static bool load_frame(uint8_t theme)
{
    // the theme takes the low four bits, the rail side the next, and the layout the high ones, so any
    // of them changing reloads
    uint8_t frame = (uint8_t)((lcars_layout() << 5) | (lcars_rail_right() << 4) | (theme & 0x0F));

    if (frame == s_loaded_frame && s_frame_bitmap)
    {
        return false;
    }

    s_loaded_frame = frame;

    if (s_frame_bitmap)
    {
        gbitmap_destroy(s_frame_bitmap);
        s_frame_bitmap = NULL;
    }

    s_frame_bitmap = gbitmap_create_with_resource(bg_resource_for(theme, lcars_layout(), lcars_rail_right()));

    if (s_frame_layer)
    {
        bitmap_layer_set_bitmap(s_frame_layer, s_frame_bitmap);
    }

    return true;
}

/**
 * @brief Whether the sun is down right now, from the sunrise and sunset the phone sent.
 *
 * Before the phone has sent either, it falls back to a plain six-to-eighteen day, so night still
 * means night rather than reading as day all the time.
 *
 * @return True while it is dark.
 */
static bool is_night(void)
{
    int rise = weather_store_sunrise();
    int set = weather_store_sunset();

    // the two readings arrive together, so a missing one means neither is known. fall back to the
    // whole plain day rather than pairing a real time with a made-up one
    if (rise < 0 || set < 0)
    {
        rise = 6 * 60;
        set = 18 * 60;
    }

    const struct tm *now = time_store_tm();
    int now_min = now->tm_hour * 60 + now->tm_min;

    return solar_day_progress(rise, set, now_min) < 0;
}

uint8_t stardate_active_theme(void)
{
    uint8_t day = settings_u8(SETTING_THEME);

    if (lcars_theme_mode() != THEME_MODE_DAYNIGHT)
    {
        return day;
    }

    // Quiet Time wins over night, and each override only counts when its own toggle is on
    Role role = role_pick(quiet_time_is_active(), lcars_use_quiet_theme(),
                          is_night(), lcars_use_night_theme());

    switch (role)
    {
        case ROLE_QUIET:
            return lcars_quiet_theme();
        case ROLE_NIGHT:
            return lcars_night_theme();
        default:
            return day;
    }
}

void stardate_refresh_theme(void)
{
    // Engineering's minutes take the theme's word colour when the engine makes their layer, so a
    // Day / Night or Quiet switch that loads another frame rebuilds to repaint them in the new one.
    // that is a rebuild a few times a day at most, against keeping the colour on every tick
    if (load_frame(stardate_active_theme()) && layout()->theme_text)
    {
        engine_rebuild();
    }
}

/**
 * @brief Paint one grid's bars, words, big panels, and glyphs.
 *
 * @param ctx The graphics context.
 * @param area The grid, 0 for the first.
 * @param ids What every cell of the layout shows.
 * @param owner The cell of its own grid that fills each cell.
 */
static void draw_area(GContext *ctx, int area, const uint8_t *ids, const int8_t *owner)
{
    // the bar word and the glyph both follow each slot's pick so they get resolved together here
    // and handed down. widgets stays a painter that is told what to draw
    const OpsGrid *grid = area_grid(area);
    const uint8_t *area_ids = &ids[area * per_area()];
    const int8_t *area_owner = &owner[area * per_area()];
    OpsChrome chrome = {.grid = grid, .labels_right = lcars_labels_right()};

    for (int slot = 0; slot < ops_cell_count(grid); slot++)
    {
        const OpsReadout *entry = ops_entry(area_ids[slot]);
        chrome.label[slot] = ops_label(entry);
        chrome.icon[slot] = ops_icon(entry);
        chrome.shape[slot] = ops_shape(area_ids[slot]);
        chrome.covered[slot] = area_owner[slot] >= 0 && area_owner[slot] != slot;
        chrome.bare[slot] = entry->bare;
    }

    // bars first: the label holder boxes are painted over the top of them
    widgets_draw_bars(ctx, &chrome);
    widgets_draw_labels(ctx, &chrome);

    for (int slot = 0; slot < ops_cell_count(grid); slot++)
    {
        if (panels_paints(area_ids[slot]))
        {
            panels_draw(ctx, grid, area_ids[slot], slot);
        }
    }

    widgets_draw_glyphs(ctx, &chrome);
}

/**
 * @brief Light a LINK and a QUIET box for the phone link and Quiet Time, whatever the show/hide
 * settings say. Quiet Time sends no change event, so its box catches up on the clock tick.
 *
 * @param ctx The graphics context.
 * @param link The LINK box with the rail on the left.
 * @param quiet The QUIET box with the rail on the left.
 * @param drop How far each word sits into its box.
 * @param radius The boxes' corner radius, 0 for a block.
 */
static void draw_link_and_quiet(GContext *ctx, GRect link, GRect quiet, int drop, int radius)
{
    GFont font = fonts_get(FONT_ANTONIO_12);

    widgets_draw_rail_word(ctx, on_rail(link), "LINK", font, drop, radius, system_store_bluetooth());
    widgets_draw_rail_word(ctx, on_rail(quiet), "QUIET", font, drop, radius, quiet_time_is_active());
}

/**
 * @brief The Stardate layout's chrome: the STARDATE bar over the date, the battery gauge, and the
 * glyphs, which honour the show/hide settings. The store owns the connect and disconnect vibe.
 *
 * @param ctx The graphics context.
 */
static void stardate_chrome(GContext *ctx)
{
    bool bt_show = settings_u8(SETTING_BLUETOOTH_ICON);
    bool qt_show = settings_u8(SETTING_QUIET_TIME_ICON);

    widgets_draw_stardate(ctx, inside(BAR_STARDATE), lcars_labels_right());
    widgets_draw_battery(ctx, system_store_battery(), on_rail(BATTERY_GAUGE), lcars_rail_right());
    widgets_draw_status(ctx, on_rail(BT_ICON), on_rail(QT_ICON), bt_show, system_store_bluetooth(), qt_show,
                        quiet_time_is_active());
}

/**
 * @brief Tactical's chrome: the battery gauge and the LINK and QUIET pills. The date is a panel
 * there, and its bar comes with it.
 *
 * @param ctx The graphics context.
 */
static void tactical_chrome(GContext *ctx)
{
    widgets_draw_battery(ctx, system_store_battery(), on_rail(BATTERY_GAUGE), lcars_rail_right());
    draw_link_and_quiet(ctx, TACTICAL_LINK, TACTICAL_QUIET, TACTICAL_PILL_WORD_DROP, TACTICAL_PILL_RADIUS);
}

/**
 * @brief Engineering's chrome: the date on its top strip, the battery as its power core, and the
 * LINK and QUIET blocks in its rail. With the rail down the right everything turns with it, and the
 * date starts from the rail's sweep.
 *
 * @param ctx The graphics context.
 */
static void engineering_chrome(GContext *ctx)
{
    char date[24];
    char word[40];
    readout_date(date, sizeof(date));
    snprintf(word, sizeof(word), "STARDATE %s", date);
    widgets_draw_word(ctx, on_rail(ENGINEERING_DATE), word, fonts_get(FONT_ANTONIO_12), lcars_rail_right());

    int lit = widgets_lit_cells(system_store_battery(), ENGINEERING_CELLS);
    widgets_draw_core(ctx, on_rail(ENGINEERING_CELL), ENGINEERING_CELL_PITCH, ENGINEERING_CELLS, lit);

    draw_link_and_quiet(ctx, ENGINEERING_LINK, ENGINEERING_QUIET, ENGINEERING_RAIL_WORD_DROP, 0);
}

/**
 * @brief Overlays draw-slot: the bars, words, and big panels of each grid, then the layout's chrome,
 * so the link and Quiet Time stay on top of anything that ever reaches over them.
 *
 * @param ctx The graphics context.
 * @param bounds The slot bounds (the full window).
 * @param data Unused.
 */
static void draw_overlays(GContext *ctx, GRect bounds, const void *data)
{
    for (int area = 0; area < area_count(); area++)
    {
        draw_area(ctx, area, s_ids, s_owner);
    }

    layout()->chrome(ctx);
}

void stardate_setup(Window *window)
{
    load_fonts();

    Layer *root = window_get_root_layer(window);
    window_set_background_color(window, GColorBlack);

    // the frame bitmap sits at the bottom under the engine's slot layers
    s_frame_layer = bitmap_layer_create(layer_get_bounds(root));
    layer_add_child(root, bitmap_layer_get_layer(s_frame_layer));
    load_frame(stardate_active_theme());

    widgets_load();
}

/**
 * @brief Set the clock's zones: the time's place and font, and its AM/PM's place.
 *
 * @param time Where the time sits.
 * @param font Its font.
 * @param meridiem Where its AM/PM sits.
 */
static void set_clock(GRect time, FontId font, GRect meridiem)
{
    s_zones[ZONE_TIME].rect = time;
    s_zones[ZONE_TIME].font_id = font;
    s_zones[ZONE_MERIDIEM].rect = meridiem;
}

/**
 * @brief Set the date's zone, its place and font at each of its four steps down.
 *
 * @param fonts The font at each step, biggest first.
 * @param rects The place at each step. A zero-sized place skips its step.
 */
static void set_date(const FontId fonts[4], const GRect rects[4])
{
    Zone *date = &s_zones[ZONE_DATE];

    date->font_id = fonts[0];
    date->rect = rects[0];
    date->font_id_fallback = fonts[1];
    date->rect_fallback = rects[1];
    date->font_id_fallback2 = fonts[2];
    date->rect_fallback2 = rects[2];
    date->font_id_fallback3 = fonts[3];
    date->rect_fallback3 = rects[3];
}

// the full date's fonts, the big banner Antonio stepping down to fit a long format
static const FontId s_date_fonts[4] = {FONT_ANTONIO_36, FONT_ANTONIO_32, FONT_ANTONIO_28, FONT_ANTONIO_24};

/**
 * @brief The Stardate layout's places: the big clock and the date under the STARDATE bar, and the
 * coordinates on the rail.
 *
 * @param clock_cell Unused, since its clock has its own place.
 * @param date_cell Unused, since its date has its own place.
 */
static void place_stardate(int clock_cell, int date_cell)
{
    const GRect dates[4] = {inside(SLOT_BANNER), inside(SLOT_BANNER_SM), inside(SLOT_BANNER_XS), inside(SLOT_BANNER_XXS)};

    set_clock(inside(SLOT_TIME), FONT_ANTONIO_62, inside(SLOT_MERIDIEM));
    set_date(s_date_fonts, dates);
    s_zones[ZONE_LAT].rect = on_rail(SLOT_LAT);
    s_zones[ZONE_LON].rect = on_rail(SLOT_LON);
}

/**
 * @brief Tactical's places: the clock and the date follow the cell each sits in, at the size picked,
 * and the coordinates sit on the bottom frame's rail.
 *
 * @param clock_cell The cell the clock fills, or -1.
 * @param date_cell The cell the date fills, or -1.
 */
static void place_tactical(int clock_cell, int date_cell)
{
    if (clock_cell >= 0)
    {
        int y = cell_bar_y(clock_cell);

        if (pick(clock_cell) == OPS_CLOCK_COMPACT)
        {
            set_clock(inside(TACTICAL_TIME_COMPACT(y)), FONT_ANTONIO_32, inside(TACTICAL_MERIDIEM_COMPACT(y)));
        }
        else
        {
            set_clock(inside(TACTICAL_TIME(y)), FONT_ANTONIO_62, inside(TACTICAL_MERIDIEM(y)));
        }
    }

    if (date_cell >= 0)
    {
        int y = cell_bar_y(date_cell);

        if (pick(date_cell) == OPS_STARDATE_COMPACT)
        {
            // the same three steps down a reading takes. the fourth's place is zero-sized, which skips it
            static const FontId compact_fonts[4] = {FONT_ANTONIO_16, FONT_ANTONIO_14, FONT_ANTONIO_12, FONT_ANTONIO_12};
            const GRect dates[4] = {inside(TACTICAL_DATE_COMPACT(y, 0)), inside(TACTICAL_DATE_COMPACT(y, OPS_DY_FB)),
                                    inside(TACTICAL_DATE_COMPACT(y, OPS_DY_FB2)), GRectZero};

            set_date(compact_fonts, dates);
        }
        else
        {
            const GRect dates[4] = {inside(TACTICAL_DATE(y)), inside(TACTICAL_DATE_SM(y)), inside(TACTICAL_DATE_XS(y)),
                                    inside(TACTICAL_DATE_XXS(y))};

            set_date(s_date_fonts, dates);
        }
    }

    s_zones[ZONE_LAT].rect = on_rail(TACTICAL_LAT);
    s_zones[ZONE_LON].rect = on_rail(TACTICAL_LON);
}

/**
 * @brief Engineering's places: the hours over the minutes, the AM or PM between them, and the battery
 * level in the rail. The minutes take the theme's word colour.
 *
 * @param clock_cell Unused, since its clock has its own place.
 * @param date_cell Unused, since its date is drawn on its strip.
 */
static void place_engineering(int clock_cell, int date_cell)
{
    s_zones[ZONE_MERIDIEM].font_id = FONT_ANTONIO_12;
    s_zones[ZONE_MERIDIEM].align = GTextAlignmentCenter;

    if (clock_is_beats())
    {
        set_clock(on_rail(ENGINEERING_BEATS), FONT_ANTONIO_62, on_rail(ENGINEERING_BEATS_WORD));
    }
    else
    {
        set_clock(on_rail(ENGINEERING_HOURS), FONT_ANTONIO_100, on_rail(ENGINEERING_MERIDIEM));
    }

    s_zones[ZONE_MINUTES].rect = on_rail(ENGINEERING_MINUTES);
    s_zones[ZONE_MINUTES].color = label_color_for_theme(stardate_active_theme());
    s_zones[ZONE_POWER].rect = on_rail(ENGINEERING_POWER);
}

/**
 * @brief Put the text where the layout on show wants it, on the side of the face the rail leaves it.
 *
 * @param clock_cell The cell the clock fills on a layout that places it, or -1.
 * @param date_cell The cell the date fills on a layout that places it, or -1.
 */
static void place_zones(int clock_cell, int date_cell)
{
    // the words on the rail sit against its edge nearest the panels, so they turn with it
    GTextAlignment rail_align = lcars_rail_right() ? GTextAlignmentLeft : GTextAlignmentRight;
    int zones_on_rail[] = {ZONE_LAT, ZONE_LON, ZONE_POWER};

    for (size_t index = 0; index < ARRAY_LENGTH(zones_on_rail); index++)
    {
        s_zones[zones_on_rail[index]].align = rail_align;
    }

    // the AM or PM is a small word tucked against the clock unless the layout moves it
    s_zones[ZONE_MERIDIEM].font_id = FONT_ANTONIO_10;
    s_zones[ZONE_MERIDIEM].align = GTextAlignmentRight;

    layout()->place(clock_cell, date_cell);
}

// the build below writes at most the overlays, the clock and its AM/PM, the date, twelve cells, and
// the two coordinates without reading max, so the engine's cap has to hold them. pebble.appinfo.json
// raises it, and a slot added below raises this count too
_Static_assert(ENGINE_SLOTS_MAX >= 18, "stardate_build writes up to eighteen slots");

/**
 * @brief Add a text slot for every cell. A cell under a tall, wide, or full panel keeps its text
 * layer and reads as empty, since the panel paints itself over the overlays.
 *
 * @param out The slots.
 * @param i The next free slot.
 * @return The next free slot after the cells.
 */
static uint8_t add_cells(EngineSlot *out, uint8_t i)
{
    for (int cell = 0; cell < cell_count(); cell++)
    {
        out[i++] = (EngineSlot){.zone = cell_zone(cell), .text = s_cell_text[cell]};
    }

    return i;
}

/**
 * @brief The text slots of the Stardate layout and Tactical: the clock and the date where they show,
 * the cells, and the coordinates.
 *
 * @param out The slots.
 * @param i The next free slot.
 * @param clock Whether the clock is on the face.
 * @param date Whether the date is on the face.
 * @return The next free slot.
 */
static uint8_t build_framed(EngineSlot *out, uint8_t i, bool clock, bool date)
{
    if (clock)
    {
        out[i++] = (EngineSlot){.zone = &s_zones[ZONE_TIME],     .text = readout_time};
        out[i++] = (EngineSlot){.zone = &s_zones[ZONE_MERIDIEM], .text = readout_meridiem};
    }

    if (date)
    {
        out[i++] = (EngineSlot){.zone = &s_zones[ZONE_DATE],     .text = readout_date};
    }

    i = add_cells(out, i);
    out[i++] = (EngineSlot){.zone = &s_zones[ZONE_LAT],          .text = readout_lat};
    out[i++] = (EngineSlot){.zone = &s_zones[ZONE_LON],          .text = readout_lon};
    return i;
}

/**
 * @brief Engineering's text slots: the stacked clock, the cells, and the battery level. Its date is
 * drawn on its strip, and it has no rail blocks for the coordinates.
 *
 * @param out The slots.
 * @param i The next free slot.
 * @param clock Unused, since its clock always shows.
 * @param date Unused, since its date is part of its chrome.
 * @return The next free slot.
 */
static uint8_t build_engineering(EngineSlot *out, uint8_t i, bool clock, bool date)
{
    out[i++] = (EngineSlot){.zone = &s_zones[ZONE_TIME],     .text = hours_text};
    out[i++] = (EngineSlot){.zone = &s_zones[ZONE_MINUTES],  .text = minutes_text};
    out[i++] = (EngineSlot){.zone = &s_zones[ZONE_MERIDIEM], .text = engineering_meridiem_text};
    i = add_cells(out, i);
    out[i++] = (EngineSlot){.zone = &s_zones[ZONE_POWER],    .text = ops_text_battery};
    return i;
}

static const LayoutSpec s_layouts[LCARS_LAYOUT_COUNT] = {
    [LCARS_LAYOUT_STARDATE] = {
        .grids = &s_stardate_grid, .areas = 1, .pick = stardate_pick,
        .chrome = stardate_chrome, .place = place_stardate, .build = build_framed,
    },
    [LCARS_LAYOUT_TACTICAL] = {
        .grids = s_tactical_grids, .areas = AREAS_MAX, .pick = lcars_slot_tactical, .places_clock = true,
        .chrome = tactical_chrome, .place = place_tactical, .build = build_framed,
    },
    [LCARS_LAYOUT_ENGINEERING] = {
        .grids = &s_engineering_grid, .areas = 1, .pick = lcars_slot_engineering, .big_clock = true,
        .theme_text = true, .chrome = engineering_chrome, .place = place_engineering, .build = build_engineering,
    },
};

/**
 * @brief The layout on show.
 *
 * @return Its row of s_layouts.
 */
static const LayoutSpec *layout(void)
{
    // the settings clamp the pick, and an unknown layout still draws Stardate rather than reading
    // past the table, as the frame lookup does
    uint8_t index = lcars_layout();

    return &s_layouts[index < LCARS_LAYOUT_COUNT ? index : LCARS_LAYOUT_STARDATE];
}

uint8_t stardate_build(EngineSlot *out, uint8_t max, GRect bounds)
{
    const LayoutSpec *spec = layout();

    // Antonio 100 is several KB of heap, so it is only loaded for a layout that draws with it. the
    // registry has no way to empty one slot, so any other layout parks the small Antonio 10 there
    // instead, which frees the big one. the engine has already pulled down the old layers
    fonts_swap(FONT_ANTONIO_100, spec->big_clock ? RESOURCE_ID_FONT_ANTONIO_100 : RESOURCE_ID_FONT_ANTONIO_10);

    // the grids go first, since the cells' zones and the clock's place are read off them. then the
    // picks, which the cells' zones, the clock's place, and every paint read
    place_grids();
    resolve(s_ids, s_owner);

    // a layout that places the clock and the date only draws them where a cell holds them
    int clock_cell = spec->places_clock ? once_cell_in(s_ids, ONCE_CLOCK) : -1;
    int date_cell = spec->places_clock ? once_cell_in(s_ids, ONCE_DATE) : -1;
    place_zones(clock_cell, date_cell);

    // overlays first so they sit under the text readouts
    out[0] = (EngineSlot){.frame = bounds, .draw = draw_overlays};

    return spec->build(out, 1, !spec->places_clock || clock_cell >= 0, !spec->places_clock || date_cell >= 0);
}

void stardate_apply_theme(void)
{
    load_frame(stardate_active_theme());
}

void stardate_teardown(void)
{
    widgets_unload();
    icons_cleanup();
    bitmap_layer_destroy(s_frame_layer);
    s_frame_layer = NULL;

    if (s_frame_bitmap)
    {
        gbitmap_destroy(s_frame_bitmap);
        s_frame_bitmap = NULL;
    }

    fonts_unload_all();
    s_loaded_frame = 0xFF;  // force a reload if the window is recreated
}
