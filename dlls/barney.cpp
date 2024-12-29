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

===== barney.cpp ========================================================

  frEEze

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"

class CBarney : public CBaseMonster
{
public:
	virtual void Spawn();
	virtual int Classify() { return 5; };
	virtual void SetActivity(int activity);
	void Barney_Pissed();
	virtual void Pain(int a2);
	virtual void Die();
	virtual int CheckEnemy(int a2, float a3);
};

LINK_ENTITY_TO_CLASS(monster_barney, CBarney);

void CBarney::Spawn()
{
	PRECACHE_SOUND("barney/ba_attack1.wav");
	PRECACHE_SOUND("barney/ba_attack2.wav");
	PRECACHE_SOUND("barney/ba_die1.wav");
	PRECACHE_SOUND("barney/ba_die2.wav");
	PRECACHE_SOUND("barney/ba_die3.wav");
	PRECACHE_SOUND("barney/ba_die4.wav");
	PRECACHE_SOUND("barney/ba_pain1.wav");
	PRECACHE_MODEL("models/barney.mdl");
	SET_MODEL(ENT(pev), "models/barney.mdl");
	UTIL_SetSize(pev, VEC_BARNEY_HULL_MIN, VEC_BARNEY_HULL_MAX);
	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	pev->effects = 0;
	pev->health = 7;
	pev->yaw_speed = 8;
	pev->sequence = 17;
	//unknownvariable = 1;
	m_bloodColor = 70;
	//unknownvariable3 = 384;
	m_flFrameRate = 128;
	pev->nextthink += UTIL_RandomFloat(0, 0.5) + 0.5;
	SetThink(&CBarney::MonsterInit);
}

void CBarney::SetActivity(int activity)
{
    int activitynum = NULL;
    switch (activity)
    {
    case 1:
        activitynum = 0;
        break;
    case 2:
        activitynum = 1;
        break;
    case 3:
        activitynum = 2;
        break;
    case 4:
        activitynum = 3;
        break;
    case 6:
        activitynum = 9;
        break;
    case 7:
        activitynum = 9;
        break;
    case 8:
        activitynum = 4;
        break;
    case 9:
        activitynum = 4;
        break;
    case 10:                                    // player interacts with barney
        //v4 = pev->origin - player->origin);
        //v5 = v4 * v4;
        //v6 = *(float*)(*((_DWORD*)this + 1) + 48) - *(float*)(*((_DWORD*)this + 79) + 48);
        //v7 = *(float*)(*((_DWORD*)this + 1) + 40) - *(float*)(*((_DWORD*)this + 79) + 40);
        //v10 = v4.normalize;
        //if (this[65] * 2.0 >= v10)
        //{
        //    if (this[65] < (double)v10)
        //        v3 = 3;
        //    else
        //        v3 = 0;
        //}
        //else
        //{
        activitynum = 4;
        //}
        break;
    case 29:
        activitynum = 4;
        break;
    case 30:
        activitynum = 10;
        break;
    case 35:
    case 38:
        activitynum = 14;
        break;
    case 36:
        activitynum = 15;
        break;
    case 37:
        activitynum = 13;
        break;
    default:
        ALERT(at_console, "Barney's monster state is bogus: %d", activity);
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
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            return;
        default:
            ALERT(at_console, "Bogus Barney anim: %d", activitynum);
            m_flFrameRate = 0.0;
            m_flGroundSpeed = 0.0;
            break;
        }
    }
}

void CBarney::Barney_Pissed()
{
    EMIT_SOUND(ENT(pev), CHAN_VOICE, "barney/ba_attack1.wav", 1, ATTN_NORM);
    //*(_DWORD*)(this + 128) = 6;
    //*(float*)(this + 272) = pev->pSystemGlobals->time + 1;
}

void CBarney::Pain(int a2)
{
    EMIT_SOUND(ENT(pev), CHAN_VOICE, "barney/ba_pain1.wav", 1, ATTN_NORM);
    Barney_Pissed();
}

void CBarney::Die()
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
        //get_death_type( 0);
    }
    else
    {
        //get_death_type( 1);
    }
}

int CBarney::CheckEnemy(int a2, float a3)
{
    /*
    if (a3 <= 64 && sub_1000BCC0(a2))
    {
        this[34] = 7;
        this[3] = barney_shoot;
        return 1;
    }
    else if (checkenemyoncrosshairs((int)this, (int)a2) && a3 <= (double)flt_1002ECA4)
    {
        this[34] = 7;                               // probably barneys ammo?
        this[3] = barney_shoot;                     // shoot player
        return 1;
    }
    else
    {
        return 0;                                   // player is not in barney's crosshair
    }
    */
    return 0;
}