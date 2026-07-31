#if defined _boss_slayer_storage_included
    #endinput
#endif
#define _boss_slayer_storage_included

bool BSR_TryGetClientSteamId(int client, char[] steamId, int maxLength)
{
    steamId[0] = 0;

    if (client < 1 || client > MaxClients)
    {
        return false;
    }

    if (!IsClientConnected(client) || IsFakeClient(client))
    {
        return false;
    }

    if (g_ClientSteamId[client][0] == 0 && !BSR_CacheClientSteamId(client))
    {
        return false;
    }

    strcopy(steamId, maxLength, g_ClientSteamId[client]);
    return true;
}

bool BSR_CacheClientSteamId(int client)
{
    if (client < 1 || client > MaxClients)
    {
        return false;
    }

    if (!IsClientConnected(client) || IsFakeClient(client))
    {
        return false;
    }

    g_ClientSteamId[client][0] = 0;

    if (GetClientAuthId(
        client,
        AuthId_SteamID64,
        g_ClientSteamId[client],
        MAX_AUTHID_LENGTH,
        true
    ))
    {
        return true;
    }

    if (GetClientAuthId(
        client,
        AuthId_Steam2,
        g_ClientSteamId[client],
        MAX_AUTHID_LENGTH,
        true
    ))
    {
        return true;
    }

    g_ClientSteamId[client][0] = 0;
    return false;
}

bool BSR_IsHumanSurvivor(int client)
{
    return client >= 1
        && client <= MaxClients
        && IsClientInGame(client)
        && !IsFakeClient(client)
        && GetClientTeam(client) == TEAM_SURVIVOR;
}

int BSR_GetBossKills(int client)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return 0;
    }

    int value = 0;
    g_BossKillsBySteamId.GetValue(steamId, value);
    return value;
}

int BSR_GetSpecialKills(int client)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return 0;
    }

    int value = 0;
    g_SpecialKillsBySteamId.GetValue(steamId, value);
    return value;
}

int BSR_GetCommonKills(int client)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return 0;
    }

    int value = 0;
    g_CommonKillsBySteamId.GetValue(steamId, value);
    return value;
}

bool BSR_SetSpecialKills(int client, int specialKills)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return false;
    }

    if (specialKills <= 0)
    {
        g_SpecialKillsBySteamId.Remove(steamId);
    }
    else
    {
        g_SpecialKillsBySteamId.SetValue(steamId, specialKills);
    }

    return true;
}

bool BSR_SetCommonKills(int client, int commonKills)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return false;
    }

    if (commonKills <= 0)
    {
        g_CommonKillsBySteamId.Remove(steamId);
    }
    else
    {
        g_CommonKillsBySteamId.SetValue(steamId, commonKills);
    }

    return true;
}

int BSR_GetPendingRewards(int client)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return 0;
    }

    int value = 0;
    g_PendingRewardsBySteamId.GetValue(steamId, value);
    return value;
}

bool BSR_SetPendingRewards(int client, int pendingRewards)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return false;
    }

    if (pendingRewards <= 0)
    {
        g_PendingRewardsBySteamId.Remove(steamId);
    }
    else
    {
        g_PendingRewardsBySteamId.SetValue(steamId, pendingRewards);
    }

    return true;
}

void BSR_BuildPerkKey(
    const char[] steamId,
    PerkType perk,
    char[] key,
    int maxLength
)
{
    Format(key, maxLength, "%s:%d", steamId, view_as<int>(perk));
}

int BSR_GetPerkLevel(int client, PerkType perk)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return 0;
    }

    char key[MAX_AUTHID_LENGTH + 16];
    BSR_BuildPerkKey(steamId, perk, key, sizeof(key));

    int level = 0;
    g_PerkLevelsBySteamId.GetValue(key, level);
    return level;
}

bool BSR_SetPerkLevel(int client, PerkType perk, int level)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return false;
    }

    char key[MAX_AUTHID_LENGTH + 16];
    BSR_BuildPerkKey(steamId, perk, key, sizeof(key));

    if (level <= 0)
    {
        g_PerkLevelsBySteamId.Remove(key);
    }
    else
    {
        g_PerkLevelsBySteamId.SetValue(key, level);
    }

    return true;
}

bool BSR_PlayerHasAnyProgress(int client)
{
    if (
        BSR_GetBossKills(client) > 0
        || BSR_GetSpecialKills(client) > 0
        || BSR_GetCommonKills(client) > 0
        || BSR_GetPendingRewards(client) > 0
    )
    {
        return true;
    }

    for (int perkIndex = 0; perkIndex < view_as<int>(Perk_Count); perkIndex++)
    {
        if (BSR_GetPerkLevel(client, view_as<PerkType>(perkIndex)) > 0)
        {
            return true;
        }
    }

    return false;
}

int BSR_GetTotalPerkLevels(int client)
{
    int total = 0;

    for (int perkIndex = 0; perkIndex < view_as<int>(Perk_Count); perkIndex++)
    {
        total += BSR_GetPerkLevel(client, view_as<PerkType>(perkIndex));
    }

    return total;
}

void BSR_ResetPlayerData(int client)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return;
    }

    g_BossKillsBySteamId.Remove(steamId);
    g_SpecialKillsBySteamId.Remove(steamId);
    g_CommonKillsBySteamId.Remove(steamId);
    g_PendingRewardsBySteamId.Remove(steamId);

    for (int perkIndex = 0; perkIndex < view_as<int>(Perk_Count); perkIndex++)
    {
        char key[MAX_AUTHID_LENGTH + 16];
        BSR_BuildPerkKey(
            steamId,
            view_as<PerkType>(perkIndex),
            key,
            sizeof(key)
        );
        g_PerkLevelsBySteamId.Remove(key);
    }

    LogMessage("Player build reset for SteamID %s.", steamId);
}

void BSR_ResetAllPlayerData()
{
    g_BossKillsBySteamId.Clear();
    g_SpecialKillsBySteamId.Clear();
    g_CommonKillsBySteamId.Clear();
    g_PendingRewardsBySteamId.Clear();
    g_PerkLevelsBySteamId.Clear();

    LogMessage("All Boss Slayer SteamID data was reset.");
}
