#if defined _boss_slayer_effects_included
    #endinput
#endif
#define _boss_slayer_effects_included

void BSR_InitializeEffects()
{
    g_PillsDecayRate = FindConVar("pain_pills_decay_rate");
    g_PillsHealthValue = FindConVar("pain_pills_health_value");
    g_AdrenalineHealthValue = FindConVar("adrenaline_health_buffer");
    g_SurvivorReviveDuration = FindConVar("survivor_revive_duration");
    g_BaseReviveDuration = g_SurvivorReviveDuration != null
        ? g_SurvivorReviveDuration.FloatValue
        : 5.0;

    for (int entity = 0; entity < MAX_TRACKED_ENTITIES; entity++)
    {
        g_DamageHooked[entity] = false;
    }

    for (int client = 1; client <= MaxClients; client++)
    {
        BSR_ResetRuntimeEffectState(client);
    }

    BSR_StartSupplyTimer();
}

void BSR_ResetRuntimeEffectState(int client)
{
    if (client < 1 || client > MaxClients)
    {
        return;
    }

    g_MedicalEmptySince[client] = 0.0;
    g_ThrowableEmptySince[client] = 0.0;
    g_LowHealthNextTick[client] = 0.0;
}

void BSR_HookDamageEntity(int entity)
{
    if (
        entity <= 0
        || entity >= MAX_TRACKED_ENTITIES
        || !IsValidEntity(entity)
        || g_DamageHooked[entity]
    )
    {
        return;
    }

    if (SDKHookEx(entity, SDKHook_OnTakeDamage, BSR_OnTakeDamage))
    {
        g_DamageHooked[entity] = true;
    }
}

void BSR_HookExistingDamageEntities()
{
    int maxEntities = GetMaxEntities();
    if (maxEntities > MAX_TRACKED_ENTITIES)
    {
        maxEntities = MAX_TRACKED_ENTITIES;
    }

    for (int entity = 1; entity < maxEntities; entity++)
    {
        if (!IsValidEntity(entity))
        {
            continue;
        }

        if (entity <= MaxClients)
        {
            if (IsClientConnected(entity))
            {
                BSR_HookDamageEntity(entity);
            }

            continue;
        }

        char classname[32];
        GetEntityClassname(entity, classname, sizeof(classname));

        if (BSR_IsHookableInfectedClass(classname))
        {
            BSR_HookDamageEntity(entity);
        }
    }
}

public void OnEntityCreated(int entity, const char[] classname)
{
    if (BSR_IsHookableInfectedClass(classname))
    {
        BSR_HookDamageEntity(entity);
    }
}

public void OnEntityDestroyed(int entity)
{
    if (entity > 0 && entity < MAX_TRACKED_ENTITIES)
    {
        g_DamageHooked[entity] = false;
    }
}

bool BSR_IsHookableInfectedClass(const char[] classname)
{
    return StrEqual(classname, "infected", false)
        || StrEqual(classname, "witch", false)
        || StrEqual(classname, "witch_bride", false);
}

public Action BSR_OnTakeDamage(
    int victim,
    int &attacker,
    int &inflictor,
    float &damage,
    int &damageType,
    int &weapon,
    float damageForce[3],
    float damagePosition[3],
    int damageCustom
)
{
    if (damage <= 0.0)
    {
        return Plugin_Continue;
    }

    bool changed = false;

    // Firepower never increases friendly fire because survivors are not valid
    // offensive targets here.
    if (
        BSR_IsHumanSurvivor(attacker)
        && attacker != victim
        && BSR_IsInfectedDamageTarget(victim)
        && BSR_IsDirectDamageType(damageType)
    )
    {
        int level = BSR_GetPerkLevel(attacker, Perk_Firepower);
        float bonus = (float(level) * BSR_GetFirepowerPerLevel())
            + BSR_GetSupplyTrainingBonus(attacker);

        if (bonus > 0.0)
        {
            damage *= 1.0 + bonus;
            changed = true;
        }
    }

    if (BSR_IsHumanSurvivor(victim) && BSR_ShouldApplyToughness(attacker, inflictor, damageType))
    {
        int level = BSR_GetPerkLevel(victim, Perk_Toughness);
        float reduction = (float(level) * BSR_GetToughnessPerLevel())
            + BSR_GetSupplyTrainingBonus(victim);

        if (reduction > 0.90)
        {
            reduction = 0.90;
        }

        if (reduction > 0.0)
        {
            damage *= 1.0 - reduction;
            changed = true;
        }
    }

    return changed ? Plugin_Changed : Plugin_Continue;
}

