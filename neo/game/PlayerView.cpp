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

#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"

const int IMPULSE_DELAY = 150;

// ---> sikk - Portal Sky Box
static int MakePowerOfTwo( int num )
{
	int pot;
	for( pot = 1; pot < num; pot <<= 1 ) {}
	return pot;
}
// <--- sikk - Portal Sky Box

/*
==============
idPlayerView::idPlayerView
==============
*/
idPlayerView::idPlayerView()
{
	memset( screenBlobs, 0, sizeof( screenBlobs ) );
	memset( &view, 0, sizeof( view ) );
	player					= NULL;

	tunnelMaterial			= declManager->FindMaterial( "postProcess/tunnel" );
	bloodSprayMaterial		= declManager->FindMaterial( "postProcess/bloodspray1" );
	bfgMaterial				= declManager->FindMaterial( "postProcess/bfgvision" );
// sikk - removed multiplayer
	//lagoMaterial			= declManager->FindMaterial( LAGO_MATERIAL, false );

// ---> sikk - PostProcess Effects
	blackMaterial			= declManager->FindMaterial( "_black" );
	whiteMaterial			= declManager->FindMaterial( "_white" );
	currentRenderMaterial	= declManager->FindMaterial( "_currentRender" );
	scratchMaterial			= declManager->FindMaterial( "_scratch" );
	depthMaterial			= declManager->FindMaterial( "render/depth" );
	edgeAAMaterial			= declManager->FindMaterial( "postProcess/edgeAA" );
	hdrLumBaseMaterial		= declManager->FindMaterial( "postProcess/hdrLumBase" );
	hdrLumAverageMaterial	= declManager->FindMaterial( "postProcess/hdrLumAverage" );
	hdrLumAdaptedMaterial	= declManager->FindMaterial( "postProcess/hdrLumAdapted" );
	hdrBrightPassMaterial	= declManager->FindMaterial( "postProcess/hdrBrightPass" );
	hdrGlareMaterial		= declManager->FindMaterial( "postProcess/hdrGlare" );
	hdrFinalMaterial		= declManager->FindMaterial( "postProcess/hdrFinal" );
	bloomMaterial			= declManager->FindMaterial( "postProcess/bloom" );
	ssaoMaterial			= declManager->FindMaterial( "postProcess/ssao" );
	sunShaftsMaterial		= declManager->FindMaterial( "postProcess/sunShafts" );
	dofMaterial				= declManager->FindMaterial( "postProcess/dof" );
	colorGradingMaterial	= declManager->FindMaterial( "postProcess/colorGrading" );
	explosionFXMaterial		= declManager->FindMaterial( "postProcess/explosionFX" );
	filmgrainMaterial		= declManager->FindMaterial( "postProcess/filmgrain" );
	vignettingMaterial		= declManager->FindMaterial( "postProcess/vignetting" );
	megashieldMaterial		= declManager->FindMaterial( "postProcess/megashield" );
	underwaterMaterial		= declManager->FindMaterial( "postProcess/underwater" );
	screenWipeMaterial		= declManager->FindMaterial( "postProcess/screenwipe" );
	bDepthRendered			= false;
// <--- sikk - PostProcess Effects

// ---> sikk - Explosion FX PostProcess
	gameLocal.explosionOrigin.Zero();
	gameLocal.explosionRadius = 0;
	gameLocal.explosionDamage = 0;
	gameLocal.explosionTime = 0;
// <--- sikk - Explosion FX PostProcess

	bfgVision				= false;
	dvFinishTime			= 0;
	kickFinishTime			= 0;
	kickAngles.Zero();
	lastDamageTime			= 0.0f;
	fadeTime				= 0;
	fadeRate				= 0.0;
	fadeFromColor.Zero();
	fadeToColor.Zero();
	fadeColor.Zero();
	shakeAng.Zero();

	ClearEffects();
}

/*
==============
idPlayerView::Save
==============
*/
void idPlayerView::Save( idSaveGame* savefile ) const
{
	int i;
	const screenBlob_t* blob;

	blob = &screenBlobs[ 0 ];
	for( i = 0; i < MAX_SCREEN_BLOBS; i++, blob++ )
	{
		savefile->WriteMaterial( blob->material );
		savefile->WriteFloat( blob->x );
		savefile->WriteFloat( blob->y );
		savefile->WriteFloat( blob->w );
		savefile->WriteFloat( blob->h );
		savefile->WriteFloat( blob->s1 );
		savefile->WriteFloat( blob->t1 );
		savefile->WriteFloat( blob->s2 );
		savefile->WriteFloat( blob->t2 );
		savefile->WriteInt( blob->finishTime );
		savefile->WriteInt( blob->startFadeTime );
		savefile->WriteFloat( blob->driftAmount );
	}

	savefile->WriteInt( dvFinishTime );
	savefile->WriteMaterial( scratchMaterial );
	savefile->WriteInt( kickFinishTime );
	savefile->WriteAngles( kickAngles );
	savefile->WriteBool( bfgVision );

	savefile->WriteMaterial( tunnelMaterial );
	savefile->WriteMaterial( bloodSprayMaterial );
	savefile->WriteMaterial( bfgMaterial );
	savefile->WriteFloat( lastDamageTime );

	savefile->WriteVec4( fadeColor );
	savefile->WriteVec4( fadeToColor );
	savefile->WriteVec4( fadeFromColor );
	savefile->WriteFloat( fadeRate );
	savefile->WriteInt( fadeTime );

	savefile->WriteAngles( shakeAng );

	savefile->WriteObject( player );
	savefile->WriteRenderView( view );
}

