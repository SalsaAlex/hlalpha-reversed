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

===== subs.cpp ========================================================

  frequently used global functions

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "doors.h"
#include "studio.h"

// These are the new entry points to entities.


class CNullEntity : public CBaseEntity
{
public:
	void Spawn(void);
};


// Null Entity, remove on startup
void CNullEntity::Spawn(void)
{
	REMOVE_ENTITY(ENT(pev));
}


LINK_ENTITY_TO_CLASS(info_null, CNullEntity);
LINK_ENTITY_TO_CLASS(info_player_deathmatch,CBaseEntity);
LINK_ENTITY_TO_CLASS(info_player_start,CBaseEntity);
LINK_ENTITY_TO_CLASS(info_landmark, CPointEntity);

// Landmark class
void CPointEntity :: Spawn( void )
{
	pev->solid = SOLID_BBOX;
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
}

void CBaseEntity::SUB_Remove( void )
{
	REMOVE_ENTITY(ENT(pev));
}

// Convenient way to explicitly do nothing (passed to functions that require a method)
void CBaseEntity :: SUB_DoNothing( void )
{
}

void CBaseDelay::SUB_UseTargets(entvars_t *pActivator)
{
	if (m_flDelay != 0)
	{
		CBaseDelay* pTemp = GetClassPtr((CBaseDelay*)NULL);
		pTemp->pev->classname = ALLOC_STRING("DelayedUse");
		pTemp->pev->nextthink = pev->pSystemGlobals->time + m_flDelay;
		pTemp->SetThink(&CBaseDelay::DelayThink);
		pTemp->m_iszKillTarget = m_iszKillTarget;
		pTemp->m_flDelay = 0; // prevent "recursion"
		pTemp->pev->target = pev->target;
	}
	else if (m_iszKillTarget)
	{
		edict_t* pentKillTarget = NULL;

		pentKillTarget = FIND_ENTITY_BY_STRING(ENT(pentKillTarget), "targetname", STRING(m_iszKillTarget));
		while (!FNullEnt(pentKillTarget))
		{
			REMOVE_ENTITY(ENT(pentKillTarget));
		}
	}
	else if (!FStringNull(pev->target))
	{
		globalvars_t *v9 = pgv;
		int v10 = 0;
		int v11 = v9->self;
		edict_t* pentTarget = NULL;
		edict_t* nextTarget = NULL;
		for (;;)
		{
			pentTarget = FIND_ENTITY_BY_STRING(nextTarget, "targetname", STRING(pev->target));
			nextTarget = pentTarget;
			if (FNullEnt(pentTarget))
				break;
			CBaseEntity* pTarget = CBaseEntity::Instance(pentTarget);
			if (pTarget)
				pTarget->Use(pActivator);
		}
	}
}

void CBaseDelay::DelayThink(void)
{
	//SUB_UseTargets(pActivator, 0);
	SUB_UseTargets(pev);
	REMOVE_ENTITY(ENT(pev));
}

void CBaseToggle::LinearMoveDone(void)
{
	Vector delta = m_vecFinalDest - pev->origin;
	float error = delta.Length();
	if (error > 0.03125)
	{
		LinearMove(m_vecFinalDest, 100);
		return;
	}

	UTIL_SetOrigin(pev, m_vecFinalDest);
	pev->velocity = g_vecZero;
	pev->nextthink = -1;
	if (m_pfnCallWhenMoveDone)
		(this->*m_pfnCallWhenMoveDone)();
}

void CBaseToggle::LinearMove(Vector	vecDest, float flSpeed)
{
	m_vecFinalDest = vecDest;

	// Already there?
	if (vecDest == pev->origin)
	{
		LinearMoveDone();
		return;
	}

	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDest - pev->origin;

	// divide vector length by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to LinearMoveDone when dest is reached
	pev->nextthink = pev->ltime + flTravelTime;
	SetThink(&CBaseToggle::LinearMoveDone);

	// scale the destdelta vector by the time spent traveling to get velocity
	pev->velocity = vecDestDelta / flTravelTime;
}

