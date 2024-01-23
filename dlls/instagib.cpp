/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// instagib.cpp
//
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"

#include "skill.h"
#include "game.h"
#include "items.h"
#include "voice_gamemgr.h"
#include "hltv.h"
#include "UserMessages.h"

//*********************************************************
// Rules for the half-life multiplayer game.
//*********************************************************

CHalfLifeInstagib::CHalfLifeInstagib()
{
}

//=========================================================
//=========================================================
void CHalfLifeInstagib::RefreshSkillData()
{
	// load all default values
	CGameRules::RefreshSkillData();

	// override some values for multiplay.

	// suitcharger
	gSkillData.suitchargerCapacity = 30;

	// Crowbar whack
	gSkillData.plrDmgCrowbar = 25;

	// Glock Round
	gSkillData.plrDmg9MM = 12;

	// 357 Round
	gSkillData.plrDmg357 = 40;

	// MP5 Round
	gSkillData.plrDmgMP5 = 12;

	// M203 grenade
	gSkillData.plrDmgM203Grenade = 90;

	// Shotgun buckshot
	gSkillData.plrDmgBuckshot = 18; // fewer pellets in deathmatch

	// Crossbow
	gSkillData.plrDmgCrossbowClient = 20;

	// RPG
	gSkillData.plrDmgRPG = 90;

	// Egon
	gSkillData.plrDmgEgonWide = 10;
	gSkillData.plrDmgEgonNarrow = 7;

	// Hand Grendade
	gSkillData.plrDmgHandGrenade = 80;

	// Satchel Charge
	gSkillData.plrDmgSatchel = 120;

	// Tripmine
	gSkillData.plrDmgTripmine = 150;

	// hornet
	gSkillData.plrDmgHornet = 10;
}

//=========================================================
//=========================================================
bool CHalfLifeInstagib::IsAllowedToSpawn(CBaseEntity* pEntity)
{
    if (FStrEq(STRING(pEntity->pev->classname), "weapon_9mmAR"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "weapon_shotgun"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "weapon_glauncher"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "weapon_gauss"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "weapon_egon"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "weapon_rpg"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "weapon_crossbow"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "item_battery"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "item_healthkit"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "item_megahealth"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "item_armor"))
    {
        return false;
    }
    if (FStrEq(STRING(pEntity->pev->classname), "item_super"))
    {
        return false;
    }
    return true;
}


//=========================================================
//=========================================================
void CHalfLifeInstagib::PlayerThink(CBasePlayer* pPlayer)
{
	if (g_fGameOver)
	{
		// check for button presses
		if ((pPlayer->m_afButtonPressed & (IN_DUCK | IN_ATTACK | IN_ATTACK2 | IN_USE | IN_JUMP)) != 0)
			m_iEndIntermissionButtonHit = true;

		// clear attack/use commands from player
		pPlayer->m_afButtonPressed = 0;
		pPlayer->pev->button = 0;
		pPlayer->m_afButtonReleased = 0;
	}

    //always replenish ammo
    pPlayer->GiveAmmo(100, "uranium", URANIUM_MAX_CARRY);
}

//=========================================================
//=========================================================
void CHalfLifeInstagib::PlayerSpawn(CBasePlayer* pPlayer)
{
	bool addDefault;
	CBaseEntity* pWeaponEntity = NULL;

	//Ensure the player switches to the Glock on spawn regardless of setting
	const int originalAutoWepSwitch = pPlayer->m_iAutoWepSwitch;
	pPlayer->m_iAutoWepSwitch = 1;

	pPlayer->SetHasSuit(true);

	addDefault = true;

	while (pWeaponEntity = UTIL_FindEntityByClassname(pWeaponEntity, "game_player_equip"))
	{
		pWeaponEntity->Touch(pPlayer);
		addDefault = false;
	}

	if (addDefault)
	{
		pPlayer->GiveNamedItem("weapon_gauss");
		pPlayer->GiveAmmo(100, "uranium", URANIUM_MAX_CARRY);
	}

	pPlayer->m_iAutoWepSwitch = originalAutoWepSwitch;
}


//=========================================================
//=========================================================
int CHalfLifeInstagib::DeadPlayerWeapons(CBasePlayer* pPlayer)
{
	return 0;
}

//=========================================================
//=========================================================
int CHalfLifeInstagib::DeadPlayerAmmo(CBasePlayer* pPlayer)
{
	return 0;
}



