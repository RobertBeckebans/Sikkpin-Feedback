
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"


CLASS_DECLARATION( idPhysics_Static, idPhysics_Liquid )
END_CLASS

/*
===============================================================================

	idPhysics_Liquid

===============================================================================
*/

/*
================
idPhysics_Liquid::idPhysics_Liquid
================
*/
idPhysics_Liquid::idPhysics_Liquid()
{
	// initializes to a water-like liquid
	density = 0.001043f;
	viscosity = 3.0f;
}

/*
================
idPhysics_Liquid::~idPhysics_Liquid
================
*/
idPhysics_Liquid::~idPhysics_Liquid()
{
}

/*
================
idPhysics_Liquid::Save
================
*/
void idPhysics_Liquid::Save( idSaveGame* savefile ) const
{
	savefile->WriteFloat( density );
	savefile->WriteFloat( viscosity );
	savefile->WriteVec3( minSplashVelocity );
	savefile->WriteVec3( minWaveVelocity );
}

/*
================
idPhysics_Liquid::Restore
================
*/
void idPhysics_Liquid::Restore( idRestoreGame* savefile )
{
	savefile->ReadFloat( density );
	savefile->ReadFloat( viscosity );
	savefile->ReadVec3( minSplashVelocity );
	savefile->ReadVec3( minWaveVelocity );
}

/*
================
idPhysics_Liquid::Splash

	Causes the liquid to splash but only if the velocity is greater than minSplashVelocity
================
*/
void idPhysics_Liquid::Splash( idEntity* other, float volume, impactInfo_t& info, trace_t& collision )
{
	collision.c.entityNum = other->entityNumber;
	self->Collide( collision, info.velocity );
}

/*
================
idPhysics_Liquid::GetDepth

	Gets the depth of a point in the liquid.  Returns -1 -1 -1 if the object is not in the liquid
================
*/
idVec3 idPhysics_Liquid::GetDepth( const idVec3& point ) const
{
	if( !isInLiquid( point ) )
	{
		return 	idVec3( -1.0f, -1.0f, -1.0f );
	}

	const idBounds& bounds = GetBounds();
	idVec3 gravityNormal = GetGravityNormal();
	idVec3 depth = ( bounds[ 1 ] + GetOrigin() - point ) * gravityNormal * gravityNormal;

	return depth;
}