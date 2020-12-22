/*
================

Spawner.cpp

================
*/

#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"
#include "Spawner.h"
#include "ai/AI.h"

const idEventDef EV_Spawner_RemoveNullActiveEntities( "removeNullActiveEntities" );
const idEventDef EV_Spawner_NumActiveEntities( "numActiveEntities", "", 'd' );
const idEventDef EV_Spawner_GetActiveEntity( "getActiveEntity", "d", 'e' );

CLASS_DECLARATION( idEntity, idSpawner )
EVENT( EV_Activate,								idSpawner::Event_Activate )
EVENT( EV_Spawner_RemoveNullActiveEntities,		idSpawner::Event_RemoveNullActiveEntities )
EVENT( EV_Spawner_NumActiveEntities,			idSpawner::Event_NumActiveEntities )
EVENT( EV_Spawner_GetActiveEntity,				idSpawner::Event_GetActiveEntity )
END_CLASS

/*
==============
idSpawner::Spawn
==============
*/
void idSpawner::Spawn()
{
	GetPhysics()->SetContents( 0 );

	// TEMP: read max_team_test until we can get it out of all the current maps
	if( !spawnArgs.GetInt( "max_active", "4", maxActive ) )
	{
		if( spawnArgs.GetInt( "max_team_test", "4", maxActive ) )
		{
			gameLocal.Warning( "spawner '%s' using outdated 'max_team_test', please change to 'max_active'", GetName() );
		}
	}

	maxToSpawn		= spawnArgs.GetInt( "count", "-1" );
	skipVisible		= spawnArgs.GetBool( "skipvisible", "0" );
	spawnWaves		= spawnArgs.GetInt( "waves", "1" );
	spawnDelay		= SEC2MS( spawnArgs.GetFloat( "delay", "2" ) );
	numSpawned		= 0;
	nextSpawnTime	= 0;

	// Spawn waves has to be less than max active
	if( spawnWaves > maxActive )
	{
		spawnWaves = maxActive;
	}

	FindSpawnTypes();
}

/*
==============
idSpawner::Save
==============
*/
void idSpawner::Save( idSaveGame* savefile ) const
{
	int i;

	savefile->WriteInt( numSpawned );
	savefile->WriteInt( maxToSpawn );
	savefile->WriteFloat( nextSpawnTime );
	savefile->WriteInt( maxActive );

	savefile->WriteInt( currentActive.Num() );
	for( i = 0; i < currentActive.Num(); i++ )
	{
		currentActive[ i ].Save( savefile );
	}

	savefile->WriteInt( spawnWaves );
	savefile->WriteInt( spawnDelay );
	savefile->WriteBool( skipVisible );

	savefile->WriteInt( spawnPoints.Num() );
	for( i = 0; i < spawnPoints.Num(); i++ )
	{
		spawnPoints[ i ].Save( savefile );
	}

	savefile->WriteInt( callbacks.Num() );
	for( i = 0; i < callbacks.Num(); i++ )
	{
		callbacks[ i ].ent.Save( savefile );
		savefile->WriteString( callbacks[ i ].event );
	}
}

/*
==============
idSpawner::Restore
==============
*/
void idSpawner::Restore( idRestoreGame* savefile )
{
	int num;
	int i;

	savefile->ReadInt( numSpawned );
	savefile->ReadInt( maxToSpawn );
	savefile->ReadFloat( nextSpawnTime );
	savefile->ReadInt( maxActive );

	savefile->ReadInt( num );
	currentActive.Clear( );
	currentActive.SetNum( num );
	for( i = 0; i < num; i++ )
	{
		currentActive[ i ].Restore( savefile );
	}

	savefile->ReadInt( spawnWaves );
	savefile->ReadInt( spawnDelay );
	savefile->ReadBool( skipVisible );

	savefile->ReadInt( num );
	spawnPoints.SetNum( num );
	for( i = 0; i < num; i ++ )
	{
		spawnPoints[ i ].Restore( savefile );
	}

	savefile->ReadInt( num );
	callbacks.SetNum( num );
	for( i = 0; i < num; i ++ )
	{
		callbacks[ i ].ent.Restore( savefile );
		savefile->ReadString( callbacks[ i ].event );
	}

	FindSpawnTypes();
}

