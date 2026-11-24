/**
 * @file layout.c
 * @brief stardate-emery face: the zone table bound to shared readouts, the baked LCARS
 * frame, and the overlays draw-slot (battery + labels + glyphs). No shell. The face drives
 * the engine directly from main.
 */
#include "layout.h"

#include "ui/fonts.h"
#include "ui/zone.h"
#include "dev/dev.h"
#include "draw/fonts.h"
#include "ops/ops.h"
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
// the clock, the date, and the coordinates. Readouts are white on the black field, and the lat/lon
// coordinates sit on coloured rail blocks so they're black like the numerals
//
// the table is written to on each build, since Tactical moves the clock and the date to whichever
// grid holds them and the coordinates to the bottom frame's rail. the Stardate layout's places are
// the ones below
static Zone s_zones[ZONE_COUNT] = {
    [ZONE_TIME]     = {.rect = SLOT_TIME,     .font_id = FONT_ANTONIO_62, .align = GTextAlignmentCenter, .color = GColorWhite},
    [ZONE_MERIDIEM] = {.rect = SLOT_MERIDIEM, .font_id = FONT_ANTONIO_10, .align = GTextAlignmentRight,  .color = GColorWhite},
    [ZONE_DATE]     = {.rect = SLOT_BANNER,   .font_id = FONT_ANTONIO_36, .align = GTextAlignmentCenter, .color = GColorWhite,
                       .font_id_fallback = FONT_ANTONIO_32, .rect_fallback = SLOT_BANNER_SM,
                       .font_id_fallback2 = FONT_ANTONIO_28, .rect_fallback2 = SLOT_BANNER_XS,
                       .font_id_fallback3 = FONT_ANTONIO_24, .rect_fallback3 = SLOT_BANNER_XXS},
    [ZONE_LAT]      = {.rect = SLOT_LAT,      .font_id = FONT_ANTONIO_12, .align = GTextAlignmentRight,  .color = COORD_TEXT_COLOR},
    [ZONE_LON]      = {.rect = SLOT_LON,      .font_id = FONT_ANTONIO_12, .align = GTextAlignmentRight,  .color = COORD_TEXT_COLOR},
    // Tactical only: the pills' words, black on the pill like the coordinates on the rail
    [ZONE_LINK]     = {.rect = TACTICAL_LINK,   .font_id = FONT_ANTONIO_12, .align = GTextAlignmentCenter, .color = COORD_TEXT_COLOR},
    [ZONE_QUIET]    = {.rect = TACTICAL_QUIET,  .font_id = FONT_ANTONIO_12, .align = GTextAlignmentCenter, .color = COORD_TEXT_COLOR},
};

/**
 * @brief The phone link pill's word: LINK while the phone is connected and NO LINK once it drops.
 *
 * The pills are Tactical's own, so they leave Show Connection Icon to the Stardate layout's glyph,
 * and the settings page hides that toggle while Tactical is picked.
 *
 * @param out The buffer.
 * @param size Its size.
 */
static void link_text(char *out, size_t size)
{
    snprintf(out, size, "%s", system_store_bluetooth() ? "LINK" : "NO LINK");
}

/**
 * @brief The Quiet Time pill's word: QUIET while Quiet Time is on, and nothing while sound is on.
 *
 * It leaves Show Quiet Time Icon to the Stardate layout the same way. Quiet Time sends no change
 * event, so the word catches up on the clock tick like the glyph does.
 *
 * @param out The buffer.
 * @param size Its size.
 */
static void quiet_text(char *out, size_t size)
{
    snprintf(out, size, "%s", quiet_time_is_active() ? "QUIET" : "");
}

// --- Ops cells ---
// the pick is read fresh on every paint rather than cached, so a settings change lands on the
// next rebuild with nothing to invalidate. dev_ops_pick hands back the saved value in any build
// that is not running the screenshot walk
//
// the Stardate layout has one grid of four cells and Tactical three. each grid's four run down
// each column, left top then left bottom then right top then right bottom, and Tactical's grids
// follow each other from the top, so its cells run 0 to 11
#define CELLS_MAX LCARS_TACTICAL_CELLS
#define AREAS_MAX (CELLS_MAX / OPS_CELLS_MAX)

