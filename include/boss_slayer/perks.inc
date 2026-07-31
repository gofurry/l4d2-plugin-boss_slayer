#if defined _boss_slayer_perks_included
    #endinput
#endif
#define _boss_slayer_perks_included

int BSR_GetPerkMaxLevel(PerkType perk)
{
    switch (perk)
    {
        case Perk_Firepower:     return 5;
        case Perk_BossHunter:    return 4;
        case Perk_SpecialHunter: return 3;
        case Perk_Toughness:     return 4;
        case Perk_BossHealing:   return 3;
        case Perk_LastStand:     return 3;
    }

    return 0;
}

void BSR_GetPerkName(PerkType perk, char[] buffer, int maxLength)
{
    switch (perk)
    {
        case Perk_Firepower:     strcopy(buffer, maxLength, "Firepower");
        case Perk_BossHunter:    strcopy(buffer, maxLength, "Boss Hunter");
        case Perk_SpecialHunter: strcopy(buffer, maxLength, "Special Hunter");
        case Perk_Toughness:     strcopy(buffer, maxLength, "Toughness");
        case Perk_BossHealing:   strcopy(buffer, maxLength, "Boss Healing");
        case Perk_LastStand:     strcopy(buffer, maxLength, "Last Stand");
        default:                 strcopy(buffer, maxLength, "Unknown Perk");
    }
}

void BSR_GetPerkDescription(PerkType perk, char[] buffer, int maxLength)
{
    switch (perk)
    {
        case Perk_Firepower:
            strcopy(buffer, maxLength, "All direct damage +6% per level.");
        case Perk_BossHunter:
            strcopy(buffer, maxLength, "Damage to Tank and Witch +15% per level.");
        case Perk_SpecialHunter:
            strcopy(buffer, maxLength, "Damage to normal special infected +10% per level.");
        case Perk_Toughness:
            strcopy(buffer, maxLength, "Incoming damage -6% per level.");
        case Perk_BossHealing:
            strcopy(buffer, maxLength, "Boss kill healing +10 health per level.");
        case Perk_LastStand:
            strcopy(buffer, maxLength, "Below 40 health, extra damage reduction +8% per level.");
        default:
            strcopy(buffer, maxLength, "No description.");
    }
}

void BSR_GetPerkShortEffect(PerkType perk, char[] buffer, int maxLength)
{
    switch (perk)
    {
        case Perk_Firepower:
            strcopy(buffer, maxLength, "+6% all damage/level");
        case Perk_BossHunter:
            strcopy(buffer, maxLength, "+15% boss damage/level");
        case Perk_SpecialHunter:
            strcopy(buffer, maxLength, "+10% SI damage/level");
        case Perk_Toughness:
            strcopy(buffer, maxLength, "-6% damage taken/level");
        case Perk_BossHealing:
            strcopy(buffer, maxLength, "+10 boss-kill healing/level");
        case Perk_LastStand:
            strcopy(buffer, maxLength, "+8% low-health DR/level");
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

void BSR_ShufflePerkPool(int perks[Perk_Count], int count)
{
    // Fisher-Yates: the first three entries become three unique choices.
    for (int i = count - 1; i > 0; i--)
    {
        int randomIndex = GetRandomInt(0, i);
        int temp = perks[i];
        perks[i] = perks[randomIndex];
        perks[randomIndex] = temp;
    }
}