/*
==============
idPlayerView::Restore
==============
*/
void idPlayerView::Restore( idRestoreGame* savefile )
{
	int i;
	screenBlob_t* blob;

	blob = &screenBlobs[ 0 ];
	for( i = 0; i < MAX_SCREEN_BLOBS; i++, blob++ )
	{
		savefile->ReadMaterial( blob->material );
		savefile->ReadFloat( blob->x );
		savefile->ReadFloat( blob->y );
		savefile->ReadFloat( blob->w );
		savefile->ReadFloat( blob->h );
		savefile->ReadFloat( blob->s1 );
		savefile->ReadFloat( blob->t1 );
		savefile->ReadFloat( blob->s2 );
		savefile->ReadFloat( blob->t2 );
		savefile->ReadInt( blob->finishTime );
		savefile->ReadInt( blob->startFadeTime );
		savefile->ReadFloat( blob->driftAmount );
	}

	savefile->ReadInt( dvFinishTime );
	savefile->ReadMaterial( scratchMaterial );
	savefile->ReadInt( kickFinishTime );
	savefile->ReadAngles( kickAngles );
	savefile->ReadBool( bfgVision );

	savefile->ReadMaterial( tunnelMaterial );
	savefile->ReadMaterial( bloodSprayMaterial );
	savefile->ReadMaterial( bfgMaterial );
	savefile->ReadFloat( lastDamageTime );

	savefile->ReadVec4( fadeColor );
	savefile->ReadVec4( fadeToColor );
	savefile->ReadVec4( fadeFromColor );
	savefile->ReadFloat( fadeRate );
	savefile->ReadInt( fadeTime );

	savefile->ReadAngles( shakeAng );

	savefile->ReadObject( reinterpret_cast<idClass*&>( player ) );
	savefile->ReadRenderView( view );
}

/*
==============
idPlayerView::SetPlayerEntity
==============
*/
void idPlayerView::SetPlayerEntity( idPlayer* playerEnt )
{
	player = playerEnt;
}

/*
==============
idPlayerView::ClearEffects
==============
*/
void idPlayerView::ClearEffects()
{
	lastDamageTime = MS2SEC( gameLocal.time - 99999 );

	dvFinishTime = ( gameLocal.time - 99999 );
	kickFinishTime = ( gameLocal.time - 99999 );

	for( int i = 0; i < MAX_SCREEN_BLOBS; i++ )
	{
		screenBlobs[i].finishTime = gameLocal.time;
	}

	fadeTime = 0;
	bfgVision = false;
}

/*
==============
idPlayerView::GetScreenBlob
==============
*/
screenBlob_t* idPlayerView::GetScreenBlob()
{
	screenBlob_t* oldest = &screenBlobs[0];

	for( int i = 1 ; i < MAX_SCREEN_BLOBS ; i++ )
	{
		if( screenBlobs[i].finishTime < oldest->finishTime )
		{
			oldest = &screenBlobs[i];
		}
	}
	return oldest;
}

/*
==============
idPlayerView::DamageImpulse

LocalKickDir is the direction of force in the player's coordinate system,
which will determine the head kick direction
==============
*/
void idPlayerView::DamageImpulse( idVec3 localKickDir, const idDict* damageDef )
{
	// keep shotgun from obliterating the view
	if( lastDamageTime > 0.0f && SEC2MS( lastDamageTime ) + IMPULSE_DELAY > gameLocal.time )
	{
		return;
	}

	float dvTime = damageDef->GetFloat( "dv_time" );
	if( dvTime )
	{
		if( dvFinishTime < gameLocal.time )
		{
			dvFinishTime = gameLocal.time;
		}
		dvFinishTime += g_dvTime.GetFloat() * dvTime;
		// don't let it add up too much in god mode
		if( dvFinishTime > gameLocal.time + 5000 )
		{
			dvFinishTime = gameLocal.time + 5000;
		}
	}

	// head angle kick
	float kickTime = damageDef->GetFloat( "kick_time" );
	if( kickTime )
	{
		kickFinishTime = gameLocal.time + g_kickTime.GetFloat() * kickTime;

		// forward / back kick will pitch view
		kickAngles[0] = localKickDir[0];
		// side kick will yaw view
		kickAngles[1] = localKickDir[1] * 0.5f;
		// up / down kick will pitch view
		kickAngles[0] += localKickDir[2];
		// roll will come from side
		kickAngles[2] = localKickDir[1];

		float kickAmplitude = damageDef->GetFloat( "kick_amplitude" );
		if( kickAmplitude )
		{
			kickAngles *= kickAmplitude;
		}
	}

	// screen blob
	float blobTime = damageDef->GetFloat( "blob_time" );
	if( blobTime )
	{
		screenBlob_t* blob = GetScreenBlob();
		blob->startFadeTime = gameLocal.time;
		blob->finishTime = gameLocal.time + blobTime * g_blobTime.GetFloat();

		blob->driftAmount = 0.0f;	// sikk - Blood Spray Screen Effect - keeps damage blood splats from drifting

		const char* materialName = damageDef->GetString( "mtr_blob" );
		blob->material = declManager->FindMaterial( materialName );
		blob->x = damageDef->GetFloat( "blob_x" );
		blob->x += ( gameLocal.random.RandomInt() & 63 ) - 32;
		blob->y = damageDef->GetFloat( "blob_y" );
		blob->y += ( gameLocal.random.RandomInt() & 63 ) - 32;

		float scale = ( 256 + ( ( gameLocal.random.RandomInt() & 63 ) - 32 ) ) / 256.0f;
		blob->w = damageDef->GetFloat( "blob_width" ) * g_blobSize.GetFloat() * scale;
		blob->h = damageDef->GetFloat( "blob_height" ) * g_blobSize.GetFloat() * scale;
		blob->s1 = 0;
		blob->t1 = 0;
		blob->s2 = 1;
		blob->t2 = 1;
	}

	// save lastDamageTime for tunnel vision attenuation
	lastDamageTime = MS2SEC( gameLocal.time );
}

