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

#ifndef __GAME_PLAYER_H__
#define __GAME_PLAYER_H__

/*
===============================================================================

	Player entity.
	
===============================================================================
*/

extern const idEventDef EV_Player_GetButtons;
extern const idEventDef EV_Player_GetMove;
extern const idEventDef EV_Player_GetViewAngles;
extern const idEventDef EV_Player_EnableWeapon;
extern const idEventDef EV_Player_DisableWeapon;
extern const idEventDef EV_Player_ExitTeleporter;
extern const idEventDef EV_Player_SelectWeapon;
extern const idEventDef EV_SpectatorTouch;

const float THIRD_PERSON_FOCUS_DISTANCE	= 512.0f;
const int	LAND_DEFLECT_TIME = 150;
const int	LAND_RETURN_TIME = 300;
const int	FOCUS_TIME = 300;
const int	FOCUS_GUI_TIME = 500;

const int MAX_WEAPONS = 16;

const int SAVING_THROW_TIME = 5000;		// maximum one "saving throw" every five seconds

const int ASYNC_PLAYER_INV_AMMO_BITS = idMath::BitsForInteger( 999 );	// 9 bits to cover the range [0, 999]
const int ASYNC_PLAYER_INV_CLIP_BITS = -7;								// -7 bits to cover the range [-1, 60]

struct idItemInfo {
	idStr name;
	idStr icon;
};

struct idObjectiveInfo {
	idStr title;
	idStr text;
	idStr screenshot;
};

struct idLevelTriggerInfo {
	idStr levelName;
	idStr triggerName;
};

// powerups - the "type" in item .def must match
enum powerups_e {
	MEGADAMAGE = 0, 
	MEGASHIELD,
	MEGAENERGY,
	MEGACHARGE,
	MAX_POWERUPS
};

// ---> sikk - Modifier System
// modifiers - the "type" in item .def must match
enum modifiers_e {
	NONE = 0,

	MOD_PLAYER_SPEED, 
	MOD_PLAYER_DOUBLEJUMP,
	MOD_PLAYER_AIRCONTROL,

	MOD_WEAPON_PROJECTILESPEED,
	MOD_WEAPON_PROJECTILEDOUBLER,
	MOD_WEAPON_INCENDIARY,

	MOD_ENERGY_RECHARGERATE,
	MOD_ENERGY_CONSERVATION,
	MOD_ENERGY_PICKUPDOUBLER,

	MOD_MISC_SCOREDOUBLER,
	MOD_MISC_POWERUPTIMEDOUBLER,
	MOD_MISC_COMBOTIMEDOUBLER,

	MAX_MODIFIERS
};

typedef struct modifiers_s {
	float	player_speed;
	bool	player_doublejump;
	float	player_aircontrol;
	int		weapon_projectilespeed;
	int		weapon_projectiledoubler;
	bool	weapon_incendiary;
	float	energy_rechargerate;
	float	energy_conservation;
	int		energy_pickupdoubler;
	int		misc_scoredoubler;
	int		misc_poweruptimedoubler;
	int		misc_combotimedoubler;
} modifiers_t;
// <--- sikk - Modifier System

// influence levels
enum influenceLevels_e {
	INFLUENCE_NONE = 0,			// none
	INFLUENCE_LEVEL1,			// no gun or hud
	INFLUENCE_LEVEL2,			// no gun, hud, movement
	INFLUENCE_LEVEL3,			// slow player movement
};

class idInventory {
public:
	int						maxHealth;
	int						maxEnergy;
	int						weapons;
	int						powerups;
	int						powerupEndTime[ MAX_POWERUPS ];
	bool					powerupEndSoundPlayed[ MAX_POWERUPS ];	// sikk - Powerup End Sounds

	//int						statsScoreTotal;	// sikk - Level Stats System: Total score
	//int						rechargerAmount;	// sikk - Recharger Pack System

	bool					turkeyScore;
	idList<idDict *>		items;

	int						lastGiveTime;

	idList<idLevelTriggerInfo> levelTriggers;

