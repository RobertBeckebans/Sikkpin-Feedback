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

#include "tr_local.h"

// ---> sikk - Removed obsolete render paths
//#include "cg_explicit.h"
//
//CGcontext cg_context;
//
//static void cg_error_callback() {
//	CGerror i = cgGetError();
//	common->Printf( "Cg error (%d): %s\n", i, cgGetErrorString(i) );
//}
// <--- sikk - Removed obsolete render paths

/*
=========================================================================================

GENERAL INTERACTION RENDERING

=========================================================================================
*/

/*
====================
GL_SelectTextureNoClient
====================
*/
static void GL_SelectTextureNoClient( int unit )
{
	backEnd.glState.currenttmu = unit;
	qglActiveTextureARB( GL_TEXTURE0_ARB + unit );
	RB_LogComment( "glActiveTextureARB( %i )\n", unit );
}

/*
==================
RB_ARB2_DrawInteraction
==================
*/
void RB_ARB2_DrawInteraction( const drawInteraction_t* din )
{
	// load all the vertex program parameters

// ---> sikk - Included non-power-of-two/frag position conversion
	// screen power of two correction factor, assuming the copy to _currentRender
	// also copied an extra row and column for the bilerp
	float parm[ 4 ];
	int w = backEnd.viewDef->viewport.x2 - backEnd.viewDef->viewport.x1 + 1;
	int h = backEnd.viewDef->viewport.y2 - backEnd.viewDef->viewport.y1 + 1;
	parm[0] = ( float )w / globalImages->currentRenderImage->uploadWidth;
	parm[1] = ( float )h / globalImages->currentRenderImage->uploadHeight;
	parm[2] = parm[0] / w;	// sikk - added - one less fragment shader instruction
	parm[3] = parm[1] / h;	// sikk - added - one less fragment shader instruction
	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_NPOT_ADJUST, parm );

	// window coord to 0.0 to 1.0 conversion
	parm[0] = 1.0 / w;
	parm[1] = 1.0 / h;
	parm[2] = w;	// sikk - added - can be useful to have resolution size in shader
	parm[3] = h;	// sikk - added - can be useful to have resolution size in shader
	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_INVERSE_RES, parm );
// <--- sikk - Included non-power-of-two/frag position conversion

	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_ORIGIN, din->localLightOrigin.ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_VIEW_ORIGIN, din->localViewOrigin.ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_PROJECT_S, din->lightProjection[0].ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_PROJECT_T, din->lightProjection[1].ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_PROJECT_Q, din->lightProjection[2].ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_LIGHT_FALLOFF_S, din->lightProjection[3].ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_BUMP_MATRIX_S, din->bumpMatrix[0].ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_BUMP_MATRIX_T, din->bumpMatrix[1].ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_DIFFUSE_MATRIX_S, din->diffuseMatrix[0].ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_DIFFUSE_MATRIX_T, din->diffuseMatrix[1].ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_SPECULAR_MATRIX_S, din->specularMatrix[0].ToFloatPtr() );
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_SPECULAR_MATRIX_T, din->specularMatrix[1].ToFloatPtr() );

// ---> sikk - Include model matrix for to-world-space transformations
	const struct viewEntity_s* space = backEnd.currentSpace;
	parm[0] = space->modelMatrix[0];
	parm[1] = space->modelMatrix[4];
	parm[2] = space->modelMatrix[8];
	parm[3] = space->modelMatrix[12];
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_MODEL_MATRIX_X, parm );
	parm[0] = space->modelMatrix[1];
	parm[1] = space->modelMatrix[5];
	parm[2] = space->modelMatrix[9];
	parm[3] = space->modelMatrix[13];
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_MODEL_MATRIX_Y, parm );
	parm[0] = space->modelMatrix[2];
	parm[1] = space->modelMatrix[6];
	parm[2] = space->modelMatrix[10];
	parm[3] = space->modelMatrix[14];
	qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_MODEL_MATRIX_Z, parm );
// <--- sikk - Include model matrix for to-world-space transformations

// sikk - removed
	// testing fragment based normal mapping
	//if ( r_testARBProgram.GetBool() ) {
	//	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 2, din->localLightOrigin.ToFloatPtr() );
	//	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 3, din->localViewOrigin.ToFloatPtr() );
	//}

