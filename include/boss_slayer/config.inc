#if defined _boss_slayer_config_included
    #endinput
#endif
#define _boss_slayer_config_included

ConVar g_CvarChatLevel;
ConVar g_CvarDebugCommands;
ConVar g_CvarBossKillerRewards;
ConVar g_CvarBossParticipantRewards;
ConVar g_CvarSpecialKillsPerReward;
ConVar g_CvarSpecialKillRewards;
ConVar g_CvarCommonKillsPerReward;
ConVar g_CvarCommonKillRewards;
ConVar g_CvarChapterRewards;
ConVar g_CvarSupplyInterval;
ConVar g_CvarFirepowerPerLevel;
ConVar g_CvarToughnessPerLevel;
ConVar g_CvarBossHealRealPerLevel;
ConVar g_CvarBossHealTempPerLevel;
ConVar g_CvarMeleeSpeedPerLevel;
ConVar g_CvarHealingBoostPerLevel;
ConVar g_CvarFieldRescuePerLevel;
ConVar g_CvarAmmoReclamationPerLevel;
ConVar g_CvarFirstAidFeedbackPerLevel;
ConVar g_CvarPrecisionHunterRoundsPerLevel;
ConVar g_CvarSupplyTrainingBonusPerStack;
ConVar g_CvarSupplyTrainingMaxStacks;
ConVar g_CvarLowHealthThreshold;
ConVar g_CvarLowHealthInterval;
ConVar g_CvarLowHealthTempPerLevel;
ConVar g_CvarExchangeRealHealth;
ConVar g_CvarExchangeTempHealth;

void BSR_InitializeConfig()
{
    CreateConVar(
        "sm_bsr_version",
        BSR_VERSION,
        "Boss Slayer Roguelite version.",
        FCVAR_NOTIFY | FCVAR_DONTRECORD
    );

    g_CvarChatLevel = CreateConVar(
        "sm_bsr_chat_level",
        "1",
        "Chat detail: 0=minimal, 1=normal, 2=detailed.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        2.0
    );
    g_CvarDebugCommands = CreateConVar(
        "sm_bsr_debug_commands",
        "0",
        "Enable ADMFLAG_CHEATS Boss Slayer development commands.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        1.0
    );
    g_CvarBossKillerRewards = CreateConVar(
        "sm_bsr_boss_killer_rewards",
        "2",
        "Perk choices awarded to a living human Boss killer.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        10.0
    );
    g_CvarBossParticipantRewards = CreateConVar(
        "sm_bsr_boss_participant_rewards",
        "1",
        "Perk choices awarded to another living human Boss participant.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        10.0
    );
    g_CvarSpecialKillsPerReward = CreateConVar(
        "sm_bsr_si_kills_per_reward",
        "30",
        "Normal special-infected kills required for one perk choice.",
        FCVAR_NONE,
        true,
        1.0,
        true,
        10000.0
    );
    g_CvarSpecialKillRewards = CreateConVar(
        "sm_bsr_si_kill_rewards",
        "1",
        "Perk choices awarded whenever the special-infected kill threshold is reached.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        10.0
    );
    g_CvarCommonKillsPerReward = CreateConVar(
        "sm_bsr_common_kills_per_reward",
        "500",
        "Common-infected kills required for one perk choice.",
        FCVAR_NONE,
        true,
        1.0,
        true,
        100000.0
    );
    g_CvarCommonKillRewards = CreateConVar(
        "sm_bsr_common_kill_rewards",
        "1",
        "Perk choices awarded whenever the common-infected kill threshold is reached.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        10.0
    );
    g_CvarChapterRewards = CreateConVar(
        "sm_bsr_chapter_rewards",
        "1",
        "Perk choices awarded on entry to each chapter after the first.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        10.0
    );
    g_CvarSupplyInterval = CreateConVar(
        "sm_bsr_supply_interval",
        "180.0",
        "Continuous empty-slot seconds before a supply perk grants an item.",
        FCVAR_NONE,
        true,
        1.0,
        true,
        3600.0
    );
    g_CvarFirepowerPerLevel = CreateConVar(
        "sm_bsr_firepower_per_level",
        "0.10",
        "Additive direct-damage bonus per Firepower level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        1.0
    );
    g_CvarToughnessPerLevel = CreateConVar(
        "sm_bsr_toughness_per_level",
        "0.06",
        "Combat damage reduction per Toughness level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        0.20
    );
    g_CvarBossHealRealPerLevel = CreateConVar(
        "sm_bsr_boss_heal_real_per_level",
        "10",
        "Real health restored per Boss Healing level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        100.0
    );
    g_CvarBossHealTempPerLevel = CreateConVar(
        "sm_bsr_boss_heal_temp_per_level",
        "10",
        "Temporary health restored per Boss Healing level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        100.0
    );
    g_CvarMeleeSpeedPerLevel = CreateConVar(
        "sm_bsr_melee_speed_per_level",
        "0.08",
        "Melee attack-speed bonus per Melee Fury level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        1.0
    );
    g_CvarHealingBoostPerLevel = CreateConVar(
        "sm_bsr_healing_boost_per_level",
        "0.06",
        "Healing amount bonus per Healing Boost level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        1.0
    );
    g_CvarFieldRescuePerLevel = CreateConVar(
        "sm_bsr_field_rescue_per_level",
        "0.08",
        "Revive and ledge-rescue speed bonus per Field Rescue level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        1.0
    );
    g_CvarAmmoReclamationPerLevel = CreateConVar(
        "sm_bsr_ammo_reclamation_per_level",
        "0.02",
        "Maximum reserve-ammo fraction restored per Ammo Reclamation level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        1.0
    );
    g_CvarFirstAidFeedbackPerLevel = CreateConVar(
        "sm_bsr_first_aid_feedback_per_level",
        "5",
        "Temporary health granted per First Aid Feedback level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        100.0
    );
    g_CvarPrecisionHunterRoundsPerLevel = CreateConVar(
        "sm_bsr_precision_hunter_rounds_per_level",
        "1",
        "Magazine rounds returned per Precision Hunter level and valid primary-weapon kill.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        30.0
    );
    g_CvarSupplyTrainingBonusPerStack = CreateConVar(
        "sm_bsr_supply_training_bonus_per_stack",
        "0.02",
        "Five-stat additive bonus per Supply Conditioning stack.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        0.20
    );
    g_CvarSupplyTrainingMaxStacks = CreateConVar(
        "sm_bsr_supply_training_max_stacks",
        "5",
        "Maximum Supply Conditioning stacks.",
        FCVAR_NONE,
        true,
        1.0,
        true,
        20.0
    );
    g_CvarLowHealthThreshold = CreateConVar(
        "sm_bsr_low_health_threshold",
        "40.0",
        "Effective-health threshold for Survival Rhythm.",
        FCVAR_NONE,
        true,
        1.0,
        true,
        100.0
    );
    g_CvarLowHealthInterval = CreateConVar(
        "sm_bsr_low_health_interval",
        "30.0",
        "Seconds between Survival Rhythm healing ticks.",
        FCVAR_NONE,
        true,
        1.0,
        true,
        600.0
    );
    g_CvarLowHealthTempPerLevel = CreateConVar(
        "sm_bsr_low_health_temp_per_level",
        "10",
        "Temporary health granted per Survival Rhythm level.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        100.0
    );
    g_CvarExchangeRealHealth = CreateConVar(
        "sm_bsr_exchange_real_health",
        "20",
        "Real health granted by the full-build health exchange.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        100.0
    );
    g_CvarExchangeTempHealth = CreateConVar(
        "sm_bsr_exchange_temp_health",
        "20",
        "Temporary health granted by the full-build health exchange.",
        FCVAR_NONE,
        true,
        0.0,
        true,
        100.0
    );

    AutoExecConfig(true, "boss_slayer");
}

