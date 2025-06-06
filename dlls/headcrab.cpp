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

===== headcrab.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

enum headcrab_anims
{
	idle = 0,
	walk,
	run,
	angry,
	jump,
	flinch,
	dieback
};

class CHeadCrab : public CBaseMonster
{
public:
	virtual void Spawn();
	virtual void SetActivity(int activity);
	virtual int Classify() { return 8; };
	virtual void Alert();
	virtual void Pain(float flDamage);
	virtual void Die();
	virtual void Idle();
	virtual int CheckEnemy(entvars_t *a2, float a3);
	void Leap();
	//void leap_touch;
};

LINK_ENTITY_TO_CLASS(monster_headcrab, CHeadCrab);

void CHeadCrab::Spawn()
{
	PRECACHE_SOUND("headcrab/hc_attack1.wav");
	PRECACHE_SOUND("headcrab/hc_alert1.wav");
	PRECACHE_SOUND("headcrab/hc_die1.wav");
	PRECACHE_SOUND("headcrab/hc_die2.wav");
	PRECACHE_SOUND("headcrab/hc_pain1.wav");
	PRECACHE_SOUND("headcrab/hc_pain2.wav");
	PRECACHE_SOUND("headcrab/hc_pain3.wav");
	PRECACHE_SOUND("headcrab/hc_idle1.wav");
	PRECACHE_SOUND("headcrab/hc_idle2.wav");
	PRECACHE_SOUND("headcrab/hc_idle3.wav");
	PRECACHE_SOUND("headcrab/hc_headbite.wav");
	PRECACHE_MODEL("models/headcrab.mdl");
	SET_MODEL(ENT(pev), "models/headcrab.mdl");
	UTIL_SetSize(pev, Vector(-12, -12, 0), Vector(12, 12, 24));
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	pev->effects = 0;
	pev->health = 12;
	pev->yaw_speed = 10;
	pev->sequence = 8;
	m_bloodColor = 54;
	m_sightDistance = 512;
	pev->nextthink += UTIL_RandomFloat(0.0, 0.5) + 0.5;
	SetThink(&CHeadCrab::MonsterInit);
}

void CHeadCrab::SetActivity(int activity)
{
	int activitynum = NULL;
	switch (activity)
	{
	case 1:
		activitynum = idle;
		break;
	case 2:
		activitynum = idle;
		break;
	case 3:
		activitynum = idle;
		break;
	case 4:
		activitynum = walk;
		break;
	case 6:
		activitynum = idle;
		break;
	case 7:
		activitynum = idle;
		break;
	case 8:
		activitynum = run;
		break;
	case 9:
		activitynum = run;
		break;
	case 18:
		activitynum = flinch;
		break;
	case 30:
		activitynum = jump;
		break;
	case 31:
		return;
		break;
	case 33:
		activitynum = flinch;
		break;
	case 35:
		activitynum = dieback;
		break;
	case 41:
		activitynum = 7; // probably a unused activity
		break;
	default:
		ALERT(at_console, "Headcrab's monster state is bogus: %d", activity);
		break;
	}
	if (pev->sequence != activitynum)
	{
		pev->sequence = activitynum;
		pev->frame = 0;
		ResetSequenceInfo(0.1);
	}
}

void CHeadCrab::Pain(float flDamage)
{
	int sound = rand() % 3;
	if (sound == 1)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "headcrab/hc_pain3.wav", 1, ATTN_NORM);
	}
	else if (sound == 2)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "headcrab/hc_pain2.wav", 1, ATTN_NORM);
	}
	else
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "headcrab/hc_pain1.wav", 1, ATTN_NORM);
	}
	if (flDamage < 5)
	{
		SetThink(&CHeadCrab::CallMonsterThink);
		if (pev->enemy)
			m_iActivity = 6;
	}
}

void CHeadCrab::Die()
{
	if (pev->health > -30)
	{
		switch (rand() % 2)
		{
		case 0:
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "headcrab/hc_die2.wav", 1, ATTN_NORM);
			break;
		case 1:
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "headcrab/hc_die1.wav", 1, ATTN_NORM);
			break;
		default:
			break;
		}
	}
	m_iActivity = 35;
	pev->ideal_yaw = pev->angles.y;
	pev->nextthink = pev->pSystemGlobals->time + 0.1;
	SetThink(&CBaseMonster::CallMonsterThink);
	SetActivity(m_iActivity);
}

void CHeadCrab::Alert()
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "headcrab/hc_alert1.wav", 1, ATTN_NORM);
	m_iActivity = 31;
	nextattack = pev->pSystemGlobals->time + 1;
}

void CHeadCrab::Idle()
{
	float sound = UTIL_RandomFloat(0.0, 1.0);
	if (sound <= 0.33)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "headcrab/hc_idle1.wav", 1, ATTN_NORM);
	}
	else if (sound <= 0.66)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "headcrab/hc_idle2.wav", 1, ATTN_NORM);
	}
	else
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "headcrab/hc_idle3.wav", 1, ATTN_NORM);
	}
	nextidle = pev->pSystemGlobals->time + UTIL_RandomFloat(0, 2) + 3;
}

void CHeadCrab::Leap()
{
	pev->nextthink = pgv->time + 0.1;
	if (m_iActivity != 30)
	{
		entvars_t* enemy = VARS(pev->enemy);
		m_iActivity = 30;
		SetActivity(30);
		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "headcrab/hc_attack1.wav", 1, ATTN_NORM);
		UTIL_MakeVectors(pev->angles);
		Vector origin = pev->origin;
		origin.z += 1.0;
		UTIL_SetOrigin(pev, origin);
		Vector distance;
		distance = pev->origin - (enemy->origin + enemy->view_ofs);
		float length = origin.Length();
		Vector direction;
		if (length < 64)
		{
			direction = 0;
		}
		else
		{
			direction.x = distance.x * (1.0 / length);
			direction.y = distance.y * (1.0 / length);
			direction.z = distance.z * (1.0 / length);
		}
		Vector v_forward;
		v_forward = (pgv->v_forward + direction) * 250.0;
		WRITE_BYTE(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(MSG_BROADCAST, TE_SHOWLINE);
		WRITE_COORD(MSG_BROADCAST, pev->origin.x);
		WRITE_COORD(MSG_BROADCAST, pev->origin.y);
		WRITE_COORD(MSG_BROADCAST, pev->origin.z);
		WRITE_COORD(MSG_BROADCAST, pev->origin.x + v_forward.x);
		WRITE_COORD(MSG_BROADCAST, pev->origin.y + v_forward.y);
		WRITE_COORD(MSG_BROADCAST, pev->origin.z + v_forward.z);
		pev->velocity = v_forward;
	}
}

int CHeadCrab::CheckEnemy(entvars_t *a2, float a3)
{
		if (CheckEnemyOnCrosshair(a2) && a3 <= 1024)
		{
			m_iIdealActivity = 7;                               // m_iactivity
			SetThink(&CHeadCrab::Leap);                   // jump
			return 1;
		}
		else
			return 0;                                   // player is not in barney's crosshair
}