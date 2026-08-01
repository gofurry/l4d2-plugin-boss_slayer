#if defined _boss_slayer_definitions_included
    #endinput
#endif
#define _boss_slayer_definitions_included

#define BSR_VERSION "1.0.1"

#define TEAM_SURVIVOR 2
#define TEAM_INFECTED 3

#define PERK_MENU_CHOICES 3
#define ZCLASS_TANK 8
#define MAX_TRACKED_ENTITIES 2049

enum BSRMessageLevel
{
    BSR_Message_Minimal = 0,
    BSR_Message_Normal,
    BSR_Message_Detailed
};

enum PerkType
{
    Perk_Firepower = 0,
    Perk_Toughness,
    Perk_BossHealing,
    Perk_MeleeSpeed,
    Perk_HealingBoost,
    Perk_MedicalSupply,
    Perk_ThrowableSupply,
    Perk_FateReroll,
    Perk_FieldRescue,
    Perk_AmmoReclamation,
    Perk_FirstAidFeedback,
    Perk_PrecisionHunter,
    Perk_SupplyTraining,
    Perk_LowHealthRecovery,

    Perk_Count
};

enum ExchangeType
{
    Exchange_Health = 0,
    Exchange_Ammo,
    Exchange_Medical,
    Exchange_Throwable
};
