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
        "%T",
        "Perk Menu Title",
        client,
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
        BSR_GetPerkName(client, perk, perkName, sizeof(perkName));
        BSR_GetPerkShortEffect(client, perk, shortEffect, sizeof(shortEffect));

        Format(
            displayText,
            sizeof(displayText),
            "%T",
            "Perk Menu Item",
            client,
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
        char rerollText[128];
        FormatEx(rerollText, sizeof(rerollText), "%T", "Menu Reroll", client);
        menu.AddItem("reroll", rerollText);
        displayedItems++;
    }

    while (displayedItems < 8)
    {
        // Hidden selectable entries make every unlisted number choose later.
        menu.AddItem("close", "", ITEMDRAW_NOTEXT);
        displayedItems++;
    }

    char closeText[64];
    FormatEx(closeText, sizeof(closeText), "%T", "Menu Choose Later", client);
    menu.AddItem("close", closeText);
    // Slot 10 (the 0 key) is also a hidden choose-later action.
    menu.AddItem("close", "", ITEMDRAW_NOTEXT);

    if (menu.Display(client, MENU_TIME_FOREVER))
    {
        g_PerkMenuOpen[client] = true;
    }
    else
    {
        delete menu;
        BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Perk Menu Open Failed");
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
            BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Reward No Longer Available");
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
                BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Reroll Already Used");
                return 0;
            }

            g_RerollUsed[client] = true;
            BSR_GeneratePerkChoices(client, true);
            BSR_PrintChat(client, BSR_Message_Normal, "%t", "Reroll Completed");
            BSR_QueueNextPerkMenu(client);
            return 0;
        }

        if (StrEqual(itemInfo, "close"))
        {
            BSR_PrintChat(client, BSR_Message_Normal, "%t", "Reward Saved");
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
            BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Perk Already Maxed");
            BSR_QueueNextPerkMenu(client);
            return 0;
        }

        if (!BSR_SetPerkLevel(client, perk, currentLevel + 1))
        {
            BSR_PrintChat(client, BSR_Message_Minimal, "%t", "SteamID Not Ready");
            return 0;
        }

        if (!BSR_SetPendingRewards(client, pendingRewards - 1))
        {
            BSR_SetPerkLevel(client, perk, currentLevel);
            BSR_PrintChat(client, BSR_Message_Minimal, "%t", "Reward Spend Failed");
            return 0;
        }

        char perkName[32];
        char description[128];
        BSR_GetPerkName(client, perk, perkName, sizeof(perkName));
        BSR_GetPerkDescription(client, perk, description, sizeof(description));

        BSR_PrintChat(
            client,
            BSR_Message_Minimal,
            "%t",
            "Perk Selected",
            perkName,
            currentLevel + 1,
            description
        );

        int remainingRewards = pendingRewards - 1;
        BSR_ResetPerkChoiceState(client);

        if (remainingRewards > 0)
        {
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
                BSR_PrintChat(client, BSR_Message_Normal, "%t", "Reward Saved");
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
