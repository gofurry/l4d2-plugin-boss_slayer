# Boss Slayer Codex instructions

## Project shape

- Compile all SourcePawn modules into one `boss_slayer.smx`.
- Keep `src/boss_slayer.sp` as the small entry point.
- Put implementation under `include/boss_slayer/` according to the module map in `README.md`.
- Prefix new internal helpers and globals with `BSR_` / `g_` respectively.
- Do not add environment-switching scripts; the user manages `metamod.vdf` manually.

## Behavioral invariants for v0.6.1

- A Boss killer receives 2 choices; another living human participant receives 1.
- Every 30 normal special-infected kills grants 1 choice.
- Every 500 common-infected kills grants 1 choice.
- Every chapter after the first grants 1 entry choice.
- SteamID-keyed progress survives chapter changes and reconnects.
- Personal death retains progress. Mission loss restores the current chapter-start snapshot; campaign completion clears all progress.
- Maxed perks are excluded from random choices; a fully maxed build redeems extra rewards instead.
- Firepower affects direct infected damage, uses additive 10% levels, and never increases friendly fire.
- Toughness reduces combat, fire, acid and friendly-fire damage but excludes non-combat world damage.
- Boss Healing is split evenly between real and temporary health.
- Medical and throwable supplies require 180 continuous seconds with the relevant inventory empty.
- Closing and reopening a perk menu preserves its choices; Fate Reroll grants one explicit reroll per reward.
- Perk and exchange menus use an explicit slot-9 choose-later item instead of the default slot-10/`0` exit.
- Supply Conditioning gains at most one 2% five-stat stack per chapter and caps at five stacks.
- Field Rescue remains dependency-free; simultaneous revives share the fastest active rescue modifier.

## Required validation

- After every SourcePawn change, run `powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1`.
- Treat compiler warnings as work to fix, not as successful completion.
- Run `scripts\deploy.ps1` only when deployment to the local game installation is requested.
- Do not edit SourceMod SDK files under the Steam game directory.
- Preserve `scripts/config.local.ps1` as machine-local configuration and never commit it.