/*
==============
idSpawner::FindSpawnTypes

Generate the list of classnames to spawn from the spawnArgs.  Anything matching the
prefix "def_spawn" will be included in the list.
==============
*/
void idSpawner::FindSpawnTypes()
{
	const idKeyValue* kv;
	for( kv = spawnArgs.MatchPrefix( "def_spawn", NULL ); kv; kv = spawnArgs.MatchPrefix( "def_spawn", kv ) )
	{
		spawnTypes.Append( kv->GetValue() );

		// precache decls
		declManager->FindType( DECL_AF, kv->GetValue(), true );
	}
}

/*
==============
idSpawner::FindTargets
==============
*/
void idSpawner::FindTargets()
{
	int i;
	idBounds bounds( idVec3( -16, -16, 0 ), idVec3( 16, 16, 72 ) );
	trace_t tr;

	idEntity::FindTargets();
	if( !spawnPoints.Num() )
	{
		const idKeyValue* kv;
		for( kv = spawnArgs.MatchPrefix( "target", NULL ); kv; kv = spawnArgs.MatchPrefix( "target", kv ) )
		{
			spawnPoints.Append( gameLocal.FindEntity( kv->GetValue() ) );
		}
	}

	// Copy the relevant targets to the spawn point list (right now only target_null entities)
	for( i = targets.Num() - 1; i >= 0; i-- )
	{
		idEntity* ent;
		ent = targets[ i ].GetEntity();
		if( idStr::Icmp( ent->spawnArgs.GetString( "classname" ), "target_null" ) )
		{
			continue;
		}

		idEntityPtr<idEntity>& entityPtr = spawnPoints.Alloc();
		entityPtr = ent;

		if( !spawnArgs.GetBool( "ignoreSpawnPointValidation" ) )
		{
			gameLocal.clip.TraceBounds( tr, ent->GetPhysics()->GetOrigin(), ent->GetPhysics()->GetOrigin(), bounds, MASK_MONSTERSOLID, NULL );
			if( gameLocal.entities[ tr.c.entityNum ] && !gameLocal.entities[ tr.c.entityNum ]->IsType( idActor::Type ) )
			{
				//drop a console warning here
				gameLocal.Warning( "Spawner '%s' can't spawn at point '%s', the monster won't fit.", GetName(), ent->GetName() );
			}
		}
	}
}

/*
==============
idSpawner::ValidateSpawnPoint
==============
*/
bool idSpawner::ValidateSpawnPoint( const idVec3 origin, const idBounds& bounds )
{
	trace_t tr;
	if( spawnArgs.GetBool( "ignoreSpawnPointValidation" ) )
	{
		return true;
	}

	gameLocal.clip.TraceBounds( tr, origin, origin, bounds, MASK_MONSTERSOLID, NULL );
	return tr.fraction >= 1.0f;
}

/*
==============
idSpawner::AddSpawnPoint
==============
*/
void idSpawner::AddSpawnPoint( idEntity* point )
{
	idEntityPtr<idEntity>& entityPtr = spawnPoints.Alloc();
	entityPtr = point;

	// If there were no spawnPoints then start with the delay
	if( spawnPoints.Num() == 1 )
	{
		nextSpawnTime = gameLocal.time + spawnDelay;
	}
}

/*
==============
idSpawner::RemoveSpawnPoint
==============
*/
void idSpawner::RemoveSpawnPoint( idEntity* point )
{
	int i;
	for( i = spawnPoints.Num() - 1; i >= 0; i-- )
	{
		if( spawnPoints[ i ].GetEntity() == point )
		{
			spawnPoints.RemoveIndex( i );
			break;
		}
	}
}

/*
==============
idSpawner::GetSpawnPoint
==============
*/
void idSpawner::AddCallback( idEntity* owner, const idEventDef* ev )
{
	spawnerCallback_t& callback = callbacks.Alloc();
	callback.event = ev->GetName();
	callback.ent = owner;
}

