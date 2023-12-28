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
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == GLAUNCHER_MAX_CLIP)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim(GLAUNCHER_IDLE);
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.0;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.0;
		m_flNextPrimaryAttack = GetNextAttackDelay(0.0);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.0;
		return;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;
		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		if (RANDOM_LONG(0, 1))
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));
		else
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));

		SendWeaponAnim(GLAUNCHER_RELOAD1);

		m_flNextReload = UTIL_WeaponTimeBase() + 0.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}

void CGLauncher::WeaponIdle()
{
    ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	//Moved to ItemPostFrame
	/*
	if ( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{
		// play pumping sound
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
		m_flPumpTime = 0;
	}
	*/

	if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
	{
		if (m_iClip == 0 && m_fInSpecialReload == 0 && 0 != m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			Reload();
		}
		else if (m_fInSpecialReload != 0)
		{
			if (m_iClip != 10 && 0 != m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload();
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim(GLAUNCHER_IDLE);

				// play cocking sound
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0, 0x1f));
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
			}
		}
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);
			if (flRand <= 0.8)
			{
				iAnim = GLAUNCHER_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0 / 12.0); // * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = GLAUNCHER_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0 / 9.0);
			}
			else
			{
				iAnim = GLAUNCHER_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0 / 9.0);
			}
			SendWeaponAnim(iAnim);
		}
	}
}