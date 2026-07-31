# Boss Slayer Codex instructions

## Project shape

- Compile all SourcePawn modules into one `boss_slayer.smx`.
- Keep `src/boss_slayer.sp` as the small entry point.
- Put implementation under `include/boss_slayer/` according to the module map in `README.md`.
- Prefix new internal helpers and globals with `BSR_` / `g_` respectively.
- Do not add environment-switching scripts; the user manages `metamod.vdf` manually.

## Behavioral invariants for v0.4.2

- A Boss killer receives 2 choices; another living human participant receives 1.
- Every 30 normal special-infected kills grants 1 choice.
- Every 500 common-infected kills grants 1 choice.
- Every chapter after the first grants 1 entry choice.
- SteamID-keyed progress survives chapter changes and reconnects.
- Real death, mission loss, and campaign completion reset the applicable build.
- Perks have levels and menus but no gameplay effects until v0.5.

## Required validation

- After every SourcePawn change, run `powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\build.ps1`.
- Treat compiler warnings as work to fix, not as successful completion.
- Run `scripts\deploy.ps1` only when deployment to the local game installation is requested.
- Do not edit SourceMod SDK files under the Steam game directory.
- Preserve `scripts/config.local.ps1` as machine-local configuration and never commit it.