/*
==============
idSpawner::GetSpawnPoint
==============
*/
idEntity* idSpawner::GetSpawnPoint()
{
	idBounds bounds( idVec3( -16, -16, 0 ), idVec3( 16, 16, 72 ) );
	idList< idEntityPtr<idEntity> >	spawns;
	int spawnIndex;
	idEntity* spawnEnt;

	if( !spawnPoints.Num() )
	{
		const idKeyValue* kv;
		for( kv = spawnArgs.MatchPrefix( "target", NULL ); kv; kv = spawnArgs.MatchPrefix( "target", kv ) )
		{
			spawnPoints.Append( gameLocal.FindEntity( kv->GetValue() ) );
		}
	}

	// Run through all spawnPoints and choose a random one. Each time a spawn point is excluded
	// it will be removed from the list until there are no more items in the list.
	for( spawns = spawnPoints; spawns.Num(); spawns.RemoveIndex( spawnIndex ) )
	{
		spawnIndex = gameLocal.random.RandomInt( spawns.Num() );
		spawnEnt   = spawns[ spawnIndex ].GetEntity();

		if( !spawnEnt || !spawnEnt->GetPhysics() )
		{
			continue;
		}

		// Check to see if something is in the way at this spawn point
		if( !ValidateSpawnPoint( spawnEnt->GetPhysics()->GetOrigin(), bounds ) )
		{
			continue;
		}

		// Skip the spawn point because its currently visible?
		if( skipVisible && gameLocal.GetLocalPlayer()->CanSee( spawnEnt, true ) )
		{
			continue;
		}

		// Found one!
		return spawnEnt;
	}

	return NULL;
}

/*
==============
idSpawner::GetSpawnType
==============
*/
const char* idSpawner::GetSpawnType( idEntity* spawnPoint )
{
	const idKeyValue* kv;

	if( spawnPoint )
	{
		// If the spawn point has any "def_spawn" keys then they override the normal spawn keys
		kv = spawnPoint->spawnArgs.MatchPrefix( "def_spawn", NULL );
		if( kv )
		{
			const char* types[ MAX_SPAWN_TYPES ];
			int			typeCount;

			for( typeCount = 0; typeCount < MAX_SPAWN_TYPES && kv; kv = spawnPoint->spawnArgs.MatchPrefix( "def_spawn", kv ) )
			{
				types[ typeCount++ ] = kv->GetValue().c_str();
			}

			return types[ gameLocal.random.RandomInt( typeCount ) ];
		}
	}

	// No spawn types?
	if( !spawnTypes.Num() )
	{
		return "";
	}

	// Return from the spawners list of types
	return spawnTypes[ gameLocal.random.RandomInt( spawnTypes.Num() ) ];
}

/*
==============
idSpawner::CopyPrefixedSpawnArgs
==============
*/
void idSpawner::CopyPrefixedSpawnArgs( idEntity* src, const char* prefix, idDict& args )
{
	const idKeyValue* kv = src->spawnArgs.MatchPrefix( prefix, NULL );
	while( kv )
	{
		args.Set( kv->GetKey().c_str() + idStr::Length( prefix ), kv->GetValue() );
		kv = src->spawnArgs.MatchPrefix( prefix, kv );
	}
}

