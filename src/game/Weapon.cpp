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

/***********************************************************************

  idWeapon  
	
***********************************************************************/

//
// event defs
//
const idEventDef EV_Weapon_Clear( "<clear>" );
const idEventDef EV_Weapon_GetOwner( "getOwner", NULL, 'e' );
const idEventDef EV_Weapon_Next( "nextWeapon" );
const idEventDef EV_Weapon_State( "weaponState", "sd" );
const idEventDef EV_Weapon_TotalEnergy( "totalEnergy", NULL, 'f' );
const idEventDef EV_Weapon_WeaponOutOfAmmo( "weaponOutOfAmmo" );
const idEventDef EV_Weapon_WeaponReady( "weaponReady" );
const idEventDef EV_Weapon_WeaponReloading( "weaponReloading" );
const idEventDef EV_Weapon_WeaponHolstered( "weaponHolstered" );
const idEventDef EV_Weapon_WeaponRising( "weaponRising" );
const idEventDef EV_Weapon_WeaponLowering( "weaponLowering" );
const idEventDef EV_Weapon_Flashlight( "flashlight", "d" );
const idEventDef EV_Weapon_LaunchProjectiles( "launchProjectiles", "dffffd" );	// sikk - Secondary Fire
const idEventDef EV_Weapon_CreateProjectile( "createProjectile", "d", 'e' );	// sikk - Secondary Fire
const idEventDef EV_Weapon_EjectBrass( "ejectBrass" );
const idEventDef EV_Weapon_Melee( "melee", "d", 'd' );	// sikk - Secondary Fire
const idEventDef EV_Weapon_GetWorldModel( "getWorldModel", NULL, 'e' );
const idEventDef EV_Weapon_IsMegadamage( "isMegadamage", NULL, 'f' );		// sikk - Berserk Check for skin update
const idEventDef EV_Weapon_IsMegashield( "isMegashield", NULL, 'f' );		// sikk - Megashield Check for skin update
const idEventDef EV_Weapon_IsInvisible( "isInvisible", NULL, 'f' );
// sikk - removed multiplayer
//const idEventDef EV_Weapon_AllowDrop( "allowDrop", "d" );
//const idEventDef EV_Weapon_NetReload( "netReload" );
//const idEventDef EV_Weapon_NetEndReload( "netEndReload" );

//
// class def
//
CLASS_DECLARATION( idAnimatedEntity, idWeapon )
	EVENT( EV_Weapon_Clear,						idWeapon::Event_Clear )
	EVENT( EV_Weapon_GetOwner,					idWeapon::Event_GetOwner )
	EVENT( EV_Weapon_State,						idWeapon::Event_WeaponState )
	EVENT( EV_Weapon_WeaponReady,				idWeapon::Event_WeaponReady )
	EVENT( EV_Weapon_WeaponOutOfAmmo,			idWeapon::Event_WeaponOutOfAmmo )
	EVENT( EV_Weapon_WeaponReloading,			idWeapon::Event_WeaponReloading )
	EVENT( EV_Weapon_WeaponHolstered,			idWeapon::Event_WeaponHolstered )
	EVENT( EV_Weapon_WeaponRising,				idWeapon::Event_WeaponRising )
	EVENT( EV_Weapon_WeaponLowering,			idWeapon::Event_WeaponLowering )
	EVENT( EV_Weapon_TotalEnergy,				idWeapon::Event_TotalEnergy )
	EVENT( AI_PlayAnim,							idWeapon::Event_PlayAnim )
	EVENT( AI_PlayCycle,						idWeapon::Event_PlayCycle )
	EVENT( AI_SetBlendFrames,					idWeapon::Event_SetBlendFrames )
	EVENT( AI_GetBlendFrames,					idWeapon::Event_GetBlendFrames )
	EVENT( AI_AnimDone,							idWeapon::Event_AnimDone )
	EVENT( EV_Weapon_Next,						idWeapon::Event_Next )
	EVENT( EV_SetSkin,							idWeapon::Event_SetSkin )
	EVENT( EV_Weapon_Flashlight,				idWeapon::Event_Flashlight )
	EVENT( EV_Light_GetLightParm,				idWeapon::Event_GetLightParm )
	EVENT( EV_Light_SetLightParm,				idWeapon::Event_SetLightParm )
	EVENT( EV_Light_SetLightParms,				idWeapon::Event_SetLightParms )
	EVENT( EV_Weapon_LaunchProjectiles,			idWeapon::Event_LaunchProjectiles )
	EVENT( EV_Weapon_CreateProjectile,			idWeapon::Event_CreateProjectile )
	EVENT( EV_Weapon_EjectBrass,				idWeapon::Event_EjectBrass )
	EVENT( EV_Weapon_Melee,						idWeapon::Event_Melee )
	EVENT( EV_Weapon_GetWorldModel,				idWeapon::Event_GetWorldModel )
	EVENT( EV_Weapon_IsMegadamage,				idWeapon::Event_IsMegadamage )		// sikk - Megadamage Check for skin update
	EVENT( EV_Weapon_IsMegashield,				idWeapon::Event_IsMegashield )		// sikk - Megashield Check for skin update
	EVENT( EV_Weapon_IsInvisible,				idWeapon::Event_IsInvisible )
// sikk - removed multiplayer
	//EVENT( EV_Weapon_AllowDrop,					idWeapon::Event_AllowDrop )
	//EVENT( EV_Weapon_NetReload,					idWeapon::Event_NetReload )
	//EVENT( EV_Weapon_NetEndReload,				idWeapon::Event_NetEndReload )
END_CLASS

/***********************************************************************

	init

***********************************************************************/

/*
================
idWeapon::idWeapon()
================
*/
idWeapon::idWeapon() {
	owner					= NULL;
	worldModel				= NULL;
	weaponDef				= NULL;
	thread					= NULL;

	memset( &guiLight, 0, sizeof( guiLight ) );
	memset( &muzzleFlash, 0, sizeof( muzzleFlash ) );
	memset( &worldMuzzleFlash, 0, sizeof( worldMuzzleFlash ) );
	memset( &nozzleGlow, 0, sizeof( nozzleGlow ) );

	muzzleFlashEnd			= 0;
	flashColor				= vec3_origin;
	muzzleFlashHandle		= -1;
	worldMuzzleFlashHandle	= -1;
	guiLightHandle			= -1;
	nozzleGlowHandle		= -1;
	modelDefHandle			= -1;

	brassDelay				= 0;

	allowDrop				= true;

	Clear();

	fl.networkSync			= true;

// ---> sikk - Railgun Beam
//	railbeamRenderEntity;
	railbeamModelDefHandle	= -1;
	railbeamTime			= 0;
	railbeamFX				= NULL;
// <--- sikk - Railgun Beam
}

/*
================
idWeapon::~idWeapon()
================
*/
idWeapon::~idWeapon() {
	Clear();
	delete worldModel.GetEntity();
}


/*
================
idWeapon::Spawn
================
*/
void idWeapon::Spawn( void ) {
// sikk - removed multiplayer
	//if ( !gameLocal.isClient ) {
		// setup the world model
		worldModel = static_cast< idAnimatedEntity * >( gameLocal.SpawnEntityType( idAnimatedEntity::Type, NULL ) );
		worldModel.GetEntity()->fl.networkSync = true;
	//}

	thread = new idThread();
	thread->ManualDelete();
	thread->ManualControl();
}

/*
================
idWeapon::SetOwner

Only called at player spawn time, not each weapon switch
================
*/
void idWeapon::SetOwner( idPlayer *_owner ) {
	assert( !owner );
	owner = _owner;
	SetName( va( "%s_weapon", owner->name.c_str() ) );

	if ( worldModel.GetEntity() ) {
		worldModel.GetEntity()->SetName( va( "%s_weapon_worldmodel", owner->name.c_str() ) );
	}
}

/*
================
idWeapon::ShouldConstructScriptObjectAtSpawn

Called during idEntity::Spawn to see if it should construct the script object or not.
Overridden by subclasses that need to spawn the script object themselves.
================
*/
bool idWeapon::ShouldConstructScriptObjectAtSpawn( void ) const {
	return false;
}

/*
================
idWeapon::CacheWeapon
================
*/
void idWeapon::CacheWeapon( const char *weaponName ) {
	const idDeclEntityDef *weaponDef;
	const char *brassDefName;
	const char *clipModelName;
	idTraceModel trm;
	const char *guiName;

	weaponDef = gameLocal.FindEntityDef( weaponName, false );
	if ( !weaponDef ) {
		return;
	}

	// precache the brass collision model
	brassDefName = weaponDef->dict.GetString( "def_ejectBrass" );
	if ( brassDefName[0] ) {
		const idDeclEntityDef *brassDef = gameLocal.FindEntityDef( brassDefName, false );
		if ( brassDef ) {
			brassDef->dict.GetString( "clipmodel", "", &clipModelName );
			if ( !clipModelName[0] ) {
				clipModelName = brassDef->dict.GetString( "model" );		// use the visual model
			}
			// load the trace model
			collisionModelManager->TrmFromModel( clipModelName, trm );
		}
	}

	guiName = weaponDef->dict.GetString( "gui" );
	if ( guiName[0] ) {
		uiManager->FindGui( guiName, true, false, true );
	}
}

/*
================
idWeapon::Save
================
*/
void idWeapon::Save( idSaveGame *savefile ) const {
	savefile->WriteInt( status );
	savefile->WriteObject( thread );
	savefile->WriteString( state );
	savefile->WriteString( idealState );
	savefile->WriteInt( animBlendFrames );
	savefile->WriteInt( animDoneTime );
	savefile->WriteBool( isLinked );

	savefile->WriteObject( owner );
	worldModel.Save( savefile );

	savefile->WriteInt( hideTime );
	savefile->WriteFloat( hideDistance );
	savefile->WriteInt( hideStartTime );
	savefile->WriteFloat( hideStart );
	savefile->WriteFloat( hideEnd );
	savefile->WriteFloat( hideOffset );
	savefile->WriteBool( hide );
	savefile->WriteBool( disabled );

	savefile->WriteVec3( playerViewOrigin );
	savefile->WriteMat3( playerViewAxis );

	savefile->WriteVec3( viewWeaponOrigin );
	savefile->WriteMat3( viewWeaponAxis );

	savefile->WriteVec3( muzzleOrigin );
	savefile->WriteMat3( muzzleAxis );

	savefile->WriteVec3( pushVelocity );

	savefile->WriteString( weaponDef->GetName() );
	savefile->WriteFloat( meleeDistance );
	savefile->WriteString( meleeDefName );
	savefile->WriteInt( brassDelay );

	savefile->WriteInt( guiLightHandle );
	savefile->WriteRenderLight( guiLight );

	savefile->WriteInt( muzzleFlashHandle );
	savefile->WriteRenderLight( muzzleFlash );

	savefile->WriteInt( worldMuzzleFlashHandle );
	savefile->WriteRenderLight( worldMuzzleFlash );

	savefile->WriteVec3( flashColor );
	savefile->WriteInt( muzzleFlashEnd );
	savefile->WriteInt( flashTime );

	savefile->WriteBool( lightOn );
	savefile->WriteBool( silent_fire );

	savefile->WriteInt( kick_endtime );
	savefile->WriteInt( muzzle_kick_time );
	savefile->WriteInt( muzzle_kick_maxtime );
	savefile->WriteAngles( muzzle_kick_angles );
	savefile->WriteVec3( muzzle_kick_offset );
// ---> sikk - Secondary Fire
	savefile->WriteInt( muzzle_kick_time_sec );
	savefile->WriteInt( muzzle_kick_maxtime_sec );
	savefile->WriteAngles( muzzle_kick_angles_sec );
	savefile->WriteVec3( muzzle_kick_offset_sec );
// <--- sikk - Secondary Fire

	// savegames <= 17
	savefile->WriteInt( 0 );

	savefile->WriteInt( zoomFov );

	savefile->WriteJoint( barrelJointView );
	savefile->WriteJoint( flashJointView );
	savefile->WriteJoint( ejectJointView );
	savefile->WriteJoint( guiLightJointView );
	savefile->WriteJoint( ventLightJointView );

	savefile->WriteJoint( flashJointWorld );
	savefile->WriteJoint( barrelJointWorld );
	savefile->WriteJoint( ejectJointWorld );

	savefile->WriteBool( hasBloodSplat );

	savefile->WriteSoundShader( sndHum );

	savefile->WriteParticle( weaponSmoke );
	savefile->WriteInt( weaponSmokeStartTime );
	savefile->WriteBool( continuousSmoke );
	savefile->WriteParticle( strikeSmoke );
	savefile->WriteInt( strikeSmokeStartTime );
	savefile->WriteVec3( strikePos );
	savefile->WriteMat3( strikeAxis );
	savefile->WriteInt( nextStrikeFx );

	savefile->WriteBool( nozzleFx );
	savefile->WriteInt( nozzleFxFade );

	savefile->WriteInt( lastAttack );

	savefile->WriteInt( nozzleGlowHandle );
	savefile->WriteRenderLight( nozzleGlow );

	savefile->WriteVec3( nozzleGlowColor );
	savefile->WriteMaterial( nozzleGlowShader );
	savefile->WriteFloat( nozzleGlowRadius );

	savefile->WriteInt( weaponAngleOffsetAverages );
	savefile->WriteFloat( weaponAngleOffsetScale );
	savefile->WriteFloat( weaponAngleOffsetMax );
	savefile->WriteFloat( weaponOffsetTime );
	savefile->WriteFloat( weaponOffsetScale );

	savefile->WriteBool( allowDrop );
	savefile->WriteObject( projectileEnt );
}

