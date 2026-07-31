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
        "Boss Slayer - All perks maxed\nRedeem one reward\nPress 9 to choose later\nPending rewards: %d",
        BSR_GetPendingRewards(client)
    );
    menu.Pagination = MENU_NO_PAGINATION;
    menu.ExitButton = false;

    char itemInfo[16];

    IntToString(view_as<int>(Exchange_Health), itemInfo, sizeof(itemInfo));
    menu.AddItem(
        itemInfo,
        "Restore 20 real + 20 temporary health",
        BSR_HasHealthCapacity(client) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED
    );

    IntToString(view_as<int>(Exchange_Ammo), itemInfo, sizeof(itemInfo));
    menu.AddItem(itemInfo, "Refill primary reserve ammo");

    IntToString(view_as<int>(Exchange_Medical), itemInfo, sizeof(itemInfo));
    menu.AddItem(
        itemInfo,
        "Receive one random medical item",
        !BSR_HasAnyMedicalItem(client) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED
    );

    IntToString(view_as<int>(Exchange_Throwable), itemInfo, sizeof(itemInfo));
    menu.AddItem(
        itemInfo,
        "Receive one random throwable",
        !BSR_HasThrowable(client) ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED
    );

    for (int spacer = 0; spacer < 4; spacer++)
    {
        menu.AddItem("spacer", "", ITEMDRAW_SPACER);
    }

    menu.AddItem("close", "Choose later / Close menu");

    if (menu.Display(client, MENU_TIME_FOREVER))
    {
        g_PerkMenuOpen[client] = true;
    }
    else
    {
        delete menu;
        PrintToChat(client, "\x04[Boss Slayer]\x01 Exchange menu failed to open. Type !perk to retry.");
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
            PrintToChat(client, "\x04[Boss Slayer]\x01 Reward saved. Type !perk to redeem later.");
            return 0;
        }

        ExchangeType exchange = view_as<ExchangeType>(StringToInt(itemInfo));

        if (!BSR_SetPendingRewards(client, pendingRewards - 1))
        {
            PrintToChat(client, "\x04[Boss Slayer]\x01 Could not spend the reward. Please retry.");
            return 0;
        }

        bool succeeded = false;

        switch (exchange)
        {
            case Exchange_Health:
                succeeded = BSR_AddSplitHealth(client, EXCHANGE_REAL_HEALTH, EXCHANGE_TEMP_HEALTH);
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
            PrintToChat(
                client,
                "\x04[Boss Slayer]\x01 That exchange is unavailable now. Your reward was not spent."
            );
            BSR_QueueNextPerkMenu(client);
            return 0;
        }

        PrintToChat(
            client,
            "\x04[Boss Slayer]\x01 Reward redeemed. Remaining rewards: %d",
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
                PrintToChat(client, "\x04[Boss Slayer]\x01 Reward saved. Type !perk to redeem later.");
            }
        }
    }
    else if (action == MenuAction_End)
    {
        delete menu;
    }

    return 0;
}
