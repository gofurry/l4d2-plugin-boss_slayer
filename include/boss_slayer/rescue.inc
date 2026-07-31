#if defined _boss_slayer_rescue_included
    #endinput
#endif
#define _boss_slayer_rescue_included

float BSR_GetRescueSpeedBonus(int client)
{
    int level = BSR_GetPerkLevel(client, Perk_FieldRescue);
    return (float(level) * FIELD_RESCUE_SPEED_PER_LEVEL)
        + BSR_GetSupplyTrainingBonus(client);
}

void BSR_RecomputeReviveDuration()
{
    if (g_SurvivorReviveDuration == null)
    {
        return;
    }

    float greatestBonus = 0.0;

    for (int client = 1; client <= MaxClients; client++)
    {
        if (!g_ActiveReviveBoost[client] || !BSR_IsHumanSurvivor(client))
        {
            continue;
        }

        float bonus = BSR_GetRescueSpeedBonus(client);
        if (bonus > greatestBonus)
        {
            greatestBonus = bonus;
        }
    }

    float duration = g_BaseReviveDuration;
    if (greatestBonus > 0.0)
    {
        duration /= 1.0 + greatestBonus;
    }

    g_SurvivorReviveDuration.FloatValue = duration;
}

void BSR_ClearReviveBoost(int client)
{
    if (client < 1 || client > MaxClients || !g_ActiveReviveBoost[client])
    {
        return;
    }

    g_ActiveReviveBoost[client] = false;
    BSR_RecomputeReviveDuration();
}

void BSR_ResetAllReviveBoosts()
{
    for (int client = 1; client <= MaxClients; client++)
    {
        g_ActiveReviveBoost[client] = false;
    }

    if (g_SurvivorReviveDuration != null)
    {
        g_SurvivorReviveDuration.FloatValue = g_BaseReviveDuration;
    }
}

public void Event_ReviveBegin(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (!BSR_IsHumanSurvivor(client) || BSR_GetRescueSpeedBonus(client) <= 0.0)
    {
        return;
    }

    g_ActiveReviveBoost[client] = true;
    BSR_RecomputeReviveDuration();
}

public void Event_ReviveStopped(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    BSR_ClearReviveBoost(client);
}

public void OnPluginEnd()
{
    BSR_ResetAllReviveBoosts();
}
