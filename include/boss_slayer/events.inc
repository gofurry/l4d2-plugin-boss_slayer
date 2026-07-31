#if defined _boss_slayer_events_included
    #endinput
#endif
#define _boss_slayer_events_included

void BSR_RegisterEvents()
{
    HookEvent("tank_spawn", Event_TankSpawn, EventHookMode_Post);
    HookEvent("witch_spawn", Event_WitchSpawn, EventHookMode_Post);
    HookEvent("player_hurt", Event_PlayerHurt, EventHookMode_Post);
    HookEvent("infected_hurt", Event_InfectedHurt, EventHookMode_Post);
    HookEvent("infected_death", Event_InfectedDeath, EventHookMode_Post);
    HookEvent("heal_success", Event_HealSuccess, EventHookMode_Post);
    HookEvent("pills_used", Event_PillsUsed, EventHookMode_Post);
    HookEvent("adrenaline_used", Event_AdrenalineUsed, EventHookMode_Post);
    HookEvent("weapon_fire", Event_WeaponFire, EventHookMode_Post);
    HookEvent("revive_begin", Event_ReviveBegin, EventHookMode_Post);
    HookEvent("revive_end", Event_ReviveStopped, EventHookMode_Post);
    HookEvent("revive_success", Event_ReviveStopped, EventHookMode_Post);

    HookEvent("tank_killed", Event_TankKilled, EventHookMode_Post);
    HookEvent("witch_killed", Event_WitchKilled, EventHookMode_Post);
    HookEvent("player_death", Event_PlayerDeath, EventHookMode_Post);
    HookEvent("player_spawn", Event_PlayerSpawn, EventHookMode_Post);

    HookEvent("map_transition", Event_MapTransition, EventHookMode_Post);
    HookEvent("mission_lost", Event_MissionLost, EventHookMode_Post);
    HookEvent("finale_win", Event_FinaleWin, EventHookMode_Post);
}

public void Event_TankSpawn(Event event, const char[] name, bool dontBroadcast)
{
    int tankEntity = event.GetInt("tankid");

    if (tankEntity <= 0)
    {
        tankEntity = GetClientOfUserId(event.GetInt("userid"));
    }

    BSR_ClearBossParticipation(tankEntity);
}

public void Event_WitchSpawn(Event event, const char[] name, bool dontBroadcast)
{
    BSR_ClearBossParticipation(event.GetInt("witchid"));
}

public void Event_PlayerHurt(Event event, const char[] name, bool dontBroadcast)
{
    if (event.GetInt("dmg_health") <= 0)
    {
        return;
    }

    int victim = GetClientOfUserId(event.GetInt("userid"));
    int attacker = GetClientOfUserId(event.GetInt("attacker"));

    if (!BSR_IsHumanSurvivor(attacker) || !BSR_IsTankClient(victim))
    {
        return;
    }

    BSR_MarkBossParticipant(victim, attacker);
}

public void Event_InfectedHurt(Event event, const char[] name, bool dontBroadcast)
{
    if (event.GetInt("amount") <= 0)
    {
        return;
    }

    int attacker = GetClientOfUserId(event.GetInt("attacker"));
    int infectedEntity = event.GetInt("entityid");

    if (!BSR_IsHumanSurvivor(attacker) || !BSR_IsWitchEntity(infectedEntity))
    {
        return;
    }

    BSR_MarkBossParticipant(infectedEntity, attacker);
}

public void Event_InfectedDeath(Event event, const char[] name, bool dontBroadcast)
{
    int attacker = GetClientOfUserId(event.GetInt("attacker"));

    if (!BSR_IsHumanSurvivor(attacker) || !IsPlayerAlive(attacker))
    {
        return;
    }

    BSR_RecordCommonInfectedKill(attacker);
}

public void Event_TankKilled(Event event, const char[] name, bool dontBroadcast)
{
    int tankEntity = GetClientOfUserId(event.GetInt("userid"));
    int killer = GetClientOfUserId(event.GetInt("attacker"));

    BSR_ResolveBossRewards(tankEntity, killer, "Boss Tank");
}

public void Event_WitchKilled(Event event, const char[] name, bool dontBroadcast)
{
    int witchEntity = event.GetInt("witchid");
    int killer = GetClientOfUserId(event.GetInt("userid"));

    BSR_ResolveBossRewards(witchEntity, killer, "Boss Witch");
}

public void Event_PlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    int victim = GetClientOfUserId(event.GetInt("userid"));

    // Individual deaths keep campaign progress. Runtime-only timers and open
    // menus are cleared until the survivor becomes playable again.
    if (BSR_IsHumanSurvivor(victim))
    {
        BSR_ResetRuntimeEffectState(victim);
        BSR_ClearReviveBoost(victim);

        if (g_PerkMenuOpen[victim])
        {
            CancelClientMenu(victim);
        }

        g_PerkMenuOpen[victim] = false;

        if (BSR_PlayerHasAnyProgress(victim))
        {
            BSR_PrintChat(
                victim,
                BSR_Message_Detailed,
                "%t",
                "Death Build Retained"
            );
        }

        return;
    }

    // Tank is handled by tank_killed and does not count toward 30 SI kills.
    if (!BSR_IsNormalSpecialInfected(victim))
    {
        return;
    }

    int attacker = GetClientOfUserId(event.GetInt("attacker"));

    if (!BSR_IsHumanSurvivor(attacker) || !IsPlayerAlive(attacker))
    {
        return;
    }

    char killWeapon[40];
    event.GetString("weapon", killWeapon, sizeof(killWeapon));

    BSR_ApplyAmmoReclamation(attacker);
    BSR_ApplyPrecisionHunter(attacker, killWeapon);
    BSR_RecordSpecialInfectedKill(attacker);
}

public void Event_PlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));

    if (client <= 0)
    {
        return;
    }

    CreateTimer(
        1.0,
        Timer_TryGrantMapEntryReward,
        GetClientUserId(client),
        TIMER_FLAG_NO_MAPCHANGE
    );
}

public void Event_MapTransition(Event event, const char[] name, bool dontBroadcast)
{
    if (!BSR_IsSupportedGameMode())
    {
        return;
    }

    g_MapTransitionArmed = true;
}

public void Event_MissionLost(Event event, const char[] name, bool dontBroadcast)
{
    if (!BSR_IsSupportedGameMode())
    {
        return;
    }

    BSR_RestoreChapterSnapshot();
    BSR_CancelAllPerkMenus();
    BSR_ClearAllBossParticipation();
    BSR_ResetAllReviveBoosts();
    g_MapTransitionArmed = false;
    g_RetryingChapter = true;

    BSR_PrintChatAll(
        BSR_Message_Minimal,
        "%t",
        "Mission Lost Restored"
    );
}

public void Event_FinaleWin(Event event, const char[] name, bool dontBroadcast)
{
    if (!BSR_IsSupportedGameMode())
    {
        return;
    }

    BSR_ResetAllPlayerData();
    BSR_CancelAllPerkMenus();
    BSR_ClearAllBossParticipation();
    BSR_ResetAllReviveBoosts();

    // The next map is the first map of a new campaign, so it gets no reward.
    g_SuppressNextMapReward = true;
    g_MapTransitionArmed = false;
    g_CurrentMapEntryRewardEnabled = false;

    BSR_PrintChatAll(
        BSR_Message_Minimal,
        "%t",
        "Campaign Complete Reset"
    );
}