/*
==================
idPlayerView::AddBloodSpray

If we need a more generic way to add blobs then we can do that
but having it localized here lets the material be pre-looked up etc.
==================
*/
void idPlayerView::AddBloodSpray( float duration )
{
	if( duration <= 0 || bloodSprayMaterial == NULL || g_skipViewEffects.GetBool() )
	{
		return;
	}

// ---> sikk - Blood Spray Screen Effect
	// Use random material
	if( gameLocal.random.RandomFloat() < 0.5f )
	{
		bloodSprayMaterial = declManager->FindMaterial( "postProcess/bloodspray1" );
	}
	else
	{
		bloodSprayMaterial = declManager->FindMaterial( "postProcess/bloodspray2" );
	}
// <--- sikk - Blood Spray Screen Effect

	// visit this for chainsaw
	screenBlob_t* blob = GetScreenBlob();
	blob->startFadeTime = gameLocal.time;
	blob->finishTime = gameLocal.time + SEC2MS( duration );
	blob->material = bloodSprayMaterial;
	blob->x = ( gameLocal.random.RandomInt() & 63 ) - 32;
	blob->y = ( gameLocal.random.RandomInt() & 63 ) - 32;
	blob->driftAmount = 0.0f;// 0.5f + gameLocal.random.CRandomFloat() * 0.5f;	// sikk - No more drifting
	float scale = ( 256 + ( ( gameLocal.random.RandomInt() & 63 ) - 32 ) ) / 256.0f;
	blob->w = 640 * g_blobSize.GetFloat() * scale;	// sikk - This was "600". Typo?
	blob->h = 480 * g_blobSize.GetFloat() * scale;
	float s1 = 0.0f;
	float t1 = 0.0f;
	float s2 = 1.0f;
	float t2 = 1.0f;
// ---> sikk - No more drifting
	/*	if ( blob->driftAmount < 0.6 ) {
			s1 = 1.0f;
			s2 = 0.0f;
		} else if ( blob->driftAmount < 0.75 ) {
			t1 = 1.0f;
			t2 = 0.0f;
		} else if ( blob->driftAmount < 0.85 ) {
			s1 = 1.0f;
			s2 = 0.0f;
			t1 = 1.0f;
			t2 = 0.0f;
		}*/
	float f = gameLocal.random.CRandomFloat();
	if( f < 0.25 )
	{
		s1 = 1.0f;
		s2 = 0.0f;
	}
	else if( f < 0.5 )
	{
		t1 = 1.0f;
		t2 = 0.0f;
	}
	else if( f < 0.75 )
	{
		s1 = 1.0f;
		s2 = 0.0f;
		t1 = 1.0f;
		t2 = 0.0f;
	}
// <--- sikk - No more drifting
	blob->s1 = s1;
	blob->t1 = t1;
	blob->s2 = s2;
	blob->t2 = t2;
}

/*
==================
idPlayerView::WeaponFireFeedback

Called when a weapon fires, generates head twitches, etc
==================
*/
void idPlayerView::WeaponFireFeedback( const idDict* weaponDef, int primary )  	// sikk - Secondary Fire
{
	int recoilTime = ( primary ) ? weaponDef->GetInt( "recoilTime" ) : weaponDef->GetInt( "recoilTime_sec" );


	// don't shorten a damage kick in progress
	if( recoilTime && kickFinishTime < gameLocal.time )
	{
		idAngles angles;
		if( primary )
		{
			weaponDef->GetAngles( "recoilAngles", "5 0 0", angles );
		}
		else
		{
			weaponDef->GetAngles( "recoilAngles_sec", "5 0 0", angles );
		}
		kickAngles = angles;

		int	finish = gameLocal.time + g_kickTime.GetFloat() * recoilTime;
		kickFinishTime = finish;
	}
}

/*
===================
idPlayerView::CalculateShake
===================
*/
void idPlayerView::CalculateShake()
{
	idVec3	origin, matrix;
	float shakeVolume = gameSoundWorld->CurrentShakeAmplitudeForPosition( gameLocal.time, player->firstPersonViewOrigin );

	// shakeVolume should somehow be molded into an angle here
	// it should be thought of as being in the range 0.0 -> 1.0, although
	// since CurrentShakeAmplitudeForPosition() returns all the shake sounds
	// the player can hear, it can go over 1.0 too.
	shakeAng[0] = gameLocal.random.CRandomFloat() * shakeVolume;
	shakeAng[1] = gameLocal.random.CRandomFloat() * shakeVolume;
	shakeAng[2] = gameLocal.random.CRandomFloat() * shakeVolume;
}

/*
===================
idPlayerView::ShakeAxis
===================
*/
idMat3 idPlayerView::ShakeAxis() const
{
	return shakeAng.ToMat3();
}

/*
===================
idPlayerView::AngleOffset

  kickVector, a world space direction that the attack should
===================
*/
idAngles idPlayerView::AngleOffset() const
{
	idAngles ang;

	ang.Zero();

	if( gameLocal.time < kickFinishTime )
	{
		float offset = kickFinishTime - gameLocal.time;
		ang = kickAngles * offset * offset * g_kickAmplitude.GetFloat();
		for( int i = 0; i < 3; i++ )
		{
			ang[i] = idMath::ClampFloat( -70.0f, 70.0f, ang[i] );
		}
	}
	return ang;
}

/*
==================
idPlayerView::SingleView
==================
*/
void idPlayerView::SingleView( const renderView_t* view )
{
	// normal rendering
	if( !view )
	{
		return;
	}

// ---> sikk - PostProccess Scaling Fix
	if( screenHeight != renderSystem->GetScreenHeight() || screenWidth != renderSystem->GetScreenWidth() )
	{
		renderSystem->GetGLSettings( screenWidth, screenHeight );
		float f = MakePowerOfTwo( screenWidth );
		shiftScale.x = ( float )screenWidth / f;
		f = MakePowerOfTwo( screenHeight );
		shiftScale.y = ( float )screenHeight / f;
	}
// <--- sikk - PostProccess Scaling Fix

	// hack the shake in at the very last moment, so it can't cause any consistency problems
	hackedView = *view;
	hackedView.viewaxis = hackedView.viewaxis * ShakeAxis();

// ---> sikk - Portal Sky Box
	if( gameLocal.portalSkyEnt.GetEntity()  && gameLocal.IsPortalSkyAcive() && g_enablePortalSky.GetBool() )
	{
		renderView_t portalView = hackedView;
		portalView.vieworg = gameLocal.portalSkyEnt.GetEntity()->GetPhysics()->GetOrigin();

		// setup global fixup projection vars
		hackedView.shaderParms[4] = shiftScale.x;
		hackedView.shaderParms[5] = shiftScale.y;

		gameRenderWorld->RenderScene( &portalView );
		renderSystem->CaptureRenderToImage( "_portalRender" );

		hackedView.forceUpdate = true;	// FIX: for smoke particles not drawing when portalSky present
	}
// <--- sikk - Portal Sky Box

	gameRenderWorld->RenderScene( &hackedView );
}

/*
=================
idPlayerView::Flash

flashes the player view with the given color
=================
*/
void idPlayerView::Flash( idVec4 color, int time )
{
	Fade( idVec4( 0.0f, 0.0f, 0.0f, 0.0f ), time );
	fadeFromColor = colorWhite;
}

