/**
 * @file widgets.h
 * @brief Overlay painters for the LCARS face, exposed as stateless draws the engine's
 * overlays draw-slot calls. This module owns the label font while the glyphs come from
 * the shared icon cache. The engine owns the layer.
 *
 * @ingroup watchface-lcars
 */
#pragma once
#include <pebble.h>

#include "ops/ops.h"

/**
 * @addtogroup watchface-lcars
 * @{
 */

/**
 * @brief What the ops slots resolved to this paint, in the grid's cell order.
 *
 * Layout works this out from the catalogue and hands it over, so the painters below stay painters
 * that are told what to draw rather than reaching for settings themselves.
 */
typedef struct
{
    const OpsGrid *grid;                ///< The ops grid, which places each slot's chrome
    const char *label[OPS_CELLS_MAX];   ///< Holder-box word per slot, "" for a slot that draws none
    uint32_t    icon[OPS_CELLS_MAX];    ///< Glyph resource per slot, 0 for none
    OpsShape    shape[OPS_CELLS_MAX];   ///< How much of the grid each slot's pick fills
    bool        covered[OPS_CELLS_MAX]; ///< A bigger panel covers the slot, so it draws no chrome of its own
    bool        bare[OPS_CELLS_MAX];    ///< The slot's pick draws no bar, word, or glyph, as the clock has none
    bool        labels_right;           ///< Each bar's word sits at its right end rather than its left
} OpsChrome;

/**
 * @brief Blit a glyph centred in a box, at its own size.
 *
 * @param ctx The graphics context.
 * @param res The resource id.
 * @param box The area to centre it in.
 */
void widgets_draw_glyph(GContext *ctx, uint32_t res, GRect box);

/** @brief Load the label font. */
void widgets_load(void);

/** @brief Free the label font. */
void widgets_unload(void);

/**
 * @brief Draw the LCARS header bars in the current theme's colours.
 *
 * The frame artwork leaves their space empty, so these are painted before anything that sits on
 * top of them (the label holder boxes). A column showing a tall panel gets one bar across its top
 * row instead of two, and a wide or full panel gets one bar across both columns.
 *
 * @param ctx The graphics context.
 * @param chrome What the four slots resolved to.
 */
void widgets_draw_bars(GContext *ctx, const OpsChrome *chrome);

/**
 * @brief Draw the LCARS bar labels.
 *
 * Each comes from whatever its slot is set to show, and an empty word leaves its holder box
 * unpainted.
 *
 * @param ctx The graphics context.
 * @param chrome What the four slots resolved to.
 */
void widgets_draw_labels(GContext *ctx, const OpsChrome *chrome);

/**
 * @brief Draw the STARDATE bar and its word, for the layout that keeps the date in a fixed place.
 *
 * @param ctx The graphics context.
 * @param bar The bar.
 * @param label_right True to put the word at the bar's right end.
 */
void widgets_draw_stardate(GContext *ctx, GRect bar, bool label_right);

/**
 * @brief Draw the battery gauge on the top corner block.
 *
 * @param ctx The graphics context.
 * @param level Battery charge level percentage.
 * @param box Where the gauge's body sits, which follows the rail.
 * @param flipped True to point the nub left and fill from the right, for a rail down the right.
 */
void widgets_draw_battery(GContext *ctx, int level, GRect box, bool flipped);

/**
 * @brief Draw the slot glyphs.
 *
 * The glyphs come from the shared icon cache, so each one loads a single time.
 *
 * @param ctx The graphics context.
 * @param chrome What the four slots resolved to.
 */
void widgets_draw_glyphs(GContext *ctx, const OpsChrome *chrome);

/**
 * @brief Draw the bluetooth and Quiet Time glyphs on the rail.
 *
 * @param ctx The graphics context.
 * @param bt_box Where the bluetooth glyph sits.
 * @param qt_box Where the Quiet Time glyph sits.
 * @param bt_show True to draw the bluetooth glyph, the show and hide setting.
 * @param bt_connected True when the phone link is up, which picks the connected glyph over the slashed one.
 * @param qt_show True to allow the Quiet Time glyph, the show and hide setting.
 * @param qt_active True while Quiet Time holds, which draws the muted speaker. Nothing draws otherwise.
 */
void widgets_draw_status(GContext *ctx, GRect bt_box, GRect qt_box, bool bt_show, bool bt_connected, bool qt_show,
                         bool qt_active);

/** @} */
