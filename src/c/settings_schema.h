/**
 * @file settings_schema.h
 * @brief The face's settings schema. A blob saved before the settings had a version is
 * lifted across by a migration. Key 5 and the 21-byte v1 size never change, since
 * changing either makes every watch already running the face lose its settings.
 *
 * @ingroup watchface-lcars
 */
#pragma once
#include "system/settings/settings.h"

/**
 * @addtogroup watchface-lcars
 * @{
 */

/**
 * @brief Gets the settings schema for this face.
 *
 * @return A pointer to the schema.
 */
const SettingsSchema *lcars_settings_schema(void);

/**
 * @brief What the upper left slot is set to show.
 *
 * The face's own settings rather than the library's, so they carry ids past SETTING_COUNT and are
 * read back here instead of through settings_u8.
 *
 * @return An OpsId.
 */
uint8_t lcars_slot_lt(void);

/**
 * @brief What the lower left slot is set to show. Ignored while the upper left holds the tall
 * weather block, which fills the whole column.
 *
 * @return An OpsId.
 */
uint8_t lcars_slot_lb(void);

/**
 * @brief What the upper right slot is set to show.
 *
 * @return An OpsId.
 */
uint8_t lcars_slot_rt(void);

/**
 * @brief What the lower right slot is set to show.
 *
 * @return An OpsId.
 */
uint8_t lcars_slot_rb(void);

/**
 * @brief Whether the first alternate time zone is picked at all.
 *
 * The setting is empty until the wearer picks a zone, and empty again once they clear the picker.
 *
 * @return True when a zone is set.
 */
bool lcars_zone_1_is_set(void);

/**
 * @brief How far the first alternate time zone runs from UTC.
 *
 * The setting arrives from Clay's location search as "offset,City, Region, CC", so this is the
 * number in front of the first comma.
 *
 * @return Minutes ahead of UTC, negative behind it. Zero when nothing is set.
 */
int16_t lcars_zone_1_offset_minutes(void);

/**
 * @brief The first alternate time zone's place name, for the holder box.
 *
 * @return Everything after the first comma, or "" when nothing is set. Never NULL.
 */
const char *lcars_zone_1_name(void);

/** @} */
