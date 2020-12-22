// sikk---> ThirdPerson Crosshair

#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"
#include "PlayerCursor.h"


/*
===============
idPlayerCursor::idPlayerCursor
===============
*/
idPlayerCursor::idPlayerCursor()
{
	cursorHandle   = -1;
	created = false;
}

/*
===============
idPlayerCursor::~idPlayerCursor
===============
*/
idPlayerCursor::~idPlayerCursor()
{
	FreeCursor();
}

/*
===============
idPlayerCursor::FreeCursor
===============
Post: tells the game render world to free the cross hair entity, sets the cursor
handle to -1 and sets created to false
*/
void idPlayerCursor::FreeCursor( void )
{
	if( cursorHandle != -1 )
	{
		gameRenderWorld->FreeEntityDef( cursorHandle );
		cursorHandle = -1;
		created = false;
	}
}

/*
===============
idPlayerCursor::Draw
===============
*/
void idPlayerCursor::Draw( const idVec3& origin, const idMat3& axis, const char* material )
{
	idPlayer* localPlayer = gameLocal.GetLocalPlayer();
	trace_t		tr;
	float		distance = 60;
	float		length;

	//detemine the point at which the weapon is aiming
	idAngles angle = axis.ToAngles();
	idVec3 endPos = ( origin + ( angle.ToForward() * 120000.0f ) );
	gameLocal.clip.TracePoint( tr, origin, endPos, MASK_SHOT_RENDERMODEL, localPlayer );
	endPos = tr.endpos;

	//find the distance from the camera to the point at which the weapon is aiming
	idMat3 cameraAxis = localPlayer->GetRenderView()->viewaxis;
	idVec3 cameraOrigin = localPlayer->GetRenderView()->vieworg;
	idVec3 vectorLength = endPos - cameraOrigin;
	length = vectorLength.Length();
	length = distance / length;

	//linearly interpolate 5 feet between the camera position and the point at which the weapon is aiming
	endPos.Lerp( cameraOrigin, endPos, length );

	if( !CreateCursor( localPlayer, endPos, cameraAxis, material ) )
	{
		UpdateCursor( localPlayer,  endPos, cameraAxis );
	}
}

/*
===============
idPlayerCursor::CreateCursor
===============
*/
bool idPlayerCursor::CreateCursor( idPlayer* player, const idVec3& origin, const idMat3& axis, const char* material )
{
	const char* mtr =  material;
	int out = cursorHandle;

	if( out >= 0 )
	{
		return false;
	}

	FreeCursor();
	memset( &renderEnt, 0, sizeof( renderEnt ) );

	renderEnt.origin					= origin;
	renderEnt.axis						= axis;
	renderEnt.shaderParms[ SHADERPARM_RED ]				= 1.0f;
	renderEnt.shaderParms[ SHADERPARM_GREEN ]			= 1.0f;
	renderEnt.shaderParms[ SHADERPARM_BLUE ]			= 1.0f;
	renderEnt.shaderParms[ SHADERPARM_ALPHA ]			= 1.0f;
	renderEnt.shaderParms[ SHADERPARM_SPRITE_WIDTH ]	= 2.5f;
	renderEnt.shaderParms[ SHADERPARM_SPRITE_HEIGHT ]	= 2.5f;
	renderEnt.hModel					= renderModelManager->FindModel( "_sprite" );
	renderEnt.callback					= NULL;
	renderEnt.numJoints					= 0;
	renderEnt.joints					= NULL;
	renderEnt.customSkin				= 0;
	renderEnt.noShadow					= true;
	renderEnt.noSelfShadow				= true;
	renderEnt.customShader				= declManager->FindMaterial( mtr );
	renderEnt.referenceShader			= 0;
	renderEnt.bounds					= renderEnt.hModel->Bounds( &renderEnt );
	renderEnt.suppressSurfaceInViewID	= -8;

	cursorHandle = gameRenderWorld->AddEntityDef( &renderEnt );

	return false;
}

/*
===============
idPlayerCursor::UpdateCursor
===============
*/
void idPlayerCursor::UpdateCursor( idPlayer* player,  const idVec3& origin, const idMat3& axis )
{
	assert( cursorHandle >= 0 );
	renderEnt.origin = origin;
	renderEnt.axis   = axis;
	gameRenderWorld->UpdateEntityDef( cursorHandle, &renderEnt );
}
// <---sikk