							idInventory() { Clear(); }
							~idInventory() { Clear(); }

	// save games
	void					Save( idSaveGame *savefile ) const;					// archives object for save game file
	void					Restore( idRestoreGame *savefile );					// unarchives object from save game file

	void					Clear( void );
	void					GivePowerUp( idPlayer *player, int powerup, int msec );
	void					ClearPowerUps( void );
	void					GetPersistantData( idDict &dict );
	void					RestoreInventory( idPlayer *owner, const idDict &dict );
	bool					Give( idPlayer *owner, const idDict &spawnArgs, const char *statname, const char *value, int *idealWeapon, bool updateHud );
	void					Drop( const idDict &spawnArgs, const char *weapon_classname, int weapon_index );
	void					AddPickupName( const char *name, const char *icon );

	int						nextItemPickup;
	int						nextItemNum;
	int						onePickupTime;
	idList<idItemInfo>		pickupItemNames;
};

typedef struct loggedAccel_s {
	int		time;
	idVec3	dir;		// scaled larger for running
} loggedAccel_t;

typedef struct aasLocation_s {
	int		areaNum;
	idVec3	pos;
} aasLocation_t;

// ---> sikk - Level Stats System
enum levelStats_e {
	TIME = 0, 
	SCORE,
	SCORETOTAL,
	LIVES,
	KILLS,
	ENEMYCOUNT,
	SECRETSFOUND,
	SECRETSTOTAL,
	ENERGY,
	COMBOS,
	SHOTSFIRED,
	SHOTSHIT,
	DAMAGEDEALT,
	DAMAGETAKEN,
	NUM_STATS
};

typedef struct levelStats_s {
	int	time;
	int	score;	// this is per level score, total score is kept in idInventory so it maintains across levels
	int	scoreTotal;
	int	lives;
	int	kills;
	int	enemyCount;
	int	secretsFound;
	int	secretsTotal;
	int	energy;
	int	combos;
	int	shotsFired;
	int	shotsHit;
	int	damageDealt;
	int	damageTaken;
} levelStats_t;
// <--- sikk - Level Stats System

class idPlayer : public idActor {
public:
	enum {
		EVENT_IMPULSE = idEntity::EVENT_MAXEVENTS,
		EVENT_EXIT_TELEPORTER,
		EVENT_ABORT_TELEPORTER,
		EVENT_POWERUP,
		EVENT_SPECTATE,
		EVENT_MAXEVENTS
	};

	usercmd_t				usercmd;

	class idPlayerView		playerView;			// handles damage kicks and effects

	bool					noclip;
	bool					godmode;

	bool					spawnAnglesSet;		// on first usercmd, we must set deltaAngles
	idAngles				spawnAngles;
	idAngles				viewAngles;			// player view angles
	idAngles				cmdAngles;			// player cmd angles

	int						buttonMask;
	int						oldButtons;
	int						oldFlags;

	int						lastHitTime;			// last time projectile fired by player hit target
	int						lastSndHitTime;			// MP hit sound - != lastHitTime because we throttle
	int						lastSavingThrowTime;	// for the "free miss" effect

	idScriptBool			AI_FORWARD;
	idScriptBool			AI_BACKWARD;
	idScriptBool			AI_STRAFE_LEFT;
	idScriptBool			AI_STRAFE_RIGHT;
	idScriptBool			AI_ATTACK_HELD;
	idScriptBool			AI_WEAPON_FIRED;
	idScriptBool			AI_JUMP;
	idScriptBool			AI_CROUCH;
	idScriptBool			AI_ONGROUND;
	idScriptBool			AI_ONLADDER;
	idScriptBool			AI_DEAD;
	idScriptBool			AI_RUN;
	idScriptBool			AI_PAIN;
	idScriptBool			AI_HARDLANDING;
	idScriptBool			AI_SOFTLANDING;
	idScriptBool			AI_RELOAD;
	idScriptBool			AI_TELEPORT;
	idScriptBool			AI_TURN_LEFT;
	idScriptBool			AI_TURN_RIGHT;

