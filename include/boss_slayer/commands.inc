#if defined _boss_slayer_commands_included
    #endinput
#endif
#define _boss_slayer_commands_included

void BSR_RegisterCommands()
{
    RegConsoleCmd("sm_bsr", Command_Status, "Show Boss Slayer status.");
    RegConsoleCmd("sm_perk", Command_OpenPerkMenu, "Open an unspent perk choice.");
    RegConsoleCmd("sm_perks", Command_ShowPerks, "Show your current Boss Slayer build.");

    // Development commands require both ADMFLAG_CHEATS and the debug ConVar.
    RegAdminCmd(
        "sm_bsr_testreward",
        Command_TestReward,
        ADMFLAG_CHEATS,
        "Grant test rewards. Usage: !bsr_testreward [1-100]"
    );
    RegAdminCmd(
        "sm_bsr_testsikills",
        Command_TestSpecialKills,
        ADMFLAG_CHEATS,
        "Add test special-infected kills. Usage: !bsr_testsikills 30"
    );
    RegAdminCmd(
        "sm_bsr_testcommonkills",
        Command_TestCommonKills,
        ADMFLAG_CHEATS,
        "Add test common-infected kills. Usage: !bsr_testcommonkills 500"
    );
    RegAdminCmd(
        "sm_bsr_testmaxperks",
        Command_TestMaxPerks,
        ADMFLAG_CHEATS,
        "Max every perk and grant one exchange reward."
    );
    RegAdminCmd(
        "sm_bsr_testperk",
        Command_TestPerk,
        ADMFLAG_CHEATS,
        "Set one perk level. Usage: !bsr_testperk [0-13] [level]"
    );
    RegAdminCmd(
        "sm_bsr_testsupplystacks",
        Command_TestSupplyStacks,
        ADMFLAG_CHEATS,
        "Set Supply Conditioning stacks. Usage: !bsr_testsupplystacks [0-5]"
    );
    RegAdminCmd(
        "sm_bsr_resetme",
        Command_ResetMe,
        ADMFLAG_CHEATS,
        "Reset your own Boss Slayer data."
    );
    RegAdminCmd("sm_bsr_resetall", Command_ResetAll, ADMFLAG_ROOT, "Reset all Boss Slayer data.");
}

bool BSR_CanUseSelfDebugCommand(int client, const char[] commandName)
{
    if (!BSR_DebugCommandsEnabled())
    {
        BSR_ReplyTranslated(client, "%t", "Debug Commands Disabled");
        return false;
    }

    if (!BSR_IsHumanSurvivor(client))
    {
        BSR_ReplyTranslated(client, "%t", "Command Human Survivor", commandName);
        return false;
    }

    return true;
}

public Action Command_Status(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        BSR_ReplyTranslated(client, "%t", "Command Human Survivor", "!bsr");
        return Plugin_Handled;
    }

    int bossKills = BSR_GetBossKills(client);
    int specialKills = BSR_GetSpecialKills(client);
    int commonKills = BSR_GetCommonKills(client);
    int pendingRewards = BSR_GetPendingRewards(client);
    int totalPerkLevels = BSR_GetTotalPerkLevels(client);

    BSR_PrintChat(
        client,
        BSR_Message_Minimal,
        "%t",
        "Status Summary",
        bossKills,
        pendingRewards,
        totalPerkLevels
    );

    int specialThreshold = BSR_GetSpecialKillsPerReward();
    BSR_PrintChat(
        client,
        BSR_Message_Minimal,
        "%t",
        "Status Special Kills",
        specialKills,
        specialKills % specialThreshold,
        specialThreshold
    );

    int commonThreshold = BSR_GetCommonKillsPerReward();
    BSR_PrintChat(
        client,
        BSR_Message_Minimal,
        "%t",
        "Status Common Kills",
        commonKills,
        commonKills % commonThreshold,
        commonThreshold
    );

    return Plugin_Handled;
}

public Action Command_OpenPerkMenu(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        BSR_ReplyTranslated(client, "%t", "Command Human Survivor", "!perk");
        return Plugin_Handled;
    }

    if (!IsPlayerAlive(client))
    {
        BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Must Be Alive");
        return Plugin_Handled;
    }

    if (BSR_GetPendingRewards(client) <= 0)
    {
        BSR_PrintChat(client, BSR_Message_Minimal, "%t", "No Pending Rewards");
        return Plugin_Handled;
    }

    BSR_ShowPerkMenu(client);
    return Plugin_Handled;
}

public Action Command_ShowPerks(int client, int args)
{
    if (!BSR_IsHumanSurvivor(client))
    {
        BSR_ReplyTranslated(client, "%t", "Command Human Survivor", "!perks");
        return Plugin_Handled;
    }

    BSR_ShowBuildMenu(client);
    return Plugin_Handled;
}