// ---> sikk - Condensed vertex color param
	static const float ignore[ 4 ]			= {  0.0, 1.0, 1.0, 1.0 };
	static const float modulate[ 4 ]		= {  1.0, 0.0, 1.0, 1.0 };
	static const float inv_modulate[ 4 ]	= { -1.0, 1.0, 1.0, 1.0 };

	switch( din->vertexColor )
	{
		case SVC_IGNORE:
			qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_COLOR_MODULATE_ADD, ignore );
			break;
		case SVC_MODULATE:
			qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_COLOR_MODULATE_ADD, modulate );
			break;
		case SVC_INVERSE_MODULATE:
			qglProgramEnvParameter4fvARB( GL_VERTEX_PROGRAM_ARB, PP_COLOR_MODULATE_ADD, inv_modulate );
			break;
	}
// <--- sikk - Condensed vertex color param

	// set the constant colors
// ---> sikk - Use single light color value for both diffuse & specular, also changed the position
	qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_LIGHT_COLOR, din->diffuseColor.ToFloatPtr() );
	//qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, 3, din->specularColor.ToFloatPtr() );
// <--- sikk - Use single light color value for both diffuse & specular

	// set the textures
// ---> sikk - changed texture order
	// texture 0 will be the light projection texture
	GL_SelectTextureNoClient( 0 );
	din->lightImage->Bind();

	// texture 1 will be the light falloff texture
	GL_SelectTextureNoClient( 1 );
	din->lightFalloffImage->Bind();

	// texture 1 will be the per-surface bump map
	GL_SelectTextureNoClient( 2 );
	din->bumpImage->Bind();

	// texture 2 is the per-surface diffuse map
	GL_SelectTextureNoClient( 3 );
	din->diffuseImage->Bind();

	// texture 3 is the per-surface specular map
	GL_SelectTextureNoClient( 4 );
	din->specularImage->Bind();

	// texture 4 is the ssao buffer
	GL_SelectTextureNoClient( 5 );
	globalImages->ssaoImage->Bind();

// ---> sikk - Auxilary textures for interaction shaders
	// per-surface auxilary texture 0 - 9
	for( int i = 0; i < din->surf->material->GetNumInteractionImages(); i++ )
	{
		GL_SelectTextureNoClient( i + 6 );
		din->surf->material->GetInteractionImage( i )->Bind();
	}
// <--- sikk - Auxilary textures for interaction shaders
// <--- sikk - changed texture order

	// draw it
	RB_DrawElementsWithCounters( din->surf->geo );
}


/*
=============
RB_ARB2_CreateDrawInteractions

=============
*/
void RB_ARB2_CreateDrawInteractions( const drawSurf_t* surf )
{
	if( !surf )
	{
		return;
	}

	// perform setup here that will be constant for all interactions
	GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHMASK | backEnd.depthFunc );

	// enable the vertex arrays
	qglEnableVertexAttribArrayARB( 8 );
	qglEnableVertexAttribArrayARB( 9 );
	qglEnableVertexAttribArrayARB( 10 );
	qglEnableVertexAttribArrayARB( 11 );
	qglEnableClientState( GL_COLOR_ARRAY );

// ---> sikk - Removed binding of normalization cubemap and specular lookup
	// texture 0 is the normalization cube map for the vector towards the light
	//GL_SelectTextureNoClient( 0 );
	//if ( backEnd.vLight->lightShader->IsAmbientLight() ) {
	//	globalImages->ambientNormalMap->Bind();
	//} else {
	//	globalImages->normalCubeMapImage->Bind();
	//}

	// texture 6 is the specular lookup table
	//GL_SelectTextureNoClient( 6 );
	//if ( r_testARBProgram.GetBool() ) {
	//	globalImages->specular2DTableImage->Bind();	// variable specularity in alpha channel
	//} else {
	//	globalImages->specularTableImage->Bind();
	//}
// <--- sikk - Removed binding of normalization cubemap and specular lookup

	for( ; surf ; surf = surf->nextOnLight )
	{
		// bind the vertex program
		if( r_testARBProgram.GetBool() )
		{
			qglBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_TEST );
			qglBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_TEST );