	// inventory
	idInventory				inventory;

	idEntityPtr<idWeapon>	weapon;
	idUserInterface *		hud;				// MP: is NULL if not local player
	idUserInterface *		stats;				// sikk - Level Stats System: GUI

	int						lastDmgTime;
	int						lastFireTime;		// sikk - time when player last fired weapon
	int						deathClearContentsTime;
	bool					doingDeathSkin;
	int						healthPool;			// amount of health to give over time	// sikk - Changed type to int
	int						energyPool;			// amount of energy to give over time	// sikk - Changed type to int
	int						nextHealthPulse;
	bool					healthPulse;
	int						nextHealthTake;
	bool					healthTake;
	int						lastEnergyTallyTime;	// sikk - 

	bool					hiddenWeapon;		// if the weapon is hidden ( in noWeapons maps )
//	idEntityPtr<idProjectile> soulCubeProjectile;

// sikk - removed multiplayer
	// mp stuff
	//static idVec3			colorBarTable[ 5 ];
	//int						spectator;
	//idVec3					colorBar;			// used for scoreboard and hud display
	//int						colorBarIndex;
	bool					scoreBoardOpen;	// sikk - we're still using this for the level stats gui
	//bool					forceScoreBoard;
	//bool					forceRespawn;
	//bool					spectating;
	//int						lastSpectateTeleport;
	bool					lastHitToggle;
	//bool					forcedReady;
	//bool					wantSpectate;		// from userInfo
	//bool					weaponGone;			// force stop firing
	//bool					useInitialSpawns;	// toggled by a map restart to be active for the first game spawn
	//int						latchedTeam;		// need to track when team gets changed
	//int						tourneyRank;		// for tourney cycling - the higher, the more likely to play next - server
	//int						tourneyLine;		// client side - our spot in the wait line. 0 means no info.
	//int						spawnedTime;		// when client first enters the game

	idEntityPtr<idEntity>	teleportEntity;		// while being teleported, this is set to the entity we'll use for exit
	int						teleportKiller;		// entity number of an entity killing us at teleporter exit
	//bool					lastManOver;		// can't respawn in last man anymore (srv only)
	//bool					lastManPlayAgain;	// play again when end game delay is cancelled out before expiring (srv only)
	//bool					lastManPresent;		// true when player was in when game started (spectators can't join a running LMS)
	//bool					isLagged;			// replicated from server, true if packets haven't been received from client.
	//bool					isChatting;			// replicated from server, true if the player is chatting.

	// timers
	int						minRespawnTime;		// can respawn when time > this, force after g_forcerespawn
	int						maxRespawnTime;		// force respawn after this time

	// the first person view values are always calculated, even
	// if a third person view is used
	idVec3					firstPersonViewOrigin;
	idMat3					firstPersonViewAxis;

	idDragEntity			dragEntity;

public:
	CLASS_PROTOTYPE( idPlayer );

							idPlayer();
	virtual					~idPlayer();

	void					Spawn( void );
	void					Think( void );

	// save games
	void					Save( idSaveGame *savefile ) const;					// archives object for save game file
	void					Restore( idRestoreGame *savefile );					// unarchives object from save game file

	virtual void			Hide( void );
	virtual void			Show( void );

	void					Init( void );
	void					PrepareForRestart( void );
	virtual void			Restart( void );
	void					LinkScriptVariables( void );
	void					SetupWeaponEntity( void );
	void					SelectInitialSpawnPoint( idVec3 &origin, idAngles &angles );
	void					SpawnFromSpawnSpot( void );
	void					SpawnToPoint( const idVec3	&spawn_origin, const idAngles &spawn_angles );
	void					SetClipModel( void );	// spectator mode uses a different bbox size

	void					SavePersistantInfo( void );
	void					RestorePersistantInfo( void );
	void					SetLevelTrigger( const char *levelName, const char *triggerName );

	bool					UserInfoChanged( bool canModify );
	idDict *				GetUserInfo( void );
// sikk - removed multiplayer
	//bool					BalanceTDM( void );

	void					CacheWeapons( void );

