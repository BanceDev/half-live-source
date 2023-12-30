/***valve
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
// damagenums.cpp
//
// implementation of CHudDamageNums class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "triangleapi.h"


DECLARE_MESSAGE(m_DamageNums, DamageNums);

bool CHudDamageNums::Init()
{
	m_iDamageAmt = 0;
    m_fFade = 0;
	m_iFlags = 0;

	HOOK_MESSAGE(DamageNums);

	gHUD.AddHudElem(this);

	return true;
}


bool CHudDamageNums::VidInit()
{
	return true;
}

bool CHudDamageNums::MsgFunc_DamageNums(const char* pszName, int iSize, void* pbuf)
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ(pbuf, iSize);
	int d = READ_SHORT();
    int x = READ_COORD();
    int y = READ_COORD();
    int z = READ_COORD();

	m_iDamageAmt = d;
    m_TargetOrigin.x = x;
    m_TargetOrigin.y = y;
    m_TargetOrigin.z = z;
    m_fFade = 20;

	return true;
}


bool CHudDamageNums::Draw(float flTime)
{
	int r, g, b, a, x, y;

	UnpackRGB(r, g, b, RGB_YELLOWISH);

    if (0 != m_fFade)
	{
		if (m_fFade > 20)
			m_fFade = 20;

		m_fFade -= (gHUD.m_flTimeDelta * 20);
		if (m_fFade <= 0)
		{
			a = 255;
			m_fFade = 0;
		}

		// Fade the health number back to dim

		a = (m_fFade / 20) * 255;
	}
	else
		a = 0;

	ScaleColors(r, g, b, a);

    Vector screen;
	gEngfuncs.pTriAPI->WorldToScreen(m_TargetOrigin, screen);
	//screen.x = (1.0f + screen.x) * ScreenHeight * 0.5f;
	//screen.y = (1.0f - screen.y) * ScreenWidth * 0.5f;

	y = screen.y;
	x = screen.x;

    gHUD.DrawHudNumber(x, y, DHN_DRAWZERO, m_iDamageAmt, r, g, b);

	return true;
}