// the Stardate layout's grid: two columns of two rows under the clock. its columns, and Tactical's,
// are placed on each build, since they follow the rail
static OpsGrid s_stardate_grid = {
    .cols = 2,
    .rows = 2,
    .col_x = {OPS_COL_L, OPS_COL_R},
    .col_right = {OPS_R_L, OPS_R_R},
    .row_y = {OPS_ROW_T, OPS_ROW_B},
    .bottom = PBL_DISPLAY_HEIGHT,
};

// one of Tactical's grids, the same two columns of two rows as the Stardate layout's from its first bar
#define TACTICAL_GRID(y)                                                       \
    {.cols = 2, .rows = 2, .col_x = {OPS_COL_L, OPS_COL_R},                  \
     .col_right = {OPS_R_L, OPS_R_R}, .row_y = {(y), (y) + TACTICAL_ROW_H},    \
     .bottom = (y) + 2 * TACTICAL_ROW_H}

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

/** @brief Puts every grid's columns inside the frame, so a panel stays in the column it was set in. */
static void place_grids(void)
{
    int shift = inside_shift();

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

/** @brief Whether the face is drawn as Tactical. */
static bool is_tactical(void)
{
    return lcars_layout() == LCARS_LAYOUT_TACTICAL;
}

/** @brief How many grids the layout on show has. */
static int area_count(void)
{
    return is_tactical() ? AREAS_MAX : 1;
}

/** @brief How many cells the layout on show has, across all its grids. */
static int cell_count(void)
{
    return area_count() * OPS_CELLS_MAX;
}

/**
 * @brief One of the layout's grids.
 *
 * @param area 0 for the first, and on Tactical 1 for the band and 2 for the bottom frame.
 * @return The grid.
 */
static const OpsGrid *area_grid(int area)
{
    return is_tactical() ? &s_tactical_grids[area] : &s_stardate_grid;
}

/**
 * @brief What a cell is saved as showing, before the shape rules.
 *
 * @param cell The cell, across all the layout's grids.
 * @return An OpsId.
 */
static uint8_t stored_pick(int cell)
{
    static uint8_t (*const stardate[])(void) = {
        lcars_slot_lt, lcars_slot_lb, lcars_slot_rt, lcars_slot_rb,
    };

    return is_tactical() ? lcars_slot_tactical(cell) : stardate[cell]();
}

/**
 * @brief Work out which cell of its own grid fills each cell, from the picks.
 *
 * @param picks The picks, one a cell.
 * @param owner Receives the cell of its own grid that fills each cell, 0 to 3, or -1 for one nothing fills.
 */
static void own_cells(const uint8_t picks[CELLS_MAX], int8_t owner[CELLS_MAX])
{
    for (int area = 0; area < area_count(); area++)
    {
        int first = area * OPS_CELLS_MAX;
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
 * @param owner Receives the cell of its own grid that fills each cell, 0 to 3, or -1 for one nothing fills.
 */
static void resolve(uint8_t ids[CELLS_MAX], int8_t owner[CELLS_MAX])
{
    uint8_t picks[CELLS_MAX];
    int count = cell_count();

    for (int cell = 0; cell < count; cell++)
    {
        uint8_t id = dev_ops_pick(cell, stored_pick(cell));

        if (once_group(id) >= 0 && !is_tactical())
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
        bool kept = owner[cell] == cell % OPS_CELLS_MAX;
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
        ids[cell] = owner[cell] == cell % OPS_CELLS_MAX ? picks[cell] : OPS_EMPTY;
    }
}

/**
 * @brief What a cell is showing.
 *
 * @param cell The cell, across all the layout's grids.
 * @return An OpsId.
 */
static uint8_t pick(int cell)
{
    uint8_t ids[CELLS_MAX];
    int8_t owner[CELLS_MAX];

    resolve(ids, owner);
    return ids[cell];
}

/**
 * @brief The cell showing a pick.
 *
 * @param id The OpsId.
 * @return The cell, or -1 when no cell shows it.
 */
static int cell_showing(uint8_t id)
{
    uint8_t ids[CELLS_MAX];
    int8_t owner[CELLS_MAX];

    resolve(ids, owner);

    for (int cell = 0; cell < cell_count(); cell++)
    {
        if (ids[cell] == id)
        {
            return cell;
        }
    }

    return -1;
}

bool stardate_ops_shows(uint8_t id)
{
    return cell_showing(id) >= 0;
}

/**
 * @brief The cell showing the clock or the date in either size.
 *
 * @param group ONCE_CLOCK or ONCE_DATE.
 * @return The cell, or -1 when neither size is on the face.
 */
static int cell_showing_once(int group)
{
    int full = cell_showing(group == ONCE_CLOCK ? OPS_CLOCK : OPS_STARDATE);

    return full >= 0 ? full : cell_showing(group == ONCE_CLOCK ? OPS_CLOCK_COMPACT : OPS_STARDATE_COMPACT);
}

bool stardate_shows_clock(void)
{
    return !is_tactical() || cell_showing_once(ONCE_CLOCK) >= 0;
}

bool stardate_shows_date(void)
{
    return !is_tactical() || cell_showing_once(ONCE_DATE) >= 0;
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
    const OpsGrid *grid = area_grid(cell / OPS_CELLS_MAX);
    int slot = cell % OPS_CELLS_MAX;
    int x = grid->col_x[slot / grid->rows];
    int y = grid->row_y[slot % grid->rows];
    int right = grid->col_right[slot / grid->rows];
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
 */
static void load_frame(uint8_t theme)
{
    // the theme takes the low four bits, the rail side the next, and the layout the high ones, so any
    // of them changing reloads
    uint8_t frame = (uint8_t)((lcars_layout() << 5) | (lcars_rail_right() << 4) | (theme & 0x0F));

    if (frame == s_loaded_frame && s_frame_bitmap)
    {
        return;
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
    load_frame(stardate_active_theme());
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
    const uint8_t *area_ids = &ids[area * OPS_CELLS_MAX];
    const int8_t *area_owner = &owner[area * OPS_CELLS_MAX];
    OpsChrome chrome = {.grid = grid, .labels_right = lcars_labels_right()};

    for (int slot = 0; slot < OPS_CELLS_MAX; slot++)
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

    for (int slot = 0; slot < OPS_CELLS_MAX; slot++)
    {
        if (panels_paints(area_ids[slot]))
        {
            panels_draw(ctx, grid, area_ids[slot], slot);
        }
    }

    widgets_draw_glyphs(ctx, &chrome);
}

/**
 * @brief Overlays draw-slot: the battery gauge, the bar labels, the big panels, and the glyphs.
 *
 * @param ctx The graphics context.
 * @param bounds The slot bounds (the full window).
 * @param data Unused.
 */
static void draw_overlays(GContext *ctx, GRect bounds, const void *data)
{
    uint8_t ids[CELLS_MAX];
    int8_t owner[CELLS_MAX];
    resolve(ids, owner);

    // the Stardate layout keeps the date under a fixed STARDATE bar. on Tactical the date is a
    // panel, and its bar comes with it
    if (!is_tactical())
    {
        widgets_draw_stardate(ctx, inside(BAR_STARDATE), lcars_labels_right());
    }

    widgets_draw_battery(ctx, system_store_battery(), on_rail(BATTERY_GAUGE), lcars_rail_right());

    for (int area = 0; area < area_count(); area++)
    {
        draw_area(ctx, area, ids, owner);
    }

    // Tactical says the phone link and Quiet Time in its pills, so the glyphs are the Stardate
    // layout's alone. they honour the show/hide settings. the store owns the connect/disconnect vibe,
    // and the quiet-time glyph reads the live SDK state (no change event, so it refreshes on the tick)
    if (!is_tactical())
    {
        bool bt_show = settings_u8(SETTING_BLUETOOTH_ICON);
        bool qt_show = settings_u8(SETTING_QUIET_TIME_ICON);
        widgets_draw_status(ctx, on_rail(BT_ICON), on_rail(QT_ICON), bt_show, system_store_bluetooth(), qt_show,
                            quiet_time_is_active());
    }
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
 * @brief Put the clock, the date, the coordinates, and the pill words where the layout on show wants
 * them, on the side of the face the rail leaves them.
 *
 * The Stardate layout keeps its fixed places. On Tactical the clock and the date follow the cell each
 * sits in, at the size picked, and the coordinates sit on the bottom frame's rail.
 *
 * @param clock_cell The cell the clock fills, or -1.
 * @param date_cell The cell the date fills, or -1.
 */
static void place_zones(int clock_cell, int date_cell)
{
    // the coordinates sit against the rail's edge nearest the panels, so they turn with it. the pill
    // words sit in the middle of their pills and need no turning
    GTextAlignment rail_align = lcars_rail_right() ? GTextAlignmentLeft : GTextAlignmentRight;
    int zones_on_rail[] = {ZONE_LAT, ZONE_LON};

    for (size_t index = 0; index < ARRAY_LENGTH(zones_on_rail); index++)
    {
        s_zones[zones_on_rail[index]].align = rail_align;
    }

    if (!is_tactical())
    {
        const GRect dates[4] = {inside(SLOT_BANNER), inside(SLOT_BANNER_SM), inside(SLOT_BANNER_XS), inside(SLOT_BANNER_XXS)};

        set_clock(inside(SLOT_TIME), FONT_ANTONIO_62, inside(SLOT_MERIDIEM));
        set_date(s_date_fonts, dates);
        s_zones[ZONE_LAT].rect = on_rail(SLOT_LAT);
        s_zones[ZONE_LON].rect = on_rail(SLOT_LON);
        return;
    }

    if (clock_cell >= 0)
    {
        const OpsGrid *grid = area_grid(clock_cell / OPS_CELLS_MAX);
        int y = grid->row_y[(clock_cell % OPS_CELLS_MAX) % grid->rows];

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
        const OpsGrid *grid = area_grid(date_cell / OPS_CELLS_MAX);
        int y = grid->row_y[(date_cell % OPS_CELLS_MAX) % grid->rows];

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
    s_zones[ZONE_LINK].rect = on_rail(TACTICAL_LINK);
    s_zones[ZONE_QUIET].rect = on_rail(TACTICAL_QUIET);
}

// the build below writes at most the overlays, the clock and its AM/PM, the date, twelve cells, the
// two coordinates, and the two pill words without reading max, so the engine's cap has to hold them.
// pebble.appinfo.json raises it, and a slot added below raises this count too
_Static_assert(ENGINE_SLOTS_MAX >= 20, "stardate_build writes up to twenty slots");

uint8_t stardate_build(EngineSlot *out, uint8_t max, GRect bounds)
{
    uint8_t i = 0;

    // on Tactical the clock and the date are only drawn where a cell holds them
    // the grids go first, since the cells' zones and the clock's place are read off them
    place_grids();

    int clock_cell = is_tactical() ? cell_showing_once(ONCE_CLOCK) : -1;
    int date_cell = is_tactical() ? cell_showing_once(ONCE_DATE) : -1;
    place_zones(clock_cell, date_cell);

    // overlays first so they sit under the text readouts
    out[i++] = (EngineSlot){.frame = bounds, .draw = draw_overlays};

    if (!is_tactical() || clock_cell >= 0)
    {
        out[i++] = (EngineSlot){.zone = &s_zones[ZONE_TIME],     .text = readout_time};
        out[i++] = (EngineSlot){.zone = &s_zones[ZONE_MERIDIEM], .text = readout_meridiem};
    }

    if (!is_tactical() || date_cell >= 0)
    {
        out[i++] = (EngineSlot){.zone = &s_zones[ZONE_DATE],     .text = readout_date};
    }

    // a cell under a tall, wide, or full panel keeps its text layer and reads as empty, since the
    // panel paints itself over the overlays
    for (int cell = 0; cell < cell_count(); cell++)
    {
        out[i++] = (EngineSlot){.zone = cell_zone(cell), .text = s_cell_text[cell]};
    }

    out[i++] = (EngineSlot){.zone = &s_zones[ZONE_LAT],      .text = readout_lat};
    out[i++] = (EngineSlot){.zone = &s_zones[ZONE_LON],      .text = readout_lon};

    if (is_tactical())
    {
        out[i++] = (EngineSlot){.zone = &s_zones[ZONE_LINK],  .text = link_text};
        out[i++] = (EngineSlot){.zone = &s_zones[ZONE_QUIET], .text = quiet_text};
    }

    return i;
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