// ---> sikk - Custom Interaction Shaders
		}
		else if( backEnd.vLight->lightShader->IsAmbientLight() )  	// Ambient Light Shader
		{
			qglBindProgramARB( GL_VERTEX_PROGRAM_ARB, surf->material->GetInteractionVP_Amb() );
			qglBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, surf->material->GetInteractionFP_Amb() );
			//if (  r_useParallaxMapping.GetInteger() == 1 ) {
			//	qglBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_INTERACTIONAMBIENT_PM );
			//} else if ( ->GetInteractionType() == IT_PARALLAX && r_useParallaxMapping.GetInteger() == 2 ) {
			//	qglBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, FPROG_INTERACTIONAMBIENT_POM );
			//}
		}
		else if( !backEnd.vLight->lightDef->parms.pointLight )  	// 2D Light Shader (projected lights)
		{
			qglBindProgramARB( GL_VERTEX_PROGRAM_ARB, surf->material->GetInteractionVP_Dir() );
			qglBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, surf->material->GetInteractionFP_2D() );
		}
		else  	// 3D Light Shader (point lights)
		{
			if( backEnd.vLight->lightDef->parms.parallel )    // shader specific for sun light (no attenuation)
			{
				qglBindProgramARB( GL_VERTEX_PROGRAM_ARB, surf->material->GetInteractionVP_Dir() );
				qglBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, surf->material->GetInteractionFP_Sun() );
			}
			else
			{
				qglBindProgramARB( GL_VERTEX_PROGRAM_ARB, surf->material->GetInteractionVP_Dir() );
				qglBindProgramARB( GL_FRAGMENT_PROGRAM_ARB, surf->material->GetInteractionFP_3D() );
			}
		}
// <--- sikk - Custom Interaction Shaders

		qglEnable( GL_VERTEX_PROGRAM_ARB );
		qglEnable( GL_FRAGMENT_PROGRAM_ARB );

// ---> sikk - Custom Interaction Shaders: Local Parameters
		const float*	regs;
		regs = surf->shaderRegisters;
		for( int i = 0; i < surf->material->GetNumInteractionParms(); i++ )
		{
			float parm[ 4 ];
			parm[ 0 ] = regs[ surf->material->GetInteractionParm( i, 0 ) ];
			parm[ 1 ] = regs[ surf->material->GetInteractionParm( i, 1 ) ];
			parm[ 2 ] = regs[ surf->material->GetInteractionParm( i, 2 ) ];
			parm[ 3 ] = regs[ surf->material->GetInteractionParm( i, 3 ) ];
			qglProgramLocalParameter4fvARB( GL_VERTEX_PROGRAM_ARB, i, parm );
			qglProgramLocalParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, i, parm );
		}
// <--- sikk - Custom Interaction Shaders: Local Parameters

// ---> sikk - Specular Exponent Scale/Bias
		float parm[ 4 ];
		parm[ 0 ] = surf->material->GetSpecExp( 0 );
		parm[ 1 ] = surf->material->GetSpecExp( 1 );
		parm[ 2 ] = 0.0f;
		parm[ 3 ] = 0.0f;
		qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_SPECULAR_EXP, parm );
// <--- sikk - Custom Interaction Shaders: Local Parameters

		// perform setup here that will not change over multiple interaction passes

		// set the vertex pointers
		idDrawVert* ac = ( idDrawVert* )vertexCache.Position( surf->geo->ambientCache );
		qglColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), ac->color );
		qglVertexAttribPointerARB( 8, 2, GL_FLOAT, false, sizeof( idDrawVert ), ac->st.ToFloatPtr() );
		qglVertexAttribPointerARB( 9, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );
		qglVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
		qglVertexAttribPointerARB( 11, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
		qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );

		// this may cause RB_ARB2_DrawInteraction to be executed multiple
		// times with different colors and images if the surface or light have multiple layers
		RB_CreateSingleDrawInteractions( surf, RB_ARB2_DrawInteraction );

		qglDisable( GL_VERTEX_PROGRAM_ARB );
		qglDisable( GL_FRAGMENT_PROGRAM_ARB );
	}

	qglDisableVertexAttribArrayARB( 8 );
	qglDisableVertexAttribArrayARB( 9 );
	qglDisableVertexAttribArrayARB( 10 );
	qglDisableVertexAttribArrayARB( 11 );
	qglDisableClientState( GL_COLOR_ARRAY );

	// disable features