/*
=================
idPlayerView::Fade

used for level transition fades
assumes: color.w is 0 or 1
=================
*/
void idPlayerView::Fade( idVec4 color, int time )
{
	if( !fadeTime )
	{
		fadeFromColor.Set( 0.0f, 0.0f, 0.0f, 1.0f - color[ 3 ] );
	}
	else
	{
		fadeFromColor = fadeColor;
	}
	fadeToColor = color;

	if( time <= 0 )
	{
		fadeRate = 0;
		time = 0;
		fadeColor = fadeToColor;
	}
	else
	{
		fadeRate = 1.0f / ( float )time;
	}

	if( gameLocal.realClientTime == 0 && time == 0 )
	{
		fadeTime = 1;
	}
	else
	{
		fadeTime = gameLocal.realClientTime + time;
	}
}

/*
=================
idPlayerView::ScreenFade
=================
*/
void idPlayerView::ScreenFade()
{
	int msec = fadeTime - gameLocal.realClientTime;
	float t;

	if( msec <= 0 )
	{
		fadeColor = fadeToColor;
		if( fadeColor[ 3 ] == 0.0f )
		{
			fadeTime = 0;
		}
	}
	else
	{
		t = ( float )msec * fadeRate;
		fadeColor = fadeFromColor * t + fadeToColor * ( 1.0f - t );
	}

	if( fadeColor[ 3 ] != 0.0f )
	{
		renderSystem->SetColor4( fadeColor[ 0 ], fadeColor[ 1 ], fadeColor[ 2 ], fadeColor[ 3 ] );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, whiteMaterial );
	}
}

/*
===================
idPlayerView::RenderPlayerView
===================
*/
void idPlayerView::RenderPlayerView( idUserInterface* hud )
{
	const renderView_t* view = player->GetRenderView();

	// place the sound origin for the player
	gameSoundWorld->PlaceListener( view->vieworg, view->viewaxis, player->entityNumber + 1, gameLocal.time, hud ? hud->State().GetString( "location" ) : "Undefined" );

	SingleView( view );

	if( !g_skipViewEffects.GetBool() )
	{
		DoPostFX();
	}

	// if the objective system is up, don't draw hud
	player->DrawHUD( hud );

	if( fadeTime )
	{
		ScreenFade();
	}

// sikk - removed multiplayer
	//if ( net_clientLagOMeter.GetBool() && lagoMaterial && gameLocal.isClient ) {
	//	renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, 1.0f );
	//	renderSystem->DrawStretchPic( 10.0f, 380.0f, 64.0f, 64.0f, 0.0f, 0.0f, 1.0f, 1.0f, lagoMaterial );
	//}

	prevTime = gameLocal.time;	// sikk - update prevTime
}

// ---> sikk - PostProcess Effects
/*
===================
idPlayerView::DoPostFX
===================
*/
void idPlayerView::DoPostFX()
{
	bDepthRendered = false;

	if( r_useEdgeAA.GetBool() )
	{
		PostFX_EdgeAA();
	}
	else
	{
		renderSystem->CaptureRenderToImage( "_currentRender" );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, declManager->FindMaterial( "currentRender" ) );
	}

	if( r_useSSAO.GetBool() )
	{
		PostFX_SSAO();
	}

	if( r_useSunShafts.GetBool() )
	{
		PostFX_SunShafts();
	}

	if( r_useHDR.GetBool() )
	{
		PostFX_HDR();
	}

	if( r_useBloom.GetBool() && !r_useHDR.GetBool() )
	{
		PostFX_Bloom();
	}

	if( g_useExplosionFX.GetBool() )
	{
		PostFX_ExplosionFX();
	}

	if( r_useDepthOfField.GetBool() )
	{
		PostFX_DoF();
	}

	if( r_useColorGrading.GetBool() )
	{
		PostFX_ColorGrading();
	}

	if( player->underwater )
	{
		PostFX_Underwater();
	}

	PostFX_ScreenBlobs();

	if( bfgVision )
	{
		PostFX_BFGVision();
	}

	if( !gameLocal.inCinematic )
	{
		PostFX_TunnelVision();
	}

	if( r_useVignetting.GetBool() && !r_useHDR.GetBool() )	// HDR uses it's own vignette solution
	{
		PostFX_Vignetting();
	}

	if( r_useFilmgrain.GetBool() )
	{
		PostFX_Filmgrain();
	}

	if( player->GetInfluenceMaterial() || player->GetInfluenceEntity() )
	{
		PostFX_InfluenceVision();
	}

	if( g_doubleVision.GetBool() && gameLocal.time < dvFinishTime )
	{
		PostFX_DoubleVision();
	}

	if( player->PowerUpActive( MEGASHIELD ) )
	{
		PostFX_MegashieldVision();
	}

	if( player->doEndLevel )
	{
		PostFX_ScreenWipe();
	}

	// test a single material drawn over everything
// sikk - removed multiplayer
	if( g_testPostProcess.GetString()[0] /*&& !player->spectating*/ )
	{
		const idMaterial* mtr = declManager->FindMaterial( g_testPostProcess.GetString(), false );
		if( !mtr )
		{
			common->Printf( "Material not found.\n" );
			g_testPostProcess.SetString( "" );
		}
		else
		{
			renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, 1.0f );
			renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, mtr );
		}
	}
}

/*
===================
idPlayerView::RenderDepth
===================
*/
void idPlayerView::RenderDepth()
{
	if( bDepthRendered )
	{
		return;
	}
	//float parm[ 4 ];
	int	nWidth	= MakePowerOfTwo( renderSystem->GetScreenWidth() );
	int	nHeight	= MakePowerOfTwo( renderSystem->GetScreenHeight() );
	//parm[ 0 ] = (float)renderSystem->GetScreenWidth() / (float)nWidth;
	//parm[ 1 ] = (float)renderSystem->GetScreenHeight() / (float)nHeight;

	renderSystem->SetColor4( nWidth, nHeight, 1.0f / ( float )nWidth, 1.0f / ( float )nHeight );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, 1280, 720, 0.0f, 0.0f, 1.0f, 1.0f, depthMaterial );
	renderSystem->CaptureRenderToImage( "_depth" );


	//renderSystem->CropRenderSize( 1024, 512, true );
	//renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, shiftScale.y, shiftScale.x, 0.0f, declManager->FindMaterial( "_depth" ) );
	//renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, depthMaterial );
	//renderSystem->CaptureRenderToImage( "_depth" );

	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, declManager->FindMaterial( "normalRender" ) );
	renderSystem->CaptureRenderToImage( "_normal" );
