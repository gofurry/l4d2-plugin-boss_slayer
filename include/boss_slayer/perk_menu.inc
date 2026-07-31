#if defined _boss_slayer_perk_menu_included
    #endinput
#endif
#define _boss_slayer_perk_menu_included

void BSR_ResetPerkChoiceState(int client)
{
    if (client < 1 || client > MaxClients)
    {
        return;
    }

    g_PerkMenuOpen[client] = false;
    g_CurrentChoiceCount[client] = 0;
    g_RerollUsed[client] = false;

    for (int index = 0; index < PERK_MENU_CHOICES; index++)
    {
        g_CurrentChoices[client][index] = -1;
    }
}

bool BSR_IsCurrentPerkChoiceValid(int client)
{
    if (g_CurrentChoiceCount[client] <= 0)
    {
        return false;
    }

    for (int index = 0; index < g_CurrentChoiceCount[client]; index++)
    {
        int perkIndex = g_CurrentChoices[client][index];
        if (perkIndex < 0 || perkIndex >= view_as<int>(Perk_Count))
        {
            return false;
        }

        PerkType perk = view_as<PerkType>(perkIndex);
        if (BSR_GetPerkLevel(client, perk) >= BSR_GetPerkMaxLevel(perk))
        {
            return false;
        }
    }

    return true;
}

bool BSR_WasPreviousChoice(int perkIndex, int previous[PERK_MENU_CHOICES], int previousCount)
{
    for (int index = 0; index < previousCount; index++)
    {
        if (previous[index] == perkIndex)
        {
            return true;
        }
    }

    return false;
}

int BSR_GeneratePerkChoices(int client, bool avoidPrevious)
{
    int previous[PERK_MENU_CHOICES];
    int previousCount = g_CurrentChoiceCount[client];

    for (int index = 0; index < PERK_MENU_CHOICES; index++)
    {
        previous[index] = g_CurrentChoices[client][index];
        g_CurrentChoices[client][index] = -1;
    }

    int availablePerks[Perk_Count];
    int availableCount = BSR_BuildAvailablePerkPool(client, availablePerks);
    BSR_ShufflePerkPool(availablePerks, availableCount);

    int desiredCount = availableCount < PERK_MENU_CHOICES
        ? availableCount
        : PERK_MENU_CHOICES;
    int selectedCount = 0;

    if (avoidPrevious)
    {
        for (int index = 0; index < availableCount && selectedCount < desiredCount; index++)
        {
            if (!BSR_WasPreviousChoice(availablePerks[index], previous, previousCount))
            {
                g_CurrentChoices[client][selectedCount++] = availablePerks[index];
            }
        }
    }

    for (int index = 0; index < availableCount && selectedCount < desiredCount; index++)
    {
        int perkIndex = availablePerks[index];
        bool alreadySelected = false;

        for (int selected = 0; selected < selectedCount; selected++)
        {
            if (g_CurrentChoices[client][selected] == perkIndex)
            {
                alreadySelected = true;
                break;
            }
        }

        if (!alreadySelected)
        {
            g_CurrentChoices[client][selectedCount++] = perkIndex;
        }
    }

    g_CurrentChoiceCount[client] = selectedCount;
    return selectedCount;
}

void BSR_ShowPerkMenu(int client)
{
    if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client) || g_PerkMenuOpen[client])
    {
        return;
    }

    int pendingRewards = BSR_GetPendingRewards(client);
    if (pendingRewards <= 0)
    {
        return;
    }

    int availablePerks[Perk_Count];
    int availableCount = BSR_BuildAvailablePerkPool(client, availablePerks);

    if (availableCount <= 0)
    {
        g_CurrentChoiceCount[client] = 0;
        g_RerollUsed[client] = false;
        BSR_ShowExchangeMenu(client);
        return;
    }

    if (!BSR_IsCurrentPerkChoiceValid(client))
    {
        BSR_GeneratePerkChoices(client, false);
    }

    Menu menu = new Menu(BSR_MenuHandler_Perk);
    menu.SetTitle(
        "Boss Slayer - Choose one perk\nPending rewards: %d\nPress 9 to choose later",
        pendingRewards
    );
    menu.Pagination = MENU_NO_PAGINATION;
    menu.ExitButton = false;

    int displayedItems = 0;

    for (int index = 0; index < g_CurrentChoiceCount[client]; index++)
    {
        PerkType perk = view_as<PerkType>(g_CurrentChoices[client][index]);
        int currentLevel = BSR_GetPerkLevel(client, perk);

        char itemInfo[16];
        char perkName[32];
        char shortEffect[80];
        char displayText[160];

        IntToString(view_as<int>(perk), itemInfo, sizeof(itemInfo));
        BSR_GetPerkName(perk, perkName, sizeof(perkName));
        BSR_GetPerkShortEffect(perk, shortEffect, sizeof(shortEffect));

        Format(
            displayText,
            sizeof(displayText),
            "%s Lv.%d -> Lv.%d (%s)",
            perkName,
            currentLevel,
            currentLevel + 1,
            shortEffect
        );

        menu.AddItem(itemInfo, displayText);
        displayedItems++;
    }

    if (BSR_GetPerkLevel(client, Perk_FateReroll) > 0 && !g_RerollUsed[client])
    {
        menu.AddItem("reroll", "Reroll these choices (free, once per reward)");
        displayedItems++;
    }

    while (displayedItems < 8)
    {
        menu.AddItem("spacer", "", ITEMDRAW_SPACER);
        displayedItems++;
    }

    menu.AddItem("close", "Choose later / Close menu");

    if (menu.Display(client, MENU_TIME_FOREVER))
    {
        g_PerkMenuOpen[client] = true;
    }
    else
    {
        delete menu;
        PrintToChat(client, "\x04[Boss Slayer]\x01 The menu could not open. Type !perk to try again.");
    }
}

