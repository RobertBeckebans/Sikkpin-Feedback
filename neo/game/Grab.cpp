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

// ---> sikk - Object Manipulation

#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"
#include "Misc.h"

#define MAX_DRAG_TRACE_DISTANCE			80.0f
#define TRACE_BOUNDS_SIZE				3.0f
#define MAX_HOLD_DISTANCE				96.0f
#define MIN_HOLD_DISTANCE				48.0f
#define DRAG_FAIL_LEN					64.0f
#define FAIL_TIME						1000
#define THROW_TIME						500
#define	THROW_SCALE						20000
#define MAX_PICKUP_VELOCITY				1000 * 1000
#define MAX_PICKUP_SIZE					96.0f

/*
===============================================================================

	Allows entities to be dragged through the world with physics.

===============================================================================
*/

CLASS_DECLARATION( idEntity, idGrabEntity )
END_CLASS

/*
==============
idGrabEntity::idGrabEntity
==============
*/
idGrabEntity::idGrabEntity()
{
	Clear();
}

/*
==============
idGrabEntity::~idGrabEntity
==============
*/
idGrabEntity::~idGrabEntity()
{
	StopDrag( owner, true );
}

/*
==============
idGrabEntity::Clear
==============
*/
void idGrabEntity::Clear()
{
	dragEnt			= NULL;
	owner			= NULL;
	id				= 0;
	oldUcmdFlags	= 0;
	dragFailTime	= 0;
	lastThrownTime	= 0;
	localPlayerPoint.Zero();
	saveGravity.Zero();
}
/*
==============
idGrabEntity::StartDrag
==============
*/
void idGrabEntity::StartDrag( idPlayer* player, idEntity* grabEnt, int id )
{
	if( grabEnt && grabEnt->GetPhysics()->GetBounds().GetRadius() < MAX_PICKUP_SIZE &&
			grabEnt->GetPhysics()->GetLinearVelocity().LengthSqr() < MAX_PICKUP_VELOCITY )
	{

		dragFailTime = gameLocal.time + FAIL_TIME;

		oldUcmdFlags = player->usercmd.flags;

		// This is the new object to drag around
		dragEnt = grabEnt;

		// Get the current physics object to manipulate
		idPhysics* phys = grabEnt->GetPhysics();

		// Turn off gravity on object
		saveGravity = phys->GetGravity();
		phys->SetGravity( vec3_origin );

		// hold it directly in front of player, distance depends on object size
		float lerp = 1.0f - ( grabEnt->GetPhysics()->GetBounds().GetRadius() / MAX_HOLD_DISTANCE );
		lerp *= lerp;
		float holdDist = lerp * MIN_HOLD_DISTANCE + ( 1.0f - lerp ) * MAX_HOLD_DISTANCE;
		localPlayerPoint = ( player->firstPersonViewAxis[0] * holdDist ) * player->firstPersonViewAxis.Transpose();

		// Start up the Force_Drag to bring it in
		drag.Init( g_dragDamping.GetFloat() );
		drag.SetPhysics( phys, id, player->firstPersonViewOrigin + localPlayerPoint * player->firstPersonViewAxis );

		player->StartSoundShader( declManager->FindSound( "use_grab" ), SND_CHANNEL_VOICE, 0, false, NULL );
	}
}

/*
==============
idGrabEntity::StopDrag
==============
*/
void idGrabEntity::StopDrag( idPlayer* player, bool drop )
{
	if( dragEnt.IsValid() )
	{
		idEntity* ent = dragEnt.GetEntity();

		// If a cinematic has started, allow dropped object to think in cinematics
		if( gameLocal.inCinematic )
		{
			ent->cinematic = true;
		}

		// Restore Gravity
		ent->GetPhysics()->SetGravity( saveGravity );

		// If the object isn't near its goal, just drop it in place.
		if( drop || drag.GetDistanceToGoal() > DRAG_FAIL_LEN )
		{
			ent->GetPhysics()->SetLinearVelocity( vec3_origin );
		}
		else  	// throw the object forward
		{
			ent->ApplyImpulse( player, 0, ent->GetPhysics()->GetOrigin(), player->firstPersonViewAxis[0] * THROW_SCALE );
			player->StartSoundShader( declManager->FindSound( "use_throw" ), SND_CHANNEL_VOICE, 0, false, NULL );
		}
		// Remove the Force_Drag's control of the entity
		drag.RemovePhysics( ent->GetPhysics() );

		lastThrownTime = gameLocal.time + THROW_TIME;
	}

	dragEnt = NULL;
}

/*
==============
idGrabEntity::Update
==============
*/
void idGrabEntity::Update( idPlayer* player )
{
	trace_t trace;
//	idEntity *newEnt;

	owner = player;

	if( lastThrownTime > gameLocal.time )
	{
		prevViewAngles = player->viewAngles;
		return;
	}

	bool valid = dragEnt.IsValid();

	// Check if object being held has been removed or player is dead
	if( valid && dragEnt.GetEntity()->IsHidden() || player->health <= 0 )
	{
		StopDrag( player, true );
		prevViewAngles = player->viewAngles;
		return;
	}

	// attack throws object
	if( valid && player->usercmd.buttons & BUTTON_ATTACK )
	{
		StopDrag( player, false );
		prevViewAngles = player->viewAngles;
		return;
	}

	// if there is an entity selected for dragging
	if( dragEnt.GetEntity() )
	{
		idPhysics* entPhys = dragEnt.GetEntity()->GetPhysics();
		idVec3 goalPos;

		// Check if the player is standing on the object
		idBounds	playerBounds;
		idBounds	objectBounds = entPhys->GetAbsBounds();
		idVec3		newPoint = player->GetPhysics()->GetOrigin();

		// create a bounds at the players feet
		playerBounds.Clear();
		playerBounds.AddPoint( newPoint );
		newPoint.z -= 1.0f;
		playerBounds.AddPoint( newPoint );
		playerBounds.ExpandSelf( 8.0f );

		// If it intersects the object bounds, then drop it
		if( playerBounds.IntersectsBounds( objectBounds ) )
		{
			StopDrag( player, true );
			prevViewAngles = player->viewAngles;
			return;
		}

		idAngles ang = entPhys->GetAxis().ToAngles();
		ang.yaw += player->viewAngles.yaw - prevViewAngles.yaw;
		if( ang.yaw > 180.0f )
		{
			ang.yaw -= 360.0f;
		}
		else if( ang.yaw < -180.0f )
		{
			ang.yaw += 360.0f;
		}
		entPhys->SetAxis( ang.ToMat3() );

		// Set and evaluate drag force
		goalPos = player->firstPersonViewOrigin + localPlayerPoint * player->firstPersonViewAxis;

		drag.SetGoalPosition( goalPos );
		drag.Evaluate( gameLocal.time );

		// If the object is stuck away from its intended position for more than 500ms, let it go.
		if( drag.GetDistanceToGoal() > DRAG_FAIL_LEN )
		{
			if( dragFailTime < gameLocal.time )
			{
				StopDrag( player, true );
				prevViewAngles = player->viewAngles;
				return;
			}
		}
		else
		{
			dragFailTime = gameLocal.time + 1000;
		}
	}
	prevViewAngles = player->viewAngles;
}
// <--- sikk - Object Manipulation