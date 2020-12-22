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
/*
===============================================================================

	Grab Object - Class to extend idWeapon to include functionality for
						manipulating physics objects.

===============================================================================
*/

class idGrabEntity : public idEntity
{
public:
	CLASS_PROTOTYPE( idGrabEntity );

	idGrabEntity( void );
	~idGrabEntity( void );

	void					Clear();

	void					Update( idPlayer* player );
	void					StartDrag( idPlayer* player, idEntity* grabEnt, int id );
	void					StopDrag( idPlayer* player, bool drop );

	idEntity* 				GetGrabEntity( void ) const
	{
		return dragEnt.GetEntity();
	}
	int						GetThrownTime( void ) const
	{
		return lastThrownTime;
	}
	int						SetThrownTime( int time )
	{
		lastThrownTime = time;
	}

private:
	idEntityPtr<idEntity>	dragEnt;			// entity being dragged
	idForce_Grab			drag;
	idPlayer*				owner;

	idVec3					saveGravity;
	idVec3					localPlayerPoint;	// dragged point in player space
	idAngles				prevViewAngles;		// Holds previous frame's player view angles

	int						id;					// id of body being dragged
	int						oldUcmdFlags;
	int						dragFailTime;
	int						lastThrownTime;

};
// <--- sikk - Object Manipulation