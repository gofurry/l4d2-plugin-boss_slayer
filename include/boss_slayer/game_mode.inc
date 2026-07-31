#if defined _boss_slayer_game_mode_included
    #endinput
#endif
#define _boss_slayer_game_mode_included

void BSR_InitializeGameMode()
{
    g_GameMode = FindConVar("mp_gamemode");

    if (g_GameMode == null)
    {
        g_GameModeSupported = false;
        LogError("Could not find mp_gamemode. Boss Slayer gameplay is disabled.");
        return;
    }

    g_GameMode.AddChangeHook(BSR_OnGameModeChanged);
    BSR_RefreshGameMode();
}

void BSR_RefreshGameMode()
{
    g_GameModeSupported = false;

    if (g_GameMode == null)
    {
        return;
    }

    char gameMode[32];
    g_GameMode.GetString(gameMode, sizeof(gameMode));

    g_GameModeSupported =
        StrEqual(gameMode, "coop", false)
        || StrEqual(gameMode, "realism", false);
}

bool BSR_IsSupportedGameMode()
{
    return g_GameModeSupported;
}

public void BSR_OnGameModeChanged(
    ConVar convar,
    const char[] oldValue,
    const char[] newValue
)
{
    bool wasSupported = g_GameModeSupported;
    BSR_RefreshGameMode();

    if (wasSupported == g_GameModeSupported)
    {
        return;
    }

    // Never let an in-memory build cross the cooperative-campaign boundary.
    BSR_ResetAllPlayerData();
    BSR_CancelAllPerkMenus();
    BSR_ClearAllBossParticipation();
    BSR_ResetAllReviveBoosts();
    g_CurrentMapEntryRewardEnabled = false;
    g_MapTransitionArmed = false;
    g_SuppressNextMapReward = false;
    g_RetryingChapter = false;

    LogMessage(
        "Game mode changed from %s to %s. Boss Slayer gameplay is now %s.",
        oldValue,
        newValue,
        g_GameModeSupported ? "enabled" : "disabled"
    );
}
