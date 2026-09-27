# Changelog - LCARS Stardate

All notable changes to the LCARS Stardate watchface are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Changed

- Moved the Alternate Time Zone setting from Location Settings to the Clock section of the settings page. The city you picked is kept.

### Fixed

- Fixed the temperature showing the old number with the new unit, such as 23F for 23°C, after switching units while the phone was offline, when a refresh already under way finished just after, on the first save after installing, or after the Pebble app's data was cleared.
- Fixed your settings sometimes not coming back after an update, leaving the defaults until the Pebble app restarted. Going back to an older version could leave the defaults for good.
- Fixed rain chance and UV showing 0 where the weather service had no reading for them. They show a dash instead.
- Fixed the sun readouts when sunset falls after midnight, as in a northern summer. Daylight read as night, and the countdown to dawn showed hours that were already daylight.
- Fixed the moon countdown jumping to 29 days partway through the night of a new or full moon. The moon now reads as now for that whole night.
- Fixed the Alternate Time Zone list on the settings page opening again after you picked a zone, where a stray tap could change your pick.
- Fixed the UTC offset shown for a zone on the settings page sometimes reading a minute short. Typing an offset such as UTC+5 also showed a name that read as UTC-5.
- Fixed a second finger on the screen moving or dropping the readout you were dragging in the slot builder.
- Fixed the settings page opening on the default for a choice such as the temperature unit, rather than what the watch was set to.
- Fixed a city typed on the settings page and saved without tapping one of the suggestions being lost. The settings page now asks you to pick a place from the list.
- Fixed clearing the alternate time zone on the settings page leaving the old city on the watch. The ZONE 1 readout now shows dashes whenever no zone is picked.
- Fixed the wind reading 0 km/h rather than a dash when OpenWeatherMap had no wind reading.
- Fixed the weather showing clear skies when Open-Meteo, the default weather provider, had no reading for the current conditions.
- Fixed the steps showing -1 just after the watchface opened, or 0 with Health turned off, and the Stats Readout set to distance reading 0.0 all day. It shows dashes until there is a reading.
- Fixed sunrise and sunset, and what follows them, showing hours off when the weather location is in another time zone. Today's high, low, and rain chance could also come from the day before or after.
- Fixed yesterday's high, low, UV, and rain chance staying on the watch as today's when part of the weather refresh kept failing.
- Fixed a weather key that ran out of calls showing API ERROR and using up more calls on retries. It now shows RATE LIMIT until the next refresh.
- Fixed the weather being fetched twice on every refresh, which used up a weather key's calls twice as fast.
- Fixed the weather readouts sometimes staying on dashes for up to half an hour after the watchface started.

### Notes

- This release clears the saved weather once. The weather readouts show dashes until the first refresh after the update, and nothing needs doing to bring them back.

## [1.12.1] - 2026-09-22

### Changed

- A new install now starts with no Alternate Time Zone rather than London, reading UTC until you pick a city.
- The prompt to pick a city again now stands out as a boxed note rather than a loose line of text.

### Fixed

- Fixed the Alternate Time Zone opening empty on the settings page while the watch still showed the old zone. The place name now comes back from the watch.
- Fixed a settings page with no city chosen dropping the Alternate Time Zone to UTC under a bare TZ heading.
- Fixed a city picked for the Alternate Time Zone saving as UTC when the page was saved straight away or with no connection.
- Fixed your settings being replaced by the defaults after installing a new version.

## [1.12.0] - 2026-09-20

### Added

- Added time zones to the Alternate Time Zone picker, so UTC, a zone name such as Europe/London, or an offset like UTC+05:30 can be picked as well as a city.

### Changed

- Changed the Alternate Time Zone sample picture in the drag-and-drop readout list to read ZONE 1 rather than LONDON. The pictures are only there to show a readout's shape, and this one made the slot look like it was fixed to London rather than naming whichever zone you pick.

## [1.11.1] - 2026-09-17

