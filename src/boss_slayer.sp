#include <sourcemod>

#pragma semicolon 1
#pragma newdecls required

#include <boss_slayer/definitions>
#include <boss_slayer/state>
#include <boss_slayer/storage>
#include <boss_slayer/perks>
#include <boss_slayer/boss_tracking>
#include <boss_slayer/perk_menu>
#include <boss_slayer/rewards>
#include <boss_slayer/events>
#include <boss_slayer/commands>
#include <boss_slayer/lifecycle>

public Plugin myinfo =
{
    name = "Boss Slayer Roguelite",
    author = "gofurry",
    description = "Tank and Witch kills grant temporary roguelite perk choices.",
    version = BSR_VERSION
};

public void OnPluginStart()
{
    BSR_InitializeState();
    BSR_RegisterEvents();
    BSR_RegisterCommands();
    BSR_CacheConnectedClients();

    PrintToServer("[Boss Slayer] Version %s loaded.", BSR_VERSION);
}
