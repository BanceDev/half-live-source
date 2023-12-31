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
// frag.cpp
//
// implementation of CHudFrag class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"


DECLARE_MESSAGE(m_Frag, Frag);

bool CHudFrag::Init()
{
	m_iFlags = 0;

	HOOK_MESSAGE(Frag);

	gHUD.AddHudElem(this);

	return true;
}


bool CHudFrag::VidInit()
{
	return true;
}

bool CHudFrag::MsgFunc_Frag(const char* pszName, int iSize, void* pbuf)
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ(pbuf, iSize);
	char* str = READ_STRING();

	m_chPlayerName = str;

	return true;
}


bool CHudFrag::Draw(float flTime)
{
	int r, g, b, x, y;

	r = 128;
    g = 128;
    b = 128;

	y = (ScreenHeight / 2) - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
	x = ScreenWidth / 2;
    char fragMessage[256];
    sprintf(fragMessage, "You Fragged %s", m_chPlayerName);

    gHUD.DrawHudString(x + 2, y-5, x*2, fragMessage, r, g, b);

	return true;
}
