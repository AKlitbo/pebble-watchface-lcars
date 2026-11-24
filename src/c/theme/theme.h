/**
 * @file theme.h
 * @brief Theme to background-resource and accent-colour mapping.
 *
 * @ingroup watchface-lcars
 */
#pragma once
#include <pebble.h>

/**
 * @addtogroup watchface-lcars
 * @{
 */

/**
 * @brief The baked frame for a theme in a layout.
 *
 * @param theme The theme setting value.
 * @param layout The layout setting value, a LcarsLayout.
 * @return The frame's resource id.
 */
uint32_t bg_resource_for(uint8_t theme, uint8_t layout);

/**
 * @brief Maps the Theme setting to its LCARS label text colour.
 *
 * @param theme The theme setting value.
 * @return The label colour.
 */
GColor label_color_for_theme(uint8_t theme);

/**
 * @brief The theme's top-left cap colour, so painted overlays (e.g. the battery gauge)
 * can match the panel they sit on instead of staying a fixed classic violet.
 *
 * @param theme The theme setting value.
 * @return The accent colour.
 */
GColor panel_accent_for_theme(uint8_t theme);

/**
 * @brief The rounded end-cap colour of the LCARS header bars.
 *
 * The bars are drawn rather than baked, so their two colours come from here instead of from the
 * frame artwork.
 *
 * @param theme The theme setting value.
 * @return The cap colour.
 */
GColor bar_cap_for_theme(uint8_t theme);

/**
 * @brief The long middle colour of the LCARS header bars.
 *
 * @param theme The theme setting value.
 * @return The bar body colour.
 */
GColor bar_body_for_theme(uint8_t theme);

/**
 * @brief The lit-segment colour for the battery gauge at a given charge level.
 *
 * Colour themes warn with red (critical) and amber (low). Mono themes stay greyscale,
 * leaving the lit-segment count to signal charge.
 *
 * @param theme The theme setting value.
 * @param level Battery charge level percentage.
 * @return The fill colour for lit segments.
 */
GColor battery_fill_for_theme(uint8_t theme, int level);

/** @} */