// ---> sikk - Auxilary textures for interaction shaders
	// per-surface auxilary texture 0 - 9
	for( int i = 15; i > 0; i-- )
	{
		GL_SelectTextureNoClient( i );
		globalImages->BindNull();
	}
// <--- sikk - Auxilary textures for interaction shaders

	//GL_SelectTextureNoClient( 5 );
	//globalImages->BindNull();

	//GL_SelectTextureNoClient( 4 );
	//globalImages->BindNull();

	//GL_SelectTextureNoClient( 3 );
	//globalImages->BindNull();

	//GL_SelectTextureNoClient( 2 );
	//globalImages->BindNull();

	//GL_SelectTextureNoClient( 1 );
	//globalImages->BindNull();

// ---> sikk - Added
	//GL_SelectTextureNoClient( 0 );
	//globalImages->BindNull();
// <--- sikk - Added

	backEnd.glState.currenttmu = -1;
	GL_SelectTexture( 0 );

	//qglDisable( GL_VERTEX_PROGRAM_ARB );
	//qglDisable( GL_FRAGMENT_PROGRAM_ARB );
}


/*
==================
RB_ARB2_DrawInteractions
==================
*/
void RB_ARB2_DrawInteractions()
{
	viewLight_t* vLight;
	const idMaterial* lightShader;

	GL_SelectTexture( 0 );
	qglDisableClientState( GL_TEXTURE_COORD_ARRAY );

	// for each light, perform adding and shadowing
	for( vLight = backEnd.viewDef->viewLights ; vLight ; vLight = vLight->next )
	{
		backEnd.vLight = vLight;

		// do fogging later
		if( vLight->lightShader->IsFogLight() )
		{
			continue;
		}
		if( vLight->lightShader->IsBlendLight() )
		{
			continue;
		}

		if( !vLight->localInteractions && !vLight->globalInteractions
				&& !vLight->translucentInteractions )
		{
			continue;
		}

		lightShader = vLight->lightShader;

// ---> sikk - Ambient Light Color
		float parm[ 3 ];
		parm[ 0 ] = vLight->shaderRegisters[ EXP_REG_PARM10 ];
		parm[ 1 ] = vLight->shaderRegisters[ EXP_REG_PARM11 ];
		parm[ 2 ] = vLight->shaderRegisters[ EXP_REG_PARM12 ];
		for( int i = 0; i < 3; i++ )
		{
			if( parm[ i ] <= 0.0f )
			{
				parm[ i ] = 0;
			}
			else if( parm[ i ] <= 0.04045f )
			{
				parm[ i ] /= 12.92f;
			}
			else
			{
				parm[ i ] = pow( ( parm[ i ] + 0.055f ) / 1.055f, 2.4f );
			}
		}
		qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_AMBIENT_COLOR_X_NEG, parm );
		parm[ 0 ] = vLight->shaderRegisters[ EXP_REG_PARM13 ];
		parm[ 1 ] = vLight->shaderRegisters[ EXP_REG_PARM14 ];
		parm[ 2 ] = vLight->shaderRegisters[ EXP_REG_PARM15 ];
		for( int i = 0; i < 3; i++ )
		{
			if( parm[ i ] <= 0.0f )
			{
				parm[ i ] = 0;
			}
			else if( parm[ i ] <= 0.04045f )
			{
				parm[ i ] /= 12.92f;
			}
			else
			{
				parm[ i ] = pow( ( parm[ i ] + 0.055f ) / 1.055f, 2.4f );
			}
		}
		qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_AMBIENT_COLOR_X_POS, parm );
		parm[ 0 ] = vLight->shaderRegisters[ EXP_REG_PARM16 ];
		parm[ 1 ] = vLight->shaderRegisters[ EXP_REG_PARM17 ];
		parm[ 2 ] = vLight->shaderRegisters[ EXP_REG_PARM18 ];
		for( int i = 0; i < 3; i++ )
		{
			if( parm[ i ] <= 0.0f )
			{
				parm[ i ] = 0;
			}
			else if( parm[ i ] <= 0.04045f )
			{
				parm[ i ] /= 12.92f;
			}
			else
			{
				parm[ i ] = pow( ( parm[ i ] + 0.055f ) / 1.055f, 2.4f );
			}
		}
		qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_AMBIENT_COLOR_Y_NEG, parm );
		parm[ 0 ] = vLight->shaderRegisters[ EXP_REG_PARM19 ];
		parm[ 1 ] = vLight->shaderRegisters[ EXP_REG_PARM20 ];
		parm[ 2 ] = vLight->shaderRegisters[ EXP_REG_PARM21 ];
		for( int i = 0; i < 3; i++ )
		{
			if( parm[ i ] <= 0.0f )
			{
				parm[ i ] = 0;
			}
			else if( parm[ i ] <= 0.04045f )
			{
				parm[ i ] /= 12.92f;
			}
			else
			{
				parm[ i ] = pow( ( parm[ i ] + 0.055f ) / 1.055f, 2.4f );
			}
		}
		qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_AMBIENT_COLOR_Y_POS, parm );
		parm[ 0 ] = vLight->shaderRegisters[ EXP_REG_PARM22 ];
		parm[ 1 ] = vLight->shaderRegisters[ EXP_REG_PARM23 ];
		parm[ 2 ] = vLight->shaderRegisters[ EXP_REG_PARM24 ];
		for( int i = 0; i < 3; i++ )
		{
			if( parm[ i ] <= 0.0f )
			{
				parm[ i ] = 0;
			}
			else if( parm[ i ] <= 0.04045f )
			{
				parm[ i ] /= 12.92f;
			}
			else
			{
				parm[ i ] = pow( ( parm[ i ] + 0.055f ) / 1.055f, 2.4f );
			}
		}
		qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_AMBIENT_COLOR_Z_NEG, parm );
		parm[ 0 ] = vLight->shaderRegisters[ EXP_REG_PARM25 ];
		parm[ 1 ] = vLight->shaderRegisters[ EXP_REG_PARM26 ];
		parm[ 2 ] = vLight->shaderRegisters[ EXP_REG_PARM27 ];
		for( int i = 0; i < 3; i++ )
		{
			if( parm[ i ] <= 0.0f )
			{
				parm[ i ] = 0;
			}
			else if( parm[ i ] <= 0.04045f )
			{
				parm[ i ] /= 12.92f;
			}
			else
			{
				parm[ i ] = pow( ( parm[ i ] + 0.055f ) / 1.055f, 2.4f );
			}
		}
		qglProgramEnvParameter4fvARB( GL_FRAGMENT_PROGRAM_ARB, PP_AMBIENT_COLOR_Z_POS, parm );
