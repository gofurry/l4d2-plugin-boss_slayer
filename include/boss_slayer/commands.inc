#if defined _boss_slayer_commands_included
    #endinput
#endif
#define _boss_slayer_commands_included

void BSR_RegisterCommands()
{
    RegConsoleCmd("sm_bsr", Command_Status, "Show Boss Slayer status.");
    RegConsoleCmd("sm_perk", Command_OpenPerkMenu, "Open an unspent perk choice.");
    RegConsoleCmd("sm_perks", Command_ShowPerks, "Show your current Boss Slayer build.");

    // Development commands. Remove or restrict these before public release.
    RegConsoleCmd("sm_bsr_testreward", Command_TestReward, "Give yourself one test reward.");
    RegConsoleCmd(
        "sm_bsr_testsikills",
        Command_TestSpecialKills,
        "Add test special-infected kills. Usage: !bsr_testsikills 30"
    );
    RegConsoleCmd(
        "sm_bsr_testcommonkills",
        Command_TestCommonKills,
        "Add test common-infected kills. Usage: !bsr_testcommonkills 500"
    );
    RegConsoleCmd("sm_bsr_resetme", Command_ResetMe, "Reset your Boss Slayer data.");
    RegAdminCmd("sm_bsr_resetall", Command_ResetAll, ADMFLAG_ROOT, "Reset all Boss Slayer data.");
}

public Action Command_Status(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        ReplyToCommand(client, "[Boss Slayer] Use !bsr as a human survivor.");
        return Plugin_Handled;
    }

    int bossKills = BSR_GetBossKills(client);
    int specialKills = BSR_GetSpecialKills(client);
    int commonKills = BSR_GetCommonKills(client);
    int pendingRewards = BSR_GetPendingRewards(client);
    int totalPerkLevels = BSR_GetTotalPerkLevels(client);

    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 Boss kills: %d | Pending rewards: %d | Perk levels: %d",
        bossKills,
        pendingRewards,
        totalPerkLevels
    );

    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 Special infected kills: %d | Next reward: %d/%d",
        specialKills,
        specialKills % SPECIAL_KILLS_PER_REWARD,
        SPECIAL_KILLS_PER_REWARD
    );

    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 Common infected kills: %d | Next reward: %d/%d",
        commonKills,
        commonKills % COMMON_KILLS_PER_REWARD,
        COMMON_KILLS_PER_REWARD
    );

    return Plugin_Handled;
}

public Action Command_OpenPerkMenu(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        ReplyToCommand(client, "[Boss Slayer] Use !perk as a human survivor.");
        return Plugin_Handled;
    }

    if (!IsPlayerAlive(client))
    {
        PrintToChat(client, "\x04[Boss Slayer]\x01 You must be alive to choose a perk.");
        return Plugin_Handled;
    }

    if (BSR_GetPendingRewards(client) <= 0)
    {
        PrintToChat(client, "\x04[Boss Slayer]\x01 You have no pending rewards.");
        return Plugin_Handled;
    }

    BSR_ShowPerkMenu(client);
    return Plugin_Handled;
}

public Action Command_ShowPerks(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        ReplyToCommand(client, "[Boss Slayer] Use !perks as a human survivor.");
        return Plugin_Handled;
    }

    PrintToChat(client, "\x04[Boss Slayer]\x01 Current build:");

    for (int perkIndex = 0; perkIndex < view_as<int>(Perk_Count); perkIndex++)
    {
        PerkType perk = view_as<PerkType>(perkIndex);
        int level = BSR_GetPerkLevel(client, perk);
        int maxLevel = BSR_GetPerkMaxLevel(perk);

        char perkName[32];
        char description[96];
        BSR_GetPerkName(perk, perkName, sizeof(perkName));
        BSR_GetPerkDescription(perk, description, sizeof(description));

        PrintToChat(
            client,
            "\x03%s\x01: Lv.%d/%d - %s",
            perkName,
            level,
            maxLevel,
            description
        );
    }

    int pendingRewards = BSR_GetPendingRewards(client);
    if (pendingRewards > 0)
    {
        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 Pending rewards: %d. Type !perk to choose.",
            pendingRewards
        );
    }

    return Plugin_Handled;
}

public Action Command_TestReward(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        ReplyToCommand(client, "[Boss Slayer] Use !bsr_testreward in chat.");
        return Plugin_Handled;
    }

    if (BSR_AddPendingRewards(client, 1))
    {
        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 Test reward granted. Pending rewards: %d",
            BSR_GetPendingRewards(client)
        );
        BSR_ShowPerkMenu(client);
    }

    return Plugin_Handled;
}

public Action Command_TestSpecialKills(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        ReplyToCommand(client, "[Boss Slayer] Use !bsr_testsikills [amount] in chat.");
        return Plugin_Handled;
    }

    int amount = 1;

    if (args >= 1)
    {
        char amountText[16];
        GetCmdArg(1, amountText, sizeof(amountText));
        amount = StringToInt(amountText);
    }

    if (amount < 1)
    {
        amount = 1;
    }
    else if (amount > 1000)
    {
        amount = 1000;
    }

    for (int i = 0; i < amount; i++)
    {
        BSR_RecordSpecialInfectedKill(client);
    }

    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 Test SI kills added: %d. Total: %d.",
        amount,
        BSR_GetSpecialKills(client)
    );

    return Plugin_Handled;
}

public Action Command_TestCommonKills(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        ReplyToCommand(client, "[Boss Slayer] Use !bsr_testcommonkills [amount] in chat.");
        return Plugin_Handled;
    }

    int amount = 1;

    if (args >= 1)
    {
        char amountText[16];
        GetCmdArg(1, amountText, sizeof(amountText));
        amount = StringToInt(amountText);
    }

    if (amount < 1)
    {
        amount = 1;
    }
    else if (amount > 5000)
    {
        amount = 5000;
    }

    for (int i = 0; i < amount; i++)
    {
        BSR_RecordCommonInfectedKill(client);
    }

    PrintToChat(
        client,
        "\x04[Boss Slayer]\x01 Test common kills added: %d. Total: %d.",
        amount,
        BSR_GetCommonKills(client)
    );

    return Plugin_Handled;
}

public Action Command_ResetMe(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        ReplyToCommand(client, "[Boss Slayer] Use !bsr_resetme in chat.");
        return Plugin_Handled;
    }

    BSR_ResetPlayerData(client);

    if (g_PerkMenuOpen[client])
    {
        g_PerkMenuOpen[client] = false;
        CancelClientMenu(client);
    }

    PrintToChat(client, "\x04[Boss Slayer]\x01 Your build has been manually reset.");
    return Plugin_Handled;
}

public Action Command_ResetAll(int client, int args)
{
    BSR_ResetAllPlayerData();
    BSR_CancelAllPerkMenus();

    PrintToChatAll(
        "\x04[Boss Slayer]\x01 All builds were reset by an administrator."
    );
    ReplyToCommand(client, "[Boss Slayer] All SteamID player data has been reset.");

    return Plugin_Handled;
}
