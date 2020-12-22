/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

CLASS_DECLARATION( idForce, idForce_Grab )
END_CLASS


/*
================
idForce_Grab::Save
================
*/
void idForce_Grab::Save( idSaveGame* savefile ) const
{
	savefile->WriteFloat( damping );
	savefile->WriteVec3( goalPosition );
	savefile->WriteFloat( distanceToGoal );
	savefile->WriteInt( id );
}

/*
================
idForce_Grab::Restore
================
*/
void idForce_Grab::Restore( idRestoreGame* savefile )
{
	//Note: Owner needs to call set physics
	savefile->ReadFloat( damping );
	savefile->ReadVec3( goalPosition );
	savefile->ReadFloat( distanceToGoal );
	savefile->ReadInt( id );
}

/*
================
idForce_Grab::idForce_Grab
================
*/
idForce_Grab::idForce_Grab( void )
{
	damping			= 0.5f;
	physics			= NULL;
	id				= 0;
}

/*
================
idForce_Grab::~idForce_Grab
================
*/
idForce_Grab::~idForce_Grab( void )
{
}

/*
================
idForce_Grab::Init
================
*/
void idForce_Grab::Init( float damping )
{
	if( damping >= 0.0f && damping < 1.0f )
	{
		this->damping = damping;
	}
}

/*
================
idForce_Grab::SetPhysics
================
*/
void idForce_Grab::SetPhysics( idPhysics* phys, int id, const idVec3& goal )
{
	this->physics = phys;
	this->id = id;
	this->goalPosition = goal;
}

/*
================
idForce_Grab::SetGoalPosition
================
*/
void idForce_Grab::SetGoalPosition( const idVec3& goal )
{
	this->goalPosition = goal;
}

/*
=================
idForce_Grab::GetDistanceToGoal
=================
*/
float idForce_Grab::GetDistanceToGoal( void )
{
	return distanceToGoal;
}

/*
================
idForce_Grab::Evaluate
================
*/
void idForce_Grab::Evaluate( int time )
{
	if( !physics )
	{
		return;
	}

	idVec3	forceDir, v, objectCenter;
	float	forceAmt;
	float	mass = physics->GetMass( id );

	objectCenter = physics->GetAbsBounds( id ).GetCenter();

	forceDir = goalPosition - objectCenter;
	distanceToGoal = forceDir.Normalize();

	float temp = distanceToGoal;
	if( temp > 12.0f && temp < 32.0f )
	{
		temp = 32.0f;
	}
	forceAmt = ( 1000.0f * mass ) + ( 500.0f * temp * mass );

	if( forceAmt / mass > 120000.0f )
	{
		forceAmt = 120000.0f * mass;
	}
	physics->AddForce( id, objectCenter, forceDir * forceAmt );

	if( distanceToGoal < 128.0f )
	{
		v = physics->GetLinearVelocity( id );
		if( distanceToGoal <= 1.0f )
		{
			physics->SetLinearVelocity( vec3_origin, id );
		}
		else
		{
			physics->SetLinearVelocity( v * damping, id );
		}
	}

	physics->SetAngularVelocity( vec3_origin, id );
	//if ( distanceToGoal < 16.0f ) {
	//	v = physics->GetAngularVelocity( id );
	//	if ( v.LengthSqr() > Square( 8 ) ) {
	//		physics->SetAngularVelocity( v * 0.99999f, id );
	//	}
	//}
}

/*
================
idForce_Grab::RemovePhysics
================
*/
void idForce_Grab::RemovePhysics( const idPhysics* phys )
{
	if( physics == phys )
	{
		physics = NULL;
	}
}

// <--- sikk - Object Manipulation