bool BSR_IsDirectDamageType(int damageType)
{
    int indirectTypes = DMG_BURN | DMG_SLOWBURN | DMG_ACID;
    if ((damageType & indirectTypes) != 0)
    {
        return false;
    }

    int directTypes = DMG_BULLET | DMG_SLASH | DMG_CLUB | DMG_BLAST;
    return (damageType & directTypes) != 0;
}

bool BSR_IsInfectedDamageTarget(int entity)
{
    if (entity >= 1 && entity <= MaxClients)
    {
        return IsClientInGame(entity) && GetClientTeam(entity) == TEAM_INFECTED;
    }

    if (entity <= MaxClients || !IsValidEntity(entity))
    {
        return false;
    }

    char classname[32];
    GetEntityClassname(entity, classname, sizeof(classname));
    return BSR_IsHookableInfectedClass(classname);
}

bool BSR_ShouldApplyToughness(int attacker, int inflictor, int damageType)
{
    int excludedTypes = DMG_FALL | DMG_DROWN | DMG_DROWNRECOVER;
    if ((damageType & excludedTypes) != 0)
    {
        return false;
    }

    // Fire and acid are explicitly covered even when the original owner is no
    // longer available as an attacker entity.
    int coveredWorldTypes = DMG_BURN | DMG_SLOWBURN | DMG_ACID;
    if ((damageType & coveredWorldTypes) != 0)
    {
        return true;
    }

    if (attacker >= 1 && attacker <= MaxClients && IsClientInGame(attacker))
    {
        int team = GetClientTeam(attacker);
        return team == TEAM_SURVIVOR || team == TEAM_INFECTED;
    }

    return BSR_IsCombatEntity(attacker) || BSR_IsCombatEntity(inflictor);
}

bool BSR_IsCombatEntity(int entity)
{
    if (entity <= MaxClients || entity <= 0 || !IsValidEntity(entity))
    {
        return false;
    }

    char classname[32];
    GetEntityClassname(entity, classname, sizeof(classname));

    return BSR_IsHookableInfectedClass(classname)
        || StrEqual(classname, "tank_rock", false)
        || StrEqual(classname, "insect_swarm", false)
        || StrEqual(classname, "inferno", false)
        || StrEqual(classname, "entityflame", false);
}

int BSR_GetMaxSurvivorHealth(int client)
{
    if (HasEntProp(client, Prop_Send, "m_iMaxHealth"))
    {
        int maxHealth = GetEntProp(client, Prop_Send, "m_iMaxHealth");
        if (maxHealth > 0)
        {
            return maxHealth;
        }
    }

    return 100;
}

float BSR_GetTemporaryHealth(int client)
{
    if (!HasEntProp(client, Prop_Send, "m_healthBuffer"))
    {
        return 0.0;
    }

    float buffer = GetEntPropFloat(client, Prop_Send, "m_healthBuffer");
    float bufferTime = GetEntPropFloat(client, Prop_Send, "m_healthBufferTime");
    float decayRate = g_PillsDecayRate != null ? g_PillsDecayRate.FloatValue : 0.27;
    float remaining = buffer - ((GetGameTime() - bufferTime) * decayRate);

    return remaining > 0.0 ? remaining : 0.0;
}