//	renderSystem->UnCrop();

	bDepthRendered = true;
}

/*
===================
idPlayerView::PostFX_EdgeAA
===================
*/
void idPlayerView::PostFX_EdgeAA()
{
	renderSystem->CaptureRenderToImage( "_currentRender" );
	renderSystem->SetColor4( r_edgeAASampleScale.GetFloat(), r_edgeAAFilterScale.GetFloat(), 1.0f, r_useEdgeAA.GetFloat() );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, edgeAAMaterial );
}

/*
===================
idPlayerView::PostFX_HDR
===================
*/
void idPlayerView::PostFX_HDR()
{
	float	fElapsedTime	= MS2SEC( gameLocal.time - prevTime );
	int		nBloomWidth		= renderSystem->GetScreenWidth() / 4;
	int		nBloomHeight	= renderSystem->GetScreenHeight() / 4;
	int		nGlareWidth		= renderSystem->GetScreenWidth() / 8;
	int		nGlareHeight	= renderSystem->GetScreenHeight() / 8;

	// capture original scene image
	renderSystem->CaptureRenderToImage( "_currentRender" );

	// create lower res luminance map
	renderSystem->CropRenderSize( 256, 256, true, true );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, shiftScale.y, shiftScale.x, 0.0f, currentRenderMaterial );
	renderSystem->CaptureRenderToImage( "_hdrLum" );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrLumBaseMaterial );
	renderSystem->CaptureRenderToImage( "_hdrLum" );
	renderSystem->CaptureRenderToImage( "_hdrLumAvg" );
	renderSystem->UnCrop();

	// create average scene luminance map by using a 4x4 downsampling chain and box-filtering
	// Output will be a 1x1 pixel of the average luminance
	for( int i = 256; i > 1; i *= 0.5 )
	{
		renderSystem->CropRenderSize( i, i, true, true );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrLumAverageMaterial );
		renderSystem->CaptureRenderToImage( "_hdrLumAvg" );
		renderSystem->UnCrop();
	}

	// create adapted luminance map based on current average luminance and previous adapted luminance maps
	renderSystem->CropRenderSize( 2, 2, true, true );
	renderSystem->SetColor4( r_hdrAdaptationRate.GetFloat(), fElapsedTime, r_hdrLumThresholdMin.GetFloat(), r_hdrLumThresholdMax.GetFloat() );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrLumAdaptedMaterial );
	renderSystem->CaptureRenderToImage( "_hdrLumAdpt" );
	renderSystem->UnCrop();

	// perform bright pass filter on _currentRender for bloom/glare/flare textures
	if( r_hdrBloomScale.GetFloat() || r_hdrGlareScale.GetFloat() || r_hdrFlareScale.GetFloat() )
	{
		renderSystem->CropRenderSize( nBloomWidth, nBloomHeight, true, true );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, shiftScale.y, shiftScale.x, 0.0f, currentRenderMaterial );
		renderSystem->CaptureRenderToImage( "_hdrBloom" );
		renderSystem->SetColor4( r_hdrBloomMiddleGray.GetFloat(), r_hdrBloomWhitePoint.GetFloat(), r_hdrBloomThreshold.GetFloat(), r_hdrBloomOffset.GetFloat() );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrBrightPassMaterial );
		renderSystem->CaptureRenderToImage( "_hdrBloom" );
		renderSystem->CaptureRenderToImage( "_hdrFlare" );

		// create bloom texture
		for( int i = 0; i < 2; i++ )
		{
			renderSystem->SetColor4( r_hdrBloomSize.GetFloat(), 0.0f, 1.0f, 1.0f );
			renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrGlareMaterial );
			renderSystem->CaptureRenderToImage( "_hdrBloom" );
			renderSystem->SetColor4( 0.0f, r_hdrBloomSize.GetFloat(), 1.0, 1.0f );
			renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrGlareMaterial );
			renderSystem->CaptureRenderToImage( "_hdrBloom" );
		}
		renderSystem->UnCrop();
	}
	else
	{
		// bloom off (clear textures)
		renderSystem->CropRenderSize( 1, 1, true, true );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, blackMaterial );
		renderSystem->CaptureRenderToImage( "_hdrBloom" );
		renderSystem->CaptureRenderToImage( "_hdrFlare" );
		renderSystem->CaptureRenderToImage( "_hdrGlare" );
		renderSystem->UnCrop();
	}

	// create glare textures
	if( r_hdrGlareScale.GetFloat() )
	{
		// crop _hdrBloom1 for glare textures
		renderSystem->CropRenderSize( nGlareWidth, nGlareHeight, true, true );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f, 1.0f, 0.0f, declManager->FindMaterial( "_hdrBloom" ) );
		renderSystem->CaptureRenderToImage( "_hdrGlare" );
		for( int i = 1; i <= 3; i++ )
		{
			renderSystem->SetColor4( r_hdrGlareSize.GetFloat(), i, 1.0f, 2.0f );
			renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrGlareMaterial );
			renderSystem->CaptureRenderToImage( "_hdrGlare" );
		}
		renderSystem->UnCrop();
	}

	// create lens flare texture
	if( r_hdrFlareScale.GetFloat() )
	{
		renderSystem->CropRenderSize( nGlareWidth, nGlareHeight, true, true );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 0.0f, 0.0f, 1.0f, declManager->FindMaterial( "_hdrFlare" ) );
		renderSystem->CaptureRenderToImage( "_hdrFlare" );
		renderSystem->SetColor4( r_hdrFlareGamma.GetFloat(), 1.0f, 1.0f, 3.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrGlareMaterial );
		renderSystem->CaptureRenderToImage( "_hdrFlare" );
		renderSystem->SetColor4( r_hdrFlareSize.GetFloat(), 1.0f, 1.0f, 4.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrGlareMaterial );
		renderSystem->CaptureRenderToImage( "_hdrFlare" );
		renderSystem->SetColor4( DEG2RAD( r_hdrFlareSize.GetFloat() ), 1.0f, 1.0f, 5.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrGlareMaterial );
		renderSystem->CaptureRenderToImage( "_hdrFlare" );
		renderSystem->SetColor4( DEG2RAD( r_hdrFlareSize.GetFloat() ), 1.0f, 1.0f, 5.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrGlareMaterial );
		renderSystem->CaptureRenderToImage( "_hdrFlare" );
		renderSystem->UnCrop();
	}

	// blend bloom/glare/flare textures and capture to a single texture
	renderSystem->CropRenderSize( nBloomWidth, nBloomHeight, true, true );
	renderSystem->SetColor4( r_hdrBloomScale.GetFloat(), r_hdrGlareScale.GetFloat(), r_hdrFlareScale.GetFloat(), 6.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrGlareMaterial );
	renderSystem->CaptureRenderToImage( "_hdrGlare" );
	renderSystem->UnCrop();


	if( r_hdrDither.GetBool() )
	{
		float size = 16.0f * r_hdrDitherSize.GetFloat();
		renderSystem->SetColor4( renderSystem->GetScreenWidth() / size, renderSystem->GetScreenHeight() / size, 1.0f, -1.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrFinalMaterial );
		renderSystem->CaptureRenderToImage( "_hdrDither" );
	}
	else
	{
		renderSystem->CropRenderSize( 1, 1, true, true );
		renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, -2.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrFinalMaterial );
		renderSystem->CaptureRenderToImage( "_hdrDither" );
		renderSystem->UnCrop();
	}

	// perform final tone mapping
	renderSystem->SetColor4( r_hdrMiddleGray.GetFloat(), r_hdrWhitePoint.GetFloat(), r_hdrBlueShiftFactor.GetFloat(), r_useVignetting.GetBool() );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, hdrFinalMaterial );
}