public int BSR_MenuHandler_Perk(Menu menu, MenuAction action, int param1, int param2)
{
    if (action == MenuAction_Select)
    {
        int client = param1;

        if (client >= 1 && client <= MaxClients)
        {
            g_PerkMenuOpen[client] = false;
        }

        if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client))
        {
            return 0;
        }

        int pendingRewards = BSR_GetPendingRewards(client);
        if (pendingRewards <= 0)
        {
            PrintToChat(client, "\x04[Boss Slayer]\x01 This reward is no longer available.");
            return 0;
        }

        char itemInfo[16];
        if (!menu.GetItem(param2, itemInfo, sizeof(itemInfo)))
        {
            LogError("Failed to read perk menu item %d for client %d.", param2, client);
            return 0;
        }

        if (StrEqual(itemInfo, "reroll"))
        {
            if (BSR_GetPerkLevel(client, Perk_FateReroll) <= 0 || g_RerollUsed[client])
            {
                PrintToChat(client, "\x04[Boss Slayer]\x01 This reward has already been rerolled.");
                return 0;
            }

            g_RerollUsed[client] = true;
            BSR_GeneratePerkChoices(client, true);
            PrintToChat(client, "\x04[Boss Slayer]\x01 Fate Reroll replaced the current choices.");
            BSR_QueueNextPerkMenu(client);
            return 0;
        }

        if (StrEqual(itemInfo, "close"))
        {
            PrintToChat(client, "\x04[Boss Slayer]\x01 Reward saved. Type !perk when you are ready.");
            return 0;
        }

        int perkIndex = StringToInt(itemInfo);
        if (perkIndex < 0 || perkIndex >= view_as<int>(Perk_Count))
        {
            LogError("Invalid perk index %d selected by client %d.", perkIndex, client);
            return 0;
        }

        PerkType perk = view_as<PerkType>(perkIndex);
        int currentLevel = BSR_GetPerkLevel(client, perk);
        int maxLevel = BSR_GetPerkMaxLevel(perk);

        if (currentLevel >= maxLevel)
        {
            g_CurrentChoiceCount[client] = 0;
            PrintToChat(client, "\x04[Boss Slayer]\x01 That perk is already at maximum level.");
            BSR_QueueNextPerkMenu(client);
            return 0;
        }

        if (!BSR_SetPerkLevel(client, perk, currentLevel + 1))
        {
            PrintToChat(client, "\x04[Boss Slayer]\x01 Your SteamID is not ready. The reward was not spent.");
            return 0;
        }

        if (!BSR_SetPendingRewards(client, pendingRewards - 1))
        {
            BSR_SetPerkLevel(client, perk, currentLevel);
            PrintToChat(client, "\x04[Boss Slayer]\x01 Could not spend the reward. Please try again.");
            return 0;
        }

        char perkName[32];
        char description[128];
        BSR_GetPerkName(perk, perkName, sizeof(perkName));
        BSR_GetPerkDescription(perk, description, sizeof(description));

        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 Selected \x03%s Lv.%d\x01. %s",
            perkName,
            currentLevel + 1,
            description
        );

        int remainingRewards = pendingRewards - 1;
        BSR_ResetPerkChoiceState(client);

        if (remainingRewards > 0)
        {
            PrintToChat(client, "\x04[Boss Slayer]\x01 Remaining rewards: %d", remainingRewards);
            BSR_QueueNextPerkMenu(client);
        }
    }
    else if (action == MenuAction_Cancel)
    {
        int client = param1;

        if (client >= 1 && client <= MaxClients)
        {
            g_PerkMenuOpen[client] = false;

            if (BSR_IsHumanSurvivor(client) && BSR_GetPendingRewards(client) > 0)
            {
                PrintToChat(client, "\x04[Boss Slayer]\x01 Reward saved. Type !perk when you are ready.");
            }
        }
    }
    else if (action == MenuAction_End)
    {
        delete menu;
    }

    return 0;
}

void BSR_QueueNextPerkMenu(int client)
{
    if (!BSR_IsHumanSurvivor(client) || !IsPlayerAlive(client))
    {
        return;
    }

    CreateTimer(0.3, Timer_ShowNextPerkMenu, GetClientUserId(client), TIMER_FLAG_NO_MAPCHANGE);
}

public Action Timer_ShowNextPerkMenu(Handle timer, any userId)
{
    int client = GetClientOfUserId(userId);

    if (BSR_IsHumanSurvivor(client) && IsPlayerAlive(client))
    {
        BSR_ShowPerkMenu(client);
    }

    return Plugin_Stop;
}

void BSR_CancelAllPerkMenus()
{
    for (int client = 1; client <= MaxClients; client++)
    {
        if (g_PerkMenuOpen[client] && IsClientInGame(client))
        {
            CancelClientMenu(client);
        }

        BSR_ResetPerkChoiceState(client);
    }
}
