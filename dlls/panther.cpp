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
/*

===== panther.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

class CPanther : public CBaseMonster
{
public:
	virtual void Spawn();
	virtual int Classify() { return 9; };
	virtual void SetActivity(int activity);
	virtual void Die();
};
void CPanther::Spawn()
{
	PRECACHE_MODEL("models/panther.mdl");
	SET_MODEL(ENT(pev), "models/panther.mdl");
	UTIL_SetSize(pev, Vector(-32, -32, 0), Vector(32, 32, 64));
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	pev->effects = 0;
	pev->health = 50;
	pev->yaw_speed = 10;
	pev->sequence = 2;
	m_bloodColor = -110;
	pev->nextthink += UTIL_RandomFloat(0.0, 0.5) + 0.5;
	SetThink(&CPanther::MonsterInit);
}

void CPanther::SetActivity(int activity)
{
	int activitynum = NULL;
	switch (activity)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 8:
	case 29:
		if (pev->sequence)
		{
			pev->sequence = 0;
			pev->frame = 0;
			ResetSequenceInfo(0.1);
		}
		break;
	default:
		ALERT(at_console, "Panther's monster state is bogus: %d", activity);
		break;
	}
}

void CPanther::Die()
{
	SetThink(&CPanther::SUB_Remove);
}