// <--- sikk - Ambient Light Color

		// clear the stencil buffer if needed
		if( vLight->globalShadows || vLight->localShadows )
		{
			backEnd.currentScissor = vLight->scissorRect;
			if( r_useScissor.GetBool() )
			{
				qglScissor( backEnd.viewDef->viewport.x1 + backEnd.currentScissor.x1,
							backEnd.viewDef->viewport.y1 + backEnd.currentScissor.y1,
							backEnd.currentScissor.x2 + 1 - backEnd.currentScissor.x1,
							backEnd.currentScissor.y2 + 1 - backEnd.currentScissor.y1 );
			}
			qglClear( GL_STENCIL_BUFFER_BIT );
		}
		else
		{
			// no shadows, so no need to read or write the stencil buffer
			// we might in theory want to use GL_ALWAYS instead of disabling
			// completely, to satisfy the invarience rules
			qglStencilFunc( GL_ALWAYS, 128, 255 );
		}

		if( r_useShadowVertexProgram.GetBool() )
		{
			qglEnable( GL_VERTEX_PROGRAM_ARB );
			qglBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_STENCIL_SHADOW );
			RB_StencilShadowPass( vLight->globalShadows );
			RB_ARB2_CreateDrawInteractions( vLight->localInteractions );
			qglEnable( GL_VERTEX_PROGRAM_ARB );
			qglBindProgramARB( GL_VERTEX_PROGRAM_ARB, VPROG_STENCIL_SHADOW );
			RB_StencilShadowPass( vLight->localShadows );
			RB_ARB2_CreateDrawInteractions( vLight->globalInteractions );
			qglDisable( GL_VERTEX_PROGRAM_ARB );	// if there weren't any globalInteractions, it would have stayed on
		}
		else
		{
			RB_StencilShadowPass( vLight->globalShadows );
			RB_ARB2_CreateDrawInteractions( vLight->localInteractions );
			RB_StencilShadowPass( vLight->localShadows );
			RB_ARB2_CreateDrawInteractions( vLight->globalInteractions );
		}

		// translucent surfaces never get stencil shadowed
		if( r_skipTranslucent.GetBool() )
		{
			continue;
		}


		qglStencilFunc( GL_ALWAYS, 128, 255 );

		backEnd.depthFunc = GLS_DEPTHFUNC_LESS;
		RB_ARB2_CreateDrawInteractions( vLight->translucentInteractions );

		backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;
	}

	// disable stencil shadow test
	qglStencilFunc( GL_ALWAYS, 128, 255 );

	GL_SelectTexture( 0 );
	qglEnableClientState( GL_TEXTURE_COORD_ARRAY );
}

