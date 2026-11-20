/**
 * @file panels.h
 * @brief Painters for the ops panels bigger than one slot: the daylight bar, steps and heart rate by
 * the hour, now playing, the next event, the hour, day, week, and year progress bars, the daily goal
 * bars, the countdown to the next sunrise or sunset, today's heart rate and steps, now playing with
 * its art, the agenda, the hourly and four day forecasts across all four, and the Sensors Block and
 * the other tall panels down a column. The weekday readout is painted here too, though it fills one
 * slot, since its days are drawn rather than written.
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
 * @brief Whether a pick is a panel this file paints, rather than a readout off the catalogue.
 *
 * @param id The OpsId.
 * @return True for a tall, wide, or full panel, and for the weekday readout.
 */
bool panels_paints(uint8_t id);

/**
 * @brief Paint a panel panels_paints says yes to under its bar.
 *
 * The bar and its word are drawn with the other slots' bars. This paints the body under it.
 *
 * @param ctx The graphics context.
 * @param grid The ops grid.
 * @param id The OpsId, one panels_paints says yes to.
 * @param slot The slot the panel starts from.
 */
void panels_draw(GContext *ctx, const OpsGrid *grid, uint8_t id, int slot);

/** @} */
