#if defined _boss_slayer_supplies_included
    #endinput
#endif
#define _boss_slayer_supplies_included

void BSR_StartSupplyTimer()
{
    if (g_SupplyTimer != null)
    {
        delete g_SupplyTimer;
    }

    g_SupplyTimer = CreateTimer(1.0, Timer_SupplyTick, 0, TIMER_REPEAT);
}

public Action Timer_SupplyTick(Handle timer, any data)
{
    float now = GetEngineTime();

    for (int client = 1; client <= MaxClients; client++)
    {
        if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client))
        {
            BSR_ResetRuntimeEffectState(client);
            continue;
        }

        BSR_UpdateMedicalSupply(client, now);
        BSR_UpdateThrowableSupply(client, now);
        BSR_UpdateLowHealthRecovery(client, now);
    }

    return Plugin_Continue;
}

void BSR_UpdateLowHealthRecovery(int client, float now)
{
    int level = BSR_GetPerkLevel(client, Perk_LowHealthRecovery);
    float effectiveHealth = float(GetClientHealth(client)) + BSR_GetTemporaryHealth(client);

    if (level <= 0 || !BSR_CanReceiveHealth(client) || effectiveHealth >= LOW_HEALTH_THRESHOLD)
    {
        g_LowHealthNextTick[client] = 0.0;
        return;
    }

    if (g_LowHealthNextTick[client] <= 0.0)
    {
        g_LowHealthNextTick[client] = now + LOW_HEALTH_INTERVAL_SECONDS;
        return;
    }

    if (now < g_LowHealthNextTick[client])
    {
        return;
    }

    g_LowHealthNextTick[client] = now + LOW_HEALTH_INTERVAL_SECONDS;
    int amount = level * LOW_HEALTH_TEMP_PER_LEVEL;

    if (BSR_AddTemporaryHealth(client, amount) > 0)
    {
        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 Survival Rhythm restored up to %d temporary health.",
            amount
        );
    }
}

void BSR_UpdateMedicalSupply(int client, float now)
{
    if (BSR_GetPerkLevel(client, Perk_MedicalSupply) <= 0 || BSR_HasAnyMedicalItem(client))
    {
        g_MedicalEmptySince[client] = 0.0;
        return;
    }

    if (g_MedicalEmptySince[client] <= 0.0)
    {
        g_MedicalEmptySince[client] = now;
        return;
    }

    if ((now - g_MedicalEmptySince[client]) < SUPPLY_INTERVAL_SECONDS)
    {
        return;
    }

    g_MedicalEmptySince[client] = now;

    if (BSR_GiveRandomMedicalItem(client))
    {
        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 Medical Supply granted a random medical item."
        );
    }
}

void BSR_UpdateThrowableSupply(int client, float now)
{
    if (BSR_GetPerkLevel(client, Perk_ThrowableSupply) <= 0 || BSR_HasThrowable(client))
    {
        g_ThrowableEmptySince[client] = 0.0;
        return;
    }

    if (g_ThrowableEmptySince[client] <= 0.0)
    {
        g_ThrowableEmptySince[client] = now;
        return;
    }

    if ((now - g_ThrowableEmptySince[client]) < SUPPLY_INTERVAL_SECONDS)
    {
        return;
    }

    g_ThrowableEmptySince[client] = now;

    if (BSR_GiveRandomThrowable(client))
    {
        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 Throwable Supply granted a random throwable."
        );
    }
}

bool BSR_HasAnyMedicalItem(int client)
{
    return BSR_HasItemInSlot(client, 3) || BSR_HasItemInSlot(client, 4);
}

bool BSR_HasThrowable(int client)
{
    return BSR_HasItemInSlot(client, 2);
}

bool BSR_HasItemInSlot(int client, int slot)
{
    int entity = GetPlayerWeaponSlot(client, slot);
    return entity > MaxClients && IsValidEntity(entity);
}

bool BSR_GiveRandomMedicalItem(int client)
{
    if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client) || BSR_HasAnyMedicalItem(client))
    {
        return false;
    }

    int selection = GetRandomInt(0, 2);
    int entity = -1;

    switch (selection)
    {
        case 0: entity = GivePlayerItem(client, "weapon_first_aid_kit");
        case 1: entity = GivePlayerItem(client, "weapon_pain_pills");
        case 2: entity = GivePlayerItem(client, "weapon_adrenaline");
    }

    return entity > MaxClients && IsValidEntity(entity);
}

