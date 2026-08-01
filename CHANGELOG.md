# Changelog

## 1.0.1

- Replaced the branded player-facing chat prefix with localized `Notice` / `提示` labels.
- Simplified perk, build and max-level exchange menu titles for easier server integration.
- Reworded player authentication and disabled-debug-command messages to avoid exposing implementation details.
- Added gameplay previews for the build menu, perk selection and localized status output.

## 1.0.0

- Published the first stable Boss Slayer Roguelite release for cooperative and realism campaigns.
- Finalized fourteen localized roguelite perks, max-build reward exchange and configurable balance values.
- Preserved SteamID-keyed builds across chapters and personal deaths, with chapter-start rollback on mission loss.
- Added Boss participation, special-infected, common-infected and chapter-entry reward paths.
- Finalized safe numeric-key menu navigation, English and Simplified Chinese translations, and configurable message noise levels.
- Restricted development commands behind SourceMod admin permissions and a default-off server switch with audit logging.
- Added an install-ready server package, build/deploy scripts, configuration reference and regression checklist.

## 0.8.0

- Restricted all self-targeted development commands to `ADMFLAG_CHEATS` (`n`).
- Added `sm_bsr_debug_commands`, disabled by default, as a second gate for development commands.
- Kept `sm_bsr_resetall` restricted to `ADMFLAG_ROOT` (`z`) and independent of the debug switch.
- Added SourceMod audit logging for successful test mutations, personal resets and global resets.
- Expanded README and configuration documentation with permission tiers, override behavior, upgrade guidance and log locations.

## 0.7.2

- Made every unlisted numeric key in perk and full-build exchange menus safely choose later instead of falling through to another selection.
- Simplified the visible slot-9 action to `Choose later` / `稍后选择`.
- Generalized `!perks` pagination: middle pages use slot 8 for Previous and slot 9 for Next; first and final pages show only the available direction, and other number keys close.
- Extended `!bsr_testreward [1-100]` to grant an explicit number of test choices.
- Enforced the gameplay boundary in code: Boss Slayer now runs only in `coop` and `realism`, and is disabled in versus, survival, scavenge and mutation modes.

## 0.7.1

- Expanded Precision Hunter to every firearm in the primary-weapon slot except the grenade launcher.
- Reset all in-memory builds when a map starts without a normal `map_transition`, preventing abandoned campaigns from leaking progress into a new run.
- Rewrote translation sections into SourceMod-compatible multiline KeyValues and added build-time syntax/key validation.
- Replaced the `!perks` default paginator with explicit `9 Next`, `8 Previous` and `9 Exit` controls, removing the broken slot-0 exit label.
- Escaped all localized percentage signs so punctuation no longer causes `%` to disappear from menus or chat messages.
- Shortened build-menu summaries to stay below the engine's safe 63-byte UTF-8 item limit and added a build-time limit check.
- Added a searchable bilingual HTML audit containing every translation phrase for manual review.
- Added a complete configuration reference that separates server-editable ConVars from code-defined rules.

## 0.7.0

- Added per-player English and Simplified Chinese translations using SourceMod's native phrase system.
- Added configurable chat verbosity and reduced repetitive reward/passive-effect messages.
- Changed `!perks` from fourteen chat lines to a paginated localized build menu.
- Moved reward thresholds, reward amounts, perk multipliers, timers and full-build exchange values into the auto-generated `cfg/sourcemod/boss_slayer.cfg`.
- Updated local deployment to install translation files alongside the compiled plugin.
- Added a release packaging script that produces an install-ready server ZIP.
- Defined human-only gameplay as an explicit project boundary; idle and Bot takeover inheritance are not supported.

## 0.6.1

- Replaced the unreliable default `0` exit entry with an explicit `9` choose-later item in perk and exchange menus.

## 0.6.0

- Personal death now retains the campaign build; mission loss restores the chapter-start snapshot.
- Increased Melee Fury from 6% to 8% attack speed per level.
- Added Fate Reroll, Field Rescue, Ammo Reclamation, First Aid Feedback and sniper-only Precision Hunter.
- Added one Supply Conditioning stack on the first successful supply use of each chapter, up to five stacks.
- Added two-level low-health temporary regeneration below 40 effective health.
- Persisted perk choices when closing and reopening the menu so Fate Reroll cannot be bypassed for free.
- Expanded development commands and the regression checklist for all fourteen perks.

## 0.5.0

- Replaced Boss Hunter, Special Hunter and Last Stand with Melee Fury, Healing Boost, Medical Supply and Throwable Supply.
- Firepower now grants 10% direct damage per level and never increases friendly fire.
- Toughness now reduces combat, fire, acid and friendly-fire damage while excluding non-combat world damage.
- Boss Healing restores 10 real and 10 temporary health per level.
- Added melee attack-speed scaling and medkit, pills and adrenaline healing bonuses.
- Added 180-second empty-slot medical and throwable supply abilities.
- Added a max-build exchange menu for 20+20 health, full reserve ammo, medical items or throwables.
- Added targeted perk and max-build development commands.

## 0.4.2

- Every 500 common-infected kills now grants one perk choice.
- Added `!bsr_testcommonkills [amount]` for development testing.
- Added initial open-source repository documentation and MIT licensing.

## 0.4.1

- Boss killers receive two perk choices.
- Other living human Boss participants receive one perk choice.
- Every 30 normal special-infected kills grants one choice.
- Entering each chapter after the first grants one choice.
- Split the single source file into maintainable modules without changing gameplay behavior.