	void					EnterCinematic( void );
	void					ExitCinematic( void );
	bool					HandleESC( void );
	bool					SkipCinematic( void );

	void					UpdateConditions( void );
	void					SetViewAngles( const idAngles &angles );

							// delta view angles to allow movers to rotate the view of the player
	void					UpdateDeltaViewAngles( const idAngles &angles );

	virtual bool			Collide( const trace_t &collision, const idVec3 &velocity );

	virtual void			GetAASLocation( idAAS *aas, idVec3 &pos, int &areaNum ) const;
	virtual void			GetAIAimTargets( const idVec3 &lastSightPos, idVec3 &headPos, idVec3 &chestPos );
	virtual void			DamageFeedback( idEntity *victim, idEntity *inflictor, int &damage );
	void					CalcDamagePoints(  idEntity *inflictor, idEntity *attacker, const idDict *damageDef,
							   const float damageScale, const int location, int *health, int *armor );
	virtual	void			Damage( idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location );

							// use exitEntityNum to specify a teleport with private camera view and delayed exit
	virtual void			Teleport( const idVec3 &origin, const idAngles &angles, idEntity *destination );

	void					Kill( bool delayRespawn, bool nodamage );
	virtual void			Killed( idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location );
	void					StartFxOnBone(const char *fx, const char *bone);

	renderView_t *			GetRenderView( void );
	void					CalculateRenderView( void );	// called every tic by player code
	void					CalculateFirstPersonView( void );

	void					DrawHUD( idUserInterface *hud );

	void					WeaponFireFeedback( const idDict *weaponDef, int primary );	// sikk - Secondary Fire

	float					DefaultFov( void ) const;
	float					CalcFov( bool honorZoom );
	void					CalculateViewWeaponPos( idVec3 &origin, idMat3 &axis );
	idVec3					GetEyePosition( void ) const;
	void					GetViewPos( idVec3 &origin, idMat3 &axis ) const;
	void					OffsetThirdPersonView( float angle, float range, float height, bool clip );

	bool					Give( const char *statname, const char *value );
	bool					GiveItem( idItem *item );
	void					GiveItem( const char *name );
	void					GiveHealthPool( int amt );	// sikk - Changed type to int
	
	bool					GiveInventoryItem( idDict *item );
	void					RemoveInventoryItem( idDict *item );
	bool					GiveInventoryItem( const char *name );
	void					RemoveInventoryItem( const char *name );
	idDict *				FindInventoryItem( const char *name );

	bool					GivePowerUp( int powerup, int time, int score = 0 );	// sikk - Level Stats System: Added score
	void					ClearPowerUps( void );
	bool					PowerUpActive( int powerup ) const;

	int						SlotForWeapon( const char *weaponName );
	void					NextWeapon( void );
	void					NextBestWeapon( void );
	void					PrevWeapon( void );
	void					SelectWeapon( int num, bool force );
// sikk - removed multiplayer
	//void					DropWeapon( bool died ) ;
	//void					StealWeapon( idPlayer *player );
	void					AddProjectilesFired( int count );
	void					AddProjectileHits( int count );
	void					SetLastHitTime( int time );
	void					LowerWeapon( void );
	void					RaiseWeapon( void );
	void					WeaponLoweringCallback( void );
	void					WeaponRisingCallback( void );
	void					RemoveWeapon( const char *weap );
	bool					CanShowWeaponViewmodel( void ) const;

	void					AddAIKill( int _energy );	// sikk - handles energy and score accumilation
	void					SetSoulCubeProjectile( idProjectile *projectile );

	void					UpdateAir( void );

	virtual bool			HandleSingleGuiCommand( idEntity *entityGui, idLexer *src );
	bool					GuiActive( void ) { return focusGUIent != NULL; }

