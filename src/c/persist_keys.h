/**
 * @file persist_keys.h
 * @brief Every persist key this face uses, in one place.
 *
 * Each key is pinned to an explicit number and must never change or be reused once shipped: a
 * watch in the field holds its data under that exact number. Keeping them all here (rather than
 * scattered as literals) is what stops a new one from silently colliding with an existing one.
 *
 * The range is split by convention: settings blobs sit in a low band, store snapshots in a high
 * one, so the two groups grow toward each other with plenty of room between. The framework's stores
 * don't know their own key, the face hands it to each one through its config.
 *
 * @ingroup watchface-lcars
 */
#pragma once

// --- settings blobs (low band, owned by settings_schema.c) ---
#define LCARS_SETTINGS_KEY 5
#define LCARS_GOALS_KEY    6 // the daily goals and the goal buzz, chained on after key 5

// --- store snapshots (high band, handed to each store's init) ---
// 254 (stock) is left reserved, since this face does not wire it. 249 holds the framework's own
// copy of the weather source choice
#define CALENDAR_STORE_KEY 251
#define HEALTH_STORE_KEY   252
#define LOCATION_STORE_KEY 253
#define WEATHER_STORE_KEY  255

// a stray edit that lets the two bands meet breaks the build instead of a watch in the field
_Static_assert(LCARS_SETTINGS_KEY < CALENDAR_STORE_KEY, "settings keys must stay below the store keys");
_Static_assert(LCARS_GOALS_KEY < CALENDAR_STORE_KEY, "settings keys must stay below the store keys");
