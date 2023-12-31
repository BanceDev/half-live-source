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
    m_fFade = 0;

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
	int victim = READ_BYTE();

	m_chPlayerName = g_PlayerInfoList[victim].name;
    m_fFade = 100;

	return true;
}


bool CHudFrag::Draw(float flTime)
{
	int r, g, b, a, x, y;

	r = 128;
    g = 128;
    b = 128;

    if (0 != m_fFade)
    {
        if (m_fFade > 100)
            m_fFade = 100;

        m_fFade -= (gHUD.m_flTimeDelta * 100);
        if (m_fFade <= 0)
        {
            a = 255;
            m_fFade = 0;
        }

        a = (m_fFade / 100) * 255;
    }
    else
        a = 0;

    ScaleColors(r, g, b, a);

    char fragMessage[256];
    sprintf(fragMessage, "You Fragged %s", m_chPlayerName);
	y = (ScreenHeight / 2) - gHUD.m_iFontHeight * 5;
	x = ScreenWidth / 2 - (5.5f * strlen(fragMessage))/2;

    gHUD.DrawHudString(x + 2, y-5, x*2, fragMessage, r, g, b);

	return true;
}
