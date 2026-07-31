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
StringMap g_SupplyTrainingStacksBySteamId;
StringMap g_SupplyTrainingChapterBySteamId;

// Chapter-start snapshots. A mission loss restores these maps rather than
// deleting campaign progress earned before the current chapter.
StringMap g_ChapterBossKillsBySteamId;
StringMap g_ChapterSpecialKillsBySteamId;
StringMap g_ChapterCommonKillsBySteamId;
StringMap g_ChapterPendingRewardsBySteamId;
StringMap g_ChapterPerkLevelsBySteamId;
StringMap g_ChapterSupplyTrainingStacksBySteamId;
StringMap g_ChapterSupplyTrainingChapterBySteamId;
StringMap g_ChapterLastMapRewardSerialBySteamId;
StringMap g_ChapterSnapshotReadyBySteamId;

// SteamID -> latest chapter reward serial claimed. This is intentionally not
// cleared on death, preventing repeated claims on the same chapter.
StringMap g_LastMapRewardSerialBySteamId;

// Temporary participation bit mask indexed by boss entity index.
int g_BossParticipationMask[MAX_TRACKED_ENTITIES];

// Chapter-transition reward state.
int g_CurrentMapRewardSerial;
int g_CurrentChapterSerial;
bool g_CurrentMapEntryRewardEnabled;
bool g_MapTransitionArmed;
bool g_SuppressNextMapReward;
bool g_RetryingChapter;
char g_LastMapName[PLATFORM_MAX_PATH];

// Client slots are temporary. Persistent data is keyed by this cached SteamID.
char g_ClientSteamId[MAXPLAYERS + 1][MAX_AUTHID_LENGTH];

// Menus are temporary UI state and do not need SteamID persistence.
bool g_PerkMenuOpen[MAXPLAYERS + 1];
int g_CurrentChoices[MAXPLAYERS + 1][PERK_MENU_CHOICES];
int g_CurrentChoiceCount[MAXPLAYERS + 1];
bool g_RerollUsed[MAXPLAYERS + 1];

// Runtime-only effect state. Builds themselves remain SteamID-keyed.
bool g_DamageHooked[MAX_TRACKED_ENTITIES];
float g_MedicalEmptySince[MAXPLAYERS + 1];
float g_ThrowableEmptySince[MAXPLAYERS + 1];
float g_LowHealthNextTick[MAXPLAYERS + 1];
bool g_ActiveReviveBoost[MAXPLAYERS + 1];

ConVar g_PillsDecayRate;
ConVar g_PillsHealthValue;
ConVar g_AdrenalineHealthValue;
ConVar g_SurvivorReviveDuration;
float g_BaseReviveDuration;
Handle g_SupplyTimer;

// The plugin is intentionally limited to cooperative campaign modes.
ConVar g_GameMode;
bool g_GameModeSupported;