int BSR_AddRealHealth(int client, int amount)
{
    if (amount <= 0 || !BSR_CanReceiveHealth(client))
    {
        return 0;
    }

    int current = GetClientHealth(client);
    int maximum = BSR_GetMaxSurvivorHealth(client);
    int temporary = RoundToCeil(BSR_GetTemporaryHealth(client));
    int maximumReal = maximum - temporary;

    if (maximumReal < current)
    {
        maximumReal = current;
    }

    int updated = current + amount;

    if (updated > maximumReal)
    {
        updated = maximumReal;
    }

    if (updated <= current)
    {
        return 0;
    }

    SetEntityHealth(client, updated);
    return updated - current;
}

int BSR_AddTemporaryHealth(int client, int amount)
{
    if (amount <= 0 || !BSR_CanReceiveHealth(client))
    {
        return 0;
    }

    int realHealth = GetClientHealth(client);
    int maximum = BSR_GetMaxSurvivorHealth(client);
    float currentBuffer = BSR_GetTemporaryHealth(client);
    float available = float(maximum - realHealth) - currentBuffer;

    if (available <= 0.0)
    {
        return 0;
    }

    float added = float(amount);
    if (added > available)
    {
        added = available;
    }

    SetEntPropFloat(client, Prop_Send, "m_healthBuffer", currentBuffer + added);
    SetEntPropFloat(client, Prop_Send, "m_healthBufferTime", GetGameTime());
    return RoundToCeil(added);
}

bool BSR_AddSplitHealth(int client, int realAmount, int temporaryAmount)
{
    int realAdded = BSR_AddRealHealth(client, realAmount);
    int temporaryAdded = BSR_AddTemporaryHealth(client, temporaryAmount);
    return realAdded > 0 || temporaryAdded > 0;
}

bool BSR_CanReceiveHealth(int client)
{
    if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client))
    {
        return false;
    }

    if (
        (HasEntProp(client, Prop_Send, "m_isIncapacitated")
            && GetEntProp(client, Prop_Send, "m_isIncapacitated") != 0)
        || (HasEntProp(client, Prop_Send, "m_isHangingFromLedge")
            && GetEntProp(client, Prop_Send, "m_isHangingFromLedge") != 0)
    )
    {
        return false;
    }

    return true;
}

bool BSR_HasHealthCapacity(int client)
{
    if (!BSR_CanReceiveHealth(client))
    {
        return false;
    }

    float effective = float(GetClientHealth(client)) + BSR_GetTemporaryHealth(client);
    return effective < float(BSR_GetMaxSurvivorHealth(client));
}

void BSR_ApplyBossHealing(int client)
{
    int level = BSR_GetPerkLevel(client, Perk_BossHealing);
    if (level <= 0)
    {
        return;
    }

    int realAmount = level * BSR_GetBossHealRealPerLevel();
    int tempAmount = level * BSR_GetBossHealTempPerLevel();

    if (BSR_AddSplitHealth(client, realAmount, tempAmount))
    {
        BSR_PrintChat(
            client,
            BSR_Message_Detailed,
            "%t",
            "Boss Healing Triggered",
            realAmount,
            tempAmount
        );
    }
}

public void Event_HealSuccess(Event event, const char[] name, bool dontBroadcast)
{
    int healer = GetClientOfUserId(event.GetInt("userid"));
    int subject = GetClientOfUserId(event.GetInt("subject"));

    if (!BSR_IsHumanSurvivor(healer) || !BSR_IsHumanSurvivor(subject))
    {
        return;
    }

    int level = BSR_GetPerkLevel(healer, Perk_HealingBoost);
    int restored = event.GetInt("health_restored");
    float bonusMultiplier = (float(level) * BSR_GetHealingBoostPerLevel())
        + BSR_GetSupplyTrainingBonus(healer);

    if (restored > 0 && bonusMultiplier > 0.0)
    {
        int bonus = RoundToNearest(float(restored) * bonusMultiplier);
        BSR_AddRealHealth(subject, bonus);
    }

    if (healer != subject)
    {
        int feedbackLevel = BSR_GetPerkLevel(healer, Perk_FirstAidFeedback);
        if (feedbackLevel > 0)
        {
            BSR_AddTemporaryHealth(
                healer,
                feedbackLevel * BSR_GetFirstAidFeedbackPerLevel()
            );
        }
    }

    BSR_RecordSupplyUse(healer);
}