/*
=============
AngularMove

calculate pev->velocity and pev->nextthink to reach vecDest from
pev->origin traveling at flSpeed
Just like LinearMove, but rotational.
===============
*/
void CBaseToggle::AngularMove(Vector vecDestAngle, float flSpeed)
{

	m_vecFinalAngle = vecDestAngle;

	// Already there?
	if (vecDestAngle == pev->angles)
	{
		AngularMoveDone();
		return;
	}

	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDestAngle - pev->angles;

	// divide by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to AngularMoveDone when dest is reached
	pev->nextthink = pev->ltime + flTravelTime;
	SetThink(&CBaseToggle::AngularMoveDone);

	// scale the destdelta vector by the time spent traveling to get velocity
	pev->avelocity = vecDestDelta / flTravelTime;
}


/*
============
After rotating, set angle to exact final angle, call "move done" function
============
*/
void CBaseToggle::AngularMoveDone(void)
{
	pev->angles = m_vecFinalAngle;
	pev->avelocity = g_vecZero;
	pev->nextthink = -1;
	if (m_pfnCallWhenMoveDone)
		(this->*m_pfnCallWhenMoveDone)();
}

void CBaseToggle::AxisDir(entvars_t* pev)
{
	if (FBitSet(pev->spawnflags, SF_DOOR_ROTATE_Z))
		pev->movedir = Vector(0, 0, 1);	// around z-axis
	else if (FBitSet(pev->spawnflags, SF_DOOR_ROTATE_X))
		pev->movedir = Vector(1, 0, 0);	// around x-axis
	else
		pev->movedir = Vector(0, 1, 0);		// around y-axis
}


float CBaseToggle::AxisDelta(int flags, const Vector& angle1, const Vector& angle2)
{
	if (FBitSet(flags, SF_DOOR_ROTATE_Z))
		return angle1.z - angle2.z;

	if (FBitSet(flags, SF_DOOR_ROTATE_X))
		return angle1.x - angle2.x;

	return angle1.y - angle2.y;
}

void CBaseMonster::MonsterInit()
{
	WalkMonsterStart();
}

void CBaseMonster::WalkMonsterStart()
{
	if (pev->pSystemGlobals->deathmatch)
	{
		REMOVE_ENTITY(ENT(pev));
		return;
	}
	pev->origin.z += 1;
	DROP_TO_FLOOR(ENT(pev)); //for some reason some monsters dissappear when this                        
							//line executes ?????????
	if (!WALK_MOVE(ENT(pev), 0, 0))
	{
		ALERT(at_warning, "Monster %s stuck in wall--level design error", STRING(pev->classname));
		pev->effects = 1;
	}
	pev->view_ofs.x = 0;
	pev->view_ofs.y = 0;
	pev->view_ofs.z = 64;
	pev->takedamage = DAMAGE_AIM;
	pev->ideal_yaw = pev->angles.y;
	SetBits(pev->flags, FL_MONSTER);
	SetThink(&CBaseMonster::CallMonsterThink)
	edict_t *player = FIND_ENTITY_BY_STRING(NULL, "classname", "player");
	pev->goalentity = OFFSET(player); //player hasnt spawned in yet but whatever
	entvars_t *goal = VARS(pev->goalentity);
	Vector goaltest;
	goaltest.x = goal->origin.x - pev->origin.x;
	goaltest.y = goal->origin.y - pev->origin.y;
	goaltest.z = goal->origin.z - pev->origin.z;
	pev->ideal_yaw = UTIL_VecToYaw(goaltest);
	if (!FClassnameIs(VARS(pev->goalentity), "path_corner"))
		ALERT(at_aiconsole, "WalkMonsterStart--monster's initial goal '%s' is not a path_corner", STRING(pev->target));
	m_iActivity = ACT_WALK;
	pev->nextthink += UTIL_RandomFloat(0.0, 0.5) + 0.5;
}