/*
================
idWeapon::Restore
================
*/
void idWeapon::Restore( idRestoreGame *savefile ) {
	savefile->ReadInt( (int &)status );
	savefile->ReadObject( reinterpret_cast<idClass *&>( thread ) );
	savefile->ReadString( state );
	savefile->ReadString( idealState );
	savefile->ReadInt( animBlendFrames );
	savefile->ReadInt( animDoneTime );
	savefile->ReadBool( isLinked );

	// Re-link script fields
	WEAPON_PRIMARY.LinkTo(		scriptObject, "WEAPON_PRIMARY" );		// sikk - Primary Attack
	WEAPON_SECONDARY.LinkTo(	scriptObject, "WEAPON_SECONDARY" );		// sikk - Secondary Attack
	WEAPON_RELOAD.LinkTo(		scriptObject, "WEAPON_RELOAD" );
	WEAPON_RAISEWEAPON.LinkTo(	scriptObject, "WEAPON_RAISEWEAPON" );
	WEAPON_LOWERWEAPON.LinkTo(	scriptObject, "WEAPON_LOWERWEAPON" );
// sikk - removed multiplayer
	//WEAPON_NETRELOAD.LinkTo(	scriptObject, "WEAPON_NETRELOAD" );
	//WEAPON_NETENDRELOAD.LinkTo(	scriptObject, "WEAPON_NETENDRELOAD" );
	//WEAPON_NETFIRING.LinkTo(	scriptObject, "WEAPON_NETFIRING" );

	savefile->ReadObject( reinterpret_cast<idClass *&>( owner ) );
	worldModel.Restore( savefile );

	savefile->ReadInt( hideTime );
	savefile->ReadFloat( hideDistance );
	savefile->ReadInt( hideStartTime );
	savefile->ReadFloat( hideStart );
	savefile->ReadFloat( hideEnd );
	savefile->ReadFloat( hideOffset );
	savefile->ReadBool( hide );
	savefile->ReadBool( disabled );

	savefile->ReadVec3( playerViewOrigin );
	savefile->ReadMat3( playerViewAxis );

	savefile->ReadVec3( viewWeaponOrigin );
	savefile->ReadMat3( viewWeaponAxis );

	savefile->ReadVec3( muzzleOrigin );
	savefile->ReadMat3( muzzleAxis );

	savefile->ReadVec3( pushVelocity );

	idStr objectname;
	savefile->ReadString( objectname );
	weaponDef = gameLocal.FindEntityDef( objectname );
	meleeDef = gameLocal.FindEntityDef( weaponDef->dict.GetString( "def_melee" ), false );

	const idDeclEntityDef *projectileDef = gameLocal.FindEntityDef( weaponDef->dict.GetString( "def_projectile" ), false );
	if ( projectileDef ) {
		projectileDict = projectileDef->dict;
	} else {
		projectileDict.Clear();
	}

// ---> sikk - Secondary Fire
	projectileDef = gameLocal.FindEntityDef( weaponDef->dict.GetString( "def_projectile_sec" ), false );
	if ( projectileDef ) {
		projectileSecDict = projectileDef->dict;
	} else {
		projectileSecDict.Clear();
	}
// <--- sikk - Secondary Fire

	const idDeclEntityDef *brassDef = gameLocal.FindEntityDef( weaponDef->dict.GetString( "def_ejectBrass" ), false );
	if ( brassDef ) {
		brassDict = brassDef->dict;
	} else {
		brassDict.Clear();
	}

	savefile->ReadFloat( meleeDistance );
	savefile->ReadString( meleeDefName );
	savefile->ReadInt( brassDelay );

	savefile->ReadInt( guiLightHandle );
	savefile->ReadRenderLight( guiLight );

	savefile->ReadInt( muzzleFlashHandle );
	savefile->ReadRenderLight( muzzleFlash );

	savefile->ReadInt( worldMuzzleFlashHandle );
	savefile->ReadRenderLight( worldMuzzleFlash );

	savefile->ReadVec3( flashColor );
	savefile->ReadInt( muzzleFlashEnd );
	savefile->ReadInt( flashTime );

	savefile->ReadBool( lightOn );
	savefile->ReadBool( silent_fire );

	savefile->ReadInt( kick_endtime );
	savefile->ReadInt( muzzle_kick_time );
	savefile->ReadInt( muzzle_kick_maxtime );
	savefile->ReadAngles( muzzle_kick_angles );
	savefile->ReadVec3( muzzle_kick_offset );
// ---> sikk - Secondary Fire
	savefile->ReadInt( muzzle_kick_time_sec );
	savefile->ReadInt( muzzle_kick_maxtime_sec );
	savefile->ReadAngles( muzzle_kick_angles_sec );
	savefile->ReadVec3( muzzle_kick_offset_sec );
// <--- sikk - Secondary Fire

	// savegame versions <= 17
	int foo;
	savefile->ReadInt( foo );

	savefile->ReadInt( zoomFov );

	savefile->ReadJoint( barrelJointView );
	savefile->ReadJoint( flashJointView );
	savefile->ReadJoint( ejectJointView );
	savefile->ReadJoint( guiLightJointView );
	savefile->ReadJoint( ventLightJointView );

	savefile->ReadJoint( flashJointWorld );
	savefile->ReadJoint( barrelJointWorld );
	savefile->ReadJoint( ejectJointWorld );

	savefile->ReadBool( hasBloodSplat );

	savefile->ReadSoundShader( sndHum );

	savefile->ReadParticle( weaponSmoke );
	savefile->ReadInt( weaponSmokeStartTime );
	savefile->ReadBool( continuousSmoke );
	savefile->ReadParticle( strikeSmoke );
	savefile->ReadInt( strikeSmokeStartTime );
	savefile->ReadVec3( strikePos );
	savefile->ReadMat3( strikeAxis );
	savefile->ReadInt( nextStrikeFx );

	savefile->ReadBool( nozzleFx );
	savefile->ReadInt( nozzleFxFade );

	savefile->ReadInt( lastAttack );

	savefile->ReadInt( nozzleGlowHandle );
	savefile->ReadRenderLight( nozzleGlow );

	savefile->ReadVec3( nozzleGlowColor );
	savefile->ReadMaterial( nozzleGlowShader );
	savefile->ReadFloat( nozzleGlowRadius );

	savefile->ReadInt( weaponAngleOffsetAverages );
	savefile->ReadFloat( weaponAngleOffsetScale );
	savefile->ReadFloat( weaponAngleOffsetMax );
	savefile->ReadFloat( weaponOffsetTime );
	savefile->ReadFloat( weaponOffsetScale );

	savefile->ReadBool( allowDrop );
	savefile->ReadObject( reinterpret_cast<idClass *&>( projectileEnt ) );
}

/***********************************************************************

	Weapon definition management

***********************************************************************/

/*
================
idWeapon::Clear
================
*/
void idWeapon::Clear( void ) {
	CancelEvents( &EV_Weapon_Clear );

	DeconstructScriptObject();
	scriptObject.Free();

	WEAPON_PRIMARY.Unlink();		// sikk - Primary Attack
	WEAPON_SECONDARY.Unlink();		// sikk - Secondary Attack
	WEAPON_RELOAD.Unlink();
	WEAPON_RAISEWEAPON.Unlink();
	WEAPON_LOWERWEAPON.Unlink();
// sikk - removed multiplayer
	//WEAPON_NETRELOAD.Unlink();
	//WEAPON_NETENDRELOAD.Unlink();
	//WEAPON_NETFIRING.Unlink();

	if ( muzzleFlashHandle != -1 ) {
		gameRenderWorld->FreeLightDef( muzzleFlashHandle );
		muzzleFlashHandle = -1;
	}
	if ( muzzleFlashHandle != -1 ) {
		gameRenderWorld->FreeLightDef( muzzleFlashHandle );
		muzzleFlashHandle = -1;
	}
	if ( worldMuzzleFlashHandle != -1 ) {
		gameRenderWorld->FreeLightDef( worldMuzzleFlashHandle );
		worldMuzzleFlashHandle = -1;
	}
	if ( guiLightHandle != -1 ) {
		gameRenderWorld->FreeLightDef( guiLightHandle );
		guiLightHandle = -1;
	}
	if ( nozzleGlowHandle != -1 ) {
		gameRenderWorld->FreeLightDef( nozzleGlowHandle );
		nozzleGlowHandle = -1;
	}

	memset( &renderEntity, 0, sizeof( renderEntity ) );
	renderEntity.entityNum	= entityNumber;

	renderEntity.noShadow		= true;
	renderEntity.noSelfShadow	= true;
	renderEntity.customSkin		= NULL;

	// set default shader parms
	renderEntity.shaderParms[ SHADERPARM_RED ]	= 1.0f;
	renderEntity.shaderParms[ SHADERPARM_GREEN ]= 1.0f;
	renderEntity.shaderParms[ SHADERPARM_BLUE ]	= 1.0f;
	renderEntity.shaderParms[3] = 1.0f;
	renderEntity.shaderParms[ SHADERPARM_TIMEOFFSET ] = 0.0f;
	renderEntity.shaderParms[5] = 0.0f;
	renderEntity.shaderParms[6] = 0.0f;
	renderEntity.shaderParms[7] = 0.0f;

	if ( refSound.referenceSound ) {
		refSound.referenceSound->Free( true );
	}
	memset( &refSound, 0, sizeof( refSound_t ) );
	
	// setting diversity to 0 results in no random sound.  -1 indicates random.
	refSound.diversity = -1.0f;

	if ( owner ) {
		// don't spatialize the weapon sounds
		refSound.listenerId = owner->GetListenerId();
	}

	// clear out the sounds from our spawnargs since we'll copy them from the weapon def
	const idKeyValue *kv = spawnArgs.MatchPrefix( "snd_" );
	while( kv ) {
		spawnArgs.Delete( kv->GetKey() );
		kv = spawnArgs.MatchPrefix( "snd_" );
	}

	hideTime		= 300;
	hideDistance	= -15.0f;
	hideStartTime	= gameLocal.time - hideTime;
	hideStart		= 0.0f;
	hideEnd			= 0.0f;
	hideOffset		= 0.0f;
	hide			= false;
	disabled		= false;

	weaponSmoke		= NULL;
	weaponSmokeStartTime = 0;
	continuousSmoke = false;
	strikeSmoke		= NULL;
	strikeSmokeStartTime = 0;
	strikePos.Zero();
	strikeAxis = mat3_identity;
	nextStrikeFx = 0;

	playerViewAxis.Identity();
	playerViewOrigin.Zero();
	viewWeaponAxis.Identity();
	viewWeaponOrigin.Zero();
	muzzleAxis.Identity();
	muzzleOrigin.Zero();
	pushVelocity.Zero();

	status			= WP_HOLSTERED;
	state			= "";
	idealState		= "";
	animBlendFrames	= 0;
	animDoneTime	= 0;

	projectileDict.Clear();
	projectileSecDict.Clear();	// sikk - Secondary Fire
	meleeDef		= NULL;
	meleeDefName	= "";
	meleeDistance	= 0.0f;
	brassDict.Clear();

	flashTime		= 250;
	lightOn			= false;
	silent_fire		= false;

	kick_endtime		= 0;
	muzzle_kick_time	= 0;
	muzzle_kick_maxtime	= 0;
	muzzle_kick_angles.Zero();
	muzzle_kick_offset.Zero();
// ---> sikk - Secondary Fire
	muzzle_kick_time_sec	= 0;
	muzzle_kick_maxtime_sec	= 0;
	muzzle_kick_angles_sec.Zero();
	muzzle_kick_offset_sec.Zero();
// <--- sikk - Secondary Fire

	zoomFov = 90;

	barrelJointView		= INVALID_JOINT;
	flashJointView		= INVALID_JOINT;
	ejectJointView		= INVALID_JOINT;
	guiLightJointView	= INVALID_JOINT;
	ventLightJointView	= INVALID_JOINT;

	barrelJointWorld	= INVALID_JOINT;
	flashJointWorld		= INVALID_JOINT;
	ejectJointWorld		= INVALID_JOINT;

	hasBloodSplat		= false;
	nozzleFx			= false;
	nozzleFxFade		= 1500;
	lastAttack			= 0;
	nozzleGlowHandle	= -1;
	nozzleGlowShader	= NULL;
	nozzleGlowRadius	= 10;
	nozzleGlowColor.Zero();

	weaponAngleOffsetAverages	= 0;
	weaponAngleOffsetScale		= 0.0f;
	weaponAngleOffsetMax		= 0.0f;
	weaponOffsetTime			= 0.0f;
	weaponOffsetScale			= 0.0f;

	allowDrop			= true;

	animator.ClearAllAnims( gameLocal.time, 0 );
	FreeModelDef();

	sndHum				= NULL;

	isLinked			= false;
	projectileEnt		= NULL;

	isFiring			= false;
}

/*
================
idWeapon::InitWorldModel
================
*/
void idWeapon::InitWorldModel( const idDeclEntityDef *def ) {
	idEntity *ent;

	ent = worldModel.GetEntity();

	assert( ent );
	assert( def );

	const char *model = def->dict.GetString( "model_world" );
	const char *attach = def->dict.GetString( "joint_attach" );

	ent->SetSkin( NULL );
	if ( model[0] && attach[0] ) {
		ent->Show();
		ent->SetModel( model );
		if ( ent->GetAnimator()->ModelDef() ) {
			ent->SetSkin( ent->GetAnimator()->ModelDef()->GetDefaultSkin() );
		}
		ent->GetPhysics()->SetContents( 0 );
		ent->GetPhysics()->SetClipModel( NULL, 1.0f );
		ent->BindToJoint( owner, attach, true );
		ent->GetPhysics()->SetOrigin( vec3_origin );
		ent->GetPhysics()->SetAxis( mat3_identity );

		// supress model in player views, but allow it in mirrors and remote views
		renderEntity_t *worldModelRenderEntity = ent->GetRenderEntity();
		if ( worldModelRenderEntity ) {
			worldModelRenderEntity->suppressSurfaceInViewID = owner->entityNumber+1;
			worldModelRenderEntity->suppressShadowInViewID = owner->entityNumber+1;
			worldModelRenderEntity->suppressShadowInLightID = LIGHTID_VIEW_MUZZLE_FLASH + owner->entityNumber;
		}
	} else {
		ent->SetModel( "" );
		ent->Hide();
	}

	flashJointWorld = ent->GetAnimator()->GetJointHandle( "flash" );
	barrelJointWorld = ent->GetAnimator()->GetJointHandle( "muzzle" );
	ejectJointWorld = ent->GetAnimator()->GetJointHandle( "eject" );
}