void BSR_ShowBuildMenu(int client, int page = 0)
{
    const int perksPerPage = 7;
    int perkCount = view_as<int>(Perk_Count);
    int pageCount = (perkCount + perksPerPage - 1) / perksPerPage;
    int lastPage = pageCount - 1;

    if (page < 0)
    {
        page = 0;
    }
    else if (page > lastPage)
    {
        page = lastPage;
    }

    Menu menu = new Menu(BSR_MenuHandler_Build);
    menu.SetTitle(
        "%T",
        "Build Menu Title",
        client,
        BSR_GetTotalPerkLevels(client),
        BSR_GetPendingRewards(client),
        BSR_GetSupplyTrainingStacks(client),
        BSR_GetSupplyTrainingMaxStacks()
    );
    menu.Pagination = MENU_NO_PAGINATION;
    menu.ExitButton = false;

    int firstPerk = page * perksPerPage;
    int lastPerk = firstPerk + perksPerPage;

    if (lastPerk > view_as<int>(Perk_Count))
    {
        lastPerk = view_as<int>(Perk_Count);
    }

    for (int perkIndex = firstPerk; perkIndex < lastPerk; perkIndex++)
    {
        PerkType perk = view_as<PerkType>(perkIndex);
        int level = BSR_GetPerkLevel(client, perk);
        int maxLevel = BSR_GetPerkMaxLevel(perk);

        char perkName[32];
        char shortEffect[96];
        char displayText[160];
        BSR_GetPerkName(client, perk, perkName, sizeof(perkName));
        BSR_GetPerkShortEffect(client, perk, shortEffect, sizeof(shortEffect));

        FormatEx(
            displayText,
            sizeof(displayText),
            "%T",
            "Build Menu Item",
            client,
            perkName,
            level,
            maxLevel,
            shortEffect
        );

        // Perk rows are informational. Pressing their number closes the menu.
        menu.AddItem("close", displayText);
    }

    int displayedPerks = lastPerk - firstPerk;
    while (displayedPerks < perksPerPage)
    {
        menu.AddItem("close", "", ITEMDRAW_NOTEXT);
        displayedPerks++;
    }

    char navigationText[64];
    char navigationInfo[16];

    if (page > 0)
    {
        FormatEx(
            navigationText,
            sizeof(navigationText),
            "%T",
            "Build Menu Previous Page",
            client
        );
        FormatEx(navigationInfo, sizeof(navigationInfo), "page:%d", page - 1);
        menu.AddItem(navigationInfo, navigationText, ITEMDRAW_CONTROL);
    }
    else
    {
        // Slot 8 closes on the first page but is not shown.
        menu.AddItem("close", "", ITEMDRAW_NOTEXT);
    }

    if (page < lastPage)
    {
        FormatEx(
            navigationText,
            sizeof(navigationText),
            "%T",
            "Build Menu Next Page",
            client
        );
        FormatEx(navigationInfo, sizeof(navigationInfo), "page:%d", page + 1);
        menu.AddItem(navigationInfo, navigationText, ITEMDRAW_CONTROL);
    }
    else
    {
        // Slot 9 closes on the final page but is not shown.
        menu.AddItem("close", "", ITEMDRAW_NOTEXT);
    }

    // The 0 key also closes without drawing the default SourceMod exit item.
    menu.AddItem("close", "", ITEMDRAW_NOTEXT);

    if (!menu.Display(client, MENU_TIME_FOREVER))
    {
        delete menu;
        BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Build Menu Open Failed");
    }
}

public int BSR_MenuHandler_Build(Menu menu, MenuAction action, int param1, int param2)
{
    if (action == MenuAction_Select)
    {
        int client = param1;

        if (!BSR_IsHumanSurvivor(client))
        {
            return 0;
        }

        char itemInfo[16];
        if (!menu.GetItem(param2, itemInfo, sizeof(itemInfo)))
        {
            return 0;
        }

        if (StrContains(itemInfo, "page:", false) == 0)
        {
            BSR_ShowBuildMenu(client, StringToInt(itemInfo[5]));
        }
    }
    else if (action == MenuAction_End)
    {
        delete menu;
    }

    return 0;
}