/*
===================
idPlayerView::PostFX_Bloom
===================
*/
void idPlayerView::PostFX_Bloom()
{
	// determine bloom buffer size
	int nBufferSize = 32;
	for( int i = 0; i < r_bloomBufferSize.GetInteger() && i < 5; i++ )
	{
		nBufferSize <<= 1;
	}

	renderSystem->CaptureRenderToImage( "_currentRender" );

	// create bloom texture
	renderSystem->CropRenderSize( nBufferSize, nBufferSize, true, true );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, shiftScale.y, shiftScale.x, 0.0f, currentRenderMaterial );
	renderSystem->CaptureRenderToImage( "_bloom" );
	renderSystem->SetColor4( r_bloomGamma.GetFloat(), 1.0f, 1.0f, 0.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, bloomMaterial );
	renderSystem->CaptureRenderToImage( "_bloom" );

	for( int i = 0; i < r_bloomBlurIterations.GetInteger(); i++ )
	{
		renderSystem->SetColor4( r_bloomBlurScaleX.GetFloat(), 1.0f, 1.0f, 1.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, bloomMaterial );
		renderSystem->CaptureRenderToImage( "_bloom" );
		renderSystem->SetColor4( r_bloomBlurScaleY.GetFloat(), 1.0f, 1.0f, 2.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, bloomMaterial );
		renderSystem->CaptureRenderToImage( "_bloom" );
	}
	renderSystem->UnCrop();

	// blend original and bloom textures
	renderSystem->SetColor4( r_bloomScale.GetFloat(), 1.0f, 1.0f, 3.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, bloomMaterial );
}

/*
===================
idPlayerView::PostFX_SSAO
===================
*/
void idPlayerView::PostFX_SSAO()
{
	int	nWidth	= MakePowerOfTwo( renderSystem->GetScreenWidth() );/// 2.0f;
	int	nHeight	= MakePowerOfTwo( renderSystem->GetScreenHeight() );// / 2.0f;

	renderSystem->CaptureRenderToImage( "_currentRender" );

	//RenderDepth();

	renderSystem->CropRenderSize( nWidth, nHeight, true );

	// sample occlusion using our depth buffer
	renderSystem->SetColor4( r_ssaoRadius.GetFloat(), r_ssaoBias.GetFloat(), r_ssaoAmount.GetFloat(), 0.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, ssaoMaterial );
	renderSystem->CaptureRenderToImage( "_ssao" );
	// blur ssao buffer
	for( int i = 0; i < r_ssaoBlurQuality.GetInteger(); i++ )
	{
		renderSystem->SetColor4( r_ssaoBlurScale.GetFloat(), r_ssaoBlurEpsilon.GetFloat(), 1.0f, 1.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, ssaoMaterial );
		renderSystem->CaptureRenderToImage( "_ssao" );
	}
	renderSystem->UnCrop();

	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, declManager->FindMaterial( "currentRender" ) );
}

/*
===================
idPlayerView::PostFX_SunShafts
===================
*/
void idPlayerView::PostFX_SunShafts()
{
	idMat3 axis;
	idVec3 origin;
	idVec3 viewVector[3];
	player->GetViewPos( origin, axis );
	player->viewAngles.ToVectors( &viewVector[0], &viewVector[1], &viewVector[2] );

	idVec3 sunOrigin	= idVec3( r_sunOriginX.GetFloat(), r_sunOriginY.GetFloat(), r_sunOriginZ.GetFloat() ) ;
	idVec3 dist			= origin - sunOrigin;
	float length		= dist.Length();
	idVec3 sunVector	= dist / length;

	float VdotS[3];
	for( int i = 0; i < 3; i++ )
	{
		VdotS[i] = viewVector[i] * -sunVector;
	}
//	float sign = VdotS[0];
//	VdotS[0] = idMath::ClampFloat( 0.0f, 1.0f, VdotS[0] );
	idVec3 ndc;
	renderSystem->GlobalToNormalizedDeviceCoordinates( sunOrigin, ndc );
	ndc.x = ndc.x * 0.5 + 0.5;
	ndc.y = ndc.y * 0.5 + 0.5;

	int	nWidth	= MakePowerOfTwo( renderSystem->GetScreenWidth() );
	int	nHeight	= MakePowerOfTwo( renderSystem->GetScreenHeight() );
	renderSystem->CaptureRenderToImage( "_currentRender" );

	RenderDepth();

	renderSystem->CropRenderSize( nWidth, nHeight, true, true );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, shiftScale.y, shiftScale.x, 0.0f, currentRenderMaterial );
	renderSystem->CaptureRenderToImage( "_sunShafts" );

	renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, 2.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, sunShaftsMaterial );
	renderSystem->CaptureRenderToImage( "_sunShafts" );

	// blur textures
	for( int i = 0; i < r_sunShaftsQuality.GetInteger(); i++ )
	{
		renderSystem->SetColor4( r_sunShaftsSize.GetFloat(), ndc.x, ndc.y, VdotS[0] );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, sunShaftsMaterial );
		renderSystem->CaptureRenderToImage( "_sunShafts" );
	}
	renderSystem->UnCrop();

	// add mask to scene
	renderSystem->SetColor4( r_sunShaftsStrength.GetFloat(), r_sunShaftsMaskStrength.GetFloat(), 1.0f, 3.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, sunShaftsMaterial );
}

