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
//
// Misc utility code
//
#ifndef ENGINECALLBACK_H
#include "enginecallback.h"
#endif

// Keeps clutter down a bit, when using a float as a bit-vector
#define SetBits(flBitVector, bits)		((flBitVector) = (int)(flBitVector) | (bits))
#define ClearBits(flBitVector, bits)	((flBitVector) = (int)(flBitVector) & ~(bits))
#define FBitSet(flBitVector, bit)		((int)(flBitVector) & (bit))

// Makes these more explicit, and easier to find
#define FILE_GLOBAL static
#define DLL_GLOBAL

// Until we figure out why "const" gives the compiler problems, we'll just have to use
// this bogus "empty" define to mark things as constant.
#define CONSTANT

typedef enum
{
	TS_AT_TOP,
	TS_AT_BOTTOM,
	TS_GOING_UP,
	TS_GOING_DOWN
} TOGGLE_STATE;

// More explicit than "int"
typedef int EOFFSET;

// In case it's not alread defined
typedef int BOOL;

// In case this ever changes
#define M_PI			3.14159265358979323846
#define SF_LIGHT_START_OFF		1

// Keeps clutter down a bit, when declaring external entity/global method prototypes
#define DECLARE_GLOBAL_METHOD(MethodName) \
		extern void DLLEXPORT MethodName( void )
#define GLOBAL_METHOD(funcname)					void DLLEXPORT funcname(void)

// This is the glue that hooks .MAP entity class names to our CPP classes
// The _declspec forces them to be exported by name so we can do a lookup with GetProcAddress()
// The function is used to intialize / allocate the object for the entity
#define LINK_ENTITY_TO_CLASS(mapClassName,DLLClassName) \
	extern "C" _declspec( dllexport ) void mapClassName( entvars_t *pev ); \
	void mapClassName( entvars_t *pev ) { GetClassPtr( (DLLClassName *)pev ); }


//
// Conversion among the three types of "entity", including identity-conversions.
//
inline edict_t		*ENT(const entvars_t *pev)		{ return pev->pContainingEntity; }
inline edict_t		*ENT(edict_t *pent)				{ return pent; }
inline edict_t		*ENT(EOFFSET eoffset)			{ return (*g_engfuncs.pfnPEntityOfEntOffset)(eoffset); }
inline EOFFSET		OFFSET(EOFFSET eoffset)			{ return eoffset; }
inline EOFFSET		OFFSET(const edict_t *pent)		{ return (*g_engfuncs.pfnEntOffsetOfPEntity)(pent); }
inline EOFFSET		OFFSET(entvars_t *pev)			{ return OFFSET(ENT(pev)); }
inline entvars_t	*VARS(entvars_t *pev)			{ return pev; }
inline entvars_t	*VARS(edict_t *pent)			{ return (*g_engfuncs.pfnGetVarsOfEnt)(pent); }
inline entvars_t*	VARS(EOFFSET eoffset)			{ return VARS(ENT(eoffset)); }
inline int			ENTINDEX(int pEdict)		{ return (*g_engfuncs.pfnIndexOfEdict)(pEdict); }

// Testing the three types of "entity" for nullity
#define eoNullEntity 0
inline BOOL FNullEnt(EOFFSET eoffset)			{ return eoffset == 0; }
inline BOOL FNullEnt(const edict_t* pent)	{ return pent == NULL || FNullEnt(OFFSET(pent)); }
inline BOOL FNullEnt(entvars_t* pev)				{ return pev == NULL || FNullEnt(OFFSET(pev)); }

// Testing strings for nullity
#define iStringNull 0
#define		BLOOD_COLOR_RED		(BYTE)247
inline BOOL FStringNull(int iString)			{ return iString == iStringNull; }

// Misc. Prototypes
extern void			UTIL_SetSize			(entvars_t* pev, Vector vecMin, Vector vecMax);
extern float		UTIL_VecToYaw			(const float* vector);
extern Vector		UTIL_VecToAngles		(const float* vector);
extern void			UTIL_MoveToOrigin		(edict_t* pent, const Vector &vecGoal, float flDist, int iMoveType);
extern void			UTIL_MoveToGoal(edict_t* pent, float flDist);
extern edict_t*		UTIL_FindEntityInSphere	(const float* vec1, float radius);
extern void			UTIL_MakeVectors		(const Vector& vecAngles);
extern void			UTIL_SetOrigin			(entvars_t* pev, Vector vecOrigin);
extern void			UTIL_EmitAmbientSound	(Vector vecOrigin, const char* samp, float vol, float attenuation);
extern void			UTIL_TraceLine			(const Vector& vecStart, const Vector& vecEnd, int ignoremonsters, edict_t* pentIgnore, TraceResult* ptr);
extern Vector		UTIL_GetAimVector		(edict_t* pent, float flSpeed);
extern void			UTIL_ParticleEffect		(const Vector& vecOrigin, const Vector& vecDirection, ULONG ulColor, ULONG ulCount);
extern int			UTIL_PointContents		(const Vector& vec);
extern void         SetMovedir              (entvars_t* pev);
inline float		UTIL_RandomFloat		( float flLow, float flHigh)
{
	float num;

	num = (rand() & 0x7fff) / ((float)0x7fff);

	return num * ( flHigh - flLow ) + flLow;
}
inline int		UTIL_RandomLong(int flLow, int flHigh)
{
	return flLow + rand() % (flHigh - flLow + 1);
}

extern DLL_GLOBAL const Vector g_vecZero;


#define AMBIENT_SOUND_EVERYWHERE		1
#define AMBIENT_SOUND_NOT_LOOPING		2

#define VEC_DUCK_HULL_MIN		Vector(-16, -16, -18 )
#define VEC_DUCK_HULL_MAX		Vector( 16,  16,  18)
#define VEC_DUCK_VIEW			Vector( 0, 0, 12 )

#define VEC_HULL_MIN		Vector(-16, -16, -36)
#define VEC_HULL_MAX		Vector( 16,  16,  36)
#define VEC_BARNEY_HULL_MIN	Vector( -18, -18, 0 )
#define VEC_BARNEY_HULL_MAX	Vector( 18, 18, 72 )
#define VEC_SCIENTIST_HULL_MIN	Vector( -16, -16, 0 )
#define VEC_SCIENTIST_HULL_MAX	Vector( 16, 16, 72 )
#define VEC_HUMAN_HULL_DUCK	Vector( 16, 16, 36 )

#define VEC_VIEW			Vector( 0, 0, 28 )

#define SVC_TEMPENTITY		23
#define SVC_INTERMISSION	30
#define SVC_CDTRACK			32
#define SVC_WEAPONANIM		35
#define SVC_ROOMTYPE		37
#define SVC_ADDANGLE	    38			// [vec3] add this angle to the view angle
#define SVC_NEWUSERMSG      39
#define	SVC_DIRECTOR		51

// Misc useful
inline BOOL FStrEq(const char*sz1, const char*sz2)
	{ return (strcmp(sz1, sz2) == 0); }
inline BOOL FClassnameIs(edict_t* pent, const char* szClassname)
	{ return FStrEq(STRING(VARS(pent)->classname), szClassname); }
inline BOOL FClassnameIs(entvars_t* pev, const char* szClassname)
	{ return FStrEq(STRING(pev->classname), szClassname); }

class CBaseEntity;