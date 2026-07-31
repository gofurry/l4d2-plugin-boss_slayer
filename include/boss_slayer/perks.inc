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

void BSR_GetPerkName(PerkType perk, char[] buffer, int maxLength)
{
    switch (perk)
    {
        case Perk_Firepower:       strcopy(buffer, maxLength, "Firepower");
        case Perk_Toughness:       strcopy(buffer, maxLength, "Toughness");
        case Perk_BossHealing:     strcopy(buffer, maxLength, "Boss Healing");
        case Perk_MeleeSpeed:      strcopy(buffer, maxLength, "Melee Fury");
        case Perk_HealingBoost:    strcopy(buffer, maxLength, "Healing Boost");
        case Perk_MedicalSupply:   strcopy(buffer, maxLength, "Medical Supply");
        case Perk_ThrowableSupply: strcopy(buffer, maxLength, "Throwable Supply");
        case Perk_FateReroll:      strcopy(buffer, maxLength, "Fate Reroll");
        case Perk_FieldRescue:     strcopy(buffer, maxLength, "Field Rescue");
        case Perk_AmmoReclamation: strcopy(buffer, maxLength, "Ammo Reclamation");
        case Perk_FirstAidFeedback:strcopy(buffer, maxLength, "First Aid Feedback");
        case Perk_PrecisionHunter: strcopy(buffer, maxLength, "Sniper Rebate");
        case Perk_SupplyTraining:  strcopy(buffer, maxLength, "Supply Conditioning");
        case Perk_LowHealthRecovery:strcopy(buffer, maxLength, "Survival Rhythm");
        default:                   strcopy(buffer, maxLength, "Unknown Perk");
    }
}

void BSR_GetPerkDescription(PerkType perk, char[] buffer, int maxLength)
{
    switch (perk)
    {
        case Perk_Firepower:
            strcopy(buffer, maxLength, "Direct damage +10% per level; friendly fire is excluded.");
        case Perk_Toughness:
            strcopy(buffer, maxLength, "Combat, fire, acid and friendly-fire damage -6% per level.");
        case Perk_BossHealing:
            strcopy(buffer, maxLength, "Boss kill restores 10 real and 10 temporary health per level.");
        case Perk_MeleeSpeed:
            strcopy(buffer, maxLength, "Melee attack speed +8% per level.");
        case Perk_HealingBoost:
            strcopy(buffer, maxLength, "Medkit, pills and adrenaline healing +6% per level.");
        case Perk_MedicalSupply:
            strcopy(buffer, maxLength, "After 180 seconds without medicine, receive one random medical item.");
        case Perk_ThrowableSupply:
            strcopy(buffer, maxLength, "After 180 seconds without a throwable, receive one at random.");
        case Perk_FateReroll:
            strcopy(buffer, maxLength, "Reroll each perk choice once without spending the reward.");
        case Perk_FieldRescue:
            strcopy(buffer, maxLength, "Revive and ledge-rescue speed +8% per level.");
        case Perk_AmmoReclamation:
            strcopy(buffer, maxLength, "Special-infected kills restore 2% max reserve ammo per level.");
        case Perk_FirstAidFeedback:
            strcopy(buffer, maxLength, "Healing a teammate with a medkit grants 5 temporary health per level.");
        case Perk_PrecisionHunter:
            strcopy(buffer, maxLength, "Sniper kills on special infected return 1 round per level to the magazine.");
        case Perk_SupplyTraining:
            strcopy(buffer, maxLength, "First supply use each chapter adds 2% to five stats, up to 10%.");
        case Perk_LowHealthRecovery:
            strcopy(buffer, maxLength, "Below 40 effective health, gain 10 temporary health per level every 30s.");
        default:
            strcopy(buffer, maxLength, "No description.");
    }
}

void BSR_GetPerkShortEffect(PerkType perk, char[] buffer, int maxLength)
{
    switch (perk)
    {
        case Perk_Firepower:
            strcopy(buffer, maxLength, "+10% direct damage/level");
        case Perk_Toughness:
            strcopy(buffer, maxLength, "-6% combat damage/level");
        case Perk_BossHealing:
            strcopy(buffer, maxLength, "+10 real/+10 temp per Boss level");
        case Perk_MeleeSpeed:
            strcopy(buffer, maxLength, "+8% melee speed/level");
        case Perk_HealingBoost:
            strcopy(buffer, maxLength, "+6% healing/level");
        case Perk_MedicalSupply:
            strcopy(buffer, maxLength, "random medicine every 180s while empty");
        case Perk_ThrowableSupply:
            strcopy(buffer, maxLength, "random throwable every 180s while empty");
        case Perk_FateReroll:
            strcopy(buffer, maxLength, "one free reroll per reward");
        case Perk_FieldRescue:
            strcopy(buffer, maxLength, "+8% rescue speed/level");
        case Perk_AmmoReclamation:
            strcopy(buffer, maxLength, "+2% max reserve per SI kill/level");
        case Perk_FirstAidFeedback:
            strcopy(buffer, maxLength, "+5 temp HP after healing a teammate/level");
        case Perk_PrecisionHunter:
            strcopy(buffer, maxLength, "+1 sniper round per SI kill/level");
        case Perk_SupplyTraining:
            strcopy(buffer, maxLength, "+2% five-stat stack once/chapter, max 10%");
        case Perk_LowHealthRecovery:
            strcopy(buffer, maxLength, "+10 temp HP/level every 30s below 40 HP");
        default:
            strcopy(buffer, maxLength, "Unknown effect");
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
