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

===== houndeye.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

enum houndeye_anims
{
	idle1 = 0,
	idle2,
	idle3,
	attack,
	run,
	die
};

class CHoundEye : public CBaseMonster
{
public:
	virtual void Spawn();
	virtual int Classify() { return 3; };
	virtual void SetActivity(int activity);
	virtual void Alert();
	virtual void Pain(float flDamage);
	virtual void Die();
	virtual void Idle();
	virtual int CheckEnemy(int a2, float a3);
	//virtual void Blast();
};

void CHoundEye::Spawn()
{
	PRECACHE_SOUND("houndeye/he_alert1.wav");
	PRECACHE_SOUND("houndeye/he_die1.wav");
	PRECACHE_SOUND("houndeye/he_die2.wav");
	PRECACHE_SOUND("houndeye/he_die3.wav");
	PRECACHE_SOUND("houndeye/he_idle1.wav");
	PRECACHE_SOUND("houndeye/he_idle2.wav");
	PRECACHE_SOUND("houndeye/he_idle3.wav");
	PRECACHE_SOUND("houndeye/he_idle4.wav");
	PRECACHE_SOUND("houndeye/he_attack1.wav");
	PRECACHE_SOUND("houndeye/he_attack2.wav");
	PRECACHE_SOUND("houndeye/he_attack3.wav");
	PRECACHE_SOUND("houndeye/he_pain1.wav");
	PRECACHE_SOUND("houndeye/he_pain2.wav");
	PRECACHE_SOUND("houndeye/he_pain4.wav");
	PRECACHE_SOUND("houndeye/he_pain5.wav");
	PRECACHE_MODEL("models/houndeye.mdl");
	SET_MODEL(ENT(pev), "models/houndeye.mdl");
	UTIL_SetSize(pev, Vector(-18, -18, 0), Vector(18, 18, 36));
	//*(_DWORD*)(this + 256) = 1124073472;
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	pev->effects = 0;
	pev->health = 15;
	pev->yaw_speed = 10;
	pev->sequence = 6;
	m_bloodColor = -61;
	//*(_DWORD*)(this + 304) = 1;
	pev->nextthink += 1;
	SetThink(&CHoundEye::MonsterInit);
}

int CHoundEye::CheckEnemy(int a2, float a3)
{
	/*
	 if ( !checkenemyoncrosshairs((int)this, a2) || a3 > (double)flt_1002ECA4 )
		return 0;
	this[34] = 7;
	this[3] = Houndeye_blast;
	return 1;
	*/
	return 0;
}

void CHoundEye::Die()
{
	if (pev->health > -30)
	{
		float sound = UTIL_RandomFloat(0.0, 1.0);
		if (sound <= 0.33)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_die1.wav", 1, ATTN_NORM);
		if (sound <= 0.66)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_die2.wav", 1, ATTN_NORM);
		else
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_die3.wav", 1, ATTN_NORM);
	}
	m_iActivity = 35;
	pev->ideal_yaw = pev->angles.y;
	pev->nextthink = pev->pSystemGlobals->time + 0.1;
	SetThink(&CBaseMonster::CallMonsterThink);
	SetActivity(m_iActivity);
}

void CHoundEye::Pain(float flDamage)
{
	float sound = UTIL_RandomFloat(0.0, 1.0);
	if (sound <= 0.25)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_pain1.wav", 1, ATTN_NORM);
	}
	else if (sound <= 0.5)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_pain2.wav", 1, ATTN_NORM);
	}
	else if (sound <= 0.75)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_pain4.wav", 1, ATTN_NORM);
	}
	else
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_pain5.wav", 1, ATTN_NORM);
	}
	//v7 = this[32];
	//if (v7 == 1 || v7 == 4)
		//Alert();
}

void CHoundEye::Idle()
{
	float sound = UTIL_RandomFloat(0.0, 1.0);
	if (sound <= 0.25)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_idle1.wav", 1, ATTN_NORM);
	}
	else if (sound <= 0.5)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_idle2.wav", 1, ATTN_NORM);
	}
	else if (sound <= 0.75)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_idle3.wav", 1, ATTN_NORM);
	}
	else
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_idle4.wav", 1, ATTN_NORM);
	}
	//nextidlesound = pev->pSystemGlobals->time + UTIL_RandomFloat(0, 2) + 1
}

void CHoundEye::Alert()
{
	/*
	if (this[76] <= 1u)
	{
		v3 = sub_1000D580(this, 512);
		if (v3)
		{
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_alert1.wav", 1, ATTN_NORM);
			v4 = this[80];
			this[76] += v3;
			while (this[1] != v4)
			{
				*(_DWORD*)(GET_PRIVATE(*(_DWORD*)(v4 + 520)) + 128) = 8;
				*(_DWORD*)(GET_PRIVATE(*(_DWORD*)(v4 + 520)) + 304) = this[76];
				v4 = *(_DWORD*)(GET_PRIVATE(*(_DWORD*)(v4 + 520)) + 320);
			}
			ALERT(at_console, "group of: %d\n", this[76]);
			v5 = this[1];
			this[32] = 8;
			this[78] = 1;
			this[76] = v3;
			v6 = VARS(*(_DWORD*)(v5 + 376));
			ENT(v6);
		}
		else
		{
			ALERT(at_console, "No Squad");
			this[32] = 7;
		}
	}
	*/
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "houndeye/he_alert1.wav", 1, ATTN_NORM);
}

void CHoundEye::SetActivity(int activity)
{
	int activitynum = NULL;
	switch (activity)
	{
	case 1:
		activitynum = idle2;
		break;
	case 2:
		activitynum = idle1;
		break;
	case 3:
		activitynum = idle3;
		break;
	case 4:
		activitynum = run;
		break;
	case 6:
		activitynum = idle2;
		break;
	case 7:
		activitynum = idle2;
		break;
	case 8:
		activitynum = run;
		break;
	case 9:
		activitynum = run;
		break;
	case 29:
		activitynum = attack;
		break;
	case 30:
		activitynum = attack;
		break;
	case 35:
		activitynum = die;
		break;
	default:
		ALERT(at_console, "Houndeye's monster state is bogus: %d", activity);
		break;
	}
	if (pev->sequence != activitynum)
	{
		pev->sequence = activitynum;
		pev->frame = 0;
		ResetSequenceInfo(0.1);
	}
}