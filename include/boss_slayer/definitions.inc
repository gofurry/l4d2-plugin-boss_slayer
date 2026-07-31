#if defined _boss_slayer_definitions_included
    #endinput
#endif
#define _boss_slayer_definitions_included

#define BSR_VERSION "0.4.2"

#define TEAM_SURVIVOR 2
#define TEAM_INFECTED 3

#define PERK_MENU_CHOICES 3
#define SPECIAL_KILLS_PER_REWARD 30
#define COMMON_KILLS_PER_REWARD 500
#define ZCLASS_TANK 8
#define MAX_TRACKED_ENTITIES 2049

enum PerkType
{
    Perk_Firepower = 0,
    Perk_BossHunter,
    Perk_SpecialHunter,
    Perk_Toughness,
    Perk_BossHealing,
    Perk_LastStand,

    Perk_Count
};