/*
==============
idSpawner::SpawnEnt
==============
*/
bool idSpawner::SpawnEnt()
{
	idDict		args;
	idEntity*	spawnPoint;
	idEntity*	spawnedEnt;
	const char* temp;

	// Find a spawn point to spawn the entity
	spawnPoint = GetSpawnPoint();

	if( spawnPoint == NULL )
	{
		return false;
	}

	// No valid spawn types for this point
	temp = GetSpawnType( spawnPoint );
	if( !temp || !*temp )
	{
		gameLocal.Warning( "Spawner '%s' could not find any valid spawn types for spawn point '%s'", GetName(), spawnPoint->GetName() );
		return false;
	}

	// Build the spawn parameters for the entity about to be spawned
	args.Set( "origin", spawnPoint->GetPhysics()->GetOrigin().ToString() );
	if( spawnArgs.GetBool( "face_enemy", "0" ) )
	{
		idVec3 dir = gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin() - spawnPoint->GetPhysics()->GetOrigin();
		dir.Normalize();
		args.SetFloat( "angle", dir.ToYaw() );
	}
	else
	{
		args.SetFloat( "angle", spawnPoint->GetPhysics()->GetAxis().ToAngles()[ YAW ] );
	}
	args.Set( "classname", temp );
	args.SetBool( "forceEnemy", spawnArgs.GetBool( "auto_target", "1" ) );
	args.SetInt( "teleport", spawnArgs.GetInt( "teleport", "4" ) );

	// Copy all keywords prefixed with "spawn_" to the entity being spawned.
	CopyPrefixedSpawnArgs( this, "spawn_", args );
	if( spawnPoint != this )
	{
		CopyPrefixedSpawnArgs( spawnPoint, "spawn_", args );
	}

	// Spawn the entity
	if( !gameLocal.SpawnEntityDef( args, &spawnedEnt ) )
	{
		return false;
	}

	// Activate the spawned entity
	spawnedEnt->ProcessEvent( &EV_Activate, this );

	// script function for spawning guys
	//if ( spawnArgs.GetString( "call", "", &temp ) && *temp ) {
	//	gameLocal.CallFrameCommand( this, temp );
	//}

	// script function for the guy being spawned
	//if ( spawnArgs.GetString( "call_spawned", "", &temp ) && *temp ) {
	//	gameLocal.CallFrameCommand( spawnedEnt, temp );
	//}

	// Call all of our callbacks
	//int c;
	//for ( c = callbacks.Num() - 1; c >= 0; c-- ) {
	//	if ( callbacks[ c ].ent ) {
	//		callbacks[ c ].ent->ProcessEvent( idEventDef::FindEvent( callbacks[ c ].event ), this, spawnedEnt );
	//	}
	//}

	// Activate the spawn point entity when an enemy is spawned there and all of its targets
	if( spawnPoint != this )
	{
		spawnPoint->ProcessEvent( &EV_Activate, spawnPoint );
		spawnPoint->ActivateTargets( spawnedEnt );

		// One time use on this target?
		if( spawnPoint->spawnArgs.GetBool( "remove" ) )
		{
			spawnPoint->PostEventMS( &EV_Remove, 0 );
		}
	}

	// Increment the total number spawned
	numSpawned++;

	return true;
}

/*
==============
idSpawner::Think
==============
*/
void idSpawner::Think()
{
	if( thinkFlags & TH_THINK )
	{
		if( ActiveListChanged() )   // If an entity has been removed and we have not been informed via Detach
		{
			nextSpawnTime = gameLocal.time + spawnDelay;
		}

		CheckSpawn();
	}
}

/*
==============
idSpawner::CheckSpawn
==============
*/
void idSpawner::CheckSpawn()
{
	int count;

	if( !spawnPoints.Num() )
	{
		const idKeyValue* kv;
		for( kv = spawnArgs.MatchPrefix( "target", NULL ); kv; kv = spawnArgs.MatchPrefix( "target", kv ) )
		{
			spawnPoints.Append( gameLocal.FindEntity( kv->GetValue() ) );
		}
	}

	// Any spawn points?
	if( !spawnPoints.Num() )
	{
		return;
	}

	// Is it time to spawn yet?
	if( nextSpawnTime == 0 || gameLocal.time < nextSpawnTime )
	{
		return;
	}

	// Any left to spawn?
	if( maxToSpawn > -1 && numSpawned >= maxToSpawn )
	{
		// Activate trigger entity when last enemey has been killed
		if( currentActive.Num() == 0 )
		{
			idEntity* ent = gameLocal.FindEntity( spawnArgs.GetString( "trigger_used_up" ) );
			if( !ent )
			{
				return;
			}
			ent->Signal( SIG_TRIGGER );
			ent->ProcessEvent( &EV_Activate, this );
			ent->TriggerGuis();
			PostEventMS( &EV_Remove, 0 );
		}
		if( !spawnArgs.GetString( "trigger_used_up" )[ 0 ] )
		{
			PostEventMS( &EV_Remove, 0 );
		}
		return;
	}

	// Spawn in waves?
	for( count = 0; count < spawnWaves; count++ )
	{
		// Too many active?
		if( currentActive.Num() >= maxActive )
		{
			return;
		}

		// Spawn a new entity
		SpawnEnt();

		// Are we at the limit now?
		if( maxToSpawn > -1 && numSpawned >= maxToSpawn )
		{
			CallScriptEvents( "call_used_up", this );
			break;
		}
	}

	// Dont spawn again until after the delay
	nextSpawnTime = gameLocal.time + spawnDelay;
}

