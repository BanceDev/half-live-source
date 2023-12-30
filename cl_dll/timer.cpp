/***
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
// timer.cpp
//
// implementation of CHudTimer class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"


DECLARE_MESSAGE(m_Timer, Timer);

bool CHudTimer::Init()
{
	m_iCurrentTime = 0;
	m_iFlags = 0;

	HOOK_MESSAGE(Timer);

	gHUD.AddHudElem(this);

	return true;
}


bool CHudTimer::VidInit()
{
	return true;
}

bool CHudTimer::MsgFunc_Timer(const char* pszName, int iSize, void* pbuf)
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ(pbuf, iSize);
	int x = READ_SHORT();

	m_iCurrentTime = x;

	return true;
}


bool CHudTimer::Draw(float flTime)
{
	int r, g, b, x, y;

	UnpackRGB(r, g, b, RGB_YELLOWISH);

	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
	x = ScreenWidth / 2;

    const char* str = ":";
	x = gHUD.DrawHudNumber(x, y, DHN_DRAWZERO, m_iCurrentTime / 60, r, g, b);
    x = gHUD.DrawHudString(x + 2, y-5, x*2, str, r, g, b);
    // probably a better way to do this but idk how
    if (m_iCurrentTime % 60 < 10) {
        x = gHUD.DrawHudNumber(x, y, DHN_DRAWZERO, 0, r, g, b);
    }
    gHUD.DrawHudNumber(x, y, DHN_DRAWZERO, m_iCurrentTime % 60, r, g, b);

	return true;
}
