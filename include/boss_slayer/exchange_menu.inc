#if defined _boss_slayer_exchange_menu_included
    #endinput
#endif
#define _boss_slayer_exchange_menu_included

void BSR_ShowExchangeMenu(int client)
{
    if (
        !BSR_IsHumanSurvivor(client)
        || !IsPlayerAlive(client)
        || g_PerkMenuOpen[client]
        || BSR_GetPendingRewards(client) <= 0
        || !BSR_AreAllPerksMaxed(client)
    )
    {
        return;
    }

    Menu menu = new Menu(BSR_MenuHandler_Exchange);
    menu.SetTitle(
        "%T",
        "Exchange Menu Title",
        client,
        BSR_GetPendingRewards(client)
    );
    menu.Pagination = MENU_NO_PAGINATION;
    menu.ExitButton = false;

    char itemInfo[16];
    char displayText[128];

    IntToString(view_as<int>(Exchange_Health), itemInfo, sizeof(itemInfo));
    FormatEx(
        displayText,
        sizeof(displayText),
        "%T",
        "Exchange Health",
        client,
        BSR_GetExchangeRealHealth(),
        BSR_GetExchangeTempHealth()
    );
    menu.AddItem(
        itemInfo,
        displayText,
        BSR_HasHealthCapacity(client) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED
    );

    IntToString(view_as<int>(Exchange_Ammo), itemInfo, sizeof(itemInfo));
    FormatEx(displayText, sizeof(displayText), "%T", "Exchange Ammo", client);
    menu.AddItem(itemInfo, displayText);

    IntToString(view_as<int>(Exchange_Medical), itemInfo, sizeof(itemInfo));
    FormatEx(displayText, sizeof(displayText), "%T", "Exchange Medical", client);
    menu.AddItem(
        itemInfo,
        displayText,
        !BSR_HasAnyMedicalItem(client) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED
    );

    IntToString(view_as<int>(Exchange_Throwable), itemInfo, sizeof(itemInfo));
    FormatEx(displayText, sizeof(displayText), "%T", "Exchange Throwable", client);
    menu.AddItem(
        itemInfo,
        displayText,
        !BSR_HasThrowable(client) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED
    );

    for (int spacer = 0; spacer < 4; spacer++)
    {
        menu.AddItem("close", "", ITEMDRAW_NOTEXT);
    }

    FormatEx(displayText, sizeof(displayText), "%T", "Menu Choose Later", client);
    menu.AddItem("close", displayText);
    menu.AddItem("close", "", ITEMDRAW_NOTEXT);

    if (menu.Display(client, MENU_TIME_FOREVER))
    {
        g_PerkMenuOpen[client] = true;
    }
    else
    {
        delete menu;
        BSR_PrintChat(
            client,
            BSR_Message_Minimal,
            "%t",
            "Exchange Menu Open Failed"
        );
    }
}

public int BSR_MenuHandler_Exchange(Menu menu, MenuAction action, int param1, int param2)
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
        if (pendingRewards <= 0 || !BSR_AreAllPerksMaxed(client))
        {
            return 0;
        }

        char itemInfo[16];
        if (!menu.GetItem(param2, itemInfo, sizeof(itemInfo)))
        {
            return 0;
        }

        if (StrEqual(itemInfo, "close"))
        {
            BSR_PrintChat(
                client,
                BSR_Message_Normal,
                "%t",
                "Reward Saved"
            );
            return 0;
        }

        ExchangeType exchange = view_as<ExchangeType>(StringToInt(itemInfo));

        if (!BSR_SetPendingRewards(client, pendingRewards - 1))
        {
            BSR_PrintChat(
                client,
                BSR_Message_Minimal,
                "%t",
                "Reward Spend Failed"
            );
            return 0;
        }

        bool succeeded = false;

        switch (exchange)
        {
            case Exchange_Health:
                succeeded = BSR_AddSplitHealth(
                    client,
                    BSR_GetExchangeRealHealth(),
                    BSR_GetExchangeTempHealth()
                );
            case Exchange_Ammo:
                succeeded = BSR_RefillPrimaryReserveAmmo(client);
            case Exchange_Medical:
                succeeded = BSR_GiveRandomMedicalItem(client);
            case Exchange_Throwable:
                succeeded = BSR_GiveRandomThrowable(client);
        }

        if (!succeeded)
        {
            BSR_SetPendingRewards(client, pendingRewards);
            BSR_PrintChat(
                client,
                BSR_Message_Minimal,
                "%t",
                "Exchange Unavailable"
            );
            BSR_QueueNextPerkMenu(client);
            return 0;
        }

        BSR_PrintChat(
            client,
            BSR_Message_Minimal,
            "%t",
            "Exchange Redeemed",
            pendingRewards - 1
        );

        if (pendingRewards - 1 > 0)
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
                BSR_PrintChat(
                    client,
                    BSR_Message_Normal,
                    "%t",
                    "Reward Saved"
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