public Action Command_TestReward(int client, int args)
{
    if (!BSR_CanUseSelfDebugCommand(client, "!bsr_testreward"))
    {
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
    else if (amount > 100)
    {
        amount = 100;
    }

    if (BSR_AddPendingRewards(client, amount))
    {
        LogAction(
            client,
            -1,
            "\"%L\" granted self %d Boss Slayer test reward(s).",
            client,
            amount
        );
        BSR_PrintChat(
            client,
            BSR_Message_Minimal,
            "%t",
            "Test Reward Granted",
            amount,
            BSR_GetPendingRewards(client)
        );
        BSR_ShowPerkMenu(client);
    }

    return Plugin_Handled;
}

public Action Command_TestSpecialKills(int client, int args)
{
    if (!BSR_CanUseSelfDebugCommand(client, "!bsr_testsikills"))
    {
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

    LogAction(
        client,
        -1,
        "\"%L\" added %d Boss Slayer test special-infected kill(s) to self.",
        client,
        amount
    );

    BSR_PrintChat(
        client,
        BSR_Message_Minimal,
        "%t",
        "Test Special Kills Added",
        amount,
        BSR_GetSpecialKills(client)
    );

    return Plugin_Handled;
}

public Action Command_TestCommonKills(int client, int args)
{
    if (!BSR_CanUseSelfDebugCommand(client, "!bsr_testcommonkills"))
    {
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

    LogAction(
        client,
        -1,
        "\"%L\" added %d Boss Slayer test common-infected kill(s) to self.",
        client,
        amount
    );

    BSR_PrintChat(
        client,
        BSR_Message_Minimal,
        "%t",
        "Test Common Kills Added",
        amount,
        BSR_GetCommonKills(client)
    );

    return Plugin_Handled;
}

public Action Command_TestMaxPerks(int client, int args)
{
    if (!BSR_CanUseSelfDebugCommand(client, "!bsr_testmaxperks"))
    {
        return Plugin_Handled;
    }

    if (g_PerkMenuOpen[client])
    {
        CancelClientMenu(client);
    }
    BSR_ResetPerkChoiceState(client);

    for (int perkIndex = 0; perkIndex < view_as<int>(Perk_Count); perkIndex++)
    {
        PerkType perk = view_as<PerkType>(perkIndex);
        BSR_SetPerkLevel(client, perk, BSR_GetPerkMaxLevel(perk));
    }

    BSR_AddPendingRewards(client, 1);
    LogAction(
        client,
        -1,
        "\"%L\" maxed all Boss Slayer perks and granted self one exchange reward.",
        client
    );
    BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Test Max Perks Granted");
    BSR_ShowPerkMenu(client);

    return Plugin_Handled;
}

public Action Command_TestPerk(int client, int args)
{
    if (!BSR_CanUseSelfDebugCommand(client, "!bsr_testperk"))
    {
        return Plugin_Handled;
    }

    if (args < 2)
    {
        BSR_PrintChat(
            client,
            BSR_Message_Minimal,
            "%t",
            "Test Perk IDs"
        );
        return Plugin_Handled;
    }

    char perkText[16];
    char levelText[16];
    GetCmdArg(1, perkText, sizeof(perkText));
    GetCmdArg(2, levelText, sizeof(levelText));

    int perkIndex = StringToInt(perkText);
    if (perkIndex < 0 || perkIndex >= view_as<int>(Perk_Count))
    {
        BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Invalid Perk ID");
        return Plugin_Handled;
    }

    PerkType perk = view_as<PerkType>(perkIndex);
    int level = StringToInt(levelText);
    int maxLevel = BSR_GetPerkMaxLevel(perk);

    if (level < 0)
    {
        level = 0;
    }
    else if (level > maxLevel)
    {
        level = maxLevel;
    }

    BSR_SetPerkLevel(client, perk, level);

    LogAction(
        client,
        -1,
        "\"%L\" set self Boss Slayer perk %d to level %d.",
        client,
        perkIndex,
        level
    );

    char perkName[32];
    BSR_GetPerkName(client, perk, perkName, sizeof(perkName));
    BSR_PrintChat(
        client,
        BSR_Message_Minimal,
        "%t",
        "Test Perk Set",
        perkName,
        level,
        maxLevel
    );

    return Plugin_Handled;
}

public Action Command_TestSupplyStacks(int client, int args)
{
    if (!BSR_CanUseSelfDebugCommand(client, "!bsr_testsupplystacks"))
    {
        return Plugin_Handled;
    }

    int stacks = 0;

    if (args >= 1)
    {
        char stackText[16];
        GetCmdArg(1, stackText, sizeof(stackText));
        stacks = StringToInt(stackText);
    }

    if (stacks < 0)
    {
        stacks = 0;
    }
    else if (stacks > BSR_GetSupplyTrainingMaxStacks())
    {
        stacks = BSR_GetSupplyTrainingMaxStacks();
    }

    BSR_SetSupplyTrainingStacks(client, stacks);
    LogAction(
        client,
        -1,
        "\"%L\" set self Boss Slayer Supply Conditioning stacks to %d.",
        client,
        stacks
    );
    BSR_PrintChat(
        client,
        BSR_Message_Minimal,
        "%t",
        "Test Supply Stacks Set",
        stacks,
        BSR_GetSupplyTrainingMaxStacks()
    );
    return Plugin_Handled;
}

public Action Command_ResetMe(int client, int args)
{
    if (!BSR_CanUseSelfDebugCommand(client, "!bsr_resetme"))
    {
        return Plugin_Handled;
    }

    BSR_ResetPlayerData(client);
    BSR_CaptureChapterSnapshotForClient(client);

    if (g_PerkMenuOpen[client])
    {
        g_PerkMenuOpen[client] = false;
        CancelClientMenu(client);
    }

    LogAction(client, -1, "\"%L\" reset their own Boss Slayer build.", client);

    BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Build Manually Reset");
    return Plugin_Handled;
}

public Action Command_ResetAll(int client, int args)
{
    BSR_ResetAllPlayerData();
    BSR_CancelAllPerkMenus();

    LogAction(client, -1, "\"%L\" reset all Boss Slayer builds.", client);

    BSR_PrintChatAll(
        BSR_Message_Minimal,
        "%t",
        "Builds Admin Reset"
    );
    BSR_ReplyTranslated(client, "%t", "Admin Reset Confirmed");

    return Plugin_Handled;
}
