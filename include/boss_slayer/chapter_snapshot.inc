#if defined _boss_slayer_chapter_snapshot_included
    #endinput
#endif
#define _boss_slayer_chapter_snapshot_included

void BSR_CopyIntMap(StringMap source, StringMap destination)
{
    destination.Clear();

    StringMapSnapshot keys = source.Snapshot();
    char key[MAX_AUTHID_LENGTH + 16];

    for (int index = 0; index < keys.Length; index++)
    {
        keys.GetKey(index, key, sizeof(key));

        int value = 0;
        if (source.GetValue(key, value))
        {
            destination.SetValue(key, value);
        }
    }

    delete keys;
}

void BSR_ClearChapterSnapshot()
{
    g_ChapterBossKillsBySteamId.Clear();
    g_ChapterSpecialKillsBySteamId.Clear();
    g_ChapterCommonKillsBySteamId.Clear();
    g_ChapterPendingRewardsBySteamId.Clear();
    g_ChapterPerkLevelsBySteamId.Clear();
    g_ChapterSupplyTrainingStacksBySteamId.Clear();
    g_ChapterSupplyTrainingChapterBySteamId.Clear();
    g_ChapterLastMapRewardSerialBySteamId.Clear();
    g_ChapterSnapshotReadyBySteamId.Clear();
}

void BSR_BeginChapterSnapshot()
{
    BSR_CopyIntMap(g_BossKillsBySteamId, g_ChapterBossKillsBySteamId);
    BSR_CopyIntMap(g_SpecialKillsBySteamId, g_ChapterSpecialKillsBySteamId);
    BSR_CopyIntMap(g_CommonKillsBySteamId, g_ChapterCommonKillsBySteamId);
    BSR_CopyIntMap(g_PendingRewardsBySteamId, g_ChapterPendingRewardsBySteamId);
    BSR_CopyIntMap(g_PerkLevelsBySteamId, g_ChapterPerkLevelsBySteamId);
    BSR_CopyIntMap(g_SupplyTrainingStacksBySteamId, g_ChapterSupplyTrainingStacksBySteamId);
    BSR_CopyIntMap(g_SupplyTrainingChapterBySteamId, g_ChapterSupplyTrainingChapterBySteamId);
    BSR_CopyIntMap(g_LastMapRewardSerialBySteamId, g_ChapterLastMapRewardSerialBySteamId);
    g_ChapterSnapshotReadyBySteamId.Clear();
}

void BSR_SetSnapshotValue(StringMap map, const char[] key, int value)
{
    if (value <= 0)
    {
        map.Remove(key);
    }
    else
    {
        map.SetValue(key, value);
    }
}

void BSR_CaptureChapterSnapshotForClient(int client)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return;
    }

    BSR_SetSnapshotValue(g_ChapterBossKillsBySteamId, steamId, BSR_GetBossKills(client));
    BSR_SetSnapshotValue(g_ChapterSpecialKillsBySteamId, steamId, BSR_GetSpecialKills(client));
    BSR_SetSnapshotValue(g_ChapterCommonKillsBySteamId, steamId, BSR_GetCommonKills(client));
    BSR_SetSnapshotValue(g_ChapterPendingRewardsBySteamId, steamId, BSR_GetPendingRewards(client));
    BSR_SetSnapshotValue(
        g_ChapterSupplyTrainingStacksBySteamId,
        steamId,
        BSR_GetSupplyTrainingStacks(client)
    );

    int supplyChapter = 0;
    g_SupplyTrainingChapterBySteamId.GetValue(steamId, supplyChapter);
    BSR_SetSnapshotValue(g_ChapterSupplyTrainingChapterBySteamId, steamId, supplyChapter);

    int mapRewardSerial = 0;
    g_LastMapRewardSerialBySteamId.GetValue(steamId, mapRewardSerial);
    BSR_SetSnapshotValue(g_ChapterLastMapRewardSerialBySteamId, steamId, mapRewardSerial);

    for (int perkIndex = 0; perkIndex < view_as<int>(Perk_Count); perkIndex++)
    {
        char perkKey[MAX_AUTHID_LENGTH + 16];
        PerkType perk = view_as<PerkType>(perkIndex);
        BSR_BuildPerkKey(steamId, perk, perkKey, sizeof(perkKey));
        BSR_SetSnapshotValue(
            g_ChapterPerkLevelsBySteamId,
            perkKey,
            BSR_GetPerkLevel(client, perk)
        );
    }

    g_ChapterSnapshotReadyBySteamId.SetValue(steamId, 1);
}

void BSR_EnsureChapterSnapshot(int client)
{
    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return;
    }

    int ready = 0;
    g_ChapterSnapshotReadyBySteamId.GetValue(steamId, ready);

    if (ready == 0)
    {
        BSR_CaptureChapterSnapshotForClient(client);
    }
}

void BSR_RestoreChapterSnapshot()
{
    BSR_CopyIntMap(g_ChapterBossKillsBySteamId, g_BossKillsBySteamId);
    BSR_CopyIntMap(g_ChapterSpecialKillsBySteamId, g_SpecialKillsBySteamId);
    BSR_CopyIntMap(g_ChapterCommonKillsBySteamId, g_CommonKillsBySteamId);
    BSR_CopyIntMap(g_ChapterPendingRewardsBySteamId, g_PendingRewardsBySteamId);
    BSR_CopyIntMap(g_ChapterPerkLevelsBySteamId, g_PerkLevelsBySteamId);
    BSR_CopyIntMap(g_ChapterSupplyTrainingStacksBySteamId, g_SupplyTrainingStacksBySteamId);
    BSR_CopyIntMap(g_ChapterSupplyTrainingChapterBySteamId, g_SupplyTrainingChapterBySteamId);
    BSR_CopyIntMap(g_ChapterLastMapRewardSerialBySteamId, g_LastMapRewardSerialBySteamId);

    for (int client = 1; client <= MaxClients; client++)
    {
        BSR_ResetRuntimeEffectState(client);
        BSR_ResetPerkChoiceState(client);
    }

    LogMessage("All builds restored to the current chapter-start snapshot.");
}
