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

        int rewardAmount = isKiller
            ? BSR_GetBossKillerRewards()
            : BSR_GetBossParticipantRewards();

        if (isKiller)
        {
            // Healing uses the level owned before this kill's reward menu.
            BSR_ApplyBossHealing(client);
            BSR_IncrementBossKills(client);
        }

        if (rewardAmount <= 0 || !BSR_AddPendingRewards(client, rewardAmount))
        {
            continue;
        }

        char localizedBoss[32];
        FormatEx(localizedBoss, sizeof(localizedBoss), "%T", bossName, client);

        if (isKiller)
        {
            BSR_PrintChat(
                client,
                BSR_Message_Normal,
                "%t",
                "Boss Killer Reward",
                localizedBoss,
                rewardAmount,
                BSR_GetPendingRewards(client)
            );
        }
        else
        {
            BSR_PrintChat(
                client,
                BSR_Message_Normal,
                "%t",
                "Boss Participant Reward",
                localizedBoss,
                rewardAmount,
                BSR_GetPendingRewards(client)
            );
        }

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

    int killsPerReward = BSR_GetSpecialKillsPerReward();
    if ((specialKills % killsPerReward) != 0)
    {
        return;
    }

    int rewardAmount = BSR_GetSpecialKillRewards();
    if (rewardAmount <= 0 || !BSR_AddPendingRewards(client, rewardAmount))
    {
        return;
    }

    BSR_PrintChat(
        client,
        BSR_Message_Normal,
        "%t",
        "Special Kill Reward",
        specialKills,
        rewardAmount,
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

    int killsPerReward = BSR_GetCommonKillsPerReward();
    if ((commonKills % killsPerReward) != 0)
    {
        return;
    }

    int rewardAmount = BSR_GetCommonKillRewards();
    if (rewardAmount <= 0 || !BSR_AddPendingRewards(client, rewardAmount))
    {
        return;
    }

    BSR_PrintChat(
        client,
        BSR_Message_Normal,
        "%t",
        "Common Kill Reward",
        commonKills,
        rewardAmount,
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

    int rewardAmount = BSR_GetChapterRewards();
    if (rewardAmount <= 0 || !BSR_AddPendingRewards(client, rewardAmount))
    {
        return;
    }

    g_LastMapRewardSerialBySteamId.SetValue(
        steamId,
        g_CurrentMapRewardSerial
    );

    // The chapter-entry reward belongs to the chapter's initial state. If the
    // team wipes, the unspent reward is restored and can be chosen again.
    BSR_CaptureChapterSnapshotForClient(client);

    BSR_PrintChat(
        client,
        BSR_Message_Normal,
        "%t",
        "Chapter Entry Reward",
        rewardAmount,
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
        BSR_EnsureChapterSnapshot(client);
    }

    return Plugin_Stop;
}

public Action Timer_TryGrantMapEntryReward(Handle timer, any userId)
{
    int client = GetClientOfUserId(userId);
    BSR_TryGrantMapEntryReward(client);
    BSR_EnsureChapterSnapshot(client);

    return Plugin_Stop;
}
