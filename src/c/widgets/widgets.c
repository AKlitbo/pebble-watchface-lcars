/**
 * @file widgets.c
 * @brief Painted overlays: battery gauge, bar labels, the slot glyphs, and the bluetooth and
 * quiet-time glyphs, drawn over the baked frame by the engine's overlays draw-slot.
 *
 * @ingroup watchface-lcars
 */
#include "widgets.h"

#include "layout.h"
#include "theme/theme.h"
#include "ui/icon_cache.h"

static GFont s_font_label;  // Antonio 10 bar labels

// --- LCARS skin primitives ---
// stateless painters (ctx in and pixels out) for the chrome only LCARS faces draw:
// the holder-box label and the segmented battery gauge

/**
 * @addtogroup watchface-lcars
 * @{
 */

/**
 * @brief Draw a label inside a black LCARS holder box, at the start or the end of its area.
 *
 * @param ctx The graphics context.
 * @param area The area to draw in.
 * @param text The label text.
 * @param font The font to use.
 * @param text_color The text colour.
 * @param pad Padding inside the box.
 * @param right True to sit the box at the area's right end.
 */
static void lcars_label(GContext *ctx, GRect area, const char *text, GFont font, GColor text_color, int pad,
                        bool right)
{
    // an ops slot set to show nothing hands over an empty word. painting it would leave a bare
    // black box on the bar, which reads as a label that failed to load rather than an empty slot
    if (!text || !text[0])
    {
        return;
    }

    GSize sz = graphics_text_layout_get_content_size(text, font, area, GTextOverflowModeFill, GTextAlignmentLeft);

    int box_w = sz.w + pad * 2;
    if (box_w > area.size.w)
    {
        box_w = area.size.w;
    }

    int box_x = right ? area.origin.x + area.size.w - box_w : area.origin.x;

    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(box_x, area.origin.y, box_w, area.size.h), 0, GCornerNone);

    // flush with the holder box. the drawn bar has a hard top edge so the label
    // needs no lift to sit on it
    graphics_context_set_text_color(ctx, text_color);
    graphics_draw_text(ctx, text, font,
        GRect(box_x + pad, area.origin.y, box_w - pad, area.size.h + 4),
        GTextOverflowModeFill, GTextAlignmentLeft, NULL);
}

/**
 * @brief Draw one LCARS header bar: a rounded cap at each end with a square body between.
 *
 * The two notches that separate the caps from the body are not drawn. They are the black field
 * showing through the gap, which is what gives the bar its segmented LCARS look.
 *
 * @param ctx The graphics context.
 * @param area The whole bar including both caps.
 * @param cap The end-cap colour.
 * @param body The middle colour.
 */
static void lcars_bar(GContext *ctx, GRect area, GColor cap, GColor body)
{
    // half the cap's width rounds it into a proper half-pill. the SDK clamps a
    // radius bigger than the box so this is the largest one that still reads as a
    // curve rather than a corner
    graphics_context_set_fill_color(ctx, cap);
    graphics_fill_rect(ctx, GRect(area.origin.x, area.origin.y, BAR_CAP_W, area.size.h),
                       BAR_CAP_W / 2, GCornersLeft);
    graphics_fill_rect(ctx, GRect(area.origin.x + area.size.w - BAR_CAP_W, area.origin.y,
                                  BAR_CAP_W, area.size.h),
                       BAR_CAP_W / 2, GCornersRight);

    int inset = BAR_CAP_W + BAR_NOTCH;
    graphics_context_set_fill_color(ctx, body);
    graphics_fill_rect(ctx, GRect(area.origin.x + inset, area.origin.y,
                                  area.size.w - inset * 2, area.size.h),
                       0, GCornerNone);
}

/**
 * @brief Draw the 5-segment battery gauge, colour-coded by level.
 *
 * @param ctx The graphics context.
 * @param area The bounding box of the battery icon.
 * @param level Battery charge level percentage.
 * @param accent_color The outline and nub colour.
 * @param fill_color The lit-segment colour (already level- and theme-resolved).
 * @param flipped True to point the nub left and fill from the right, for a rail down the right.
 */