	void					PerformImpulse( int impulse );
	void					Spectate( bool spectate );
	void					ToggleScoreboard( void );
	void					RouteGuiMouse( idUserInterface *gui );
	void					UpdateHud( void );
	void					UpdateHudWeapon( bool flashWeapon = true );
	void					UpdateHudStats( idUserInterface *hud );
	void					SetInfluenceFov( float fov );
	void					SetInfluenceView( const char *mtr, const char *skinname, float radius, idEntity *ent );
	void					SetInfluenceLevel( int level );
	int						GetInfluenceLevel( void ) { return influenceActive; };
	void					SetPrivateCameraView( idCamera *camView );
	idCamera *				GetPrivateCameraView( void ) const { return privateCameraView; }
	void					StartFxFov( float duration  );

// sikk - removed multiplayer
	//virtual void			ClientPredictionThink( void );
	//virtual void			WriteToSnapshot( idBitMsgDelta &msg ) const;
	//virtual void			ReadFromSnapshot( const idBitMsgDelta &msg );
	//void					WritePlayerStateToSnapshot( idBitMsgDelta &msg ) const;
	//void					ReadPlayerStateFromSnapshot( const idBitMsgDelta &msg );

	//virtual bool			ServerReceiveEvent( int event, int time, const idBitMsg &msg );
	//virtual bool			ClientReceiveEvent( int event, int time, const idBitMsg &msg );

	virtual bool			GetPhysicsToVisualTransform( idVec3 &origin, idMat3 &axis );
	virtual bool			GetPhysicsToSoundTransform( idVec3 &origin, idMat3 &axis );

	bool					IsReady( void );
	bool					IsRespawning( void );
	bool					IsInTeleport( void );

	idEntity*				GetInfluenceEntity( void ) { return influenceEntity; };
	const idMaterial*		GetInfluenceMaterial( void ) { return influenceMaterial; };
	float					GetInfluenceRadius( void ) { return influenceRadius; };

	// server side work for in/out of spectate. takes care of spawning it into the world as well
// sikk - removed multiplayer
	//void					ServerSpectate( bool spectate );
	// for very specific usage. != GetPhysics()
	//idPhysics*				GetPlayerPhysics( void );
	void					TeleportDeath( int killer );
	//void					SetLeader( bool lead );
	//bool					IsLeader( void );

	//void					UpdateSkinSetup( bool restart );

	bool					OnLadder( void ) const;

// sikk - removed multiplayer
	//virtual	void			UpdatePlayerIcons( void );
	//virtual	void			DrawPlayerIcons( void );
	//virtual	void			HidePlayerIcons( void );
	//bool					NeedsIcon( void );

	//bool					SelfSmooth( void );
	//void					SetSelfSmooth( bool b );

	bool					underwater;	// sikk - Liquid Physics

	float					sticky;	// sikk - Sticky movement modifier

	int						endLevelTime;	// sikk - End Level Time
	bool					doEndLevel;		// sikk - End Level Flag


// ---> sikk - Modifier System
	modifiers_t				modifiers;
	int						activeModifier;
	void					GiveModifier( int modifier ) { activeModifier = modifier; };
	int						GetActiveModifier( void ) { return activeModifier; };
	void					ClearModifier( void ) { activeModifier = 0; };
	void					ResetModifiers( void );
	void					UpdateModifiers( void );
// <--- sikk - Modifier System

// ---> sikk - Lives System
	int						nextScoreThreshold;
	int						respawnerAmount;
	idVec3					respawnPos;
	idAngles				respawnAng;
	void					Respawn( void );
	void					UpdateRespawnPos( void );
// <--- sikk - Lives System

// ---> sikk - Energy System
	int						nextHealthGive;
	int						energy;
	int						energyTally;
	void					GiveEnergy( int amt );
	int						GetEnergy( void );
	void					GiveEnergyTally( int amt );
	int						GetEnergyTally( void ) { return energyTally; };
// <--- sikk - Energy System

// ---> sikk - Forward Velocity Damage Multiplier
	float					velocityDamageScale;
	void					UpdateForwardVelocity( void );
// <--- sikk - Forward Velocity Damage Multiplier

// ---> sikk - Level Stats System
	levelStats_t			levelStats;
	int						GetLevelStat( int stat );
	void					SetLevelStat( int stat, int amount );
	void					AddToLevelStat( int stat, int amount );
	void					ClearLevelStats( void );
	void					UpdateLevelStats( void );
	int						GetTotalScore( void );
	void					SaveLevelStats( idSaveGame *savefile ) const;
// <--- sikk - Level Stats System

// ---> sikk - Level Music
	int						musicTime;
	int						musicFlag;
	void					SpawnMusic( void );
	void					UpdateMusic( void );
// <--- sikk - Level Music

// ---> sikk - Object Manipulation
	idGrabEntity			grabEntity;
	idEntity*				focusMoveable;
	int						focusMoveableId;
	int						focusMoveableTimer;
// <--- sikk - Object Manipulation

// ---> sikk - Crosshair
	idVec4					crosshairColor;
// <--- sikk - Crosshair

