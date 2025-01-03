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

===== scientist.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

enum scientist_anims
{
    idle1 = 0,
    idle2,
    idle3,
    walk,
    walktostand,
    run,
    stairsup,
    flinch,
    dieviolent,
    diebackward,
    dieforward
};

class CScientist : public CBaseMonster
{
public:
	virtual void Spawn();
	virtual int Classify() { return 5; };
	virtual void SetActivity(int activity);
	virtual void Pain(float flDamage);
	virtual void Die();
};

void CScientist::Spawn()
{
	PRECACHE_SOUND("barney/ba_die1.wav");
	PRECACHE_SOUND("barney/ba_die2.wav");
	PRECACHE_SOUND("barney/ba_die3.wav");
	PRECACHE_SOUND("barney/ba_die4.wav");
	PRECACHE_SOUND("barney/ba_pain1.wav");
	PRECACHE_MODEL("models/scientist.mdl");
	SET_MODEL(ENT(pev), "models/scientist.mdl");
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 64));
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	pev->effects = 0;
	pev->health = 7;
	pev->yaw_speed = 8;
	pev->sequence = 17;
	//unknownvariable = 1;
	m_bloodColor = 70;
	//unknownvariable3 = 384;
	//unknownvariable4 = 128;
    pev->nextthink = pev->pSystemGlobals->time + 0.5;
    if (pev->body == -1)
    {
        pev->body = rand() % 3;
    }
	SetThink(&CScientist::MonsterInit);
}

void CScientist::SetActivity(int activity)
{
    int activitynum = NULL;
    switch (activity)
    {
    case ACT_IDLE1:
        activitynum = idle1;
        break;
    case ACT_IDLE2:
        activitynum = idle2;
        break;
    case ACT_IDLE3:
        activitynum = idle3;
        break;
    case ACT_WALK:
        activitynum = walk;
        break;
    case 6:
        activitynum = ACT_AIM;
        break;
    case ACT_RUN:
        activitynum = run;
        break;
    case ACT_RUN2:
        activitynum = run;
        break;
    case ACT_FOLLOWPLAYER:
        //v4 = pev->origin - player->origin);
        //v5 = v4 * v4;
        //v6 = *(float*)(*((_DWORD*)this + 1) + 48) - *(float*)(*((_DWORD*)this + 79) + 48);
        //v7 = *(float*)(*((_DWORD*)this + 1) + 40) - *(float*)(*((_DWORD*)this + 79) + 40);
        //v10 = v4.normalize;
        //if (this[65] * 2.0 >= v10)
        //{
        //    if (this[65] < (double)v10)
        //        v3 = walk;
        //    else
        //        v3 = idle1;
        //}
        //else
        //{
        activitynum = run;
        //}
        break;
    case 29:
        activitynum = flinch;
        break;
    case 35:
    case 38:
        activitynum = dieforward;
        break;
    case 36:
        activitynum = dieviolent;
        break;
    case 37:
        activitynum = diebackward;
        break;
    default:
        ALERT(at_console, "Scientist's monster state is bogus: %d", activity);
        break;
    }
    if (pev->sequence != activitynum)
    {
        pev->sequence = activitynum;
        if ((activitynum != 4 && activitynum != 3) || (pev->sequence != 4 && pev->sequence != 4))
            pev->frame = 0;
        ResetSequenceInfo(0.1);
        switch (activitynum)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            return;
        default:
            ALERT(at_console, "Bogus Scientist anim: %d", activitynum);
            //this[66] = 0.0;
            //this[67] = 0.0;
            break;
        }
    }
}

void CScientist::Pain(float flDamage)
{
    EMIT_SOUND(ENT(pev), CHAN_VOICE, "barney/ba_pain1.wav", 1, ATTN_NORM);
}

void CScientist::Die()
{
    if (pev->health > -30)
    {
        switch (rand() % 4)
        {
        case 0:
            EMIT_SOUND(ENT(pev), CHAN_VOICE, "barney/ba_die1.wav", 1, ATTN_NORM);
            break;
        case 1:
            EMIT_SOUND(ENT(pev), CHAN_VOICE, "barney/ba_die2.wav", 1, ATTN_NORM);
            break;
        case 2:
            EMIT_SOUND(ENT(pev), CHAN_VOICE, "barney/ba_die3.wav", 1, ATTN_NORM);
            break;
        case 3:
            EMIT_SOUND(ENT(pev), CHAN_VOICE, "barney/ba_die4.wav", 1, ATTN_NORM);
            break;
        default:
            break;
        }
        m_iActivity = 35;
        pev->ideal_yaw = pev->angles.y;
        pev->nextthink = pev->pSystemGlobals->time + 0.1;
        SetThink(&CBaseMonster::CallMonsterThink);
        SetActivity(m_iActivity);
    }
    else 
    {
        pev->origin.z += 1;
        if (FBitSet(pev->flags, FL_ONGROUND))
        {
            ClearBits(pev->flags, FL_ONGROUND);
            pev->velocity.x = 0;
            pev->velocity.y = 0;
            pev->velocity.z = 200;
        }
        m_iActivity = 36;
        pev->ideal_yaw = pev->angles.y;
        pev->nextthink = pev->pSystemGlobals->time + 0.1;
        SetThink(&CBaseMonster::CallMonsterThink);
        SetActivity(m_iActivity);
    }
}