/*
==============
idSpawner::CallScriptEvents
==============
*/
void idSpawner::CallScriptEvents( const char* prefixKey, idEntity* parm )
{
	if( !prefixKey || !prefixKey[ 0 ] )
	{
		return;
	}

	/*	rvScriptFuncUtility func;
		for ( const idKeyValue* kv = spawnArgs.MatchPrefix(prefixKey); kv; kv = spawnArgs.MatchPrefix( prefixKey, kv ) ) {
			if( !kv->GetValue().Length() ) {
				continue;
			}

			if( func.Init( kv->GetValue()) <= SFU_ERROR ) {
				continue;
			}

			func.InsertEntity( parm, 0 );
			func.CallFunc( &spawnArgs );
		}*/
}

/*
==============
idSpawner::ActiveListChanged
==============
*/
bool idSpawner::ActiveListChanged()
{
	int previousCount = currentActive.Num();

// ---> sikk - RemoveNull() function
//	currentActive.RemoveNull();
	for( int i = currentActive.Num() - 1; i >= 0; --i )
	{
		if( !currentActive[ i ].GetEntity() )
		{
			currentActive.RemoveIndex( i );
		}
	}
// <--- sikk -

	return previousCount > currentActive.Num();
}

/*
==============
idSpawner::Attach

Attach the given AI to the spawner.  This will increase the active count of the spawner and
set the spawner pointer in the ai.
==============
*/
void idSpawner::Attach( idEntity* ent )
{
	currentActive.AddUnique( ent );
}

/*
==============
idSpawner::Detach

Detaches the given AI from the spawner which will free up an active spot for spawning.
Attach the given AI to the spawner.  This will increase the active count of the spawner and
set the spawner pointer in the ai.
==============
*/
void idSpawner::Detach( idEntity* ent )
{
	currentActive.Remove( ent );

	nextSpawnTime = gameLocal.time + spawnDelay;
}

/*
==============
idSpawner::Event_Activate
==============
*/
void idSpawner::Event_Activate( idEntity* activator )
{

	// "trigger_only" spawners will attempt to spawn when triggered
	if( spawnArgs.GetBool( "trigger_only" ) )
	{
		// Update next spawn time to follo CheckSpawn into thinking its time to spawn again
		nextSpawnTime = gameLocal.time;
		CheckSpawn();
		return;
	}

	// If nextSpawnTime is zero then the spawner is currently deactivated
	if( nextSpawnTime == 0 )
	{
		// Start thinking
		BecomeActive( TH_THINK );

		// Allow immediate spawn
		nextSpawnTime = gameLocal.time;

		// Spawn any ai targets and add them to the current count
		ActivateTargets( this );
	}
	else
	{
		nextSpawnTime = 0;
		BecomeInactive( TH_THINK );

		// Remove the spawner if need be
		if( spawnArgs.GetBool( "remove", "1" ) )
		{
			PostEventMS( &EV_Remove, 0 );
		}
	}
}

/*
==============
idSpawner::Event_RemoveNullActiveEntities
==============
*/
void idSpawner::Event_RemoveNullActiveEntities()
{
	for( int ix = currentActive.Num() - 1; ix >= 0; --ix )
	{
		if( !currentActive[ ix ].IsValid() )
		{
			currentActive.RemoveIndex( ix );
		}
	}
}

/*
==============
idSpawner::Event_NumActiveEntities
==============
*/
void idSpawner::Event_NumActiveEntities()
{
	idThread::ReturnInt( currentActive.Num() );
}

/*
==============
idSpawner::Event_GetActiveEntity
==============
*/
void idSpawner::Event_GetActiveEntity( int index )
{
	idThread::ReturnEntity( ( index < 0 || index >= currentActive.Num() ) ? NULL : currentActive[ index ].GetEntity() );
}