//===================================================================================

typedef struct
{
	GLenum	target;
	GLuint	ident;
	char	name[ 64 ];
} progDef_t;

static const int MAX_GLPROGS = 256;	// sikk - Increased from 200 to 256, a nice pot number

// a single file can have both a vertex program and a fragment program
static progDef_t progs[ MAX_GLPROGS ] =
{
// ---> sikk - Removed unecessary shaders/Added custom shaders
	{ GL_VERTEX_PROGRAM_ARB,	VPROG_STENCIL_SHADOW,	"shadow.vp" },

	// default interaction vertex/fragment programs
	{ GL_VERTEX_PROGRAM_ARB,	VPROG_INTERACTIONDIR,	"interaction_Dir.vp" },
	{ GL_FRAGMENT_PROGRAM_ARB,	FPROG_INTERACTION2D,	"interaction_2D.fp" },
	{ GL_FRAGMENT_PROGRAM_ARB,	FPROG_INTERACTION3D,	"interaction_3D.fp" },
	{ GL_FRAGMENT_PROGRAM_ARB,	FPROG_INTERACTIONSUN,	"interaction_Sun.fp" },
	{ GL_VERTEX_PROGRAM_ARB,	VPROG_INTERACTIONAMB,	"interaction_Amb.vp" },
	{ GL_FRAGMENT_PROGRAM_ARB,	FPROG_INTERACTIONAMB,	"interaction_Amb.fp" },

	{ GL_VERTEX_PROGRAM_ARB,	VPROG_ENVIRONMENT,		"environment.vfp" },
	{ GL_FRAGMENT_PROGRAM_ARB,	FPROG_ENVIRONMENT,		"environment.vfp" },
	{ GL_VERTEX_PROGRAM_ARB,	VPROG_ENVIRONMENTBUMP,	"environmentBumpmap.vfp" },
	{ GL_FRAGMENT_PROGRAM_ARB,	FPROG_ENVIRONMENTBUMP,	"environmentBumpmap.vfp" },

	{ GL_VERTEX_PROGRAM_ARB,	VPROG_TEST,				"test.vfp" },
	{ GL_FRAGMENT_PROGRAM_ARB,	FPROG_TEST,				"test.vfp" }
// <--- sikk - Removed unecessary shaders/Added custom shaders

	// additional programs can be dynamically specified in materials
};

