/*
================

Spawner.h

================
*/

#ifndef __GAME_SPAWNER_H__
#define __GAME_SPAWNER_H__

const int MAX_SPAWN_TYPES = 32;

class idSpawner;

typedef void ( *spawnerCallbackProc_t )( idSpawner* spawner, idEntity* spawned, int userdata );

typedef struct spawnerCallback_s
{
	idEntityPtr<idEntity>	ent;
	idStr					event;
} spawnerCallback_t;

/*
===============================================================================

  idSpawner

===============================================================================
*/
class idSpawner : public idEntity
{
public:
	CLASS_PROTOTYPE( idSpawner );

	void				Spawn();
	void				Think();

	void				Attach( idEntity* ent );
	void				Detach( idEntity* ent );

	void				Save( idSaveGame* savefile ) const;
	void				Restore( idRestoreGame* savefile );

	void				AddSpawnPoint( idEntity* point );
	void				RemoveSpawnPoint( idEntity* point );

	int					GetNumSpawnPoints() const;
	int					GetNumActive() const;
	int					GetMaxActive() const;
	idEntity*			GetSpawnPoint( int index );

	virtual void		FindTargets();
	bool				ActiveListChanged();

	void				CallScriptEvents( const char* prefixKey, idEntity* parm );

	void				AddCallback( idEntity* owner, const idEventDef* ev );

protected:
	int								numSpawned;
	int								maxToSpawn;
	float							nextSpawnTime;
	int								maxActive;
	idList< idEntityPtr<idEntity> >	currentActive;
	int								spawnWaves;
	int								spawnDelay;
	bool							skipVisible;
	idStrList						spawnTypes;

	idList< idEntityPtr<idEntity> >	spawnPoints;

	idList< spawnerCallback_t >		callbacks;

	// Check to see if its time to spawn
	void				CheckSpawn();

	// Spawn a new entity
	bool				SpawnEnt();

	// Populate the spawnType list with the available spawn types
	void				FindSpawnTypes();

	// Get a random spawnpoint to spawn at
	idEntity*			GetSpawnPoint();

	// Get a random spawn type
	const char*			GetSpawnType( idEntity* spawnPoint );

	// Validate the given spawn point for spawning
	bool				ValidateSpawnPoint( const idVec3 origin, const idBounds& bounds );

	// Copy key/values from the given entity to the given dictionary using the specified prefix
	void				CopyPrefixedSpawnArgs( idEntity* src, const char* prefix, idDict& args );

private:
	void				Event_Activate( idEntity* activator );
	void				Event_RemoveNullActiveEntities();
	void				Event_NumActiveEntities();
	void				Event_GetActiveEntity( int index );
};


ID_INLINE int idSpawner::GetNumSpawnPoints() const
{
	return spawnPoints.Num();
}

ID_INLINE idEntity* idSpawner::GetSpawnPoint( int index )
{
	return spawnPoints[ index ].GetEntity();
}

ID_INLINE int idSpawner::GetNumActive() const
{
	return currentActive.Num();
}

ID_INLINE int idSpawner::GetMaxActive() const
{
	return maxActive;
}

#endif // __GAME_SPAWNER_H__
