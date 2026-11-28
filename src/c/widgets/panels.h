/**
 * @file panels.h
 * @brief Painters for the ops panels bigger than one slot: the daylight bar, steps and heart rate by
 * the hour, now playing, the next event, the hour, day, week, and year progress bars, the daily goal
 * bars, the countdown to the next sunrise or sunset, today's heart rate and steps, now playing with
 * its art, the agenda, the hourly and four day forecasts across all four, and the Sensors Block and
 * the other tall panels down a column. The weekday readout is painted here too, though it fills one
 * slot, since its days are drawn rather than written. The rail gauges read their fill from here too,
 * the same way the tall panels they stand for fill their bars.
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

/** @brief A gauge on a rail: the word on its head, how full it is, and the reading under its cells. */
typedef struct
{
    const char *word; ///< The rail's word, such as PWR or STEP, short enough for a 24 wide rail
    int pct;          ///< How full the gauge is, 0 to 100
    char text[12];    ///< The reading, such as 64% or 4H, or two dashes with nothing to read yet
} PanelGauge;

/**
 * @brief What a rail gauge shows for a pick.
 *
 * A gauge stands for one of the tall panels with a bar, such as the Battery, Step Goal, or Moon
 * Block, and fills the same way that panel's bar does, or for the phone's battery. Any other pick reads
 * as the battery, so a stray saved value still shows a working gauge.
 *
 * @param id The OpsId the gauge is set to.
 * @param narrow Whether the rail is too narrow for 100%, so a full gauge reads 100.
 * @param out Receives the word, the fill, and the reading.
 */
void panels_gauge(uint8_t id, bool narrow, PanelGauge *out);

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
