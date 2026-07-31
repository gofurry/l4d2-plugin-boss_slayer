#if defined _boss_slayer_state_included
    #endinput
#endif
#define _boss_slayer_state_included

// SteamID -> progress for the player's current campaign life.
StringMap g_BossKillsBySteamId;
StringMap g_SpecialKillsBySteamId;
StringMap g_CommonKillsBySteamId;
StringMap g_PendingRewardsBySteamId;

// "SteamID:perkId" -> perk level.
StringMap g_PerkLevelsBySteamId;

// SteamID -> latest chapter reward serial claimed. This is intentionally not
// cleared on death, preventing repeated claims on the same chapter.
StringMap g_LastMapRewardSerialBySteamId;

// Temporary participation bit mask indexed by boss entity index.
int g_BossParticipationMask[MAX_TRACKED_ENTITIES];

// Chapter-transition reward state.
int g_CurrentMapRewardSerial;
bool g_CurrentMapEntryRewardEnabled;
bool g_MapTransitionArmed;
bool g_SuppressNextMapReward;
char g_LastMapName[PLATFORM_MAX_PATH];

// Client slots are temporary. Persistent data is keyed by this cached SteamID.
char g_ClientSteamId[MAXPLAYERS + 1][MAX_AUTHID_LENGTH];

// Menus are temporary UI state and do not need SteamID persistence.
bool g_PerkMenuOpen[MAXPLAYERS + 1];