/*
================
idWeapon::GetWeaponDef
================
*/
void idWeapon::GetWeaponDef( const char *objectname, int ammoinclip ) {
	const char *shader;
	const char *objectType;
	const char *vmodel;
	const char *guiName;
	const char *projectileName;
	const char *brassDefName;
	const char *smokeName;

	Clear();

	if ( !objectname || !objectname[ 0 ] ) {
		return;
	}

	assert( owner );

	weaponDef			= gameLocal.FindEntityDef( objectname );

	silent_fire			= weaponDef->dict.GetBool( "silent_fire" );

	muzzle_kick_time	= SEC2MS( weaponDef->dict.GetFloat( "muzzle_kick_time" ) );
	muzzle_kick_maxtime	= SEC2MS( weaponDef->dict.GetFloat( "muzzle_kick_maxtime" ) );
	muzzle_kick_angles	= weaponDef->dict.GetAngles( "muzzle_kick_angles" );
	muzzle_kick_offset	= weaponDef->dict.GetVector( "muzzle_kick_offset" );
// ---> sikk - Secondary Fire
	muzzle_kick_time_sec	= SEC2MS( weaponDef->dict.GetFloat( "muzzle_kick_time_sec" ) );
	muzzle_kick_maxtime_sec	= SEC2MS( weaponDef->dict.GetFloat( "muzzle_kick_maxtime_sec" ) );
	muzzle_kick_angles_sec	= weaponDef->dict.GetAngles( "muzzle_kick_angles_sec" );
	muzzle_kick_offset_sec	= weaponDef->dict.GetVector( "muzzle_kick_offset_sec" );
// <--- sikk - Secondary Fire

	hideTime			= SEC2MS( weaponDef->dict.GetFloat( "hide_time", "0.3" ) );
	hideDistance		= weaponDef->dict.GetFloat( "hide_distance", "-15" );

	// muzzle smoke
	smokeName = weaponDef->dict.GetString( "smoke_muzzle" );
	if ( *smokeName != '\0' ) {
		weaponSmoke = static_cast<const idDeclParticle *>( declManager->FindType( DECL_PARTICLE, smokeName ) );
	} else {
		weaponSmoke = NULL;
	}
	continuousSmoke = weaponDef->dict.GetBool( "continuousSmoke" );
	weaponSmokeStartTime = ( continuousSmoke ) ? gameLocal.time : 0;

	smokeName = weaponDef->dict.GetString( "smoke_strike" );
	if ( *smokeName != '\0' ) {
		strikeSmoke = static_cast<const idDeclParticle *>( declManager->FindType( DECL_PARTICLE, smokeName ) );
	} else {
		strikeSmoke = NULL;
	}
	strikeSmokeStartTime = 0;
	strikePos.Zero();
	strikeAxis = mat3_identity;
	nextStrikeFx = 0;

	// setup gui light
	memset( &guiLight, 0, sizeof( guiLight ) );
	const char *guiLightShader = weaponDef->dict.GetString( "mtr_guiLightShader" );
	if ( *guiLightShader != '\0' ) {
		guiLight.shader = declManager->FindMaterial( guiLightShader, false );
		guiLight.lightRadius[0] = guiLight.lightRadius[1] = guiLight.lightRadius[2] = 3;
		guiLight.pointLight = true;
	}

	// setup the view model
	vmodel = weaponDef->dict.GetString( "model_view" );
	SetModel( vmodel );

	// setup the world model
	InitWorldModel( weaponDef );

	// copy the sounds from the weapon view model def into out spawnargs
	const idKeyValue *kv = weaponDef->dict.MatchPrefix( "snd_" );
	while( kv ) {
		spawnArgs.Set( kv->GetKey(), kv->GetValue() );
		kv = weaponDef->dict.MatchPrefix( "snd_", kv );
	}

	// find some joints in the model for locating effects
	barrelJointView = animator.GetJointHandle( "barrel" );
	flashJointView = animator.GetJointHandle( "flash" );
	ejectJointView = animator.GetJointHandle( "eject" );
	guiLightJointView = animator.GetJointHandle( "guiLight" );
	ventLightJointView = animator.GetJointHandle( "ventLight" );

	// get the projectile
	projectileDict.Clear();

	projectileName = weaponDef->dict.GetString( "def_projectile" );
	if ( projectileName[0] != '\0' ) {
		const idDeclEntityDef *projectileDef = gameLocal.FindEntityDef( projectileName, false );
		if ( !projectileDef ) {
			gameLocal.Warning( "Unknown projectile '%s' in weapon '%s'", projectileName, objectname );
		} else {
			const char *spawnclass = projectileDef->dict.GetString( "spawnclass" );
			idTypeInfo *cls = idClass::GetClass( spawnclass );
			if ( !cls || !cls->IsType( idProjectile::Type ) ) {
				gameLocal.Warning( "Invalid spawnclass '%s' on projectile '%s' (used by weapon '%s')", spawnclass, projectileName, objectname );
			} else {
				projectileDict = projectileDef->dict;
			}
		}
	}

// ---> sikk - Secondary Fire
	projectileSecDict.Clear();

	projectileName = weaponDef->dict.GetString( "def_projectile_sec" );
	if ( projectileName[0] != '\0' ) {
		const idDeclEntityDef *projectileDef = gameLocal.FindEntityDef( projectileName, false );
		if ( !projectileDef ) {
			gameLocal.Warning( "Unknown projectile '%s' in weapon '%s'", projectileName, objectname );
		} else {
			const char *spawnclass = projectileDef->dict.GetString( "spawnclass" );
			idTypeInfo *cls = idClass::GetClass( spawnclass );
			if ( !cls || !cls->IsType( idProjectile::Type ) ) {
				gameLocal.Warning( "Invalid spawnclass '%s' on projectile '%s' (used by weapon '%s')", spawnclass, projectileName, objectname );
			} else {
				projectileSecDict = projectileDef->dict;
			}
		}
	}
// <--- sikk - Secondary Fire

	// set up muzzleflash render light
	const idMaterial*flashShader;
	idVec3			flashTarget;
	idVec3			flashUp;
	idVec3			flashRight;
	float			flashRadius;
	bool			flashPointLight;

	weaponDef->dict.GetString( "mtr_flashShader", "", &shader );
	flashShader		= declManager->FindMaterial( shader, false );
	flashPointLight = weaponDef->dict.GetBool( "flashPointLight", "1" );
	flashColor		= weaponDef->dict.GetVector( "flashColor", "0 0 0" );
	flashRadius		= (float)weaponDef->dict.GetInt( "flashRadius" );	// if 0, no light will spawn
	flashTime		= SEC2MS( weaponDef->dict.GetFloat( "flashTime", "0.25" ) );
	flashTarget		= weaponDef->dict.GetVector( "flashTarget" );
	flashUp			= weaponDef->dict.GetVector( "flashUp" );
	flashRight		= weaponDef->dict.GetVector( "flashRight" );

	memset( &muzzleFlash, 0, sizeof( muzzleFlash ) );
	muzzleFlash.lightId = LIGHTID_VIEW_MUZZLE_FLASH + owner->entityNumber;
	muzzleFlash.allowLightInViewID = owner->entityNumber+1;

	// the weapon lights will only be in first person
	guiLight.allowLightInViewID = owner->entityNumber+1;
	nozzleGlow.allowLightInViewID = owner->entityNumber+1;

	muzzleFlash.pointLight								= flashPointLight;
	muzzleFlash.shader									= flashShader;
	muzzleFlash.shaderParms[ SHADERPARM_RED ]			= flashColor[0];
	muzzleFlash.shaderParms[ SHADERPARM_GREEN ]			= flashColor[1];
	muzzleFlash.shaderParms[ SHADERPARM_BLUE ]			= flashColor[2];
	muzzleFlash.shaderParms[ SHADERPARM_TIMESCALE ]		= 1.0f;

	muzzleFlash.lightRadius[0]							= flashRadius;
	muzzleFlash.lightRadius[1]							= flashRadius;
	muzzleFlash.lightRadius[2]							= flashRadius;

	if ( !flashPointLight ) {
		muzzleFlash.target								= flashTarget;
		muzzleFlash.up									= flashUp;
		muzzleFlash.right								= flashRight;
		muzzleFlash.end									= flashTarget;
	}

	// the world muzzle flash is the same, just positioned differently
	worldMuzzleFlash = muzzleFlash;
	worldMuzzleFlash.suppressLightInViewID = owner->entityNumber+1;
	worldMuzzleFlash.allowLightInViewID = 0;
	worldMuzzleFlash.lightId = LIGHTID_WORLD_MUZZLE_FLASH + owner->entityNumber;

	//-----------------------------------

	nozzleFx			= weaponDef->dict.GetBool( "nozzleFx" );
	nozzleFxFade		= weaponDef->dict.GetInt( "nozzleFxFade", "1500" );
	nozzleGlowColor		= weaponDef->dict.GetVector( "nozzleGlowColor", "1 1 1" );
	nozzleGlowRadius	= weaponDef->dict.GetFloat( "nozzleGlowRadius", "10" );
	weaponDef->dict.GetString( "mtr_nozzleGlowShader", "", &shader );
	nozzleGlowShader = declManager->FindMaterial( shader, false );

	// get the melee damage def
	meleeDistance = weaponDef->dict.GetFloat( "melee_distance" );
	meleeDefName = weaponDef->dict.GetString( "def_melee" );
	if ( meleeDefName.Length() ) {
		meleeDef = gameLocal.FindEntityDef( meleeDefName, false );
		if ( !meleeDef ) {
			gameLocal.Error( "Unknown melee '%s'", meleeDefName.c_str() );
		}
	}

	// get the brass def
	brassDict.Clear();
	brassDelay = weaponDef->dict.GetInt( "ejectBrassDelay", "0" );
	brassDefName = weaponDef->dict.GetString( "def_ejectBrass" );

	if ( brassDefName[0] ) {
		const idDeclEntityDef *brassDef = gameLocal.FindEntityDef( brassDefName, false );
		if ( !brassDef ) {
			gameLocal.Warning( "Unknown brass '%s'", brassDefName );
		} else {
			brassDict = brassDef->dict;
		}
	}

	renderEntity.gui[ 0 ] = NULL;
	guiName = weaponDef->dict.GetString( "gui" );
	if ( guiName[0] ) {
		renderEntity.gui[ 0 ] = uiManager->FindGui( guiName, true, false, true );
	}

	zoomFov = weaponDef->dict.GetInt( "zoomFov", "70" );

	weaponAngleOffsetAverages = weaponDef->dict.GetInt( "weaponAngleOffsetAverages", "10" );
	weaponAngleOffsetScale = weaponDef->dict.GetFloat( "weaponAngleOffsetScale", "0.25" );
	weaponAngleOffsetMax = weaponDef->dict.GetFloat( "weaponAngleOffsetMax", "10" );

	weaponOffsetTime = weaponDef->dict.GetFloat( "weaponOffsetTime", "400" );
	weaponOffsetScale = weaponDef->dict.GetFloat( "weaponOffsetScale", "0.005" );

	if ( !weaponDef->dict.GetString( "weapon_scriptobject", NULL, &objectType ) ) {
		gameLocal.Error( "No 'weapon_scriptobject' set on '%s'.", objectname );
	}
	
	// setup script object
	if ( !scriptObject.SetType( objectType ) ) {
		gameLocal.Error( "Script object '%s' not found on weapon '%s'.", objectType, objectname );
	}

	WEAPON_PRIMARY.LinkTo(		scriptObject, "WEAPON_PRIMARY" );		// sikk - Primary Attack
	WEAPON_SECONDARY.LinkTo(	scriptObject, "WEAPON_SECONDARY" );		// sikk - Secondary Attack
	WEAPON_RELOAD.LinkTo(		scriptObject, "WEAPON_RELOAD" );
	WEAPON_RAISEWEAPON.LinkTo(	scriptObject, "WEAPON_RAISEWEAPON" );
	WEAPON_LOWERWEAPON.LinkTo(	scriptObject, "WEAPON_LOWERWEAPON" );
// sikk - removed multiplayer
	//WEAPON_NETRELOAD.LinkTo(	scriptObject, "WEAPON_NETRELOAD" );
	//WEAPON_NETENDRELOAD.LinkTo(	scriptObject, "WEAPON_NETENDRELOAD" );
	//WEAPON_NETFIRING.LinkTo(	scriptObject, "WEAPON_NETFIRING" );

	spawnArgs = weaponDef->dict;

	shader = spawnArgs.GetString( "snd_hum" );
	if ( shader && *shader ) {
		sndHum = declManager->FindSound( shader );
		StartSoundShader( sndHum, SND_CHANNEL_BODY, 0, false, NULL );
	}

	isLinked = true;

	// call script object's constructor
	ConstructScriptObject();

	// make sure we have the correct skin
	UpdateSkin();
}

/***********************************************************************

	GUIs

***********************************************************************/

/*
================
idWeapon::UpdateGUI
================
*/
void idWeapon::UpdateGUI( void ) {
	if ( !renderEntity.gui[ 0 ] ) {
		return;
	}
	
	if ( status == WP_HOLSTERED ) {
		return;
	}

// sikk - removed multiplayer
	//if ( owner->weaponGone ) {
	//	// dropping weapons was implemented wierd, so we have to not update the gui when it happens or we'll get a negative ammo count
	//	return;
	//}

	//if ( gameLocal.localClientNum != owner->entityNumber ) {
	//	// if updating the hud for a followed client
	//	if ( gameLocal.localClientNum >= 0 && gameLocal.entities[ gameLocal.localClientNum ] && gameLocal.entities[ gameLocal.localClientNum ]->IsType( idPlayer::Type ) ) {
	//		idPlayer *p = static_cast< idPlayer * >( gameLocal.entities[ gameLocal.localClientNum ] );
	//		if ( !p->spectating || p->spectator != owner->entityNumber ) {
	//			return;
	//		}
	//	} else {
	//		return;
	//	}
	//}
}