void CBaseMonster::CallMonsterThink()
{
	entvars_t *enemy = NULL;
	float distance = NULL;
	pev->nextthink = pgv->time + 0.1;
	SetActivity(m_iActivity);
	StudioFrameAdvance(0.1);
	//ResetSequenceInfo(0.1); // test
	pev->goalentity = OFFSET(FIND_ENTITY_BY_STRING(NULL, "classname", "player"));
	if (pev->health > 0)
		m_iActivity = 4;

	if (pev->radsuit_finished < pgv->time)
	{
		Alert();
		pev->radsuit_finished = pgv->time + UTIL_RandomFloat(1, 4);
	}
	switch (m_iActivity)
	{
	case 1:
	case 2:
	case 3:
		//if (pgv->time > nextidle)
		//	Idle();
		//if (variable)
		//int random = rand() % 10;
		//if (random)
		//{
		//	if (random == 1)
		//		m_iActivity = 3;
		//	else
		//		m_iActivity = 1;
		// }
		//else
		//	m_iActivity = 2;
		break;
	case 4:
		UTIL_MoveToOrigin(ENT(pev), VARS(pev->goalentity)->origin, m_flGroundSpeed * 3, 1);
		//  the line above causes the screen to blink and monsters dissappear
		//  and a bunch of other funky stuff i literally
		//  do not know why
		//ALERT(at_console, "%f, %f,%f\n", VARS(pev->goalentity)->origin.x, VARS(pev->goalentity)->origin.y, VARS(pev->goalentity)->origin.z);
		break;
	case 5:
		CHANGE_YAW(ENT(pev));
		CheckEnemy(enemy, distance);
		break;
	case 6:
		CHANGE_YAW(ENT(pev));
		if (!CheckEnemy(enemy, distance))
		{
			//v25 = this[248];
			//if ((v25 & 2) != 0 && (v25 & 1) != 0)
			//	*((_DWORD*)this + 32) = ACT_AIM2;
		}
	case 7:
		CHANGE_YAW(ENT(pev));
		if (!CheckEnemy(enemy, distance) && WALK_MOVE(ENT(pev), pev->ideal_yaw, 15))
		{
			m_iActivity = ACT_RUN;
		}
	case 27:
		break;
	case 29:
	case 30:
		CHANGE_YAW(ENT(pev));
		break;
	case 31:
		if (!CheckEnemy(enemy, distance))
			m_iActivity = 6;
		break;
	case 35:
	case 36:
	case 37:
	case 38:
		CHANGE_YAW(ENT(pev));
		if (m_fSequenceFinished)
		{
			pev->framerate = 0;
			pev->solid = SOLID_NOT;
			m_iActivity = 42;
			SetThink(NULL);
		}
		break;
	case 42:
		SetThink(NULL);
		break;
	}
	if (Classify() != 5)
	{
		if (!FBitSet(pev->spawnflags, 1))
		{
			//pev->enemy = OFFSET(FIND_CLIENT_IN_PVS());
			//pev->goalentity = pev->enemy;
			//goal_origin = VARS(pev->enemy)->origin;
			//Alert();
		}
	}
}

void GetSequenceInfo(void* pmodel, entvars_t* pev, float* pflFrameRate, float* pflGroundSpeed)
{
	studiohdr_t* pstudiohdr;

	pstudiohdr = (studiohdr_t*)pmodel;
	if (!pstudiohdr)
		return;

	mstudioseqdesc_t* pseqdesc;

	if (pev->sequence >= pstudiohdr->numseq)
	{
		*pflFrameRate = 0.0;
		*pflGroundSpeed = 0.0;
		return;
	}

	pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	if (pseqdesc->numframes > 1)
	{
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = sqrt(pseqdesc->linearmovement[0] * pseqdesc->linearmovement[0] + pseqdesc->linearmovement[1] * pseqdesc->linearmovement[1] + pseqdesc->linearmovement[2] * pseqdesc->linearmovement[2]);
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}

void CBaseAnimating::StudioFrameAdvance(float flInterval)
{
	if (pev->animtime)
		pev->frame = (pgv->time - pev->animtime) * pev->framerate * m_flFrameRate + pev->frame;
	pev->animtime = pgv->time;
	if (pev->frame < 0.0 || pev->frame >= 256.0)
		pev->frame += (int)(pev->frame / 256.0) * -256.0;
	float flEnd = pev->frame + flInterval * m_flFrameRate * pev->framerate;

	m_fSequenceFinished = FALSE;
	if (flEnd >= 256 || flEnd <= 0.0)
		m_fSequenceFinished = TRUE;
}

void CBaseAnimating::ResetSequenceInfo(float flInterval)
{
	void* pmodel = GET_MODEL_PTR(ENT(pev));

	GetSequenceInfo(pmodel, pev, &m_flFrameRate, &m_flGroundSpeed);
	pev->animtime = pgv->time;
	pev->framerate = 1;
	m_fSequenceFinished = FALSE;
	m_flGroundSpeed *= flInterval;
}

void CBaseAnimating::DispatchAnimEvents(float flInterval)
{
	//HandleAnimEvent(GET_MODEL_PTR(ENT(pev)), pev, flInterval);
}