	float					focusDistance;	// sikk - Depth of Field PostProcess
// ---> sikk - Combo Multiplier
	bool					comboFull;
	int						comboTime;
	float					comboMultiplier;
// <--- sikk - Combo Multiplier

private:
	jointHandle_t			hipJoint;
	jointHandle_t			chestJoint;
	jointHandle_t			headJoint;

	idPhysics_Player		physicsObj;			// player physics

	idList<aasLocation_t>	aasLocation;		// for AI tracking the player

	int						bobFoot;
	float					bobFrac;
	float					bobfracsin;
	int						bobCycle;			// for view bobbing and footstep generation
	float					xyspeed;
	int						stepUpTime;
	float					stepUpDelta;
	float					idealLegsYaw;
	float					legsYaw;
	bool					legsForward;
	float					oldViewYaw;
	idAngles				viewBobAngles;
	idVec3					viewBob;
	int						landChange;
	int						landTime;

	int						currentWeapon;
	int						idealWeapon;
	int						previousWeapon;
	int						weaponSwitchTime;
	bool					weaponEnabled;
	bool					showWeaponViewModel;

	const idDeclSkin *		skin;
	const idDeclSkin *		powerUpSkin;
	idStr					baseSkinName;

	bool					airless;
	int						airTics;				// set to pm_airTics at start, drops in vacuum
	int						lastAirDamage;

	bool					gibDeath;
	bool					gibsLaunched;
	idVec3					gibsDir;

	idInterpolate<float>	zoomFov;
	idInterpolate<float>	centerView;
	bool					fxFov;

	float					influenceFov;
	int						influenceActive;		// level of influence.. 1 == no gun or hud .. 2 == 1 + no movement
	idEntity *				influenceEntity;
	const idMaterial *		influenceMaterial;
	float					influenceRadius;
	const idDeclSkin *		influenceSkin;

	idCamera *				privateCameraView;

	static const int		NUM_LOGGED_VIEW_ANGLES = 64;		// for weapon turning angle offsets
	idAngles				loggedViewAngles[NUM_LOGGED_VIEW_ANGLES];	// [gameLocal.framenum&(LOGGED_VIEW_ANGLES-1)]
	static const int		NUM_LOGGED_ACCELS = 16;			// for weapon turning angle offsets
	loggedAccel_t			loggedAccel[NUM_LOGGED_ACCELS];	// [currentLoggedAccel & (NUM_LOGGED_ACCELS-1)]
	int						currentLoggedAccel;

	// if there is a focusGUIent, the attack button will be changed into mouse clicks
	idEntity *				focusGUIent;
	idUserInterface *		focusUI;				// focusGUIent->renderEntity.gui, gui2, or gui3
	idAI *					focusCharacter;
	int						talkCursor;				// show the state of the focusCharacter (0 == can't talk/dead, 1 == ready to talk, 2 == busy talking)
	int						focusTime;
	idAFEntity_Vehicle *	focusVehicle;
	idUserInterface *		cursor;
	
	// full screen guis track mouse movements directly
	int						oldMouseX;
	int						oldMouseY;