/***********************************************************************

	Model and muzzleflash

***********************************************************************/

/*
================
idWeapon::UpdateFlashPosition
================
*/
void idWeapon::UpdateFlashPosition( void ) {
	// the flash has an explicit joint for locating it
	GetGlobalJointTransform( true, flashJointView, muzzleFlash.origin, muzzleFlash.axis );

	// if the desired point is inside or very close to a wall, back it up until it is clear
	idVec3	start = muzzleFlash.origin - playerViewAxis[0] * 16;
	idVec3	end = muzzleFlash.origin + playerViewAxis[0] * 8;
	trace_t	tr;
	gameLocal.clip.TracePoint( tr, start, end, MASK_SHOT_RENDERMODEL, owner );
	// be at least 8 units away from a solid
	muzzleFlash.origin = tr.endpos - playerViewAxis[0] * 8;

	// put the world muzzle flash on the end of the joint, no matter what
	GetGlobalJointTransform( false, flashJointWorld, worldMuzzleFlash.origin, worldMuzzleFlash.axis );
}

/*
================
idWeapon::MuzzleFlashLight
================
*/
void idWeapon::MuzzleFlashLight( void ) {
	
	if ( !lightOn && ( !g_muzzleFlash.GetBool() || !muzzleFlash.lightRadius[0] ) ) {
		return;
	}

	if ( flashJointView == INVALID_JOINT ) {
		return;
	}

	UpdateFlashPosition();

	// these will be different each fire
	muzzleFlash.shaderParms[ SHADERPARM_TIMEOFFSET ]	= -MS2SEC( gameLocal.time );
	muzzleFlash.shaderParms[ SHADERPARM_DIVERSITY ]		= renderEntity.shaderParms[ SHADERPARM_DIVERSITY ];

	worldMuzzleFlash.shaderParms[ SHADERPARM_TIMEOFFSET ]	= -MS2SEC( gameLocal.time );
	worldMuzzleFlash.shaderParms[ SHADERPARM_DIVERSITY ]	= renderEntity.shaderParms[ SHADERPARM_DIVERSITY ];

	// the light will be removed at this time
	muzzleFlashEnd = gameLocal.time + flashTime;

	if ( muzzleFlashHandle != -1 ) {
		gameRenderWorld->UpdateLightDef( muzzleFlashHandle, &muzzleFlash );
		gameRenderWorld->UpdateLightDef( worldMuzzleFlashHandle, &worldMuzzleFlash );
	} else {
		muzzleFlashHandle = gameRenderWorld->AddLightDef( &muzzleFlash );
		worldMuzzleFlashHandle = gameRenderWorld->AddLightDef( &worldMuzzleFlash );
	}
}

/*
================
idWeapon::UpdateSkin
================
*/
bool idWeapon::UpdateSkin( void ) {
	const function_t *func;

	if ( !isLinked ) {
		return false;
	}

	func = scriptObject.GetFunction( "UpdateSkin" );
	if ( !func ) {
		common->Warning( "Can't find function 'UpdateSkin' in object '%s'", scriptObject.GetTypeName() );
		return false;
	}
	
	// use the frameCommandThread since it's safe to use outside of framecommands
	gameLocal.frameCommandThread->CallFunction( this, func, true );
	gameLocal.frameCommandThread->Execute();

	return true;
}

/*
================
idWeapon::SetModel
================
*/
void idWeapon::SetModel( const char *modelname ) {
	assert( modelname );

	if ( modelDefHandle >= 0 ) {
		gameRenderWorld->RemoveDecals( modelDefHandle );
	}

	renderEntity.hModel = animator.SetModel( modelname );
	if ( renderEntity.hModel ) {
		renderEntity.customSkin = animator.ModelDef()->GetDefaultSkin();
		animator.GetJoints( &renderEntity.numJoints, &renderEntity.joints );
	} else {
		renderEntity.customSkin = NULL;
		renderEntity.callback = NULL;
		renderEntity.numJoints = 0;
		renderEntity.joints = NULL;
	}

	// hide the model until an animation is played
	Hide();
}

/*
================
idWeapon::GetGlobalJointTransform

This returns the offset and axis of a weapon bone in world space, suitable for attaching models or lights
================
*/
bool idWeapon::GetGlobalJointTransform( bool viewModel, const jointHandle_t jointHandle, idVec3 &offset, idMat3 &axis ) {
	if ( viewModel ) {
		// view model
		if ( animator.GetJointTransform( jointHandle, gameLocal.time, offset, axis ) ) {
			offset = offset * viewWeaponAxis + viewWeaponOrigin;
			axis = axis * viewWeaponAxis;
			return true;
		}
	} else {
		// world model
		if ( worldModel.GetEntity() && worldModel.GetEntity()->GetAnimator()->GetJointTransform( jointHandle, gameLocal.time, offset, axis ) ) {
			offset = worldModel.GetEntity()->GetPhysics()->GetOrigin() + offset * worldModel.GetEntity()->GetPhysics()->GetAxis();
			axis = axis * worldModel.GetEntity()->GetPhysics()->GetAxis();
			return true;
		}
	}
	offset = viewWeaponOrigin;
	axis = viewWeaponAxis;
	return false;
}

/*
================
idWeapon::SetPushVelocity
================
*/
void idWeapon::SetPushVelocity( const idVec3 &pushVelocity ) {
	this->pushVelocity = pushVelocity;
}


/***********************************************************************

	State control/player interface

***********************************************************************/

/*
================
idWeapon::Think
================
*/
void idWeapon::Think( void ) {
	// do nothing because the present is called from the player through PresentWeapon

// ---> sikk - Railgun Beam
	if ( railbeamModelDefHandle >= 0 ) {
		float alpha;
		if ( WEAPON_SECONDARY && gameLocal.time > railbeamTime + 3000 && owner->GetEnergy() > spawnArgs.GetInt( "minEnergy", "20" ) ) {
			trace_t tr;
			idVec3 start = playerViewOrigin;
			idVec3 end = start + playerViewAxis[ 0 ] * 32768;
			int contents = MASK_SHOT_RENDERMODEL | CONTENTS_PROJECTILE;
			gameLocal.clip.TracePoint( tr, start, end, contents, owner );
			idVec3 beanEnd = tr.endpos;
			GetGlobalJointTransform( true, barrelJointView, muzzleOrigin, muzzleAxis );
			railbeamRenderEntity.origin = muzzleOrigin;
			railbeamRenderEntity.axis = muzzleAxis;
			railbeamRenderEntity.shaderParms[ SHADERPARM_BEAM_END_X ] = beanEnd.x;
			railbeamRenderEntity.shaderParms[ SHADERPARM_BEAM_END_Y ] = beanEnd.y;
			railbeamRenderEntity.shaderParms[ SHADERPARM_BEAM_END_Z ] = beanEnd.z;

			if ( railbeamFX ) {
				railbeamFX->SetOrigin( beanEnd );
			}

			alpha = 1.0;
		} else if ( !railType ) {
			alpha = 0.0f;
		} else {
			alpha = 1.0f - MS2SEC( gameLocal.time - railbeamTime );
		}
		if ( alpha > 0.0f ) {
			railbeamRenderEntity.shaderParms[ SHADERPARM_RED ] = alpha;
			railbeamRenderEntity.shaderParms[ SHADERPARM_GREEN ] = alpha;
			railbeamRenderEntity.shaderParms[ SHADERPARM_BLUE ] = alpha;
			railbeamRenderEntity.shaderParms[ SHADERPARM_ALPHA ] = alpha;
			gameRenderWorld->UpdateEntityDef( railbeamModelDefHandle, &railbeamRenderEntity );
		} else {
//			railbeamRenderEntity.hModel = NULL;
			gameRenderWorld->FreeEntityDef( railbeamModelDefHandle );
			railbeamModelDefHandle = -1;

			if ( railbeamFX ) {
//				railbeamFX->Stop();
				delete railbeamFX;
				railbeamFX = NULL;
			}
		}
		UpdateVisuals();
	}
// <--- sikk - Railgun Beam
}

/*
================
idWeapon::Raise
================
*/
void idWeapon::Raise( void ) {
	if ( isLinked ) {
		WEAPON_RAISEWEAPON = true;
	}
}

/*
================
idWeapon::PutAway
================
*/
void idWeapon::PutAway( void ) {
	hasBloodSplat = false;
	if ( isLinked ) {
		WEAPON_LOWERWEAPON = true;
	}
}

/*
================
idWeapon::Reload
NOTE: this is only for impulse-triggered reload, auto reload is scripted
================
*/
void idWeapon::Reload( void ) {
	if ( isLinked ) {
		WEAPON_RELOAD = true;
	}
}

/*
================
idWeapon::LowerWeapon
================
*/
void idWeapon::LowerWeapon( void ) {
	if ( !hide ) {
		hideStart	= 0.0f;
		hideEnd		= hideDistance;
		if ( gameLocal.time - hideStartTime < hideTime ) {
			hideStartTime = gameLocal.time - ( hideTime - ( gameLocal.time - hideStartTime ) );
		} else {
			hideStartTime = gameLocal.time;
		}
		hide = true;
	}
}

/*
================
idWeapon::RaiseWeapon
================
*/
void idWeapon::RaiseWeapon( void ) {
	Show();

	if ( hide ) {
		hideStart	= hideDistance;
		hideEnd		= 0.0f;
		if ( gameLocal.time - hideStartTime < hideTime ) {
			hideStartTime = gameLocal.time - ( hideTime - ( gameLocal.time - hideStartTime ) );
		} else {
			hideStartTime = gameLocal.time;
		}
		hide = false;
	}
}

/*
================
idWeapon::HideWeapon
================
*/
void idWeapon::HideWeapon( void ) {
	Hide();
	if ( worldModel.GetEntity() ) {
		worldModel.GetEntity()->Hide();
	}
	muzzleFlashEnd = 0;
}

/*
================
idWeapon::ShowWeapon
================
*/
void idWeapon::ShowWeapon( void ) {
	Show();
	if ( worldModel.GetEntity() ) {
		worldModel.GetEntity()->Show();
	}
	if ( lightOn ) {
		MuzzleFlashLight();
	}
}

/*
================
idWeapon::HideWorldModel
================
*/
void idWeapon::HideWorldModel( void ) {
	if ( worldModel.GetEntity() ) {
		worldModel.GetEntity()->Hide();
	}
}

/*
================
idWeapon::ShowWorldModel
================
*/
void idWeapon::ShowWorldModel( void ) {
	if ( worldModel.GetEntity() ) {
		worldModel.GetEntity()->Show();
	}
}

/*
================
idWeapon::OwnerDied
================
*/
void idWeapon::OwnerDied( void ) {
	if ( isLinked ) {
		SetState( "OwnerDied", 0 );
		thread->Execute();
	}

	Hide();
	if ( worldModel.GetEntity() ) {
		worldModel.GetEntity()->Hide();
	}

	// don't clear the weapon immediately since the owner might have killed himself by firing the weapon
	// within the current stack frame
	PostEventMS( &EV_Weapon_Clear, 0 );
}

/*
================
idWeapon::BeginAttack
================
*/
void idWeapon::BeginAttack( bool primary ) {	
	if ( status != WP_OUTOFAMMO ) {
		lastAttack = gameLocal.time;
	}

	if ( !isLinked ) {
		return;
	}

	if ( !WEAPON_PRIMARY || WEAPON_SECONDARY ) {	// sikk - Secondary Attack
		if ( sndHum ) {
			StopSound( SND_CHANNEL_BODY, false );
		}
	}
	
// sikk - Secondary Attack
	WEAPON_PRIMARY = primary;
	WEAPON_SECONDARY = !primary;

	railType = primary;	// this is so we can immediately remove the beam at the end of the charge attack
}

/*
================
idWeapon::EndAttack
================
*/
void idWeapon::EndAttack( void ) {
// sikk - Secondary Attack
	if ( !WEAPON_PRIMARY.IsLinked() || !WEAPON_SECONDARY.IsLinked() ) {
		return;
	}
	if ( WEAPON_PRIMARY ) {
		WEAPON_PRIMARY = false;
	}
	if ( WEAPON_SECONDARY ) {
		WEAPON_SECONDARY = false;
	}
	if ( sndHum ) {
		StartSoundShader( sndHum, SND_CHANNEL_BODY, 0, false, NULL );
	}
}

/*
================
idWeapon::isReady
================
*/
bool idWeapon::IsReady( void ) const {
// sikk - Modified
	//return !hide && !IsHidden() && ( ( status == WP_RELOAD ) || ( status == WP_READY ) || ( status == WP_OUTOFAMMO ) );
	return !hide && !IsHidden() && !( status == WP_RELOAD ) && ( ( status == WP_READY ) || ( status == WP_OUTOFAMMO ) );
}

/*
================
idWeapon::IsReloading
================
*/
bool idWeapon::IsReloading( void ) const {
	return ( status == WP_RELOAD );
}

/*
================
idWeapon::IsHolstered
================
*/
bool idWeapon::IsHolstered( void ) const {
	return ( status == WP_HOLSTERED );
}

/*
================
idWeapon::ShowCrosshair
================
*/
bool idWeapon::ShowCrosshair( void ) const {
	return !( state == idStr( WP_RISING ) || state == idStr( WP_LOWERING ) || state == idStr( WP_HOLSTERED ) );
}

/*
=====================
idWeapon::CanDrop
=====================
*/
// sikk - removed multiplayer
//bool idWeapon::CanDrop( void ) const {
//	if ( !weaponDef || !worldModel.GetEntity() ) {
//		return false;
//	}
//	const char *classname = weaponDef->dict.GetString( "def_dropItem" );
//	if ( !classname[ 0 ] ) {
//		return false;
//	}
//	return true;
//}

