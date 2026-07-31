#if defined _boss_slayer_boss_tracking_included
    #endinput
#endif
#define _boss_slayer_boss_tracking_included

void BSR_MarkBossParticipant(int bossEntity, int client)
{
    if (
        bossEntity <= 0
        || bossEntity >= MAX_TRACKED_ENTITIES
        || client < 1
        || client > 32
    )
    {
        return;
    }

    g_BossParticipationMask[bossEntity] |= (1 << (client - 1));
}

bool BSR_WasBossParticipant(int bossEntity, int client)
{
    if (
        bossEntity <= 0
        || bossEntity >= MAX_TRACKED_ENTITIES
        || client < 1
        || client > 32
    )
    {
        return false;
    }

    return (
        g_BossParticipationMask[bossEntity]
        & (1 << (client - 1))
    ) != 0;
}

void BSR_ClearBossParticipation(int bossEntity)
{
    if (bossEntity <= 0 || bossEntity >= MAX_TRACKED_ENTITIES)
    {
        return;
    }

    g_BossParticipationMask[bossEntity] = 0;
}

void BSR_ClearClientBossParticipation(int client)
{
    if (client < 1 || client > 32)
    {
        return;
    }

    int clientBit = (1 << (client - 1));

    for (int entity = 1; entity < MAX_TRACKED_ENTITIES; entity++)
    {
        g_BossParticipationMask[entity] &= ~clientBit;
    }
}

void BSR_ClearAllBossParticipation()
{
    for (int entity = 0; entity < MAX_TRACKED_ENTITIES; entity++)
    {
        g_BossParticipationMask[entity] = 0;
    }
}

bool BSR_IsTankClient(int client)
{
    return client >= 1
        && client <= MaxClients
        && IsClientInGame(client)
        && GetClientTeam(client) == TEAM_INFECTED
        && HasEntProp(client, Prop_Send, "m_zombieClass")
        && GetEntProp(client, Prop_Send, "m_zombieClass") == ZCLASS_TANK;
}

bool BSR_IsNormalSpecialInfected(int client)
{
    if (
        client < 1
        || client > MaxClients
        || !IsClientInGame(client)
        || GetClientTeam(client) != TEAM_INFECTED
        || !HasEntProp(client, Prop_Send, "m_zombieClass")
    )
    {
        return false;
    }

    int zombieClass = GetEntProp(client, Prop_Send, "m_zombieClass");

    // Smoker, Boomer, Hunter, Spitter, Jockey and Charger.
    return zombieClass >= 1 && zombieClass <= 6;
}

bool BSR_IsWitchEntity(int entity)
{
    if (entity <= MaxClients || entity <= 0 || !IsValidEntity(entity))
    {
        return false;
    }

    char classname[32];
    GetEntityClassname(entity, classname, sizeof(classname));

    return StrEqual(classname, "witch", false);
}
