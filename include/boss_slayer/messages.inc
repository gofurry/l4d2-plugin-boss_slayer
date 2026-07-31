#if defined _boss_slayer_messages_included
    #endinput
#endif
#define _boss_slayer_messages_included

bool BSR_ShouldPrintMessage(BSRMessageLevel requiredLevel)
{
    return BSR_GetChatLevel() >= view_as<int>(requiredLevel);
}

void BSR_PrintChat(
    int client,
    BSRMessageLevel requiredLevel,
    const char[] format,
    any ...
)
{
    if (
        !BSR_ShouldPrintMessage(requiredLevel)
        || client < 1
        || client > MaxClients
        || !IsClientInGame(client)
        || IsFakeClient(client)
    )
    {
        return;
    }

    char message[256];
    SetGlobalTransTarget(client);
    VFormat(message, sizeof(message), format, 4);
    PrintToChat(client, "\x04[Boss Slayer]\x01 %s", message);
}

void BSR_PrintChatAll(
    BSRMessageLevel requiredLevel,
    const char[] format,
    any ...
)
{
    if (!BSR_ShouldPrintMessage(requiredLevel))
    {
        return;
    }

    for (int client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client) || IsFakeClient(client))
        {
            continue;
        }

        char message[256];
        SetGlobalTransTarget(client);
        VFormat(message, sizeof(message), format, 3);
        PrintToChat(client, "\x04[Boss Slayer]\x01 %s", message);
    }
}

void BSR_ReplyTranslated(int client, const char[] format, any ...)
{
    char message[256];
    SetGlobalTransTarget(client > 0 ? client : LANG_SERVER);
    VFormat(message, sizeof(message), format, 3);
    ReplyToCommand(client, "[Boss Slayer] %s", message);
}