/*
================
idWeapon::WeaponStolen
================
*/
// sikk - removed multiplayer
//void idWeapon::WeaponStolen( void ) {
//	assert( !gameLocal.isClient );
//	if ( projectileEnt ) {
//		if ( isLinked ) {
//			SetState( "WeaponStolen", 0 );
//			thread->Execute();
//		}
//		projectileEnt = NULL;
//	}
//
//	// set to holstered so we can switch weapons right away
//	status = WP_HOLSTERED;
//
//	HideWeapon();
//}

/*
=====================
idWeapon::DropItem
=====================
*/
// sikk - removed multiplayer
//idEntity * idWeapon::DropItem( const idVec3 &velocity, int activateDelay, int removeDelay, bool died ) {
//	if ( !weaponDef || !worldModel.GetEntity() ) {
//		return NULL;
//	}
//	if ( !allowDrop ) {
//		return NULL;
//	}
//	const char *classname = weaponDef->dict.GetString( "def_dropItem" );
//	if ( !classname[0] ) {
//		return NULL;
//	}
//	StopSound( SND_CHANNEL_BODY, true );
//	StopSound( SND_CHANNEL_BODY3, true );
//
//	return idMoveableItem::DropItem( classname, worldModel.GetEntity()->GetPhysics()->GetOrigin(), worldModel.GetEntity()->GetPhysics()->GetAxis(), velocity, activateDelay, removeDelay );
//}

/***********************************************************************

	Script state management

***********************************************************************/

/*
=====================
idWeapon::SetState
=====================
*/
void idWeapon::SetState( const char *statename, int blendFrames ) {
	const function_t *func;

	if ( !isLinked ) {
		return;
	}

	func = scriptObject.GetFunction( statename );
	if ( !func ) {
		assert( 0 );
		gameLocal.Error( "Can't find function '%s' in object '%s'", statename, scriptObject.GetTypeName() );
	}

	thread->CallFunction( this, func, true );
	state = statename;

	animBlendFrames = blendFrames;
	if ( g_debugWeapon.GetBool() ) {
		gameLocal.Printf( "%d: weapon state : %s\n", gameLocal.time, statename );
	}

	idealState = "";
}


/***********************************************************************

	Particles/Effects

***********************************************************************/

/*
================
idWeapon::UpdateNozzelFx
================
*/
void idWeapon::UpdateNozzleFx( void ) {
	if ( !nozzleFx ) {
		return;
	}

	//
	// shader parms
	//
	int la = gameLocal.time - lastAttack + 1;
	float s = 1.0f;
	float l = 0.0f;
	if ( la < nozzleFxFade ) {
		s = ( (float)la / nozzleFxFade );
		l = 1.0f - s;
	}
	renderEntity.shaderParms[5] = s;
	renderEntity.shaderParms[6] = l;

	if ( ventLightJointView == INVALID_JOINT ) {
		return;
	}

	//
	// vent light
	//
	if ( nozzleGlowHandle == -1 ) {
		memset( &nozzleGlow, 0, sizeof( nozzleGlow ) );
		if ( owner ) {
			nozzleGlow.allowLightInViewID = owner->entityNumber+1;
		}
		nozzleGlow.pointLight = true;
		nozzleGlow.noShadows = true;
		nozzleGlow.lightRadius.x = nozzleGlowRadius;
		nozzleGlow.lightRadius.y = nozzleGlowRadius;
		nozzleGlow.lightRadius.z = nozzleGlowRadius;
		nozzleGlow.shader = nozzleGlowShader;
		nozzleGlow.shaderParms[ SHADERPARM_TIMESCALE ]	= 1.0f;
		nozzleGlow.shaderParms[ SHADERPARM_TIMEOFFSET ]	= -MS2SEC( gameLocal.time );
		GetGlobalJointTransform( true, ventLightJointView, nozzleGlow.origin, nozzleGlow.axis );
		nozzleGlowHandle = gameRenderWorld->AddLightDef( &nozzleGlow );
	}

	GetGlobalJointTransform( true, ventLightJointView, nozzleGlow.origin, nozzleGlow.axis );

	nozzleGlow.shaderParms[ SHADERPARM_RED ] = nozzleGlowColor.x * s;
	nozzleGlow.shaderParms[ SHADERPARM_GREEN ] = nozzleGlowColor.y * s;
	nozzleGlow.shaderParms[ SHADERPARM_BLUE ] = nozzleGlowColor.z * s;
	gameRenderWorld->UpdateLightDef(nozzleGlowHandle, &nozzleGlow);
}


/*
================
idWeapon::BloodSplat
================
*/
bool idWeapon::BloodSplat( float size ) {
	float s, c;
	idMat3 localAxis, axistemp;
	idVec3 localOrigin, normal;

	if ( hasBloodSplat ) {
		return true;
	}

	hasBloodSplat = true;

	if ( modelDefHandle < 0 ) {
		return false;
	}

	if ( !GetGlobalJointTransform( true, ejectJointView, localOrigin, localAxis ) ) {
		return false;
	}

	localOrigin[0] += gameLocal.random.RandomFloat() * -10.0f;
	localOrigin[1] += gameLocal.random.RandomFloat() * 1.0f;
	localOrigin[2] += gameLocal.random.RandomFloat() * -2.0f;

	normal = idVec3( gameLocal.random.CRandomFloat(), -gameLocal.random.RandomFloat(), -1 );
	normal.Normalize();

	idMath::SinCos16( gameLocal.random.RandomFloat() * idMath::TWO_PI, s, c );

	localAxis[2] = -normal;
	localAxis[2].NormalVectors( axistemp[0], axistemp[1] );
	localAxis[0] = axistemp[ 0 ] * c + axistemp[ 1 ] * -s;
	localAxis[1] = axistemp[ 0 ] * -s + axistemp[ 1 ] * -c;

	localAxis[0] *= 1.0f / size;
	localAxis[1] *= 1.0f / size;

	idPlane		localPlane[2];

	localPlane[0] = localAxis[0];
	localPlane[0][3] = -(localOrigin * localAxis[0]) + 0.5f;

	localPlane[1] = localAxis[1];
	localPlane[1][3] = -(localOrigin * localAxis[1]) + 0.5f;

	const idMaterial *mtr = declManager->FindMaterial( "textures/decals/duffysplatgun" );

	gameRenderWorld->ProjectOverlay( modelDefHandle, localPlane, mtr );

	return true;
}


/***********************************************************************

	Visual presentation

***********************************************************************/

/*
================
idWeapon::MuzzleRise

The machinegun and chaingun will incrementally back up as they are being fired
================
*/
void idWeapon::MuzzleRise( idVec3 &origin, idMat3 &axis ) {
	int			time;
	float		amount;
	idAngles	ang;
	idVec3		offset;

	// sikk - Secondary Fire
	int mkTime	= ( WEAPON_PRIMARY ? muzzle_kick_maxtime : muzzle_kick_maxtime_sec );

	time = kick_endtime - gameLocal.time;
	if ( time <= 0 || mkTime <= 0 ) {
		return;
	}

	if ( time > mkTime ) {
		time = mkTime;
	}
	
	amount	= ( float )time / ( float )mkTime;
	if ( !WEAPON_PRIMARY ) {
		amount *= amount;
		amount *= amount;
		amount = 1.0f - idMath::Fabs( amount * 2.0f - 1.0f );
	}
	ang		= ( WEAPON_PRIMARY ? muzzle_kick_angles : muzzle_kick_angles_sec ) * amount;
	offset	= ( WEAPON_PRIMARY ? muzzle_kick_offset : muzzle_kick_offset_sec ) * amount;

	origin = origin - axis * offset;
	axis = ang.ToMat3() * axis;
}

/*
================
idWeapon::ConstructScriptObject

Called during idEntity::Spawn.  Calls the constructor on the script object.
Can be overridden by subclasses when a thread doesn't need to be allocated.
================
*/
idThread *idWeapon::ConstructScriptObject( void ) {
	const function_t *constructor;

	thread->EndThread();

	// call script object's constructor
	constructor = scriptObject.GetConstructor();
	if ( !constructor ) {
		gameLocal.Error( "Missing constructor on '%s' for weapon", scriptObject.GetTypeName() );
	}

	// init the script object's data
	scriptObject.ClearObject();
	thread->CallFunction( this, constructor, true );
	thread->Execute();

	return thread;
}

/*
================
idWeapon::DeconstructScriptObject

Called during idEntity::~idEntity.  Calls the destructor on the script object.
Can be overridden by subclasses when a thread doesn't need to be allocated.
Not called during idGameLocal::MapShutdown.
================
*/
void idWeapon::DeconstructScriptObject( void ) {
	const function_t *destructor;

	if ( !thread ) {
		return;
	}
	
	// don't bother calling the script object's destructor on map shutdown
	if ( gameLocal.GameState() == GAMESTATE_SHUTDOWN ) {
		return;
	}

	thread->EndThread();

	// call script object's destructor
	destructor = scriptObject.GetDestructor();
	if ( destructor ) {
		// start a thread that will run immediately and end
		thread->CallFunction( this, destructor, true );
		thread->Execute();
		thread->EndThread();
	}

	// clear out the object's memory
	scriptObject.ClearObject();
}

/*
================
idWeapon::UpdateScript
================
*/
void idWeapon::UpdateScript( void ) {
	int	count;

	if ( !isLinked ) {
		return;
	}

	// only update the script on new frames
	if ( !gameLocal.isNewFrame ) {
		return;
	}

	if ( idealState.Length() ) {
		SetState( idealState, animBlendFrames );
	}

	// update script state, which may call Event_LaunchProjectiles, among other things
	count = 10;
	while( ( thread->Execute() || idealState.Length() ) && count-- ) {
		// happens for weapons with no clip (like grenades)
		if ( idealState.Length() ) {
			SetState( idealState, animBlendFrames );
		}
	}

	WEAPON_RELOAD = false;
}

/*
================
idWeapon::AlertMonsters
================
*/
void idWeapon::AlertMonsters( void ) {
	trace_t	tr;
	idEntity *ent;
	idVec3 end = muzzleFlash.origin + muzzleFlash.axis * muzzleFlash.target;

	gameLocal.clip.TracePoint( tr, muzzleFlash.origin, end, CONTENTS_OPAQUE | MASK_SHOT_RENDERMODEL | CONTENTS_FLASHLIGHT_TRIGGER, owner );
	if ( g_debugWeapon.GetBool() ) {
		gameRenderWorld->DebugLine( colorYellow, muzzleFlash.origin, end, 0 );
		gameRenderWorld->DebugArrow( colorGreen, muzzleFlash.origin, tr.endpos, 2, 0 );
	}

	if ( tr.fraction < 1.0f ) {
		ent = gameLocal.GetTraceEntity( tr );
		if ( ent->IsType( idAI::Type ) ) {
			static_cast<idAI *>( ent )->TouchedByFlashlight( owner );
		} else if ( ent->IsType( idTrigger::Type ) ) {
			ent->Signal( SIG_TOUCH );
			ent->ProcessEvent( &EV_Touch, owner, &tr );
		}
	}

	// jitter the trace to try to catch cases where a trace down the center doesn't hit the monster
	end += muzzleFlash.axis * muzzleFlash.right * idMath::Sin16( MS2SEC( gameLocal.time ) * 31.34f );
	end += muzzleFlash.axis * muzzleFlash.up * idMath::Sin16( MS2SEC( gameLocal.time ) * 12.17f );
	gameLocal.clip.TracePoint( tr, muzzleFlash.origin, end, CONTENTS_OPAQUE | MASK_SHOT_RENDERMODEL | CONTENTS_FLASHLIGHT_TRIGGER, owner );
	if ( g_debugWeapon.GetBool() ) {
		gameRenderWorld->DebugLine( colorYellow, muzzleFlash.origin, end, 0 );
		gameRenderWorld->DebugArrow( colorGreen, muzzleFlash.origin, tr.endpos, 2, 0 );
	}

	if ( tr.fraction < 1.0f ) {
		ent = gameLocal.GetTraceEntity( tr );
		if ( ent->IsType( idAI::Type ) ) {
			static_cast<idAI *>( ent )->TouchedByFlashlight( owner );
		} else if ( ent->IsType( idTrigger::Type ) ) {
			ent->Signal( SIG_TOUCH );
			ent->ProcessEvent( &EV_Touch, owner, &tr );
		}
	}
}

