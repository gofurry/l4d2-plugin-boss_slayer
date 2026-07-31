#if defined _boss_slayer_perks_included
    #endinput
#endif
#define _boss_slayer_perks_included

int BSR_GetPerkMaxLevel(PerkType perk)
{
    switch (perk)
    {
        case Perk_Firepower:       return 5;
        case Perk_Toughness:       return 4;
        case Perk_BossHealing:     return 3;
        case Perk_MeleeSpeed:      return 4;
        case Perk_HealingBoost:    return 4;
        case Perk_MedicalSupply:   return 1;
        case Perk_ThrowableSupply: return 1;
        case Perk_FateReroll:      return 1;
        case Perk_FieldRescue:     return 4;
        case Perk_AmmoReclamation:return 3;
        case Perk_FirstAidFeedback:return 3;
        case Perk_PrecisionHunter: return 3;
        case Perk_SupplyTraining:  return 1;
        case Perk_LowHealthRecovery:return 2;
    }

    return 0;
}

void BSR_GetPerkName(int client, PerkType perk, char[] buffer, int maxLength)
{
    switch (perk)
    {
        case Perk_Firepower:
            FormatEx(buffer, maxLength, "%T", "Perk Firepower Name", client);
        case Perk_Toughness:
            FormatEx(buffer, maxLength, "%T", "Perk Toughness Name", client);
        case Perk_BossHealing:
            FormatEx(buffer, maxLength, "%T", "Perk Boss Healing Name", client);
        case Perk_MeleeSpeed:
            FormatEx(buffer, maxLength, "%T", "Perk Melee Fury Name", client);
        case Perk_HealingBoost:
            FormatEx(buffer, maxLength, "%T", "Perk Healing Boost Name", client);
        case Perk_MedicalSupply:
            FormatEx(buffer, maxLength, "%T", "Perk Medical Supply Name", client);
        case Perk_ThrowableSupply:
            FormatEx(buffer, maxLength, "%T", "Perk Throwable Supply Name", client);
        case Perk_FateReroll:
            FormatEx(buffer, maxLength, "%T", "Perk Fate Reroll Name", client);
        case Perk_FieldRescue:
            FormatEx(buffer, maxLength, "%T", "Perk Field Rescue Name", client);
        case Perk_AmmoReclamation:
            FormatEx(buffer, maxLength, "%T", "Perk Ammo Reclamation Name", client);
        case Perk_FirstAidFeedback:
            FormatEx(buffer, maxLength, "%T", "Perk First Aid Feedback Name", client);
        case Perk_PrecisionHunter:
            FormatEx(buffer, maxLength, "%T", "Perk Precision Hunter Name", client);
        case Perk_SupplyTraining:
            FormatEx(buffer, maxLength, "%T", "Perk Supply Conditioning Name", client);
        case Perk_LowHealthRecovery:
            FormatEx(buffer, maxLength, "%T", "Perk Survival Rhythm Name", client);
        default:
            FormatEx(buffer, maxLength, "%T", "Perk Unknown Name", client);
    }
}

void BSR_GetPerkDescription(int client, PerkType perk, char[] buffer, int maxLength)
{
    switch (perk)
    {
        case Perk_Firepower:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Firepower Description",
                client,
                BSR_Percent(BSR_GetFirepowerPerLevel())
            );
        case Perk_Toughness:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Toughness Description",
                client,
                BSR_Percent(BSR_GetToughnessPerLevel())
            );
        case Perk_BossHealing:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Boss Healing Description",
                client,
                BSR_GetBossHealRealPerLevel(),
                BSR_GetBossHealTempPerLevel()
            );
        case Perk_MeleeSpeed:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Melee Fury Description",
                client,
                BSR_Percent(BSR_GetMeleeSpeedPerLevel())
            );
        case Perk_HealingBoost:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Healing Boost Description",
                client,
                BSR_Percent(BSR_GetHealingBoostPerLevel())
            );
        case Perk_MedicalSupply:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Medical Supply Description",
                client,
                RoundToNearest(BSR_GetSupplyInterval())
            );
        case Perk_ThrowableSupply:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Throwable Supply Description",
                client,
                RoundToNearest(BSR_GetSupplyInterval())
            );
        case Perk_FateReroll:
            FormatEx(buffer, maxLength, "%T", "Perk Fate Reroll Description", client);
        case Perk_FieldRescue:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Field Rescue Description",
                client,
                BSR_Percent(BSR_GetFieldRescuePerLevel())
            );
        case Perk_AmmoReclamation:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Ammo Reclamation Description",
                client,
                BSR_Percent(BSR_GetAmmoReclamationPerLevel())
            );
        case Perk_FirstAidFeedback:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk First Aid Feedback Description",
                client,
                BSR_GetFirstAidFeedbackPerLevel()
            );
        case Perk_PrecisionHunter:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Precision Hunter Description",
                client,
                BSR_GetPrecisionHunterRoundsPerLevel()
            );
        case Perk_SupplyTraining:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Supply Conditioning Description",
                client,
                BSR_Percent(BSR_GetSupplyTrainingBonusPerStack()),
                BSR_Percent(
                    BSR_GetSupplyTrainingBonusPerStack()
                    * float(BSR_GetSupplyTrainingMaxStacks())
                )
            );
        case Perk_LowHealthRecovery:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Survival Rhythm Description",
                client,
                RoundToNearest(BSR_GetLowHealthThreshold()),
                BSR_GetLowHealthTempPerLevel(),
                RoundToNearest(BSR_GetLowHealthInterval())
            );
        default:
            FormatEx(buffer, maxLength, "%T", "Perk Unknown Description", client);
    }
}

