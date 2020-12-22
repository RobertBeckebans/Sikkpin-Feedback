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

#ifndef __GAME_PLAYERVIEW_H__
#define __GAME_PLAYERVIEW_H__

/*
===============================================================================

  Player view.

===============================================================================
*/

// screenBlob_t are for the on-screen damage claw marks, etc
typedef struct screenBlob_s
{
	const idMaterial*	material;
	float				x, y, w, h;
	float				s1, t1, s2, t2;
	int					finishTime;
	int					startFadeTime;
	float				driftAmount;
} screenBlob_t;

#define	MAX_SCREEN_BLOBS	8

class idPlayerView
{
public:
	idPlayerView();

	void				Save( idSaveGame* savefile ) const;
	void				Restore( idRestoreGame* savefile );

	void				SetPlayerEntity( class idPlayer* playerEnt );

	void				ClearEffects( void );

	void				DamageImpulse( idVec3 localKickDir, const idDict* damageDef );

	void				WeaponFireFeedback( const idDict* weaponDef, int primary );	// sikk - Secondary Fire

	idAngles			AngleOffset( void ) const;			// returns the current kick angle

	idMat3				ShakeAxis( void ) const;			// returns the current shake angle

	void				CalculateShake( void );

	// this may involve rendering to a texture and displaying
	// that with a warp model or in double vision mode
	void				RenderPlayerView( idUserInterface* hud );

	void				Fade( idVec4 color, int time );
	void				Flash( idVec4 color, int time );
	void				AddBloodSpray( float duration );

	// temp for view testing
	void				EnableBFGVision( bool b )
	{
		bfgVision = b;
	};

private:
	void				SingleView( const renderView_t* view );
	void				ScreenFade( void );
	screenBlob_t* 		GetScreenBlob( void );

	const idMaterial* 	tunnelMaterial;			// health tunnel vision
	const idMaterial* 	bloodSprayMaterial;		// blood spray
	const idMaterial* 	bfgMaterial;			// when targeted with BFG
// sikk - removed multiplayer
//	const idMaterial *	lagoMaterial;			// lagometer drawing

	int					dvFinishTime;			// double vision will be stopped at this time
	int					kickFinishTime;			// view kick will be stopped at this time
	idAngles			kickAngles;
	bool				bfgVision;				//
	idVec4				fadeColor;				// fade color
	idVec4				fadeToColor;			// color to fade to
	idVec4				fadeFromColor;			// color to fade from
	float				fadeRate;				// fade rate
	int					fadeTime;				// fade time
	float				lastDamageTime;			// accentuate the tunnel effect for a while

	screenBlob_t		screenBlobs[MAX_SCREEN_BLOBS];

	idAngles			shakeAng;				// from the sound sources

	idPlayer* 			player;
	renderView_t		view;

// ---> sikk - PostProcess Effects
	void				DoPostFX( void );
	void				PostFX_EdgeAA( void );
	void				PostFX_HDR( void );
	void				PostFX_Bloom( void );
	void				PostFX_SSAO( void );
	void				PostFX_SunShafts( void );
	void				PostFX_DoF( void );
	void				PostFX_ColorGrading( void );
	void				PostFX_ExplosionFX( void );
	void				PostFX_Filmgrain( void );
	void				PostFX_Vignetting( void );
	void				PostFX_Underwater( void );
	void				PostFX_ScreenWipe( void );

	void				PostFX_DoubleVision( void );
	void				PostFX_MegashieldVision( void );
	void				PostFX_InfluenceVision( void );
	void				PostFX_BFGVision( void );
	void				PostFX_TunnelVision( void );
	void				PostFX_ScreenBlobs( void );

	void				RenderDepth( void );

	const idMaterial* 	blackMaterial;			// Black material (for general use)
	const idMaterial* 	whiteMaterial;			// White material (for general use)
	const idMaterial* 	currentRenderMaterial;	// Current Render material (for general use)
	const idMaterial* 	scratchMaterial;		// Scratch material (for general use)
	const idMaterial* 	depthMaterial;			// Depth material (for general use)
	const idMaterial* 	edgeAAMaterial;			// Edge AA material
	const idMaterial* 	hdrLumBaseMaterial;		// HDR Luminance Base material
	const idMaterial* 	hdrLumAverageMaterial;	// HDR Luminance Average material
	const idMaterial* 	hdrLumAdaptedMaterial;	// HDR Luminance Adapted material
	const idMaterial* 	hdrBrightPassMaterial;	// HDR Bright Pass Filter material
	const idMaterial* 	hdrGlareMaterial;		// HDR Bloom/Glare/Lens Flare material
	const idMaterial* 	hdrFinalMaterial;		// HDR Final Tone Mapping material
	const idMaterial* 	bloomMaterial;			// Bloom material
	const idMaterial* 	ssaoMaterial;			// SSAO material
	const idMaterial* 	sunShaftsMaterial;		// Sun Shafts material
	const idMaterial* 	dofMaterial;			// DoF material
	const idMaterial* 	colorGradingMaterial;	// Color Grading material
	const idMaterial* 	filmgrainMaterial;		// Filmgrain material
	const idMaterial* 	vignettingMaterial;		// Vignetting material
	const idMaterial* 	explosionFXMaterial;	// Explosion FX material
	const idMaterial* 	megashieldMaterial;		// Megashield material
	const idMaterial* 	underwaterMaterial;		// Underwater material
	const idMaterial* 	screenWipeMaterial;		// End Level Screen Wipe material

	renderView_t		hackedView;
	int					prevTime;				// Holds previous frame's time
	bool				bDepthRendered;			// Holds whether the depth map has been rendered for the current frame
// <--- sikk - PostProcess Effects

// ---> sikk - PostProccess Scaling Fix
	int					screenHeight;
	int					screenWidth;
	idVec2				shiftScale;
// <--- sikk - PostProccess Scaling Fix
};

#endif /* !__GAME_PLAYERVIEW_H__ */