/*
================
idWeapon::PresentWeapon
================
*/
void idWeapon::PresentWeapon( bool showViewModel ) {
	playerViewOrigin = owner->firstPersonViewOrigin;
	playerViewAxis = owner->firstPersonViewAxis;

	// calculate weapon position based on player movement bobbing
	owner->CalculateViewWeaponPos( viewWeaponOrigin, viewWeaponAxis );

	// hide offset is for dropping the gun when approaching a GUI or NPC
	// This is simpler to manage than doing the weapon put-away animation
	if ( gameLocal.time - hideStartTime < hideTime ) {		
		float frac = ( float )( gameLocal.time - hideStartTime ) / ( float )hideTime;
		if ( hideStart < hideEnd ) {
			frac = 1.0f - frac;
			frac = 1.0f - frac * frac;
		} else {
			frac = frac * frac;
		}
		hideOffset = hideStart + ( hideEnd - hideStart ) * frac;
	} else {
		hideOffset = hideEnd;
		if ( hide && disabled ) {
			Hide();
		}
	}
	viewWeaponOrigin += hideOffset * viewWeaponAxis[ 2 ];

	// kick up based on repeat firing
	MuzzleRise( viewWeaponOrigin, viewWeaponAxis );

	// set the physics position and orientation
	GetPhysics()->SetOrigin( viewWeaponOrigin );
	GetPhysics()->SetAxis( viewWeaponAxis );
	UpdateVisuals();

	// update the weapon script
	UpdateScript();

	UpdateGUI();

	// update animation
	UpdateAnimation();

	// only show the surface in player view
	renderEntity.allowSurfaceInViewID = owner->entityNumber+1;

	// crunch the depth range so it never pokes into walls this breaks the machine gun gui
	renderEntity.weaponDepthHack = true;

	// present the model
	if ( showViewModel ) {
		Present();
	} else {
		FreeModelDef();
	}

	if ( worldModel.GetEntity() && worldModel.GetEntity()->GetRenderEntity() ) {
		// deal with the third-person visible world model
		// don't show shadows of the world model in first person
// sikk - removed multiplayer
		if ( /*gameLocal.isMultiplayer ||*/ g_showPlayerShadow.GetBool() || pm_thirdPerson.GetBool() ) {
			worldModel.GetEntity()->GetRenderEntity()->suppressShadowInViewID	= 0;
		} else {
			worldModel.GetEntity()->GetRenderEntity()->suppressShadowInViewID	= owner->entityNumber+1;
			worldModel.GetEntity()->GetRenderEntity()->suppressShadowInLightID = LIGHTID_VIEW_MUZZLE_FLASH + owner->entityNumber;
		}
	}

	if ( nozzleFx ) {
		UpdateNozzleFx();
	}

	// muzzle smoke
	if ( showViewModel && !disabled && weaponSmoke && ( weaponSmokeStartTime != 0 ) ) {
		// use the barrel joint if available
		if ( barrelJointView ) {
			GetGlobalJointTransform( true, barrelJointView, muzzleOrigin, muzzleAxis );
		} else {
			// default to going straight out the view
			muzzleOrigin = playerViewOrigin;
			muzzleAxis = playerViewAxis;
		}
		// spit out a particle
		if ( !gameLocal.smokeParticles->EmitSmoke( weaponSmoke, weaponSmokeStartTime, gameLocal.random.RandomFloat(), muzzleOrigin, muzzleAxis ) ) {
			weaponSmokeStartTime = ( continuousSmoke ) ? gameLocal.time : 0;
		}
	}

	if ( showViewModel && strikeSmoke && strikeSmokeStartTime != 0 ) {
		// spit out a particle
		if ( !gameLocal.smokeParticles->EmitSmoke( strikeSmoke, strikeSmokeStartTime, gameLocal.random.RandomFloat(), strikePos, strikeAxis ) ) {
			strikeSmokeStartTime = 0;
		}
	}

	// remove the muzzle flash light when it's done
	if ( ( !lightOn && ( gameLocal.time >= muzzleFlashEnd ) ) || IsHidden() ) {
		if ( muzzleFlashHandle != -1 ) {
			gameRenderWorld->FreeLightDef( muzzleFlashHandle );
			muzzleFlashHandle = -1;
		}
		if ( worldMuzzleFlashHandle != -1 ) {
			gameRenderWorld->FreeLightDef( worldMuzzleFlashHandle );
			worldMuzzleFlashHandle = -1;
		}
	}

	// update the muzzle flash light, so it moves with the gun
	if ( muzzleFlashHandle != -1 ) {
		UpdateFlashPosition();
		gameRenderWorld->UpdateLightDef( muzzleFlashHandle, &muzzleFlash );
		gameRenderWorld->UpdateLightDef( worldMuzzleFlashHandle, &worldMuzzleFlash );

		// wake up monsters with the flashlight
// sikk - removed multiplayer
		if ( /*!gameLocal.isMultiplayer &&*/ lightOn && !owner->fl.notarget ) {
			AlertMonsters();
		}
	}

	// update the gui light
	if ( guiLight.lightRadius[0] && guiLightJointView != INVALID_JOINT ) {
		GetGlobalJointTransform( true, guiLightJointView, guiLight.origin, guiLight.axis );

		if ( ( guiLightHandle != -1 ) ) {
			gameRenderWorld->UpdateLightDef( guiLightHandle, &guiLight );
		} else {
			guiLightHandle = gameRenderWorld->AddLightDef( &guiLight );
		}
	}

	if ( status != WP_READY && sndHum ) {
		StopSound( SND_CHANNEL_BODY, false );
	}

	UpdateSound();
}

/*
================
idWeapon::EnterCinematic
================
*/
void idWeapon::EnterCinematic( void ) {
	StopSound( SND_CHANNEL_ANY, false );

	if ( isLinked ) {
		SetState( "EnterCinematic", 0 );
		thread->Execute();

		WEAPON_PRIMARY		= false;	// sikk - Primary Attack
		WEAPON_SECONDARY	= false;	// sikk - Secondary Attack
		WEAPON_RELOAD		= false;
		WEAPON_RAISEWEAPON	= false;
		WEAPON_LOWERWEAPON	= false;
// sikk - removed multiplayer
		//WEAPON_NETRELOAD	= false;
		//WEAPON_NETENDRELOAD	= false;
		//WEAPON_NETFIRING	= false;
	}

	disabled = true;

	LowerWeapon();
}

/*
================
idWeapon::ExitCinematic
================
*/
void idWeapon::ExitCinematic( void ) {
	disabled = false;

	if ( isLinked ) {
		SetState( "ExitCinematic", 0 );
		thread->Execute();
	}

	RaiseWeapon();
}

/*
================
idWeapon::NetCatchup
================
*/
// sikk - removed multiplayer
//void idWeapon::NetCatchup( void ) {
//	if ( isLinked ) {
//		SetState( "NetCatchup", 0 );
//		thread->Execute();
//	}
//}

/*
================
idWeapon::GetZoomFov
================
*/
int	idWeapon::GetZoomFov( void ) {
	return zoomFov;
}

/*
================
idWeapon::GetWeaponAngleOffsets
================
*/
void idWeapon::GetWeaponAngleOffsets( int *average, float *scale, float *max ) {
	*average = weaponAngleOffsetAverages;
	*scale = weaponAngleOffsetScale;
	*max = weaponAngleOffsetMax;
}

/*
================
idWeapon::GetWeaponTimeOffsets
================
*/
void idWeapon::GetWeaponTimeOffsets( float *time, float *scale ) {
	*time = weaponOffsetTime;
	*scale = weaponOffsetScale;
}


/***********************************************************************

	Ammo

***********************************************************************/

/*
================
idWeapon::WriteToSnapshot
================
*/
// sikk - removed multiplayer
//void idWeapon::WriteToSnapshot( idBitMsgDelta &msg ) const {
//	msg.WriteBits( worldModel.GetSpawnId(), 32 );
//	msg.WriteBits( lightOn, 1 );
//	msg.WriteBits( isFiring ? 1 : 0, 1 );
//}

/*
================
idWeapon::ReadFromSnapshot
================
*/
// sikk - removed multiplayer
//void idWeapon::ReadFromSnapshot( const idBitMsgDelta &msg ) {	
//	worldModel.SetSpawnId( msg.ReadBits( 32 ) );
//	bool snapLight = msg.ReadBits( 1 ) != 0;
//	isFiring = msg.ReadBits( 1 ) != 0;
//
//	// WEAPON_NETFIRING is only turned on for other clients we're predicting. not for local client
//	if ( owner && gameLocal.localClientNum != owner->entityNumber && WEAPON_NETFIRING.IsLinked() ) {
//
//		// immediately go to the firing state so we don't skip fire animations
//		if ( !WEAPON_NETFIRING && isFiring ) {
//			idealState = "Fire";
//		}
//
//        // immediately switch back to idle
//        if ( WEAPON_NETFIRING && !isFiring ) {
//            idealState = "Idle";
//        }
//
//		WEAPON_NETFIRING = isFiring;
//	}
//
//	if ( snapLight != lightOn ) {
//		Reload();
//	}
//}

/*
================
idWeapon::ClientReceiveEvent
================
*/
// sikk - removed multiplayer
//bool idWeapon::ClientReceiveEvent( int event, int time, const idBitMsg &msg ) {
//
//	switch( event ) {
//		case EVENT_RELOAD: {
//			if ( gameLocal.time - time < 1000 ) {
//				if ( WEAPON_NETRELOAD.IsLinked() ) {
//					WEAPON_NETRELOAD = true;
//					WEAPON_NETENDRELOAD = false;
//				}
//			}
//			return true;
//		}
//		case EVENT_ENDRELOAD: {
//			if ( WEAPON_NETENDRELOAD.IsLinked() ) {
//				WEAPON_NETENDRELOAD = true;
//			}
//			return true;
//		}
//		case EVENT_CHANGESKIN: {
//			int index = gameLocal.ClientRemapDecl( DECL_SKIN, msg.ReadLong() );
//			renderEntity.customSkin = ( index != -1 ) ? static_cast<const idDeclSkin *>( declManager->DeclByIndex( DECL_SKIN, index ) ) : NULL;
//			UpdateVisuals();
//			if ( worldModel.GetEntity() ) {
//				worldModel.GetEntity()->SetSkin( renderEntity.customSkin );
//			}
//			return true;
//		}
//		default: {
//			return idEntity::ClientReceiveEvent( event, time, msg );
//		}
//	}
////	return false;	// sikk - warning C4702: unreachable code
//}

/***********************************************************************

	Script events

***********************************************************************/

/*
===============
idWeapon::Event_Clear
===============
*/
void idWeapon::Event_Clear( void ) {
	Clear();
}

/*
===============
idWeapon::Event_GetOwner
===============
*/
void idWeapon::Event_GetOwner( void ) {
	idThread::ReturnEntity( owner );
}

/*
===============
idWeapon::Event_WeaponState
===============
*/
void idWeapon::Event_WeaponState( const char *statename, int blendFrames ) {
	const function_t *func;

	func = scriptObject.GetFunction( statename );
	if ( !func ) {
		assert( 0 );
		gameLocal.Error( "Can't find function '%s' in object '%s'", statename, scriptObject.GetTypeName() );
	}

	idealState = statename;

	if ( !idealState.Icmp( "Fire" ) ) {
		isFiring = true;
	} else {
		isFiring = false;
	}

	animBlendFrames = blendFrames;
	thread->DoneProcessing();
}

/*
===============
idWeapon::Event_WeaponReady
===============
*/
void idWeapon::Event_WeaponReady( void ) {
	status = WP_READY;
	if ( isLinked ) {
		WEAPON_RAISEWEAPON = false;
	}
	if ( sndHum ) {
		StartSoundShader( sndHum, SND_CHANNEL_BODY, 0, false, NULL );
	}

}

/*
===============
idWeapon::Event_WeaponOutOfAmmo
===============
*/
void idWeapon::Event_WeaponOutOfAmmo( void ) {
	status = WP_OUTOFAMMO;
	if ( isLinked ) {
		WEAPON_RAISEWEAPON = false;
	}
}

/*
===============
idWeapon::Event_WeaponReloading
===============
*/
void idWeapon::Event_WeaponReloading( void ) {
	status = WP_RELOAD;
}

/*
===============
idWeapon::Event_WeaponHolstered
===============
*/
void idWeapon::Event_WeaponHolstered( void ) {
	status = WP_HOLSTERED;
	if ( isLinked ) {
		WEAPON_LOWERWEAPON = false;
	}
}

/*
===============
idWeapon::Event_WeaponRising
===============
*/
void idWeapon::Event_WeaponRising( void ) {
	status = WP_RISING;
	if ( isLinked ) {
		WEAPON_LOWERWEAPON = false;
	}
	owner->WeaponRisingCallback();
}

/*
===============
idWeapon::Event_WeaponLowering
===============
*/
void idWeapon::Event_WeaponLowering( void ) {
	status = WP_LOWERING;
	if ( isLinked ) {
		WEAPON_RAISEWEAPON = false;
	}
	owner->WeaponLoweringCallback();
}

/*
===============
idWeapon::Event_TotalEnergy
===============
*/
void idWeapon::Event_TotalEnergy( void ) {
	idThread::ReturnFloat( owner->GetEnergy() ); // sikk - Universal Energy Pool
}

/*
===============
idWeapon::Event_NetReload
===============
*/
// sikk - removed multiplayer
//void idWeapon::Event_NetReload( void ) {
//	assert( owner );
//	if ( gameLocal.isServer ) {
//		ServerSendEvent( EVENT_RELOAD, NULL, false, -1 );
//	}
//}

/*
===============
idWeapon::Event_NetEndReload
===============
*/
// sikk - removed multiplayer
//void idWeapon::Event_NetEndReload( void ) {
//	assert( owner );
//	if ( gameLocal.isServer ) {
//		ServerSendEvent( EVENT_ENDRELOAD, NULL, false, -1 );
//	}
//}

/*
===============
idWeapon::Event_PlayAnim
===============
*/
void idWeapon::Event_PlayAnim( int channel, const char *animname ) {
	int anim;
	
	anim = animator.GetAnim( animname );
	if ( !anim ) {
		gameLocal.Warning( "missing '%s' animation on '%s' (%s)", animname, name.c_str(), GetEntityDefName() );
		animator.Clear( channel, gameLocal.time, FRAME2MS( animBlendFrames ) );
		animDoneTime = 0;
	} else {
		if ( !( owner && owner->GetInfluenceLevel() ) ) {
			Show();
		}
		animator.PlayAnim( channel, anim, gameLocal.time, FRAME2MS( animBlendFrames ) );
		animDoneTime = animator.CurrentAnim( channel )->GetEndTime();
		if ( worldModel.GetEntity() ) {
			anim = worldModel.GetEntity()->GetAnimator()->GetAnim( animname );
			if ( anim ) {
				worldModel.GetEntity()->GetAnimator()->PlayAnim( channel, anim, gameLocal.time, FRAME2MS( animBlendFrames ) );
			}
		}
	}
	animBlendFrames = 0;
	idThread::ReturnInt( 0 );
}

/*
===============
idWeapon::Event_PlayCycle
===============
*/
void idWeapon::Event_PlayCycle( int channel, const char *animname ) {
	int anim;

	anim = animator.GetAnim( animname );
	if ( !anim ) {
		gameLocal.Warning( "missing '%s' animation on '%s' (%s)", animname, name.c_str(), GetEntityDefName() );
		animator.Clear( channel, gameLocal.time, FRAME2MS( animBlendFrames ) );
		animDoneTime = 0;
	} else {
		if ( !( owner && owner->GetInfluenceLevel() ) ) {
			Show();
		}
		animator.CycleAnim( channel, anim, gameLocal.time, FRAME2MS( animBlendFrames ) );
		animDoneTime = animator.CurrentAnim( channel )->GetEndTime();
		if ( worldModel.GetEntity() ) {
			anim = worldModel.GetEntity()->GetAnimator()->GetAnim( animname );
			worldModel.GetEntity()->GetAnimator()->CycleAnim( channel, anim, gameLocal.time, FRAME2MS( animBlendFrames ) );
		}
	}
	animBlendFrames = 0;
	idThread::ReturnInt( 0 );
}

