#if defined _boss_slayer_perk_menu_included
    #endinput
#endif
#define _boss_slayer_perk_menu_included

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
        // v0.4.1 has no max-level compensation. Avoid leaving unusable rewards.
        BSR_SetPendingRewards(client, 0);
        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 All perks are maxed. Remaining rewards were consumed."
        );
        return;
    }

    BSR_ShufflePerkPool(availablePerks, availableCount);

    Menu menu = new Menu(BSR_MenuHandler_Perk);
    menu.SetTitle(
        "Boss Slayer - Choose one perk\nPending rewards: %d\nPress 0 to choose later",
        pendingRewards
    );
    menu.ExitButton = true;

    int choiceCount = availableCount < PERK_MENU_CHOICES
        ? availableCount
        : PERK_MENU_CHOICES;

    for (int i = 0; i < choiceCount; i++)
    {
        PerkType perk = view_as<PerkType>(availablePerks[i]);
        int currentLevel = BSR_GetPerkLevel(client, perk);

        char itemInfo[16];
        char perkName[32];
        char shortEffect[64];
        char displayText[128];

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
    }

    if (menu.Display(client, MENU_TIME_FOREVER))
    {
        g_PerkMenuOpen[client] = true;
    }
    else
    {
        delete menu;
        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 The menu could not open. Type !perk to try again."
        );
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

        int perkIndex = StringToInt(itemInfo);
        if (perkIndex < 0 || perkIndex >= view_as<int>(Perk_Count))
        {
            LogError("Invalid perk index %d selected by client %d.", perkIndex, client);
            return 0;
        }

        PerkType perk = view_as<PerkType>(perkIndex);
        int currentLevel = BSR_GetPerkLevel(client, perk);
        int maxLevel = BSR_GetPerkMaxLevel(perk);

        // Protect against a stale menu or duplicated selection.
        if (currentLevel >= maxLevel)
        {
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
            // Roll back the level when spending the reward fails.
            BSR_SetPerkLevel(client, perk, currentLevel);
            PrintToChat(client, "\x04[Boss Slayer]\x01 Could not spend the reward. Please try again.");
            return 0;
        }

        char perkName[32];
        char description[96];
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
        if (remainingRewards > 0)
        {
            PrintToChat(
                client,
                "\x04[Boss Slayer]\x01 Remaining rewards: %d",
                remainingRewards
            );
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
                PrintToChat(
                    client,
                    "\x04[Boss Slayer]\x01 Reward saved. Type !perk when you are ready."
                );
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

    CreateTimer(
        0.3,
        Timer_ShowNextPerkMenu,
        GetClientUserId(client),
        TIMER_FLAG_NO_MAPCHANGE
    );
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
        if (!g_PerkMenuOpen[client])
        {
            continue;
        }

        if (IsClientInGame(client))
        {
            CancelClientMenu(client);
        }

        g_PerkMenuOpen[client] = false;
    }
}