static void lcars_battery_gauge(GContext *ctx, GRect area, int level, GColor accent_color, GColor fill_color,
                                bool flipped)
{
    if (level < 0)
    {
        level = 0;
    }

    if (level > 100)
    {
        level = 100;
    }

    GRect body = area;
    int nub_x = flipped ? area.origin.x - 3 : area.origin.x + area.size.w;
    GRect nub = GRect(nub_x, area.origin.y + area.size.h / 4, 3, area.size.h / 2);

    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, body, 2, GCornersAll);
    graphics_fill_rect(ctx, nub, 1, flipped ? GCornersLeft : GCornersRight);

    GRect inner = GRect(body.origin.x + 2, body.origin.y + 2, body.size.w - 4, body.size.h - 4);

    const int segments = 5, gap = 2;
    int seg_w = (inner.size.w - (segments - 1) * gap) / segments;
    if (seg_w < 1)
    {
        seg_w = 1;
    }

    int lit = (level * segments + 50) / 100;
    if (lit == 0 && level > 0)
    {
        lit = 1;
    }

    for (int i = 0; i < segments; i++)
    {
        // a flipped gauge counts its cells from the right, so like the unflipped one it fills from the
        // end away from its nub
        int step = flipped ? segments - 1 - i : i;
        GRect cell = GRect(inner.origin.x + step * (seg_w + gap), inner.origin.y, seg_w, inner.size.h);

        // lit cells are filled solid and empty cells are hollow. a black interior with
        // an accent outline so the cell stays visible without reading as charged
        graphics_context_set_fill_color(ctx, i < lit ? fill_color : GColorBlack);
        graphics_fill_rect(ctx, cell, 1, GCornersAll);

        if (i >= lit)
        {
            graphics_context_set_stroke_color(ctx, accent_color);
            graphics_draw_round_rect(ctx, cell, 1);
        }
    }
}

/**
 * @brief Label definition mapping an area to its string text.
 */
typedef struct
{
    GRect area;
    const char *text;
} LcarsLabel;

/**
 * @brief Blit a transparent icon bitmap, skipping nulls.
 *
 * @param ctx The graphics context.
 * @param bmp The bitmap to draw.
 * @param r The destination rectangle.
 */
static void draw_icon(GContext *ctx, GBitmap *bmp, GRect r)
{
    if (!bmp)
    {
        return;
    }

    graphics_context_set_compositing_mode(ctx, GCompOpSet);  // honor PNG transparency
    graphics_draw_bitmap_in_rect(ctx, bmp, r);
}

// the ops glyphs come from the catalogue and are not all cut to the same shape: a thermometer is
// taller than it is wide. graphics_draw_bitmap_in_rect repeats a bitmap that is smaller than the
// box it is given, so the destination has to be the picture's own size or the glyph tiles
void widgets_draw_glyph(GContext *ctx, uint32_t res, GRect box)
{
    GBitmap *bmp = icon_get(res);
    if (!bmp)
    {
        return;
    }

    GSize sz = icon_size(res);
    draw_icon(ctx, bmp, GRect(box.origin.x + (box.size.w - sz.w) / 2,
                              box.origin.y + (box.size.h - sz.h) / 2,
                              sz.w, sz.h));
}

void widgets_load(void)
{
    s_font_label = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ANTONIO_10));
}

void widgets_unload(void)
{
    fonts_unload_custom_font(s_font_label);
}

/** @brief Where a slot's bar starts on the screen, from its column and row in the grid. */
static GPoint slot_origin(const OpsChrome *chrome, int slot)
{
    const OpsGrid *grid = chrome->grid;

    return GPoint(grid->col_x[slot / grid->rows], grid->row_y[slot % grid->rows]);
}

/** @brief How wide a bar runs from the left column to the end of the right one, for a wide or full panel. */
static int span_width(const OpsChrome *chrome)
{
    const OpsGrid *grid = chrome->grid;

    return grid->col_x[grid->cols - 1] + OPS_BAR(0, 0).size.w - grid->col_x[0];
}

/**
 * @brief Whether a slot draws its own chrome at all.
 *
 * A slot a bigger panel covers does not. Its bar, word and glyph all belong to the panel, such as
 * the lower slot under a tall panel.
 *
 * @param chrome What the slots resolved to.
 * @param slot The slot index.
 * @return True when the slot owns its row.
 */
static bool slot_draws(const OpsChrome *chrome, int slot)
{
    return !chrome->covered[slot] && !chrome->bare[slot];
}

/** @brief Whether a slot's pick runs across both columns. */
static bool spans_columns(const OpsChrome *chrome, int slot)
{
    return chrome->shape[slot] == OPS_SHAPE_WIDE || chrome->shape[slot] == OPS_SHAPE_FULL;
}

