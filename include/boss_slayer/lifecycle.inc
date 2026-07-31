#if defined _boss_slayer_lifecycle_included
    #endinput
#endif
#define _boss_slayer_lifecycle_included

void BSR_InitializeState()
{
    g_BossKillsBySteamId = new StringMap();
    g_SpecialKillsBySteamId = new StringMap();
    g_CommonKillsBySteamId = new StringMap();
    g_PendingRewardsBySteamId = new StringMap();
    g_PerkLevelsBySteamId = new StringMap();
    g_LastMapRewardSerialBySteamId = new StringMap();

    g_CurrentMapRewardSerial = 0;
    g_CurrentMapEntryRewardEnabled = false;
    g_MapTransitionArmed = false;
    g_SuppressNextMapReward = false;
    g_LastMapName[0] = 0;
    GetCurrentMap(g_LastMapName, sizeof(g_LastMapName));

    BSR_ClearAllBossParticipation();
}

void BSR_CacheConnectedClients()
{
    // Support late plugin loading while players are already connected.
    for (int client = 1; client <= MaxClients; client++)
    {
        g_PerkMenuOpen[client] = false;
        g_ClientSteamId[client][0] = 0;

        if (IsClientConnected(client) && !IsFakeClient(client))
        {
            BSR_CacheClientSteamId(client);
        }
    }
}

public void OnMapStart()
{
    // Menus and encounter participation do not survive a map change.
    for (int client = 1; client <= MaxClients; client++)
    {
        g_PerkMenuOpen[client] = false;
    }

    BSR_ClearAllBossParticipation();

    char currentMap[PLATFORM_MAX_PATH];
    currentMap[0] = 0;
    GetCurrentMap(currentMap, sizeof(currentMap));

    bool mapChanged =
        g_LastMapName[0] != 0
        && currentMap[0] != 0
        && !StrEqual(g_LastMapName, currentMap, false);

    // map_transition handles normal play; the name fallback supports changelevel.
    // finale_win suppresses the first-map reward of the next campaign.
    bool shouldGrantMapReward =
        !g_SuppressNextMapReward
        && (g_MapTransitionArmed || mapChanged);

    g_MapTransitionArmed = false;
    g_SuppressNextMapReward = false;

    if (currentMap[0] != 0)
    {
        strcopy(g_LastMapName, sizeof(g_LastMapName), currentMap);
    }

    if (shouldGrantMapReward)
    {
        g_CurrentMapRewardSerial++;
        g_CurrentMapEntryRewardEnabled = true;
        CreateTimer(3.0, Timer_GrantMapEntryRewards, 0, TIMER_FLAG_NO_MAPCHANGE);
    }
    else
    {
        g_CurrentMapEntryRewardEnabled = false;
    }
}

public void OnClientAuthorized(int client, const char[] auth)
{
    if (client < 1 || client > MaxClients || StrEqual(auth, "BOT"))
    {
        return;
    }

    g_ClientSteamId[client][0] = 0;

    if (!GetClientAuthId(
        client,
        AuthId_SteamID64,
        g_ClientSteamId[client],
        MAX_AUTHID_LENGTH,
        true
    ))
    {
        // Fallback for environments where SteamID64 is temporarily unavailable.
        strcopy(g_ClientSteamId[client], MAX_AUTHID_LENGTH, auth);
    }
}

public void OnClientDisconnect(int client)
{
    if (client < 1 || client > MaxClients)
    {
        return;
    }

    g_PerkMenuOpen[client] = false;
    g_ClientSteamId[client][0] = 0;
    BSR_ClearClientBossParticipation(client);

    // Do not delete SteamID data here. This preserves chapter changes and
    // reconnects while preventing a new occupant of this slot inheriting data.
}
