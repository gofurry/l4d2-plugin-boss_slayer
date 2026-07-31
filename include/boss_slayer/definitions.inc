#if defined _boss_slayer_definitions_included
    #endinput
#endif
#define _boss_slayer_definitions_included

#define BSR_VERSION "0.6.1"

#define TEAM_SURVIVOR 2
#define TEAM_INFECTED 3

#define PERK_MENU_CHOICES 3
#define SPECIAL_KILLS_PER_REWARD 30
#define COMMON_KILLS_PER_REWARD 500
#define SUPPLY_INTERVAL_SECONDS 180.0
#define FIREPOWER_DAMAGE_PER_LEVEL 0.10
#define TOUGHNESS_REDUCTION_PER_LEVEL 0.06
#define BOSS_HEAL_REAL_PER_LEVEL 10
#define BOSS_HEAL_TEMP_PER_LEVEL 10
#define MELEE_SPEED_PER_LEVEL 0.08
#define HEALING_BOOST_PER_LEVEL 0.06
#define FIELD_RESCUE_SPEED_PER_LEVEL 0.08
#define AMMO_RECLAIM_PERCENT_PER_LEVEL 0.02
#define FIRST_AID_FEEDBACK_PER_LEVEL 5
#define PRECISION_HUNTER_ROUNDS_PER_LEVEL 1
#define SUPPLY_TRAINING_BONUS_PER_STACK 0.02
#define SUPPLY_TRAINING_MAX_STACKS 5
#define LOW_HEALTH_THRESHOLD 40.0
#define LOW_HEALTH_INTERVAL_SECONDS 30.0
#define LOW_HEALTH_TEMP_PER_LEVEL 10
#define EXCHANGE_REAL_HEALTH 20
#define EXCHANGE_TEMP_HEALTH 20
#define ZCLASS_TANK 8
#define MAX_TRACKED_ENTITIES 2049

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