bool BSR_GiveRandomThrowable(int client)
{
    if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client) || BSR_HasThrowable(client))
    {
        return false;
    }

    int selection = GetRandomInt(0, 2);
    int entity = -1;

    switch (selection)
    {
        case 0: entity = GivePlayerItem(client, "weapon_molotov");
        case 1: entity = GivePlayerItem(client, "weapon_pipe_bomb");
        case 2: entity = GivePlayerItem(client, "weapon_vomitjar");
    }

    return entity > MaxClients && IsValidEntity(entity);
}

bool BSR_IsConsumableWeaponName(const char[] weaponName)
{
    return StrEqual(weaponName, "molotov", false)
        || StrEqual(weaponName, "weapon_molotov", false)
        || StrEqual(weaponName, "pipe_bomb", false)
        || StrEqual(weaponName, "weapon_pipe_bomb", false)
        || StrEqual(weaponName, "vomitjar", false)
        || StrEqual(weaponName, "weapon_vomitjar", false);
}

void BSR_RecordSupplyUse(int client)
{
    if (
        BSR_GetPerkLevel(client, Perk_SupplyTraining) <= 0
        || g_CurrentChapterSerial <= 0
    )
    {
        return;
    }

    int stacks = BSR_GetSupplyTrainingStacks(client);
    if (stacks >= SUPPLY_TRAINING_MAX_STACKS)
    {
        return;
    }

    char steamId[MAX_AUTHID_LENGTH];
    if (!BSR_TryGetClientSteamId(client, steamId, sizeof(steamId)))
    {
        return;
    }

    int lastChapter = 0;
    g_SupplyTrainingChapterBySteamId.GetValue(steamId, lastChapter);

    if (lastChapter == g_CurrentChapterSerial)
    {
        return;
    }

    if (!BSR_SetSupplyTrainingStacks(client, stacks + 1))
    {
        return;
    }

    g_SupplyTrainingChapterBySteamId.SetValue(steamId, g_CurrentChapterSerial);

    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 Supply Conditioning: %d/%d stacks, five stats +%d%%.",
        stacks + 1,
        SUPPLY_TRAINING_MAX_STACKS,
        (stacks + 1) * 2
    );
}

int BSR_GetWeaponReserveMaximum(int weapon)
{
    if (weapon <= MaxClients || !IsValidEntity(weapon))
    {
        return 0;
    }

    char classname[40];
    char cvarName[40];
    GetEntityClassname(weapon, classname, sizeof(classname));
    cvarName[0] = 0;

    if (
        StrEqual(classname, "weapon_smg")
        || StrEqual(classname, "weapon_smg_silenced")
        || StrEqual(classname, "weapon_smg_mp5")
    )
    {
        strcopy(cvarName, sizeof(cvarName), "ammo_smg_max");
    }
    else if (StrEqual(classname, "weapon_pumpshotgun") || StrEqual(classname, "weapon_shotgun_chrome"))
    {
        strcopy(cvarName, sizeof(cvarName), "ammo_shotgun_max");
    }
    else if (StrEqual(classname, "weapon_autoshotgun") || StrEqual(classname, "weapon_shotgun_spas"))
    {
        strcopy(cvarName, sizeof(cvarName), "ammo_autoshotgun_max");
    }
    else if (
        StrEqual(classname, "weapon_rifle")
        || StrEqual(classname, "weapon_rifle_ak47")
        || StrEqual(classname, "weapon_rifle_desert")
        || StrEqual(classname, "weapon_rifle_sg552")
    )
    {
        strcopy(cvarName, sizeof(cvarName), "ammo_rifle_max");
    }
    else if (StrEqual(classname, "weapon_hunting_rifle"))
    {
        strcopy(cvarName, sizeof(cvarName), "ammo_huntingrifle_max");
    }
    else if (
        StrEqual(classname, "weapon_sniper_military")
        || StrEqual(classname, "weapon_sniper_scout")
        || StrEqual(classname, "weapon_sniper_awp")
    )
    {
        strcopy(cvarName, sizeof(cvarName), "ammo_sniperrifle_max");
    }
    else if (StrEqual(classname, "weapon_grenade_launcher"))
    {
        strcopy(cvarName, sizeof(cvarName), "ammo_grenadelauncher_max");
    }

    if (cvarName[0] == 0)
    {
        return 0;
    }

    ConVar maximum = FindConVar(cvarName);
    return maximum != null ? maximum.IntValue : 0;
}

