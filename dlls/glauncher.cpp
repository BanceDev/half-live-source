#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"


LINK_ENTITY_TO_CLASS(weapon_grenade_launcher, CGLauncher)

void CGLauncher::Spawn()
{
    Precache();
    m_iId = WEAPON_GLAUNCHER;

    SET_MODEL(ENT(pev), "models/w_glauncher.mdl");
    m_iDefaultAmmo = GLAUNCHER_DEFAULT_GIVE;
    
    Materialize();
}

void CGLauncher::Precache()
{
    PRECACHE_MODEL("models/w_glauncher.mdl");
	PRECACHE_MODEL("models/v_glauncher.mdl");
	PRECACHE_MODEL("models/p_glauncher.mdl");

    PRECACHE_SOUND("items/9mmclip1.wav");

    PRECACHE_SOUND("weapons/glauncher.wav");
	PRECACHE_SOUND("weapons/glauncher2.wav");

    PRECACHE_SOUND("weapons/reload1.wav");
	PRECACHE_SOUND("weapons/reload3.wav"); 

    m_usGLauncher = PRECACHE_EVENT(1, "events/glauncher.sc");
}

bool CGLauncher::GetItemInfo(ItemInfo* p)
{
    p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "rockets";
	p->iMaxAmmo1 = ROCKET_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = GLAUNCHER_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_GLAUNCHER;
	p->iFlags = 0;
	p->iWeight = GLAUNCHER_WEIGHT;

	return true;
}

void CGLauncher::SecondaryAttack()
{
    m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.2;
}

void CGLauncher::PrimaryAttack()
{
    // don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		Reload();
		if (m_iClip == 0)
			PlayEmptySound();
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_iClip--;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	// we don't add in player velocity anymore.
	CGrenade::ShootTimed(m_pPlayer->pev,
		m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16,
		gpGlobals->v_forward * 800, 1.5);

	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT(flags, m_pPlayer->edict(), m_usGLauncher);

	m_flNextPrimaryAttack = GetNextAttackDelay(0.6);
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.6;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.63; // idle pretty soon after shooting
    m_fInSpecialReload = 0;
}

bool CGLauncher::Deploy()
{
    if (m_iClip == 0)
	{
		return DefaultDeploy("models/v_glauncher.mdl", "models/p_glauncher.mdl", GLAUNCHER_DRAW1, "rpg");
	}

	return DefaultDeploy("models/v_glauncher.mdl", "models/p_glauncher.mdl", GLAUNCHER_DRAW1, "rpg");
}

void CGLauncher::Holster()
{
    m_fInReload = false; // cancel any reload in progress.

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	SendWeaponAnim(GLAUNCHER_HOLSTER1);
}

void CGLauncher::Reload()
{
	if (m_iClip > 0)
	{
		// don't bother with any of this if don't need to reload.
		return;
	}

	if (m_pPlayer->ammo_rockets <= 0)
		return;

	// because the RPG waits to autoreload when no missiles are active while  the LTD is on, the
	// weapons code is constantly calling into this function, but is often denied because
	// a) missiles are in flight, but the LTD is on
	// or
	// b) player is totally out of ammo and has nothing to switch to, and should be allowed to
	//    shine the designator around
	//
	// Set the next attack time into the future so that WeaponIdle will get called more often
	// than reload, allowing the RPG LTD to be updated

	m_flNextPrimaryAttack = GetNextAttackDelay(0.5);

	if (m_iClip == 0)
	{
		const bool iResult = DefaultReload(RPG_MAX_CLIP, GLAUNCHER_RELOAD1, 2);

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase();
	}
}

void CGLauncher::WeaponIdle()
{
    // Reset when the player lets go of the trigger.
	if ((m_pPlayer->pev->button & (IN_ATTACK | IN_ATTACK2)) == 0)
	{
		ResetEmptySound();
	}


	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	SendWeaponAnim(GLAUNCHER_IDLE);
}