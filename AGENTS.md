# Boss Slayer Codex instructions

## Project shape

- Compile all SourcePawn modules into one `boss_slayer.smx`.
- Keep `src/boss_slayer.sp` as the small entry point.
- Put implementation under `include/boss_slayer/` according to the module map in `README.md`.
- Prefix new internal helpers and globals with `BSR_` / `g_` respectively.
- Do not add environment-switching scripts; the user manages `metamod.vdf` manually.

## Stable behavioral invariants

- A Boss killer receives 2 choices; another living human participant receives 1.
- Every 30 normal special-infected kills grants 1 choice.
- Every 500 common-infected kills grants 1 choice.
- Every chapter after the first grants 1 entry choice.
- SteamID-keyed progress survives chapter changes and reconnects.
- Only a normal `map_transition` continues the run; an unarmed map start resets all builds and is treated as a new first chapter.
- Personal death retains progress. Mission loss restores the current chapter-start snapshot; campaign completion clears all progress.
- Maxed perks are excluded from random choices; a fully maxed build redeems extra rewards instead.
- Firepower affects direct infected damage, uses additive 10% levels, and never increases friendly fire.
- Toughness reduces combat, fire, acid and friendly-fire damage but excludes non-combat world damage.
- Boss Healing is split evenly between real and temporary health.
- Medical and throwable supplies require 180 continuous seconds with the relevant inventory empty.
- Closing and reopening a perk menu preserves its choices; Fate Reroll grants one explicit reroll per reward.
- Perk and exchange menus use an explicit slot-9 choose-later item; every unlisted number also safely chooses later.
- `!perks` uses slot 8 for Previous and slot 9 for Next; non-navigation number keys close the menu.
- Supply Conditioning gains at most one 2% five-stat stack per chapter and caps at five stacks.
- Field Rescue remains dependency-free; simultaneous revives share the fastest active rescue modifier.
- Precision Hunter applies to firearm primary weapons except the grenade launcher.
- Gameplay progression and perk effects apply only to human survivors. Do not add idle/Bot ownership transfer.
- Gameplay is enabled only for `coop` and `realism`; all versus, survival, scavenge and mutation modes are outside scope.
- Player-facing text uses SourceMod translations with English as the fallback and Simplified Chinese under `translations/chi/`.
- Balance values come from the auto-generated `cfg/sourcemod/boss_slayer.cfg` ConVars.
- Chat verbosity follows `sm_bsr_chat_level`: 0 minimal, 1 normal, 2 detailed.
- Development commands require ADMFLAG_CHEATS and `sm_bsr_debug_commands 1`; the switch defaults to off.
- `sm_bsr_resetall` remains ADMFLAG_ROOT-only and is independent of the debug switch.
- Successful administrative mutations must be written with `LogAction`.

## Required validation

- After every SourcePawn change, run `powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1`.
- Treat compiler warnings as work to fix, not as successful completion.
- Run `scripts\deploy.ps1` only when deployment to the local game installation is requested.
- Do not edit SourceMod SDK files under the Steam game directory.
- Preserve `scripts/config.local.ps1` as machine-local configuration and never commit it.
- Keep English and Simplified Chinese translation phrase keys synchronized.
