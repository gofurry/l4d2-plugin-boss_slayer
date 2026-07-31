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

    if (
        level <= 0
        || !BSR_CanReceiveHealth(client)
        || effectiveHealth >= BSR_GetLowHealthThreshold()
    )
    {
        g_LowHealthNextTick[client] = 0.0;
        return;
    }

    if (g_LowHealthNextTick[client] <= 0.0)
    {
        g_LowHealthNextTick[client] = now + BSR_GetLowHealthInterval();
        return;
    }

    if (now < g_LowHealthNextTick[client])
    {
        return;
    }

    g_LowHealthNextTick[client] = now + BSR_GetLowHealthInterval();
    int amount = level * BSR_GetLowHealthTempPerLevel();

    if (BSR_AddTemporaryHealth(client, amount) > 0)
    {
        BSR_PrintChat(
            client,
            BSR_Message_Detailed,
            "%t",
            "Survival Rhythm Triggered",
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

    if ((now - g_MedicalEmptySince[client]) < BSR_GetSupplyInterval())
    {
        return;
    }

    g_MedicalEmptySince[client] = now;

    if (BSR_GiveRandomMedicalItem(client))
    {
        BSR_PrintChat(
            client,
            BSR_Message_Normal,
            "%t",
            "Medical Supply Granted"
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

    if ((now - g_ThrowableEmptySince[client]) < BSR_GetSupplyInterval())
    {
        return;
    }

    g_ThrowableEmptySince[client] = now;

    if (BSR_GiveRandomThrowable(client))
    {
        BSR_PrintChat(
            client,
            BSR_Message_Normal,
            "%t",
            "Throwable Supply Granted"
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
    int maxStacks = BSR_GetSupplyTrainingMaxStacks();
    if (stacks >= maxStacks)
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

    BSR_PrintChat(
        client,
        BSR_Message_Normal,
        "%t",
        "Supply Conditioning Stack",
        stacks + 1,
        maxStacks,
        BSR_Percent(float(stacks + 1) * BSR_GetSupplyTrainingBonusPerStack())
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
    int amount = RoundToCeil(
        float(maximum) * float(level) * BSR_GetAmmoReclamationPerLevel()
    );

    if (ammoType >= 0 && amount > 0)
    {
        GivePlayerAmmo(client, amount, ammoType, true);
    }
}

int BSR_GetPrimaryClipMaximum(const char[] classname)
{
    if (
        StrEqual(classname, "weapon_smg", false)
        || StrEqual(classname, "weapon_smg_silenced", false)
        || StrEqual(classname, "weapon_smg_mp5", false)
    )
    {
        return 50;
    }

    if (
        StrEqual(classname, "weapon_pumpshotgun", false)
        || StrEqual(classname, "weapon_shotgun_chrome", false)
    )
    {
        return 8;
    }

    if (
        StrEqual(classname, "weapon_autoshotgun", false)
        || StrEqual(classname, "weapon_shotgun_spas", false)
    )
    {
        return 10;
    }

    if (
        StrEqual(classname, "weapon_rifle", false)
        || StrEqual(classname, "weapon_rifle_sg552", false)
    )
    {
        return 50;
    }

    if (StrEqual(classname, "weapon_rifle_ak47", false))
    {
        return 40;
    }

    if (StrEqual(classname, "weapon_rifle_desert", false))
    {
        return 60;
    }

    if (StrEqual(classname, "weapon_rifle_m60", false))
    {
        return 150;
    }

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

bool BSR_KillWeaponMatchesPrimary(
    const char[] killWeapon,
    const char[] primaryClassname
)
{
    char normalizedKillWeapon[40];

    if (StrContains(killWeapon, "weapon_", false) == 0)
    {
        strcopy(
            normalizedKillWeapon,
            sizeof(normalizedKillWeapon),
            killWeapon
        );
    }
    else
    {
        FormatEx(
            normalizedKillWeapon,
            sizeof(normalizedKillWeapon),
            "weapon_%s",
            killWeapon
        );
    }

    return StrEqual(normalizedKillWeapon, primaryClassname, false);
}

void BSR_ApplyPrecisionHunter(int client, const char[] killWeapon)
{
    int level = BSR_GetPerkLevel(client, Perk_PrecisionHunter);
    if (level <= 0)
    {
        return;
    }

    int weapon = GetPlayerWeaponSlot(client, 0);
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

    int maximum = BSR_GetPrimaryClipMaximum(classname);
    if (
        maximum <= 0
        || !BSR_KillWeaponMatchesPrimary(killWeapon, classname)
    )
    {
        return;
    }

    int current = GetEntProp(weapon, Prop_Send, "m_iClip1");
    int updated = current + (level * BSR_GetPrecisionHunterRoundsPerLevel());

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