void BSR_ApplyAmmoReclamation(int client)
{
    int level = BSR_GetPerkLevel(client, Perk_AmmoReclamation);
    if (level <= 0)
    {
        return;
    }

    int weapon = GetPlayerWeaponSlot(client, 0);
    int maximum = BSR_GetWeaponReserveMaximum(weapon);

    if (
        maximum <= 0
        || weapon <= MaxClients
        || !IsValidEntity(weapon)
        || !HasEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType")
    )
    {
        return;
    }

    int ammoType = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType");
    int amount = RoundToCeil(float(maximum) * float(level) * AMMO_RECLAIM_PERCENT_PER_LEVEL);

    if (ammoType >= 0 && amount > 0)
    {
        GivePlayerAmmo(client, amount, ammoType, true);
    }
}

bool BSR_IsSniperClassname(const char[] classname)
{
    return StrEqual(classname, "weapon_hunting_rifle", false)
        || StrEqual(classname, "weapon_sniper_military", false)
        || StrEqual(classname, "weapon_sniper_scout", false)
        || StrEqual(classname, "weapon_sniper_awp", false);
}

bool BSR_IsSniperKillWeapon(const char[] weaponName)
{
    return StrEqual(weaponName, "hunting_rifle", false)
        || StrEqual(weaponName, "weapon_hunting_rifle", false)
        || StrEqual(weaponName, "sniper_military", false)
        || StrEqual(weaponName, "weapon_sniper_military", false)
        || StrEqual(weaponName, "sniper_scout", false)
        || StrEqual(weaponName, "weapon_sniper_scout", false)
        || StrEqual(weaponName, "sniper_awp", false)
        || StrEqual(weaponName, "weapon_sniper_awp", false);
}

int BSR_GetSniperClipMaximum(const char[] classname)
{
    if (StrEqual(classname, "weapon_hunting_rifle", false))
    {
        return 15;
    }

    if (StrEqual(classname, "weapon_sniper_military", false))
    {
        return 30;
    }

    if (StrEqual(classname, "weapon_sniper_scout", false))
    {
        return 15;
    }

    if (StrEqual(classname, "weapon_sniper_awp", false))
    {
        return 20;
    }

    return 0;
}

void BSR_ApplyPrecisionHunter(int client, const char[] killWeapon)
{
    int level = BSR_GetPerkLevel(client, Perk_PrecisionHunter);
    if (level <= 0 || !BSR_IsSniperKillWeapon(killWeapon))
    {
        return;
    }

    int weapon = GetEntPropEnt(client, Prop_Send, "m_hActiveWeapon");
    if (
        weapon <= MaxClients
        || !IsValidEntity(weapon)
        || !HasEntProp(weapon, Prop_Send, "m_iClip1")
    )
    {
        return;
    }

    char classname[40];
    GetEntityClassname(weapon, classname, sizeof(classname));

    if (!BSR_IsSniperClassname(classname))
    {
        return;
    }

    int maximum = BSR_GetSniperClipMaximum(classname);
    int current = GetEntProp(weapon, Prop_Send, "m_iClip1");
    int updated = current + (level * PRECISION_HUNTER_ROUNDS_PER_LEVEL);

    if (updated > maximum)
    {
        updated = maximum;
    }

    if (updated > current)
    {
        SetEntProp(weapon, Prop_Send, "m_iClip1", updated);
    }
}

bool BSR_RefillPrimaryReserveAmmo(int client)
{
    if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client))
    {
        return false;
    }

    int weapon = GetPlayerWeaponSlot(client, 0);
    if (
        weapon <= MaxClients
        || !IsValidEntity(weapon)
        || !HasEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType")
    )
    {
        return false;
    }

    int ammoType = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType");
    if (ammoType < 0)
    {
        return false;
    }

    return GivePlayerAmmo(client, 9999, ammoType, true) > 0;
}
