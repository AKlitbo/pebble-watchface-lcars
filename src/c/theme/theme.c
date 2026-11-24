/**
 * @file theme.c
 * @brief Per-theme lookups: background resource ids and panel accent colours.
 */
#include "theme.h"

#include "settings_schema.h"

// every baked frame by layout, then rail side, then theme setting value. a Lower Decks frame sits
// at 1 because that is the number the theme picker has always saved for it
//
// each row below names nine frames. a short row would fill with zeros rather than fail, so a theme
// added to the settings without its frames here stops the build instead of drawing nothing
_Static_assert(LCARS_THEME_COUNT == 9, "a new theme needs its frames added to every row of s_frames");

static const uint32_t s_frames[LCARS_LAYOUT_COUNT][LCARS_SIDE_COUNT][LCARS_THEME_COUNT] = {
    [LCARS_LAYOUT_STARDATE] = {
        {RESOURCE_ID_IMAGE_BG_CLASSIC, RESOURCE_ID_IMAGE_BG_LOWER_DECKS, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_PADD, RESOURCE_ID_IMAGE_BG_NEMESIS_BLUE, RESOURCE_ID_IMAGE_BG_MONO, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_MONO, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_PADD_MONO, RESOURCE_ID_IMAGE_BG_VOYAGER, RESOURCE_ID_IMAGE_BG_VOYAGER_MONO},
        {RESOURCE_ID_IMAGE_BG_CLASSIC_MIRROR, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_MIRROR, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_PADD_MIRROR, RESOURCE_ID_IMAGE_BG_NEMESIS_BLUE_MIRROR, RESOURCE_ID_IMAGE_BG_MONO_MIRROR, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_MONO_MIRROR, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_PADD_MONO_MIRROR, RESOURCE_ID_IMAGE_BG_VOYAGER_MIRROR, RESOURCE_ID_IMAGE_BG_VOYAGER_MONO_MIRROR},
    },
    [LCARS_LAYOUT_TACTICAL] = {
        {RESOURCE_ID_IMAGE_BG_CLASSIC_TACTICAL, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_TACTICAL, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_PADD_TACTICAL, RESOURCE_ID_IMAGE_BG_NEMESIS_BLUE_TACTICAL, RESOURCE_ID_IMAGE_BG_MONO_TACTICAL, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_MONO_TACTICAL, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_PADD_MONO_TACTICAL, RESOURCE_ID_IMAGE_BG_VOYAGER_TACTICAL, RESOURCE_ID_IMAGE_BG_VOYAGER_MONO_TACTICAL},
        {RESOURCE_ID_IMAGE_BG_CLASSIC_TACTICAL_MIRROR, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_TACTICAL_MIRROR, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_PADD_TACTICAL_MIRROR, RESOURCE_ID_IMAGE_BG_NEMESIS_BLUE_TACTICAL_MIRROR, RESOURCE_ID_IMAGE_BG_MONO_TACTICAL_MIRROR, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_MONO_TACTICAL_MIRROR, RESOURCE_ID_IMAGE_BG_LOWER_DECKS_PADD_MONO_TACTICAL_MIRROR, RESOURCE_ID_IMAGE_BG_VOYAGER_TACTICAL_MIRROR, RESOURCE_ID_IMAGE_BG_VOYAGER_MONO_TACTICAL_MIRROR},
    },
};

uint32_t bg_resource_for(uint8_t theme, uint8_t layout, bool rail_right)
{
    // a number past the ones here reads as Classic and the Stardate layout, the same as the rest of
    // the face
    uint8_t frames_for = layout < LCARS_LAYOUT_COUNT ? layout : LCARS_LAYOUT_STARDATE;
    uint8_t look = theme < LCARS_THEME_COUNT ? theme : 0;

    return s_frames[frames_for][rail_right ? LCARS_SIDE_RIGHT : LCARS_SIDE_LEFT][look];
}

GColor label_color_for_theme(uint8_t theme)
{
    switch (theme)
    {
        case 4:  // Classic Mono
        case 5:  // Lower Decks Mono
        case 6:  // PADD Mono
        case 8:  // Voyager Mono
            return GColorLightGray;
        default:
            return GColorChromeYellow;
    }
}

GColor panel_accent_for_theme(uint8_t theme)
{
    switch (theme)
    {
        case 1:  // harvestgold (#FFAA55)
            return GColorRajah;
        case 2:  // night-rain (#5555AA)
            return GColorLiberty;
        case 3:  // tangerine (#FFAA55)
            return GColorRajah;
        case 4:  // Classic Mono (white)
            return GColorWhite;
        case 5:  // Lower Decks Mono (white)
            return GColorWhite;
        case 6:  // PADD Mono (white)
            return GColorWhite;
        case 7:  // Voyager (command gold #FFAA00)
            return GColorChromeYellow;
        case 8:  // Voyager Mono (white)
            return GColorWhite;
        default: // african-violet (#AAAAFF)
            return GColorBabyBlueEyes;
    }
}

// the two bar colours per theme. they match the frame artwork so a drawn bar sits
// on the same pixels as the chrome around it
// every value is a real Pebble-64 colour because that is all the frame
// stylesheets carry
GColor bar_cap_for_theme(uint8_t theme)
{
    switch (theme)
    {
        case 1:  // Lower Decks
            return GColorRajah;
        case 2:  // PADD
            return GColorCadetBlue;
        case 3:  // Nemesis Blue
            return GColorBlue;
        case 4:  // Classic Mono
            return GColorDarkGray;
        case 5:  // Lower Decks Mono
            return GColorWhite;
        case 6:  // PADD Mono
            return GColorDarkGray;
        case 7:  // Voyager
            return GColorBlueMoon;
        case 8:  // Voyager Mono
            return GColorDarkGray;
        default: // Classic
            return GColorLavenderIndigo;
    }
}

GColor bar_body_for_theme(uint8_t theme)
{
    switch (theme)
    {
        case 1:  // Lower Decks
            return GColorOrange;
        case 2:  // PADD
            return GColorLiberty;
        case 3:  // Nemesis Blue
            return GColorBlueMoon;
        case 4:  // Classic Mono
            return GColorWhite;
        case 5:  // Lower Decks Mono
            return GColorDarkGray;
        case 6:  // PADD Mono
            return GColorDarkGray;
        case 7:  // Voyager
            return GColorLiberty;
        case 8:  // Voyager Mono
            return GColorLightGray;
        default: // Classic
            return GColorBabyBlueEyes;
    }
}

GColor battery_fill_for_theme(uint8_t theme, int level)
{
    GColor healthy = panel_accent_for_theme(theme);

    // mono themes keep the gauge greyscale. the lit-segment count already shows the
    // charge so the red/amber low-battery warnings would only break the monochrome look
    switch (theme)
    {
        case 4:  // Classic Mono
        case 5:  // Lower Decks Mono
        case 6:  // PADD Mono
        case 8:  // Voyager Mono
            return healthy;
        default:
            break;
    }

    if (level <= 20)
    {
        return GColorRed;
    }

    if (level <= 40)
    {
        return GColorChromeYellow;
    }

    return healthy;
}