/*
===================
idPlayerView::PostFX_DoF
===================
*/
void idPlayerView::PostFX_DoF()
{
	if( gameLocal.inCinematic || r_useDepthOfField.GetInteger() > 1 )
	{
		int	nWidth	= MakePowerOfTwo( renderSystem->GetScreenWidth() );
		int	nHeight	= MakePowerOfTwo( renderSystem->GetScreenHeight() );

		renderSystem->CaptureRenderToImage( "_currentRender" );

		//RenderDepth();

		renderSystem->CropRenderSize( nWidth, nHeight, true, true );
		if( r_useDepthOfField.GetInteger() == 2 )
		{
			renderSystem->SetColor4( r_dofNear.GetInteger(), r_dofFocus.GetInteger(), r_dofFar.GetInteger(), 1.0f );
		}
		else if( gameLocal.inCinematic )
		{
			renderSystem->SetColor4( r_dofNear.GetInteger(), r_dofFocus.GetInteger(), r_dofFar.GetInteger(), 1.0f );    // don't blur in front of the focal plane for cinematics
		}
		else
		{
			renderSystem->SetColor4( player->focusDistance, 1.0f, 1.0f, 0.0f );
		}
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, dofMaterial );
		renderSystem->CaptureRenderToImage( "_dof" );
		renderSystem->UnCrop();

		// blur scene using our depth of field mask
		renderSystem->SetColor4( r_dofBlurScale.GetFloat(), r_dofBlurQuality.GetInteger(), 1.0f, 2.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, dofMaterial );
		if( r_dofBlurQuality.GetInteger() == 2 )
		{
			renderSystem->CaptureRenderToImage( "_currentRender" );
			renderSystem->SetColor4( r_dofBlurScale.GetFloat(), r_dofBlurQuality.GetInteger() + 2.0f, 1.0f, 2.0f );
			renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, dofMaterial );
		}
	}
}

/*
===================
idPlayerView::PostFX_ColorGrading
===================
*/
void idPlayerView::PostFX_ColorGrading()
{
	int	nWidth	= MakePowerOfTwo( renderSystem->GetScreenWidth() );
	int	nHeight	= MakePowerOfTwo( renderSystem->GetScreenHeight() );

	renderSystem->CaptureRenderToImage( "_currentRender" );

	// unsharp mask buffer
	renderSystem->CropRenderSize( nWidth, nHeight, true, true );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, shiftScale.y, shiftScale.x, 0.0f, currentRenderMaterial );
	renderSystem->CaptureRenderToImage( "_blurRender" );
	renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, 0.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, colorGradingMaterial );
	renderSystem->CaptureRenderToImage( "_blurRender" );
	renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, 1.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, colorGradingMaterial );
	renderSystem->CaptureRenderToImage( "_blurRender" );
	renderSystem->UnCrop();

	renderSystem->SetColor4( r_colorGradingParm.GetInteger(), r_colorGradingSharpness.GetFloat(), 1.0f, 2.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, colorGradingMaterial );
}

/*
===================
idPlayerView::PostFX_Underwater
===================
*/
void idPlayerView::PostFX_Underwater()
{
	int	nWidth	= MakePowerOfTwo( renderSystem->GetScreenWidth() );
	int	nHeight	= MakePowerOfTwo( renderSystem->GetScreenHeight() );

	renderSystem->CaptureRenderToImage( "_currentRender" );

	// unsharp mask buffer
	renderSystem->CropRenderSize( nWidth, nHeight, true, true );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, shiftScale.y, shiftScale.x, 0.0f, currentRenderMaterial );
	renderSystem->CaptureRenderToImage( "_blurRender" );
	renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, 0.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, underwaterMaterial );
	renderSystem->CaptureRenderToImage( "_blurRender" );
	renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, 1.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, underwaterMaterial );
	renderSystem->CaptureRenderToImage( "_blurRender" );
	renderSystem->UnCrop();

	renderSystem->SetColor4( player->viewAngles.yaw, player->viewAngles.pitch, 1.0f, 2.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, underwaterMaterial );
}

/*
===================
idPlayerView::PostFX_ExplosionFX
===================
*/
void idPlayerView::PostFX_ExplosionFX()
{
	if( gameLocal.explosionTime >= gameLocal.time )
	{
		idMat3 axis;
		idVec3 origin;
		idVec3 viewVector[3];

		player->GetViewPos( origin, axis );
		player->viewAngles.ToVectors( &viewVector[ 0 ], &viewVector[ 1 ], &viewVector[ 2 ] );
		idVec3 expVector	= origin - gameLocal.explosionOrigin;
		float length		= expVector.Normalize();

		idVec3 ndc;
		renderSystem->GlobalToNormalizedDeviceCoordinates( gameLocal.explosionOrigin, ndc );
		ndc.x = ndc.x * 0.5 + 0.5;
		ndc.y = ndc.y * 0.5 + 0.5;

		float time		= ( float )gameLocal.explosionTime - ( float )gameLocal.time;
		float radius	= idMath::ClampFloat( 0.0f, 1.0f, gameLocal.explosionRadius / 200.0f );
		float damage	= idMath::ClampFloat( 0.0f, 1.0f, gameLocal.explosionDamage / 250.0f );
		float distance	= 1.0f - idMath::ClampFloat( 0.0f, 1.0f, length / 1024.0f );
		float atten		= idMath::ClampFloat( 0.0f, 1.0f, time / SEC2MS( g_explosionFXTime.GetFloat() ) );
		float VdotE		= idMath::ClampFloat( 0.0f, 1.0f, viewVector[ 0 ] * -expVector );
		float scale		= radius * damage * distance * atten * VdotE * g_explosionFXScale.GetFloat();

		renderSystem->CaptureRenderToImage( "_currentRender" );
		renderSystem->SetColor4( ndc.x, ndc.y, scale, 1.0f );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, explosionFXMaterial );
	}
}