### Changed

- Made the weather arrive sooner on OpenWeatherMap, which is now asked for the reading and the extra readouts together rather than one after the other.

### Fixed

- Fixed a second clock drifting an hour when the clocks changed. Its offset was fixed when you picked the city, so a London picked in winter ran an hour behind all summer. Pick the city again in settings to take up the fix, which the settings page now prompts you to do.
- Fixed the weather sticking on an old reading, or staying blank, until the phone app was restarted. A reading that failed to reach the watch is now sent again, and the watch asking repeatedly while it waits no longer spends a weather lookup each time.
- Fixed drizzle showing the N/A icon on OpenWeatherMap.
- Fixed saving settings always fetching the weather again. Changing a colour or a vibration no longer spends one of your provider's daily lookups.
- Fixed the settings page opening with the wrong values when the watchface had a lot of settings to send back. The watch now sends the whole lot or none of it.

### Notes

- This release clears the saved weather once. The weather readouts show dashes until the next refresh rather than the reading from before the update, and nothing needs doing to bring them back.

## [1.11.0] - 2026-09-07

### Added

- Added a Next Alarm readout for the ops slots. It shows when your next alarm goes off, read straight off the watch, so nothing needs setting up. A slot with no alarm scheduled reads as dashes.

## [1.10.0] - 2026-08-26

### Added

- Added two date formats ending in a Swatch Beats reading, so the date line can show internet time while the clock stays on normal time. The date banner shrinks as needed to fit the longer format.

### Fixed

- Fixed the Classic and Nemesis Blue frames not loading, leaving the watchface on a bare black background. Their artwork exceeded the sixteen-colour limit, causing the frames to use twice the expected memory.

## [1.9.0] - 2026-08-12

### Added

- Added drag-and-drop panel configuration, with a preview of the watchface and available readouts.
- Added a Default button that restores the arrangement the watchface ships with, and a Clear button that empties all four panels.
- Added LCARS-styled settings matching the watchface.
- Added support for splitting the left column into two panels.
- Added moon readouts for illumination, phase, and days until the next full or new moon.
- Added sun readouts for sunrise, sunset, daylight duration, and countdowns to the next sunrise or sunset.
- Added weather readouts for humidity, wind, UV index, and daily high and low.
- Added readouts for battery level, calories, sleep, and active minutes.
- Added calendar readouts for Julian date, day of year, and week number.
- Added an Epoch Clock readout showing the raw Unix time.
- Added a Swatch Beats readout, so you can see internet time without setting the main clock to it.
- Added an Alternate Time Zone readout. Search for a city under Location Settings and the panel shows that city's time under its name.

### Changed

- Panel bars are now drawn by the watchface and take their colour from the theme, reducing the artwork by about a third.
- Sleep, active minutes, and calories are read from the watch again now that they can be displayed.
- Upgrading preserves the existing layout, with panels starting on weather, heart rate, and steps.

### Fixed

- Fixed the partly cloudy icon sitting against the panel bar above it.

## [1.8.0] - 2026-08-03

### Changed

- The watchface no longer tracks or stores health history that it never displays, eliminating unnecessary storage writes.
- The watchface no longer reads sleep, active minutes, or calories from the watch. It has never displayed them, and reading them required an unnecessary storage access once per minute.

### Fixed

- Fixed the watchface making the watch feel slow and unresponsive. It was building and storing a full history of heart rate and step activity that it never displays, and rebuilding it every minute delayed the watch's background work.
- Fixed the heart rate, step, and distance readings going stale while the watch was sitting still. They now refresh once per minute whether or not you have been moving.

## [1.7.0] - 2026-07-29

### Added

- Added a 12-hour time format without a leading zero, so 8:30 rather than 08:30.

## [1.6.1] - 2026-07-28

### Fixed

- Fixed the saved weather reading being lost whenever the watchface reloaded, leaving the watchface blank until the next weather update.
- Fixed failed weather updates not being retried until the weather reading changed.
- Weather now refreshes when the phone reconnects if the previous reading has gone stale.

