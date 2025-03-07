//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#if !defined EDICT_H
#define EDICT_H
#ifdef _WIN32
#pragma once
#endif
#define	MAX_ENT_LEAFS	48

#include "progdefs.h"

struct edict_s
{
	int	free;
	link_t		area;				// linked to a division node or leaf
	int serialnumber;
	
	int			num_leafs;
	short		leafnums[MAX_ENT_LEAFS];

	void*		pvPrivateData;		// Alloced and freed by engine, used by DLLs	
	entvars_t	v;					// C exported fields from progs

	// other fields from progs come immediately after
};

#endif