void BSR_GetPerkShortEffect(int client, PerkType perk, char[] buffer, int maxLength)
{
    switch (perk)
    {
        case Perk_Firepower:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Firepower Short",
                client,
                BSR_Percent(BSR_GetFirepowerPerLevel())
            );
        case Perk_Toughness:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Toughness Short",
                client,
                BSR_Percent(BSR_GetToughnessPerLevel())
            );
        case Perk_BossHealing:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Boss Healing Short",
                client,
                BSR_GetBossHealRealPerLevel(),
                BSR_GetBossHealTempPerLevel()
            );
        case Perk_MeleeSpeed:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Melee Fury Short",
                client,
                BSR_Percent(BSR_GetMeleeSpeedPerLevel())
            );
        case Perk_HealingBoost:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Healing Boost Short",
                client,
                BSR_Percent(BSR_GetHealingBoostPerLevel())
            );
        case Perk_MedicalSupply:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Medical Supply Short",
                client,
                RoundToNearest(BSR_GetSupplyInterval())
            );
        case Perk_ThrowableSupply:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Throwable Supply Short",
                client,
                RoundToNearest(BSR_GetSupplyInterval())
            );
        case Perk_FateReroll:
            FormatEx(buffer, maxLength, "%T", "Perk Fate Reroll Short", client);
        case Perk_FieldRescue:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Field Rescue Short",
                client,
                BSR_Percent(BSR_GetFieldRescuePerLevel())
            );
        case Perk_AmmoReclamation:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Ammo Reclamation Short",
                client,
                BSR_Percent(BSR_GetAmmoReclamationPerLevel())
            );
        case Perk_FirstAidFeedback:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk First Aid Feedback Short",
                client,
                BSR_GetFirstAidFeedbackPerLevel()
            );
        case Perk_PrecisionHunter:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Precision Hunter Short",
                client,
                BSR_GetPrecisionHunterRoundsPerLevel()
            );
        case Perk_SupplyTraining:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Supply Conditioning Short",
                client,
                BSR_Percent(BSR_GetSupplyTrainingBonusPerStack()),
                BSR_Percent(
                    BSR_GetSupplyTrainingBonusPerStack()
                    * float(BSR_GetSupplyTrainingMaxStacks())
                )
            );
        case Perk_LowHealthRecovery:
            FormatEx(
                buffer,
                maxLength,
                "%T",
                "Perk Survival Rhythm Short",
                client,
                BSR_GetLowHealthTempPerLevel(),
                RoundToNearest(BSR_GetLowHealthInterval()),
                RoundToNearest(BSR_GetLowHealthThreshold())
            );
        default:
            FormatEx(buffer, maxLength, "%T", "Perk Unknown Short", client);
    }
}

int BSR_BuildAvailablePerkPool(int client, int perks[Perk_Count])
{
    int count = 0;

    for (int perkIndex = 0; perkIndex < view_as<int>(Perk_Count); perkIndex++)
    {
        PerkType perk = view_as<PerkType>(perkIndex);

        if (BSR_GetPerkLevel(client, perk) < BSR_GetPerkMaxLevel(perk))
        {
            perks[count] = perkIndex;
            count++;
        }
    }

    return count;
}

bool BSR_AreAllPerksMaxed(int client)
{
    for (int perkIndex = 0; perkIndex < view_as<int>(Perk_Count); perkIndex++)
    {
        PerkType perk = view_as<PerkType>(perkIndex);

        if (BSR_GetPerkLevel(client, perk) < BSR_GetPerkMaxLevel(perk))
        {
            return false;
        }
    }

    return true;
}

void BSR_ShufflePerkPool(int perks[Perk_Count], int count)
{
    for (int i = count - 1; i > 0; i--)
    {
        int randomIndex = GetRandomInt(0, i);
        int temp = perks[i];
        perks[i] = perks[randomIndex];
        perks[randomIndex] = temp;
    }
}
