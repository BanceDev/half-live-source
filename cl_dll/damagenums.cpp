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

#include <queue>

DECLARE_MESSAGE(m_DamageNums, DamageNums);

bool CHudDamageNums::Init()
{
	m_DamageInfo.damageAmt = 0;
	m_DamageInfo.fade = 0;
	m_DamageInfo.targetPos.x = 0;
	m_DamageInfo.targetPos.y = 0;
	m_DamageInfo.targetPos.z = 0;
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
    float x = READ_COORD();
    float y = READ_COORD();
    float z = READ_COORD();

	m_DamageInfo.damageAmt = d;
	m_DamageInfo.targetPos.x = x;
	m_DamageInfo.targetPos.y = y;
	m_DamageInfo.targetPos.z = z;
	m_DamageInfo.fade = 20;

	m_DamageQueue.push(m_DamageInfo);

	return true;
}


bool CHudDamageNums::Draw(float flTime)
{
	int i;
	for (i = 0; i < m_DamageQueue.size(); i++) {
		int r, g, b, a, x, y;

		UnpackRGB(r, g, b, RGB_REDISH);

		if (0 != m_DamageQueue.front().fade)
		{
			if (m_DamageQueue.front().fade > 20)
				m_DamageQueue.front().fade = 20;

			m_DamageQueue.front().fade -= (gHUD.m_flTimeDelta * 20);
			if (m_DamageQueue.front().fade <= 0)
			{
				a = 255;
				m_DamageQueue.front().fade = 0;
			}

			// Fade the health number back to dim

			a = (m_DamageQueue.front().fade / 20) * 255;
		}
		else
			a = 0;

		ScaleColors(r, g, b, a);

		Vector screen;
		gEngfuncs.pTriAPI->WorldToScreen(m_DamageQueue.front().targetPos, screen);
		y = (1.0f - screen.y) * ScreenHeight * 0.5f - (m_DamageQueue.front().fade);
		x = (1.0f + screen.x) * ScreenWidth * 0.5f;
		gHUD.DrawHudNumber(x, y, DHN_DRAWZERO, m_DamageQueue.front().damageAmt, r, g, b);

		//reorder the queue
		DamageInfo temp = m_DamageQueue.front();
		m_DamageQueue.pop();
		if(temp.fade > 0) {
			m_DamageQueue.push(temp);
		}
	}
	

	return true;
}