/*
===================
idPlayerView::PostFX_Vignetting
===================
*/
void idPlayerView::PostFX_Vignetting()
{
	renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, 1.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, vignettingMaterial );
}

/*
===================
idPlayerView::PostFX_Filmgrain
===================
*/
void idPlayerView::PostFX_Filmgrain()
{
	float size = 128.0f * r_filmgrainScale.GetFloat();
	renderSystem->SetColor4( renderSystem->GetScreenWidth() / size, renderSystem->GetScreenHeight() / size, r_filmgrainStrength.GetFloat(), r_filmgrainBlendMode.GetInteger() );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, filmgrainMaterial );
}

/*
===================
idPlayerView::PostFX_DoubleVision
===================
*/
void idPlayerView::PostFX_DoubleVision()
{
	int	nWidth	= MakePowerOfTwo( renderSystem->GetScreenWidth() );
	int	nHeight	= MakePowerOfTwo( renderSystem->GetScreenHeight() );
	int offset	= dvFinishTime - gameLocal.time;
	float scale	= ( offset * g_dvAmplitude.GetFloat() ) > 0.5f ? 0.5f : offset * g_dvAmplitude.GetFloat();
	float shift	= fabs( scale * sin( sqrtf( offset ) * g_dvFrequency.GetFloat() ) );

	renderSystem->CaptureRenderToImage( "_currentRender" );

	// if double vision, render to a texture
	renderSystem->CropRenderSize( nWidth, nHeight, true, true );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, shiftScale.y, shiftScale.x, 0.0f, currentRenderMaterial );
	renderSystem->CaptureRenderToImage( "_scratch" );
	renderSystem->UnCrop();

	idVec4 color( 1.0f, 1.0f, 1.0f, 1.0f );

	renderSystem->SetColor4( color.x, color.y, color.z, 1.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, shift, 1.0f, 1.0f, 0.0f, scratchMaterial );
	renderSystem->SetColor4( color.x, color.y, color.z, 0.5f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f, 1.0f - shift, 0.0f, scratchMaterial );
}

/*
===================
idPlayerView::PostFX_MegashieldVision
===================
*/
void idPlayerView::PostFX_MegashieldVision()
{
	int shieldTime = player->inventory.powerupEndTime[ MEGASHIELD ] - gameLocal.time;
	float alpha = ( shieldTime < 3000 ) ? ( float )shieldTime / 3000 : 1.0f;
	alpha = ( alpha < 0.0f ) ? 0.0f : alpha;

	renderSystem->SetColor4( alpha, alpha, alpha, alpha );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, megashieldMaterial );
}

/*
===================
idPlayerView::PostFX_InfluenceVision
===================
*/
void idPlayerView::PostFX_InfluenceVision()
{
	float distance = 0.0f;
	float pct = 1.0f;

	if( player->GetInfluenceEntity() )
	{
		distance = ( player->GetInfluenceEntity()->GetPhysics()->GetOrigin() - player->GetPhysics()->GetOrigin() ).Length();
		if( player->GetInfluenceRadius() != 0.0f && distance < player->GetInfluenceRadius() )
		{
			pct = distance / player->GetInfluenceRadius();
			pct = 1.0f - idMath::ClampFloat( 0.0f, 1.0f, pct );
		}
	}
	if( player->GetInfluenceMaterial() )
	{
		renderSystem->CaptureRenderToImage( "_currentRender" );
		renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, pct );
		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, player->GetInfluenceMaterial() );
	}// else if ( player->GetInfluenceEntity() ) {
//		int offset = 25 + sinf( gameLocal.time );	// sikk - variable initialized but not used
//		PostFX_DoubleVision( view, pct * offset );
//	}
}

/*
===================
idPlayerView::PostFX_BFGVision
===================
*/
void idPlayerView::PostFX_BFGVision()
{
	renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, 1.0f );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, bfgMaterial );
}

/*
===================
idPlayerView::PostFX_TunnelVision
===================
*/
void idPlayerView::PostFX_TunnelVision()
{
	float health = 0.0f;
	float alpha;
	float denominator = g_energySystem.GetBool() ? 200 : 1000;

	if( g_testHealthVision.GetFloat() != 0.0f )
	{
		health = g_testHealthVision.GetFloat();
	}
	else
	{
		health = player->health;
	}

	alpha = idMath::ClampFloat( 0.0f, 1.0f, health / denominator );

	if( alpha < 1.0f )
	{
		if( g_testHealthVision.GetFloat() )  	// sikk - fix for "g_testHealthVision"
		{
			renderSystem->SetColor4( MS2SEC( gameLocal.time ), 1.0f, 1.0f, alpha );
		}
		else
		{
			renderSystem->SetColor4( ( player->health <= 0.0f ) ? MS2SEC( gameLocal.time ) : lastDamageTime, 1.0f, 1.0f, ( player->health <= 0.0f ) ? 0.0f : alpha );
		}

		renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, tunnelMaterial );
	}
}

/*
===================
idPlayerView::PostFX_ScreenBlobs
===================
*/
void idPlayerView::PostFX_ScreenBlobs()
{
	for( int i = 0; i < MAX_SCREEN_BLOBS; i++ )
	{
		screenBlob_t* blob = &screenBlobs[ i ];

		if( blob->finishTime <= gameLocal.time )
		{
			continue;
		}

		blob->y += blob->driftAmount;

		float fade = ( float )( blob->finishTime - gameLocal.time ) / ( blob->finishTime - blob->startFadeTime );

		if( fade > 1.0f )
		{
			fade = 1.0f;
		}

		if( fade )
		{
			renderSystem->SetColor4( 1.0f, 1.0f, 1.0f, fade );
			renderSystem->DrawStretchPic( blob->x, blob->y, blob->w, blob->h, blob->s1, blob->t1, blob->s2, blob->t2, blob->material );
		}
	}
}

/*
===================
idPlayerView::PostFX_ScreenWipe
===================
*/
void idPlayerView::PostFX_ScreenWipe()
{
	renderSystem->SetColor4( 1.0, 1.0, 1.0, gameLocal.time - player->endLevelTime );
	renderSystem->DrawStretchPic( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, screenWipeMaterial );
}
// <--- sikk - PostProcess Effects