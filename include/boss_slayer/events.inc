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

    BSR_ResolveBossRewards(tankEntity, killer, "Tank");
}

public void Event_WitchKilled(Event event, const char[] name, bool dontBroadcast)
{
    int witchEntity = event.GetInt("witchid");
    int killer = GetClientOfUserId(event.GetInt("userid"));

    BSR_ResolveBossRewards(witchEntity, killer, "Witch");
}

public void Event_PlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    int victim = GetClientOfUserId(event.GetInt("userid"));

    // A real human survivor death ends that player's entire build.
    if (BSR_IsHumanSurvivor(victim))
    {
        bool hadBuild = BSR_PlayerHasAnyProgress(victim);

        BSR_ResetPlayerData(victim);

        if (g_PerkMenuOpen[victim])
        {
            g_PerkMenuOpen[victim] = false;
            CancelClientMenu(victim);
        }

        if (hadBuild)
        {
            PrintToChat(
                victim,
                "\x04[Boss Slayer]\x01 You died. Your entire build has been reset."
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
    g_MapTransitionArmed = true;
}

public void Event_MissionLost(Event event, const char[] name, bool dontBroadcast)
{
    BSR_ResetAllPlayerData();
    BSR_CancelAllPerkMenus();
    BSR_ClearAllBossParticipation();
    g_MapTransitionArmed = false;

    PrintToChatAll(
        "\x04[Boss Slayer]\x01 Mission lost. All builds have been reset."
    );
}

public void Event_FinaleWin(Event event, const char[] name, bool dontBroadcast)
{
    BSR_ResetAllPlayerData();
    BSR_CancelAllPerkMenus();
    BSR_ClearAllBossParticipation();

    // The next map is the first map of a new campaign, so it gets no reward.
    g_SuppressNextMapReward = true;
    g_MapTransitionArmed = false;
    g_CurrentMapEntryRewardEnabled = false;

    PrintToChatAll(
        "\x04[Boss Slayer]\x01 Campaign completed. All builds have been reset."
    );
}
