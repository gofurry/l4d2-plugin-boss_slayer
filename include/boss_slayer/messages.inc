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
    char prefix[32];
    SetGlobalTransTarget(client);
    VFormat(message, sizeof(message), format, 4);
    FormatEx(prefix, sizeof(prefix), "%T", "Chat Prefix", client);
    PrintToChat(client, "\x04[%s]\x01 %s", prefix, message);
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
        char prefix[32];
        SetGlobalTransTarget(client);
        VFormat(message, sizeof(message), format, 3);
        FormatEx(prefix, sizeof(prefix), "%T", "Chat Prefix", client);
        PrintToChat(client, "\x04[%s]\x01 %s", prefix, message);
    }
}

void BSR_ReplyTranslated(int client, const char[] format, any ...)
{
    char message[256];
    char prefix[32];
    int languageTarget = client > 0 ? client : LANG_SERVER;

    SetGlobalTransTarget(languageTarget);
    VFormat(message, sizeof(message), format, 3);
    FormatEx(prefix, sizeof(prefix), "%T", "Chat Prefix", languageTarget);
    ReplyToCommand(client, "[%s] %s", prefix, message);
}