/*
===============
idWeapon::Event_AnimDone
===============
*/
void idWeapon::Event_AnimDone( int channel, int blendFrames ) {
	if ( animDoneTime - FRAME2MS( blendFrames ) <= gameLocal.time ) {
		idThread::ReturnInt( true );
	} else {
		idThread::ReturnInt( false );
	}
}

/*
===============
idWeapon::Event_SetBlendFrames
===============
*/
void idWeapon::Event_SetBlendFrames( int channel, int blendFrames ) {
	animBlendFrames = blendFrames;
}

/*
===============
idWeapon::Event_GetBlendFrames
===============
*/
void idWeapon::Event_GetBlendFrames( int channel ) {
	idThread::ReturnInt( animBlendFrames );
}

/*
================
idWeapon::Event_Next
================
*/
void idWeapon::Event_Next( void ) {
	// change to another weapon if possible
	owner->NextBestWeapon();
}

/*
================
idWeapon::Event_SetSkin
================
*/
void idWeapon::Event_SetSkin( const char *skinname ) {
	const idDeclSkin *skinDecl;

	if ( !skinname || !skinname[ 0 ] ) {
		skinDecl = NULL;
	} else {
		skinDecl = declManager->FindSkin( skinname );
	}

	renderEntity.customSkin = skinDecl;
	UpdateVisuals();

	if ( worldModel.GetEntity() ) {
		worldModel.GetEntity()->SetSkin( skinDecl );
	}

// sikk - removed multiplayer
	//if ( gameLocal.isServer ) {
	//	idBitMsg	msg;
	//	byte		msgBuf[MAX_EVENT_PARAM_SIZE];

	//	msg.Init( msgBuf, sizeof( msgBuf ) );
	//	msg.WriteLong( ( skinDecl != NULL ) ? gameLocal.ServerRemapDecl( -1, DECL_SKIN, skinDecl->Index() ) : -1 );
	//	ServerSendEvent( EVENT_CHANGESKIN, &msg, false, -1 );
	//}
}

/*
================
idWeapon::Event_Flashlight
================
*/
void idWeapon::Event_Flashlight( int enable ) {
	if ( enable ) {
		lightOn = true;
		MuzzleFlashLight();
	} else {
		lightOn = false;
		muzzleFlashEnd = 0;
	}
}

/*
================
idWeapon::Event_GetLightParm
================
*/
void idWeapon::Event_GetLightParm( int parmnum ) {
	if ( ( parmnum < 0 ) || ( parmnum >= MAX_ENTITY_SHADER_PARMS ) ) {
		gameLocal.Error( "shader parm index (%d) out of range", parmnum );
	}

	idThread::ReturnFloat( muzzleFlash.shaderParms[ parmnum ] );
}

/*
================
idWeapon::Event_SetLightParm
================
*/
void idWeapon::Event_SetLightParm( int parmnum, float value ) {
	if ( ( parmnum < 0 ) || ( parmnum >= MAX_ENTITY_SHADER_PARMS ) ) {
		gameLocal.Error( "shader parm index (%d) out of range", parmnum );
	}

	muzzleFlash.shaderParms[ parmnum ]		= value;
	worldMuzzleFlash.shaderParms[ parmnum ]	= value;
	UpdateVisuals();
}

/*
================
idWeapon::Event_SetLightParms
================
*/
void idWeapon::Event_SetLightParms( float parm0, float parm1, float parm2, float parm3 ) {
	muzzleFlash.shaderParms[ SHADERPARM_RED ]			= parm0;
	muzzleFlash.shaderParms[ SHADERPARM_GREEN ]			= parm1;
	muzzleFlash.shaderParms[ SHADERPARM_BLUE ]			= parm2;
	muzzleFlash.shaderParms[ SHADERPARM_ALPHA ]			= parm3;

	worldMuzzleFlash.shaderParms[ SHADERPARM_RED ]		= parm0;
	worldMuzzleFlash.shaderParms[ SHADERPARM_GREEN ]	= parm1;
	worldMuzzleFlash.shaderParms[ SHADERPARM_BLUE ]		= parm2;
	worldMuzzleFlash.shaderParms[ SHADERPARM_ALPHA ]	= parm3;

	UpdateVisuals();
}

/*
================
idWeapon::Event_CreateProjectile
================
*/
void idWeapon::Event_CreateProjectile( int primary ) {
// sikk - removed multiplayer
	//if ( !gameLocal.isClient ) {
		projectileEnt = NULL;

		gameLocal.SpawnEntityDef( primary ? projectileDict : projectileSecDict, &projectileEnt, false );	// sikk - Secondary Fire
		if ( projectileEnt ) {
			projectileEnt->SetOrigin( GetPhysics()->GetOrigin() );
			projectileEnt->Bind( owner, false );
			projectileEnt->Hide();
		}
		idThread::ReturnEntity( projectileEnt );
	//} else {
	//	idThread::ReturnEntity( NULL );
	//}
}

/*
================
idWeapon::Event_LaunchProjectiles
================
*/
void idWeapon::Event_LaunchProjectiles( int num_projectiles, float spread, float fuseOffset, float launchPower, float dmgPower, int primary ) {
	idProjectile	*proj;
	idEntity		*ent;
	int				i;
	idVec3			dir;
	float			ang;
	float			spin;
	float			distance;
	trace_t			tr;
	idVec3			muzzle_pos;
	idBounds		ownerBounds, projBounds;

	idDict*			projDict = primary ? &projectileDict : &projectileSecDict;	// sikk - Secondary Fire

	if ( IsHidden() ) {
		return;
	}

	if ( !projDict->GetNumKeyVals() ) {
		const char *classname = weaponDef->dict.GetString( "classname" );
		gameLocal.Warning( "No projectile defined on '%s'", classname );
		return;
	}

	// avoid all ammo considerations on an MP client
// sikk - removed multiplayer
	//if ( !gameLocal.isClient ) {
		if ( owner->GetEnergy() <= num_projectiles * projDict->GetInt( "energyAmount", "1" ) )	// sikk - 
			return;
	//}

	if ( !silent_fire ) {
		// wake up nearby monsters
		gameLocal.AlertAI( owner );
	}

	// set the shader parm to the time of last projectile firing,
	// which the gun material shaders can reference for single shot barrel glows, etc
	renderEntity.shaderParms[ SHADERPARM_DIVERSITY ]	= gameLocal.random.CRandomFloat();
	renderEntity.shaderParms[ SHADERPARM_TIMEOFFSET ]	= -MS2SEC( gameLocal.realClientTime );

	if ( worldModel.GetEntity() ) {
		worldModel.GetEntity()->SetShaderParm( SHADERPARM_DIVERSITY, renderEntity.shaderParms[ SHADERPARM_DIVERSITY ] );
		worldModel.GetEntity()->SetShaderParm( SHADERPARM_TIMEOFFSET, renderEntity.shaderParms[ SHADERPARM_TIMEOFFSET ] );
	}

	// calculate the muzzle position
	if ( barrelJointView != INVALID_JOINT && projDict->GetBool( "launchFromBarrel" ) ) {
		// there is an explicit joint for the muzzle
		GetGlobalJointTransform( true, barrelJointView, muzzleOrigin, muzzleAxis );
	} else {
		// go straight out of the view
		muzzleOrigin = playerViewOrigin;
		muzzleAxis = playerViewAxis;
	}

	// add some to the kick time, incrementally moving repeat firing weapons back
	if ( kick_endtime < gameLocal.realClientTime ) {
		kick_endtime = gameLocal.realClientTime;
	}

// ---> sikk - Secondary Fire
	int mkMaxTime = ( WEAPON_PRIMARY ? muzzle_kick_maxtime : muzzle_kick_maxtime_sec );
	kick_endtime += ( WEAPON_PRIMARY ? muzzle_kick_time : muzzle_kick_time_sec );
	if ( kick_endtime > gameLocal.realClientTime + mkMaxTime ) {
		kick_endtime = gameLocal.realClientTime + mkMaxTime;
	}
// <--- sikk - Secondary Fire

// sikk - removed multiplayer
	//if ( gameLocal.isClient ) {
	//	// predict instant hit projectiles
	//	if ( projDict->GetBool( "net_instanthit" ) ) {
	//		float spreadRad = DEG2RAD( spread );
	//		muzzle_pos = muzzleOrigin + playerViewAxis[ 0 ] * 2.0f;
	//		for( i = 0; i < num_projectiles; i++ ) {
	//			ang = idMath::Sin( spreadRad * gameLocal.random.RandomFloat() );
	//			spin = (float)DEG2RAD( 360.0f ) * gameLocal.random.RandomFloat();
	//			dir = playerViewAxis[ 0 ] + playerViewAxis[ 2 ] * ( ang * idMath::Sin( spin ) ) - playerViewAxis[ 1 ] * ( ang * idMath::Cos( spin ) );
	//			dir.Normalize();
	//			gameLocal.clip.Translation( tr, muzzle_pos, muzzle_pos + dir * 4096.0f, NULL, mat3_identity, MASK_SHOT_RENDERMODEL, owner );
	//			if ( tr.fraction < 1.0f ) {
	//				idProjectile::ClientPredictionCollide( this, *projDict, tr, vec3_origin, true );
	//			}
	//		}
	//	}
	//} else {
		ownerBounds = owner->GetPhysics()->GetAbsBounds();

		owner->AddProjectilesFired( num_projectiles * owner->modifiers.weapon_projectiledoubler );	// sikk - Modifier System

// ---> sikk - Projectiles will launch from the weapons barrel but be directed towards 
//			   what is being aimed at through the center of the first person view
		idVec3 start = playerViewOrigin;
		idVec3 end = start + playerViewAxis[ 0 ] * projDict->GetFloat( "range", "32768" );
		int contents = MASK_SHOT_RENDERMODEL | CONTENTS_PROJECTILE;
		gameLocal.clip.TracePoint( tr, start, end, contents, owner );
		idVec3 projDir = tr.endpos - muzzleOrigin;
		projDir.Normalize();
		idMat3 projAxis = projDir.ToMat3();
// <--- sikk -

		int spreadMod = ( owner->GetActiveModifier() == MOD_WEAPON_PROJECTILEDOUBLER ) ? 2 : 0;	// sikk - Modifier System
		float spreadRad = DEG2RAD( spread + spreadMod );
		for ( i = 0; i < num_projectiles * owner->modifiers.weapon_projectiledoubler; i++ ) {	// sikk - Modifier System
			ang = idMath::Sin( spreadRad * gameLocal.random.RandomFloat() );
			spin = (float)DEG2RAD( 360.0f ) * gameLocal.random.RandomFloat();
			dir = projAxis[ 0 ] + projAxis[ 2 ] * ( ang * idMath::Sin( spin ) ) - projAxis[ 1 ] * ( ang * idMath::Cos( spin ) );
			dir.Normalize();

// ---> sikk - Hitscan
			if ( spawnArgs.GetBool( "hitscan", "0" ) ) {

// ---> sikk - Railgun Beam
				idVec3 beanEnd;
				if ( projDict->GetFloat( "penetrate" ) ) {
					beanEnd = playerViewOrigin + ( dir.ToMat3() * idVec3( projDict->GetFloat( "range", "32768" ), 0, 0 ) );
				} else {
					beanEnd = tr.endpos;
				}
				if ( primary ) {
					railbeamTime = gameLocal.time;
					if ( railbeamModelDefHandle >= 0 ) {
						gameRenderWorld->FreeEntityDef( railbeamModelDefHandle );
						railbeamModelDefHandle = -1;
					}
				} else {
					//idEntityFx::StartFx( projDict->GetString( "fx_impact" ), &beanEnd, &tr.endAxis, NULL, false );
					if ( railbeamFX == NULL ) {
						railbeamFX->StartFx( projDict->GetString( "fx_impact" ), &beanEnd, &tr.endAxis, NULL, false );
	
						// delete the testModel if active
	//					if ( !railbeamFX ) {
							//delete railbeamFX;
							//railbeamFX = NULL;
						//idDict dict;

						//dict.Set( "fx", projDict->GetString( "fx_impact" ) );
						//dict.SetVector( "origin", beanEnd );
						//dict.SetBool( "start", true );
						//railbeamFX = ( idEntityFx * )gameLocal.SpawnEntityType( idEntityFx::Type, &dict );
//						railbeamFX->Start( gameLocal.time );
						}
				}

				if ( railbeamModelDefHandle == -1 ) {
					memset( &railbeamRenderEntity, 0, sizeof( renderEntity_t ) );
					railbeamRenderEntity.origin = muzzleOrigin;
					railbeamRenderEntity.axis = muzzleAxis;
					railbeamRenderEntity.shaderParms[ SHADERPARM_BEAM_END_X ]	= beanEnd.x;
					railbeamRenderEntity.shaderParms[ SHADERPARM_BEAM_END_Y ]	= beanEnd.y;
					railbeamRenderEntity.shaderParms[ SHADERPARM_BEAM_END_Z ]	= beanEnd.z;
					railbeamRenderEntity.shaderParms[ SHADERPARM_BEAM_WIDTH ]	= 8.0f;
					railbeamRenderEntity.shaderParms[ SHADERPARM_RED ]			= 1.0f;
					railbeamRenderEntity.shaderParms[ SHADERPARM_GREEN ]		= 1.0f;
					railbeamRenderEntity.shaderParms[ SHADERPARM_BLUE ]			= 1.0f;
					railbeamRenderEntity.shaderParms[ SHADERPARM_ALPHA ]		= 1.0f;
					railbeamRenderEntity.hModel = renderModelManager->FindModel( "_beam" );
					railbeamRenderEntity.callback = NULL;
					railbeamRenderEntity.numJoints = 0;
					railbeamRenderEntity.joints = NULL;
					railbeamRenderEntity.bounds.Clear();
					railbeamRenderEntity.customSkin = declManager->FindSkin( projDict->GetString( "skin" ) );
					railbeamModelDefHandle = gameRenderWorld->AddEntityDef( &railbeamRenderEntity );
				}
// <--- sikk - Railgun Beam
				
				gameLocal.HitScan( *projDict, playerViewOrigin, playerViewAxis[ 0 ], playerViewOrigin, owner, false, 1.0f );
// <--- sikk - Hitscan
			} else {
				if ( projectileEnt ) {
					ent = projectileEnt;
					ent->Show();
					ent->Unbind();
					projectileEnt = NULL;
				} else {
					gameLocal.SpawnEntityDef( *projDict, &ent, false );
				}

				if ( !ent || !ent->IsType( idProjectile::Type ) ) {
					const char *projectileName = weaponDef->dict.GetString( primary ? "def_projectile" : "def_projectile_sec" );
					gameLocal.Error( "'%s' is not an idProjectile", projectileName );
				}

				proj = static_cast<idProjectile *>(ent);
				proj->Create( owner, muzzleOrigin, dir );

				projBounds = proj->GetPhysics()->GetBounds().Rotate( proj->GetPhysics()->GetAxis() );

				// make sure the projectile starts inside the bounding box of the owner
				if ( i == 0 ) {
					muzzle_pos = muzzleOrigin + playerViewAxis[ 0 ] * 2.0f;
					if ( ( ownerBounds - projBounds).RayIntersection( muzzle_pos, playerViewAxis[0], distance ) ) {
						start = muzzle_pos + distance * playerViewAxis[ 0 ];
					} else {
						start = ownerBounds.GetCenter();
					}
					gameLocal.clip.Translation( tr, start, muzzle_pos, proj->GetPhysics()->GetClipModel(), proj->GetPhysics()->GetClipModel()->GetAxis(), MASK_SHOT_RENDERMODEL, owner );
					muzzle_pos = tr.endpos;
				}
// ---> sikk - Player's forward velocity affects projectile's velocity & damage
				float newlaunchPower = launchPower * owner->modifiers.weapon_projectilespeed;	// sikk - Modifier System
				newlaunchPower *= owner->velocityDamageScale + 1.0f;
				float newdmgPower = dmgPower * owner->velocityDamageScale + 1.0f;

//				float velocity = projDict->GetVector( "velocity", "0 0 0" );
// <--- sikk -
				proj->Launch( muzzle_pos, dir, pushVelocity, fuseOffset, newlaunchPower, newdmgPower );
			}

// ---> sikk - Modifier System
			int amount = projDict->GetInt( "energyAmount", "1" );
			if ( amount ) {
				float modifier = ( owner->GetActiveModifier() == MOD_ENERGY_CONSERVATION ) ? 0.5f : 1.0f;
				amount *= modifier;
				amount = ( amount < 1 ) ? 1 : amount;
			}
// <--- sikk - Modifier System
			if ( owner->GetEnergy() - amount <= 0 ) {
				owner->GiveEnergy( -( owner->GetEnergy() - 1 ) );	// sikk - Consume energy
			} else {
				owner->GiveEnergy( -amount );	// sikk - Consume energy
			}
			owner->lastFireTime = gameLocal.time;	// set last fire time
		}

		// toss the brass
		PostEventMS( &EV_Weapon_EjectBrass, brassDelay );
// sikk - removed multiplayer
	//}

	// add the light for the muzzleflash
	if ( !lightOn ) {
		MuzzleFlashLight();
	}

	owner->WeaponFireFeedback( &weaponDef->dict, primary );	// sikk - Secondary Fire

	// reset muzzle smoke
	weaponSmokeStartTime = gameLocal.realClientTime;
}