/*
=================
R_LoadARBProgram
=================
*/
void R_LoadARBProgram( int progIndex )
{
	int		ofs;
	int		err;
	char*	fileBuffer;
	char*	buffer;
	char*	start, *end;
	idStr	fullPath = "glprogs/";
	fullPath += progs[ progIndex ].name;

	common->Printf( "%s", fullPath.c_str() );

	// load the program even if we don't support it, so
	// fs_copyfiles can generate cross-platform data dumps
	fileSystem->ReadFile( fullPath.c_str(), ( void** )&fileBuffer, NULL );
	if( !fileBuffer )
	{
		common->Printf( ": File not found\n" );
		return;
	}

	// copy to stack memory and free
	buffer = ( char* )_alloca( strlen( fileBuffer ) + 1 );
	strcpy( buffer, fileBuffer );
	fileSystem->FreeFile( fileBuffer );

	if( !glConfig.isInitialized )
	{
		return;
	}

	// submit the program string at start to GL
	if( progs[ progIndex ].ident == 0 )
	{
		// allocate a new identifier for this program
		progs[ progIndex ].ident = PROG_USER + progIndex;
	}

	// vertex and fragment programs can both be present in a single file, so
	// scan for the proper header to be the start point, and stamp a 0 in after the end

	if( progs[ progIndex ].target == GL_VERTEX_PROGRAM_ARB )
	{
		if( !glConfig.ARBVertexProgramAvailable )
		{
			common->Printf( ": GL_VERTEX_PROGRAM_ARB not available\n" );
			return;
		}
		start = strstr( ( char* )buffer, "!!ARBvp" );
	}
	if( progs[ progIndex ].target == GL_FRAGMENT_PROGRAM_ARB )
	{
		if( !glConfig.ARBFragmentProgramAvailable )
		{
			common->Printf( ": GL_FRAGMENT_PROGRAM_ARB not available\n" );
			return;
		}
		start = strstr( ( char* )buffer, "!!ARBfp" );
	}
	if( !start )
	{
		common->Printf( ": !!ARB not found\n" );
		return;
	}
	end = strstr( start, "END" );

	if( !end )
	{
		common->Printf( ": END not found\n" );
		return;
	}
	end[ 3 ] = 0;

	qglBindProgramARB( progs[ progIndex ].target, progs[ progIndex ].ident );
	qglGetError();

	qglProgramStringARB( progs[ progIndex ].target, GL_PROGRAM_FORMAT_ASCII_ARB, strlen( start ), ( unsigned char* )start );

	err = qglGetError();
	qglGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, ( GLint* )&ofs );
	if( err == GL_INVALID_OPERATION )
	{
		const GLubyte* str = qglGetString( GL_PROGRAM_ERROR_STRING_ARB );
		common->Printf( "\nGL_PROGRAM_ERROR_STRING_ARB: %s\n", str );
		if( ofs < 0 )
		{
			common->Printf( "GL_PROGRAM_ERROR_POSITION_ARB < 0 with error\n" );
		}
		else if( ofs >= ( int )strlen( ( char* )start ) )
		{
			common->Printf( "error at end of program\n" );
		}
		else
		{
			common->Printf( "error at %i:\n%s", ofs, start + ofs );
		}
		return;
	}
	if( ofs != -1 )
	{
		common->Printf( "\nGL_PROGRAM_ERROR_POSITION_ARB != -1 without error\n" );
		return;
	}

	common->Printf( "\n" );
}

/*
==================
R_FindARBProgram

Returns a GL identifier that can be bound to the given target, parsing
a text file if it hasn't already been loaded.
==================
*/
int R_FindARBProgram( GLenum target, const char* program )
{
	int		i;
	idStr	stripped = program;

	stripped.StripFileExtension();

	// see if it is already loaded
	for( i = 0; progs[ i ].name[ 0 ]; i++ )
	{
		if( progs[ i ].target != target )
		{
			continue;
		}

		idStr compare = progs[ i ].name;
		compare.StripFileExtension();

		if( !idStr::Icmp( stripped.c_str(), compare.c_str() ) )
		{
			return progs[ i ].ident;
		}
	}

	if( i == MAX_GLPROGS )
	{
		common->Error( "R_FindARBProgram: MAX_GLPROGS" );
	}

	// add it to the list and load it
	progs[ i ].ident = ( program_t )0;	// will be gen'd by R_LoadARBProgram
	progs[ i ].target = target;
	strncpy( progs[ i ].name, program, sizeof( progs[ i ].name ) - 1 );

	R_LoadARBProgram( i );

	return progs[ i ].ident;
}

/*
==================
R_ReloadARBPrograms_f
==================
*/
void R_ReloadARBPrograms_f( const idCmdArgs& args )
{
	common->Printf( "----- R_ReloadARBPrograms -----\n" );
	for( int i = 0; progs[ i ].name[ 0 ]; i++ )
	{
		R_LoadARBProgram( i );
	}
	common->Printf( "-------------------------------\n" );
}

/*
==================
R_ARB2_Init
==================
*/
void R_ARB2_Init()
{
	glConfig.allowARB2Path = false;
	common->Printf( "---------- R_ARB2_Init ----------\n" );
	if( !glConfig.ARBVertexProgramAvailable || !glConfig.ARBFragmentProgramAvailable )
	{
		common->Printf( "Not available.\n" );
		return;
	}
	common->Printf( "Available.\n" );
	common->Printf( "---------------------------------\n" );
	glConfig.allowARB2Path = true;
}