public void Event_PillsUsed(Event event, const char[] name, bool dontBroadcast)
{
    int user = GetClientOfUserId(event.GetInt("userid"));
    int subject = GetClientOfUserId(event.GetInt("subject"));

    if (!BSR_IsHumanSurvivor(user))
    {
        return;
    }

    if (!BSR_IsHumanSurvivor(subject))
    {
        subject = user;
    }

    int level = BSR_GetPerkLevel(user, Perk_HealingBoost);
    float bonusMultiplier = (float(level) * BSR_GetHealingBoostPerLevel())
        + BSR_GetSupplyTrainingBonus(user);

    if (bonusMultiplier > 0.0)
    {
        float baseAmount = g_PillsHealthValue != null ? g_PillsHealthValue.FloatValue : 50.0;
        int bonus = RoundToNearest(baseAmount * bonusMultiplier);
        BSR_AddTemporaryHealth(subject, bonus);
    }

    BSR_RecordSupplyUse(user);
}

public void Event_AdrenalineUsed(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (!BSR_IsHumanSurvivor(client))
    {
        return;
    }

    int level = BSR_GetPerkLevel(client, Perk_HealingBoost);
    float bonusMultiplier = (float(level) * BSR_GetHealingBoostPerLevel())
        + BSR_GetSupplyTrainingBonus(client);

    if (bonusMultiplier > 0.0)
    {
        float baseAmount = g_AdrenalineHealthValue != null ? g_AdrenalineHealthValue.FloatValue : 25.0;
        int bonus = RoundToNearest(baseAmount * bonusMultiplier);
        BSR_AddTemporaryHealth(client, bonus);
    }

    BSR_RecordSupplyUse(client);
}

public void Event_WeaponFire(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client))
    {
        return;
    }

    char weaponName[32];
    event.GetString("weapon", weaponName, sizeof(weaponName));

    if (BSR_IsConsumableWeaponName(weaponName))
    {
        BSR_RecordSupplyUse(client);
    }

    if (!StrEqual(weaponName, "melee", false))
    {
        return;
    }

    RequestFrame(BSR_Frame_AdjustMeleeAttack, GetClientUserId(client));
}

public void BSR_Frame_AdjustMeleeAttack(any userId)
{
    int client = GetClientOfUserId(userId);
    if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client))
    {
        return;
    }

    int level = BSR_GetPerkLevel(client, Perk_MeleeSpeed);
    float speedBonus = (float(level) * BSR_GetMeleeSpeedPerLevel())
        + BSR_GetSupplyTrainingBonus(client);
    int weapon = GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon");

    if (speedBonus <= 0.0 || weapon <= MaxClients || !IsValidEntity(weapon))
    {
        return;
    }

    char classname[32];
    GetEntityClassname(weapon, classname, sizeof(classname));

    if (!StrEqual(classname, "weapon_melee", false)
        || !HasEntProp(weapon, Prop_Send, "m_flNextPrimaryAttack"))
    {
        return;
    }

    float gameTime = GetGameTime();
    float nextAttack = GetEntPropFloat(weapon, Prop_Send, "m_flNextPrimaryAttack");
    float remaining = nextAttack - gameTime;

    if (remaining <= 0.0)
    {
        return;
    }

    float speedMultiplier = 1.0 + speedBonus;
    SetEntPropFloat(
        weapon,
        Prop_Send,
        "m_flNextPrimaryAttack",
        gameTime + (remaining / speedMultiplier)
    );
}