	int						lastDamageDef;
	idVec3					lastDamageDir;
	int						lastDamageLocation;
	int						smoothedFrame;
	bool					smoothedOriginUpdated;
	idVec3					smoothedOrigin;
	idAngles				smoothedAngles;

// sikk - removed multiplayer
	// mp
	//bool					ready;					// from userInfo
	//bool					respawning;				// set to true while in SpawnToPoint for telefrag checks
	//bool					leader;					// for sudden death situations
	//int						lastSpectateChange;
	//int						lastTeleFX;
	//unsigned int			lastSnapshotSequence;	// track state hitches on clients
	//bool					weaponCatchup;			// raise up the weapon silently ( state catchups )
	//int						MPAim;					// player num in aim
	//int						lastMPAim;
	//int						lastMPAimTime;			// last time the aim changed
	//int						MPAimFadeTime;			// for GUI fade
	//bool					MPAimHighlight;
	//bool					isTelefragged;			// proper obituaries

	//idPlayerIcon			playerIcon;

	//bool					selfSmooth;

	void					LookAtKiller( idEntity *inflictor, idEntity *attacker );

	void					StopFiring( void );
	void					FireWeapon( bool primary );	// sikk - Secondary Fire
	void					Weapon_Combat( void );
	void					Weapon_NPC( void );
	void					Weapon_GUI( void );
	void					UpdateWeapon( void );
// sikk - removed multiplayer
	//void					UpdateSpectating( void );
	//void					SpectateFreeFly( bool force );	// ignore the timeout to force when followed spec is no longer valid
	//void					SpectateCycle( void );
	idAngles				GunTurningOffset( void );
	idVec3					GunAcceleratingOffset( void );

	void					UseObjects( void );
	void					CrashLand( const idVec3 &oldOrigin, const idVec3 &oldVelocity );
	void					BobCycle( const idVec3 &pushVelocity );
	void					UpdateViewAngles( void );
	void					EvaluateControls( void );
	void					AdjustSpeed( void );
	void					AdjustBodyAngles( void );
	void					InitAASLocation( void );
	void					SetAASLocation( void );
	void					Move( void );
	void					UpdatePowerUps( void );
	void					UpdateDeathSkin( bool state_hitch );
	void					ClearPowerup( int i );
// sikk - removed multiplayer
	//void					SetSpectateOrigin( void );

	void					ClearFocus( void );
	void					UpdateFocus( void );
	void					UpdateLocation( void );
	idUserInterface *		ActiveGui( void );

	void					UseVehicle( bool drive );	// sikk - function modified to support use function

	void					Event_GetButtons( void );
	void					Event_GetMove( void );
	void					Event_GetViewAngles( void );
	void					Event_StopFxFov( void );
	void					Event_EnableWeapon( void );
	void					Event_DisableWeapon( void );
	void					Event_GetCurrentWeapon( void );
	void					Event_GetPreviousWeapon( void );
	void					Event_SelectWeapon( const char *weaponName );
	void					Event_GetWeaponEntity( void );
	void					Event_ExitTeleporter( void );
	void					Event_LevelTrigger( void );
	void					Event_Gibbed( void );
	void					Event_GetIdealWeapon( void );
};

// sikk - removed multiplayer
//ID_INLINE bool idPlayer::IsReady( void ) {
//	return ready || forcedReady;
//}
//
//ID_INLINE bool idPlayer::IsRespawning( void ) {
//	return respawning;
//}
//
//ID_INLINE idPhysics* idPlayer::GetPlayerPhysics( void ) {
//	return &physicsObj;
//}
//
ID_INLINE bool idPlayer::IsInTeleport( void ) {
	return ( teleportEntity.GetEntity() != NULL );
}
//
//ID_INLINE void idPlayer::SetLeader( bool lead ) {
//	leader = lead;
//}
//
//ID_INLINE bool idPlayer::IsLeader( void ) {
//	return leader;
//}
//
//ID_INLINE bool idPlayer::SelfSmooth( void ) {
//	return selfSmooth;
//}
//
//ID_INLINE void idPlayer::SetSelfSmooth( bool b ) {
//	selfSmooth = b;
//}

#endif /* !__GAME_PLAYER_H__ */