## [1.6.0] - 2026-07-27

### Added

- Added a Quiet Time indicator beside the Bluetooth icon. A new Show Quiet Time Icon setting under Appearance lets you turn it on or off.
- Added an Hourly Vibration option that vibrates at the top of every hour and stays silent during Quiet Time.
- Added two new frame themes, Voyager and Voyager Mono.

### Changed

- Reorganized settings into Clock, Health, and Weather sections.
- Replaced the Fahrenheit toggle with a Temperature Unit dropdown.

## [1.5.0] - 2026-07-23

> [!IMPORTANT]
> This release corrects the watchface's message keys. As a result, some settings may reset to their defaults after updating and need to be set again.

### Fixed

- Fixed settings changes occasionally not reaching the watch.
- Fixed large settings changes occasionally failing to save.
- Fixed weather sometimes remaining blank after a failed update.
- Fixed weather sometimes remaining blank when using Fallback to Manual Location.

### Changed

- Reworked communication between the phone and watch to improve the reliability of settings, weather, and other updates.

### Notes

- This release changes how weather settings are stored. If weather appears stuck after updating, switch to a different weather provider, save, then switch back to your preferred provider and save again. This refreshes the stored weather configuration.

## [1.4.0] - 2026-07-03

### Added

- Weather now keeps its last reading when a refresh fails or the watchface reloads.
- Weather conditions now use night-specific icons after dark.

### Changed

- Updated watchface backgrounds and settings layout to improve readability.

## [1.3.0] - 2026-06-25

### Added

- Added three fully grayscale Mono theme variations: Classic Mono, Lower Decks Mono, and Lower Decks PADD Mono.

## [1.2.2] - 2026-06-24

### Fixed

- Fixed weather provider errors and rate limits leaving stale weather.
- Fixed location search occasionally saving the wrong city.
- Fixed non-weather settings changes unnecessarily refreshing the weather.

## [1.2.1] - 2026-06-24

### Fixed

- Fixed unrecognised weather conditions showing no icon.
- Fixed switching to or from Swatch Internet Time (.beats) interrupting weather updates.

## [1.2.0] - 2026-06-20

### Added

- Added a Bluetooth connection icon with an optional visibility toggle.
- Added optional Bluetooth connection alerts with configurable vibration patterns.

### Fixed

- Fixed long date formats being clipped in the date banner.

### Fixed

- The longest date formats now shrink to fit within the date banner instead of being clipped. Wide layouts such as the text month and day-of-year formats previously overflowed the banner, and the smaller fallback sizes now sit centered within it.

## [1.1.0] - 2026-06-18

### Added

- Added additional date format options, including day-first, month-first, year-first, text month, and day-of-year formats.

## [1.0.3] - 2026-06-17

### Fixed

- Fixed Swatch Internet Time (.beats) updating only once per minute.

### Changed

- Reduced unnecessary date redraws.

## [1.0.2] - 2026-06-17

### Fixed

- Fixed the Lower Decks PADD battery gauge being difficult to read.

### Changed

- Unified the battery gauge style across all themes.

## [1.0.1] - 2026-06-16

> [!IMPORTANT]
> Due to a build mix-up, the 1.0.0 app store release was not built from the initial commit on main. This changelog tracks changes against the main branch history, so some entries below may not match the exact contents of the published 1.0.0 binary.

### Fixed

- Fixed coordinate readouts occasionally displaying the wrong final digit.
- Fixed saved settings not surviving app updates.
- Fixed the settings screen not reflecting the current watch configuration.
- Fixed unnecessary weather icon reloads.
- Fixed the weather condition text clipping at night.

### Changed

- Rounded traversal distance to the nearest tenth.
- Reduced health stat updates to improve battery life.

## [1.0.0] - 2026-06-14

### Added

- First release of the LCARS Stardate watchface.