/*
=====================
idWeapon::Event_Melee
=====================
*/
void idWeapon::Event_Melee( int primary ) {
	idEntity	*ent;
	trace_t		tr;

	if ( !meleeDef ) {
		gameLocal.Error( "No meleeDef on '%s'", weaponDef->dict.GetString( "classname" ) );
	}

// sikk - removed multiplayer
	//if ( !gameLocal.isClient ) {
		idVec3 start = playerViewOrigin;
		idVec3 end = start + playerViewAxis[0] * ( meleeDistance * ( owner->PowerUpActive( MEGADAMAGE ) + 1.0f ) );
		gameLocal.clip.TracePoint( tr, start, end, MASK_SHOT_RENDERMODEL, owner );
		if ( tr.fraction < 1.0f ) {
			ent = gameLocal.GetTraceEntity( tr );
		} else {
			ent = NULL;
		}

		if ( g_debugWeapon.GetBool() ) {
			gameRenderWorld->DebugLine( colorYellow, start, end, 100 );
			if ( ent ) {
				gameRenderWorld->DebugBounds( colorRed, ent->GetPhysics()->GetBounds(), ent->GetPhysics()->GetOrigin(), 100 );
			}
		}

		bool hit = false;
		const char *hitSound = meleeDef->dict.GetString( "snd_miss" );

		if ( ent ) {
			float push = meleeDef->dict.GetFloat( "push" );
			idVec3 impulse = -push * tr.c.normal;

			if ( gameLocal.world->spawnArgs.GetBool( "no_Weapons" ) && ( ent->IsType( idActor::Type ) || ent->IsType( idAFAttachment::Type) ) ) {
				idThread::ReturnInt( 0 );
				return;
			}

			ent->ApplyImpulse( this, tr.c.id, tr.c.point, impulse );

			// weapon stealing - do this before damaging so weapons are not dropped twice
// sikk - removed multiplayer
			//if ( gameLocal.isMultiplayer
			//	&& weaponDef && weaponDef->dict.GetBool( "stealing" )
			//	&& ent->IsType( idPlayer::Type )
			//	&& !owner->PowerUpActive( BERSERK )
			//	&& ( gameLocal.gameType != GAME_TDM || gameLocal.serverInfo.GetBool( "si_teamDamage" ) || ( owner->team != static_cast< idPlayer * >( ent )->team ) )
			//	) {
			//	owner->StealWeapon( static_cast< idPlayer * >( ent ) );
			//}

			if ( ent->fl.takedamage ) {
				idVec3 kickDir, globalKickDir;
				meleeDef->dict.GetVector( "kickDir", "0 0 0", kickDir );
				globalKickDir = muzzleAxis * kickDir;

				float dmgScale = owner->velocityDamageScale + ( owner->PowerUpActive( MEGADAMAGE ) * 3.0f + 1.0f );	// sikk - Player's forward veloctiy affects melee damage scale

				ent->Damage( owner, owner, globalKickDir, meleeDefName, dmgScale, tr.c.id );
				hit = true;

// ---> sikk - Modifier System
				if ( owner->modifiers.weapon_incendiary ) {
					const idDict *damageDef = gameLocal.FindEntityDefDict( meleeDefName );
					if ( damageDef ) {
						ent->burnPower += damageDef->GetInt( "damage" ) * dmgScale * 0.1f;
						if ( ent->burnPower < 1 )
							ent->burnPower = 1;
					}
					ent->burnTime = gameLocal.time + SEC2MS( g_burnTime.GetFloat() );
				}
// <--- sikk - Modifier System

// ---> sikk - Chainsaw View Sticking
				if ( ent->IsType( idAI::Type ) && !idStr::Icmp( weaponDef->GetName(), "weapon_chainsaw" ) ) {
					idVec3 playerOrigin;
					idMat3 playerAxis;
					idVec3 targetVec = ent->GetPhysics()->GetAbsBounds().GetCenter();
					owner->GetViewPos( playerOrigin, playerAxis );
					targetVec = ent->GetPhysics()->GetAbsBounds().GetCenter() - playerOrigin;
					targetVec[2] *= 0.5f;
					targetVec.Normalize();
					idAngles delta = targetVec.ToAngles() - owner->cmdAngles - owner->GetDeltaViewAngles();
					delta.Normalize180();
					float fade = 1.0f - idMath::Fabs( playerAxis[ 0 ].z );

					// move the view towards the monster
					owner->SetDeltaViewAngles( owner->GetDeltaViewAngles() + delta * fade );

					// push the player towards the monster
					owner->ApplyImpulse( ent, 0, playerOrigin, playerAxis[ 0 ] * 20000.0f );
				}
// <--- sikk - Chainsaw View Sticking
			}

			if ( weaponDef->dict.GetBool( "impact_damage_effect" ) ) {
				if ( ent->spawnArgs.GetBool( "bleed" ) ) {
					hitSound = meleeDef->dict.GetString( owner->PowerUpActive( MEGADAMAGE ) ? "snd_hit_megadamage" : "snd_hit" );
					ent->AddDamageEffect( tr, impulse, meleeDef->dict.GetString( "classname" ) );
				} else {
					int type = tr.c.material->GetSurfaceType();
					if ( type == SURFTYPE_NONE ) {
						type = GetDefaultSurfaceType();
					}

					const char *materialType = gameLocal.sufaceTypeNames[ type ];

					// start impact sound based on material type
					hitSound = meleeDef->dict.GetString( va( "snd_%s", materialType ) );
					if ( *hitSound == '\0' ) {
						hitSound = meleeDef->dict.GetString( "snd_metal" );
					}

					if ( gameLocal.time > nextStrikeFx ) {
						const char *decal;
						// project decal
						decal = weaponDef->dict.GetString( "mtr_strike" );
						if ( decal && *decal ) {
							gameLocal.ProjectDecal( tr.c.point, -tr.c.normal, 8.0f, true, 6.0, decal );
						}
						nextStrikeFx = gameLocal.time + 200;
					} else {
						hitSound = "";
					}

					strikeSmokeStartTime = gameLocal.time;
					strikePos = tr.c.point;
					strikeAxis = -tr.endAxis;
				}
			}
		}

		if ( *hitSound != '\0' ) {
			const idSoundShader *snd = declManager->FindSound( hitSound );
			StartSoundShader( snd, SND_CHANNEL_BODY2, 0, true, NULL );
		}

		idThread::ReturnInt( hit );
		owner->WeaponFireFeedback( &weaponDef->dict, primary );	// sikk - Secondary Fire

// ---> sikk - Blood Spray Screen Effect
		if ( g_showBloodSpray.GetBool() ) {
			if ( ( gameLocal.random.RandomFloat() * 0.99999f ) < g_bloodSprayFrequency.GetFloat() && hit )
				GetOwner()->playerView.AddBloodSpray( g_bloodSprayTime.GetFloat() );
		}
// <--- sikk - Blood Spray Screen Effect

		return;
	//}

	//idThread::ReturnInt( 0 );
	//owner->WeaponFireFeedback( &weaponDef->dict );
}

/*
=====================
idWeapon::Event_GetWorldModel
=====================
*/
void idWeapon::Event_GetWorldModel( void ) {
	idThread::ReturnEntity( worldModel.GetEntity() );
}

/*
================
idWeapon::Event_EjectBrass

Toss a shell model out from the breach if the bone is present
================
*/
void idWeapon::Event_EjectBrass( void ) {
	if ( !g_showBrass.GetBool() || !owner->CanShowWeaponViewmodel() ) {
		return;
	}

	if ( ejectJointView == INVALID_JOINT || !brassDict.GetNumKeyVals() ) {
		return;
	}

// sikk - removed multiplayer
	//if ( gameLocal.isClient ) {
	//	return;
	//}

	idMat3 axis;
	idVec3 origin, linear_velocity, angular_velocity;
	idEntity *ent;

	if ( !GetGlobalJointTransform( true, ejectJointView, origin, axis ) ) {
		return;
	}

	gameLocal.SpawnEntityDef( brassDict, &ent, false );
	if ( !ent || !ent->IsType( idDebris::Type ) ) {
		gameLocal.Error( "'%s' is not an idDebris", weaponDef ? weaponDef->dict.GetString( "def_ejectBrass" ) : "def_ejectBrass" );
	}
	idDebris *debris = static_cast<idDebris *>(ent);
	debris->Create( owner, origin, axis );
	debris->Launch();

	linear_velocity = 40 * ( playerViewAxis[0] + playerViewAxis[1] + playerViewAxis[2] );
	angular_velocity.Set( 10 * gameLocal.random.CRandomFloat(), 10 * gameLocal.random.CRandomFloat(), 10 * gameLocal.random.CRandomFloat() );

	debris->GetPhysics()->SetLinearVelocity( linear_velocity );
	debris->GetPhysics()->SetAngularVelocity( angular_velocity );
}

// ---> sikk - Powerup Checks for skin update
/*
===============
idWeapon::Event_IsMegadamage
===============
*/
void idWeapon::Event_IsMegadamage( void ) {
	if ( !owner ) {
		idThread::ReturnFloat( 0 );
		return;
	}
	idThread::ReturnFloat( owner->PowerUpActive( MEGADAMAGE ) ? 1 : 0 );
}

/*
===============
idWeapon::Event_IsMegashield
===============
*/
void idWeapon::Event_IsMegashield( void ) {
	if ( !owner ) {
		idThread::ReturnFloat( 0 );
		return;
	}
	idThread::ReturnFloat( owner->PowerUpActive( MEGASHIELD ) ? 1 : 0 );
}
// <--- sikk - Powerup Checks for skin update

/*
===============
idWeapon::Event_IsInvisible
===============
*/
void idWeapon::Event_IsInvisible( void ) {
	if ( !owner ) {
		idThread::ReturnFloat( 0 );
		return;
	}
	idThread::ReturnFloat( owner->PowerUpActive( MEGASHIELD ) ? 1 : 0 );
}

/*
===============
idWeapon::ClientPredictionThink
===============
*/
// sikk - removed multiplayer
//void idWeapon::ClientPredictionThink( void ) {
//	UpdateAnimation();	
//}