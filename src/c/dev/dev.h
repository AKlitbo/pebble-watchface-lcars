/**
 * @file dev.h
 * @brief This face's own dev walks, beside the dev plugin's hooks. `paf build lcars-stardate --dev`
 * turns dev mode on, and the walk comes from paf.dev.config.json beside paf.config.json.
 *
 * `"walk": "themes"` is the plugin's theme walk. This face adds two of its own: `"walk": "ops"` steps
 * both ops slots through the readout catalogue in `dev/dev_ops`, and `"walk": "wx"` steps the
 * condition through every weather glyph in `dev/dev_wx`. Each seeds its own readings over the
 * plugin's fixture and takes the taps once the plugin has painted the fixture.
 *
 * The hooks are static inline and empty with dev mode off, so a release build carries none of it.
 *
 * @ingroup watchface-lcars
 */
#pragma once
#include <pebble.h>
#include "dev/dev_hooks.h"

// the build sets DEV_TAP_WALK_<NAME> only for the walk it picks, so each is checked with defined
#if DEV_MODE && defined(DEV_TAP_WALK_OPS) && DEV_TAP_WALK_OPS
#define LCARS_DEV_WALK_OPS 1
#include "dev/dev_ops.h"
#elif DEV_MODE && defined(DEV_TAP_WALK_WX) && DEV_TAP_WALK_WX
#define LCARS_DEV_WALK_WX 1
#include "dev/dev_wx.h"
#endif

/**
 * @brief Seeds the readings this face's own walk needs over the plugin's fixture. Does nothing
 * unless the ops or weather walk is picked. Call it after `dev_seed_stores`.
 */
static inline void lcars_dev_seed_stores(void)
{
#if defined(LCARS_DEV_WALK_OPS)
    dev_ops_seed_stores();
#elif defined(LCARS_DEV_WALK_WX)
    dev_wx_seed_stores(DEV_TIME_HOUR, DEV_TIME_MIN);
#endif
}

/**
 * @brief Starts this face's own walk, painting its first step and taking the taps. Does nothing
 * unless the ops or weather walk is picked. Call it after `dev_start`.
 *
 * @param apply_theme The face's theme-apply hook.
 */
static inline void lcars_dev_start(void (*apply_theme)(void))
{
#if defined(LCARS_DEV_WALK_OPS)
    dev_ops_init(apply_theme);
#elif defined(LCARS_DEV_WALK_WX)
    dev_wx_init(apply_theme);
#else
    (void)apply_theme;
#endif
}

/** @brief Drops this face's own walk's tap subscription. Does nothing unless one is running. */
static inline void lcars_dev_stop(void)
{
#if defined(LCARS_DEV_WALK_OPS)
    dev_ops_deinit();
#elif defined(LCARS_DEV_WALK_WX)
    dev_wx_deinit();
#endif
}

/**
 * @brief What a slot should show: the walk's pick while the ops walk is running, otherwise
 * whatever the user saved.
 *
 * @param slot 0 for the upper slot, 1 for the lower.
 * @param stored The saved pick.
 * @return An OpsId.
 */
static inline uint8_t dev_ops_pick(int slot, uint8_t stored)
{
#if defined(LCARS_DEV_WALK_OPS)
    (void)stored;
    return dev_ops_walk_pick(slot);
#else
    (void)slot;
    return stored;
#endif
}