/** @brief A slot's bar, stretched across both columns for a wide or full panel. */
static GRect slot_bar(const OpsChrome *chrome, int slot)
{
    GPoint at = slot_origin(chrome, slot);
    GRect bar = OPS_BAR(at.x, at.y);

    if (spans_columns(chrome, slot))
    {
        bar.size.w = span_width(chrome);
    }

    return bar;
}

/** @brief A slot's label box, with the longer room a stretched bar gives it. */
static GRect slot_label(const OpsChrome *chrome, int slot)
{
    GPoint at = slot_origin(chrome, slot);
    GRect label = OPS_LABEL(at.x, at.y);

    if (spans_columns(chrome, slot))
    {
        label.size.w = span_width(chrome) - (OPS_BAR(at.x, at.y).size.w - label.size.w);
    }

    return label;
}

void widgets_draw_bars(GContext *ctx, const OpsChrome *chrome)
{
    uint8_t theme = stardate_active_theme();
    GColor cap = bar_cap_for_theme(theme);
    GColor body = bar_body_for_theme(theme);

    for (int slot = 0; slot < ops_cell_count(chrome->grid); slot++)
    {
        if (slot_draws(chrome, slot))
        {
            lcars_bar(ctx, slot_bar(chrome, slot), cap, body);
        }
    }
}

/**
 * @brief The room a word has between a bar's two caps, for a word that sits at the bar's right end.
 *
 * @param bar The bar.
 * @return The bar less its caps.
 */
static GRect between_caps(GRect bar)
{
    return GRect(bar.origin.x + BAR_CAP_W, bar.origin.y, bar.size.w - 2 * BAR_CAP_W, bar.size.h);
}

void widgets_draw_labels(GContext *ctx, const OpsChrome *chrome)
{
    GColor lbl_color = label_color_for_theme(stardate_active_theme());

    for (int slot = 0; slot < ops_cell_count(chrome->grid); slot++)
    {
        if (slot_draws(chrome, slot))
        {
            GRect area = chrome->labels_right ? between_caps(slot_bar(chrome, slot)) : slot_label(chrome, slot);
            lcars_label(ctx, area, chrome->label[slot], s_font_label, lbl_color, 3, chrome->labels_right);
        }
    }
}

void widgets_draw_stardate(GContext *ctx, GRect bar, bool label_right)
{
    uint8_t theme = stardate_active_theme();

    lcars_bar(ctx, bar, bar_cap_for_theme(theme), bar_body_for_theme(theme));
    lcars_label(ctx, between_caps(bar), "STARDATE", s_font_label, label_color_for_theme(theme), 3, label_right);
}

void widgets_draw_battery(GContext *ctx, int level, GRect box, bool flipped)
{
    uint8_t theme = stardate_active_theme();
    GColor accent = panel_accent_for_theme(theme);
    GColor fill = battery_fill_for_theme(theme, level);
    lcars_battery_gauge(ctx, box, level, accent, fill, flipped);
}

void widgets_draw_glyphs(GContext *ctx, const OpsChrome *chrome)
{
    // the cache loads each glyph once and hands back the same picture on later paints
    for (int slot = 0; slot < ops_cell_count(chrome->grid); slot++)
    {
        // a slot showing nothing hands over 0 and the cache has no picture for that
        if (slot_draws(chrome, slot) && chrome->icon[slot])
        {
            GPoint at = slot_origin(chrome, slot);
            widgets_draw_glyph(ctx, chrome->icon[slot], OPS_GLYPH(at.x, at.y));
        }
    }
}

void widgets_draw_status(GContext *ctx, GRect bt_box, GRect qt_box, bool bt_show, bool bt_connected, bool qt_show,
                         bool qt_active)
{
    if (bt_show)
    {
        GBitmap *bt = icon_get(bt_connected ? RESOURCE_ID_ICON_BLUETOOTH : RESOURCE_ID_ICON_BLUETOOTH_SLASH);
        icon_tint(bt, GColorBlack);  // the master glyph is white so paint it black to match the panel text
        draw_icon(ctx, bt, bt_box);
    }

    if (qt_show && qt_active)
    {
        // a muted speaker while Quiet Time holds, and nothing when sound is on
        GBitmap *qt = icon_get(RESOURCE_ID_ICON_VOLUME_MUTED);
        icon_tint(qt, GColorBlack);  // white master glyph, tinted black to match the panel like BT
        draw_icon(ctx, qt, qt_box);
    }
}

/** @} */