int BSR_GetChatLevel()
{
    return g_CvarChatLevel.IntValue;
}

bool BSR_DebugCommandsEnabled()
{
    return g_CvarDebugCommands.BoolValue;
}

int BSR_GetBossKillerRewards()
{
    return g_CvarBossKillerRewards.IntValue;
}

int BSR_GetBossParticipantRewards()
{
    return g_CvarBossParticipantRewards.IntValue;
}

int BSR_GetSpecialKillsPerReward()
{
    return g_CvarSpecialKillsPerReward.IntValue;
}

int BSR_GetSpecialKillRewards()
{
    return g_CvarSpecialKillRewards.IntValue;
}

int BSR_GetCommonKillsPerReward()
{
    return g_CvarCommonKillsPerReward.IntValue;
}

int BSR_GetCommonKillRewards()
{
    return g_CvarCommonKillRewards.IntValue;
}

int BSR_GetChapterRewards()
{
    return g_CvarChapterRewards.IntValue;
}

float BSR_GetSupplyInterval()
{
    return g_CvarSupplyInterval.FloatValue;
}

float BSR_GetFirepowerPerLevel()
{
    return g_CvarFirepowerPerLevel.FloatValue;
}

float BSR_GetToughnessPerLevel()
{
    return g_CvarToughnessPerLevel.FloatValue;
}

int BSR_GetBossHealRealPerLevel()
{
    return g_CvarBossHealRealPerLevel.IntValue;
}

int BSR_GetBossHealTempPerLevel()
{
    return g_CvarBossHealTempPerLevel.IntValue;
}

float BSR_GetMeleeSpeedPerLevel()
{
    return g_CvarMeleeSpeedPerLevel.FloatValue;
}

float BSR_GetHealingBoostPerLevel()
{
    return g_CvarHealingBoostPerLevel.FloatValue;
}

float BSR_GetFieldRescuePerLevel()
{
    return g_CvarFieldRescuePerLevel.FloatValue;
}

float BSR_GetAmmoReclamationPerLevel()
{
    return g_CvarAmmoReclamationPerLevel.FloatValue;
}

int BSR_GetFirstAidFeedbackPerLevel()
{
    return g_CvarFirstAidFeedbackPerLevel.IntValue;
}

int BSR_GetPrecisionHunterRoundsPerLevel()
{
    return g_CvarPrecisionHunterRoundsPerLevel.IntValue;
}

float BSR_GetSupplyTrainingBonusPerStack()
{
    return g_CvarSupplyTrainingBonusPerStack.FloatValue;
}

int BSR_GetSupplyTrainingMaxStacks()
{
    return g_CvarSupplyTrainingMaxStacks.IntValue;
}

float BSR_GetLowHealthThreshold()
{
    return g_CvarLowHealthThreshold.FloatValue;
}

float BSR_GetLowHealthInterval()
{
    return g_CvarLowHealthInterval.FloatValue;
}

int BSR_GetLowHealthTempPerLevel()
{
    return g_CvarLowHealthTempPerLevel.IntValue;
}

int BSR_GetExchangeRealHealth()
{
    return g_CvarExchangeRealHealth.IntValue;
}

int BSR_GetExchangeTempHealth()
{
    return g_CvarExchangeTempHealth.IntValue;
}

int BSR_Percent(float value)
{
    return RoundToNearest(value * 100.0);
}
