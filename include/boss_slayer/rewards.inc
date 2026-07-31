#if defined _boss_slayer_rewards_included
    #endinput
#endif
#define _boss_slayer_rewards_included

bool BSR_AddPendingRewards(int client, int amount)
{
    if (amount <= 0)
    {
        return false;
    }

    return BSR_SetPendingRewards(
        client,
        BSR_GetPendingRewards(client) + amount
    );
}

bool BSR_IncrementBossKills(int client)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return false;
    }

    g_BossKillsBySteamId.SetValue(
        steamId,
        BSR_GetBossKills(client) + 1
    );

    return true;
}

void BSR_ResolveBossRewards(
    int bossEntity,
    int killer,
    const char[] bossName
)
{
    for (int client = 1; client <= MaxClients; client++)
    {
        if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client))
        {
            continue;
        }

        bool isKiller = client == killer;
        bool participated =
            isKiller
            || BSR_WasBossParticipant(bossEntity, client);

        if (!participated)
        {
            continue;
        }

        int rewardAmount = isKiller ? 2 : 1;

        if (isKiller)
        {
            BSR_IncrementBossKills(client);
        }

        if (!BSR_AddPendingRewards(client, rewardAmount))
        {
            continue;
        }

        if (isKiller)
        {
            PrintToChat(
                client,
                "\x04[Boss Slayer]\x01 You killed the %s: +2 perk choices.",
                bossName
            );
        }
        else
        {
            PrintToChat(
                client,
                "\x04[Boss Slayer]\x01 You helped kill the %s: +1 perk choice.",
                bossName
            );
        }

        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 Pending rewards: %d",
            BSR_GetPendingRewards(client)
        );

        BSR_ShowPerkMenu(client);
    }

    BSR_ClearBossParticipation(bossEntity);
}

void BSR_RecordSpecialInfectedKill(int client)
{
    int specialKills = BSR_GetSpecialKills(client) + 1;

    if (!BSR_SetSpecialKills(client, specialKills))
    {
        return;
    }

    if ((specialKills % SPECIAL_KILLS_PER_REWARD) != 0)
    {
        return;
    }

    if (!BSR_AddPendingRewards(client, 1))
    {
        return;
    }

    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 %d special infected kills reached: +1 perk choice.",
        specialKills
    );
    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 Pending rewards: %d",
        BSR_GetPendingRewards(client)
    );

    BSR_ShowPerkMenu(client);
}

void BSR_RecordCommonInfectedKill(int client)
{
    int commonKills = BSR_GetCommonKills(client) + 1;

    if (!BSR_SetCommonKills(client, commonKills))
    {
        return;
    }

    if ((commonKills % COMMON_KILLS_PER_REWARD) != 0)
    {
        return;
    }

    if (!BSR_AddPendingRewards(client, 1))
    {
        return;
    }

    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 %d common infected kills reached: +1 perk choice.",
        commonKills
    );
    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 Pending rewards: %d",
        BSR_GetPendingRewards(client)
    );

    BSR_ShowPerkMenu(client);
}

void BSR_TryGrantMapEntryReward(int client)
{
    if (
        !g_CurrentMapEntryRewardEnabled
        || g_CurrentMapRewardSerial <= 0
        || !BSR_IsHumanSurvivor(client)
        || !IsPlayerAlive(client)
    )
    {
        return;
    }

    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return;
    }

    int lastClaimedSerial = 0;
    g_LastMapRewardSerialBySteamId.GetValue(steamId, lastClaimedSerial);

    if (lastClaimedSerial == g_CurrentMapRewardSerial)
    {
        return;
    }

    if (!BSR_AddPendingRewards(client, 1))
    {
        return;
    }

    g_LastMapRewardSerialBySteamId.SetValue(
        steamId,
        g_CurrentMapRewardSerial
    );

    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 New chapter compensation: +1 perk choice."
    );
    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 Pending rewards: %d",
        BSR_GetPendingRewards(client)
    );

    BSR_ShowPerkMenu(client);
}

public Action Timer_GrantMapEntryRewards(Handle timer, any data)
{
    if (!g_CurrentMapEntryRewardEnabled)
    {
        return Plugin_Stop;
    }

    for (int client = 1; client <= MaxClients; client++)
    {
        BSR_TryGrantMapEntryReward(client);
    }

    return Plugin_Stop;
}

public Action Timer_TryGrantMapEntryReward(Handle timer, any userId)
{
    int client = GetClientOfUserId(userId);
    BSR_TryGrantMapEntryReward(client);

    return Plugin_Stop;
}
