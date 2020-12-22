/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

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

shaderProgram_t		interactionDirShader;
shaderProgram_t		interactionAmbShader;
shaderProgram_t		stencilShadowShader;

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
RB_GLSL_DrawInteraction
==================
*/
static void RB_GLSL_DrawInteraction( const drawInteraction_t* din )
{
	// load all the shader parameters
	if( din->ambientLight )
	{
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
		qglUniform4fvARB( interactionAmbShader.nonPoT, 1, parm );

		// window coord to 0.0 to 1.0 conversion
		parm[0] = 1.0 / w;
		parm[1] = 1.0 / h;
		parm[2] = w;	// sikk - added - can be useful to have resolution size in shader
		parm[3] = h;	// sikk - added - can be useful to have resolution size in shader
		qglUniform4fvARB( interactionAmbShader.invRes, 1, parm );
// <--- sikk - Included non-power-of-two/frag position conversion

		qglUniform4fvARB( interactionAmbShader.localLightOrigin, 1, din->localLightOrigin.ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.localViewOrigin, 1, din->localViewOrigin.ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.lightProjectionS, 1, din->lightProjection[0].ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.lightProjectionT, 1, din->lightProjection[1].ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.lightProjectionQ, 1, din->lightProjection[2].ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.lightFalloff, 1, din->lightProjection[3].ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.bumpMatrixS, 1, din->bumpMatrix[0].ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.bumpMatrixT, 1, din->bumpMatrix[1].ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.diffuseMatrixS, 1, din->diffuseMatrix[0].ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.diffuseMatrixT, 1, din->diffuseMatrix[1].ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.specularMatrixS, 1, din->specularMatrix[0].ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.specularMatrixT, 1, din->specularMatrix[1].ToFloatPtr() );

// ---> sikk - Include model matrix for to-world-space transformations
		const struct viewEntity_s* space = backEnd.currentSpace;
		qglUniformMatrix4fvARB( interactionAmbShader.modelMatrix, 1, 0, space->modelMatrix );
// <--- sikk - Include model matrix for to-world-space transformations

		static const float ignore[ 4 ]			= {  0.0, 1.0, 1.0, 1.0 };
		static const float modulate[ 4 ]		= {  1.0, 0.0, 1.0, 1.0 };
		static const float inv_modulate[ 4 ]	= { -1.0, 1.0, 1.0, 1.0 };

		switch( din->vertexColor )
		{
			case SVC_IGNORE:
				qglUniform4fvARB( interactionAmbShader.colorMAD, 1, ignore );
				break;
			case SVC_MODULATE:
				qglUniform4fvARB( interactionAmbShader.colorMAD, 1, modulate );
				break;
			case SVC_INVERSE_MODULATE:
				qglUniform4fvARB( interactionAmbShader.colorMAD, 1, inv_modulate );
				break;
		}

		// set the constant color
		qglUniform4fvARB( interactionAmbShader.diffuseColor, 1, din->diffuseColor.ToFloatPtr() );
		qglUniform4fvARB( interactionAmbShader.specularColor, 1, din->specularColor.ToFloatPtr() );
	}
	else
	{
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
		qglUniform4fvARB( interactionDirShader.nonPoT, 1, parm );

		// window coord to 0.0 to 1.0 conversion
		parm[0] = 1.0 / w;
		parm[1] = 1.0 / h;
		parm[2] = w;	// sikk - added - can be useful to have resolution size in shader
		parm[3] = h;	// sikk - added - can be useful to have resolution size in shader
		qglUniform4fvARB( interactionDirShader.invRes, 1, parm );
// <--- sikk - Included non-power-of-two/frag position conversion

		qglUniform4fvARB( interactionDirShader.localLightOrigin, 1, din->localLightOrigin.ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.localViewOrigin, 1, din->localViewOrigin.ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.lightProjectionS, 1, din->lightProjection[0].ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.lightProjectionT, 1, din->lightProjection[1].ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.lightProjectionQ, 1, din->lightProjection[2].ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.lightFalloff, 1, din->lightProjection[3].ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.bumpMatrixS, 1, din->bumpMatrix[0].ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.bumpMatrixT, 1, din->bumpMatrix[1].ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.diffuseMatrixS, 1, din->diffuseMatrix[0].ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.diffuseMatrixT, 1, din->diffuseMatrix[1].ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.specularMatrixS, 1, din->specularMatrix[0].ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.specularMatrixT, 1, din->specularMatrix[1].ToFloatPtr() );

// ---> sikk - Include model matrix for to-world-space transformations
		const struct viewEntity_s* space = backEnd.currentSpace;
		qglUniformMatrix4fvARB( interactionDirShader.modelMatrix, 1, 0, space->modelMatrix );
// <--- sikk - Include model matrix for to-world-space transformations

		if( !backEnd.vLight->lightDef->parms.pointLight )  	// 2D Light Shader (projected lights)
		{
			qglUniform1iARB( interactionDirShader.falloffType, 2 );
			//qglUseProgramObjectARB( interactionDirShader.program );
			//qglUniform1iARB( interactionDirShader.u_lightProjectionTexture, 0 );
			//qglUseProgramObjectARB( 0 );
		}
		else  	// 3D Light Shader (point lights)
		{
			if( backEnd.vLight->lightDef->parms.parallel )  	// shader specific for sun light (no attenuation)
			{
				qglUniform1iARB( interactionDirShader.falloffType, 1 );
			}
			else  											// default quadratic attenuation
			{
				qglUniform1iARB( interactionDirShader.falloffType, 0 );
			}
		}

		static const float ignore[ 4 ]			= {  0.0, 1.0, 1.0, 1.0 };
		static const float modulate[ 4 ]		= {  1.0, 0.0, 1.0, 1.0 };
		static const float inv_modulate[ 4 ]	= { -1.0, 1.0, 1.0, 1.0 };

		switch( din->vertexColor )
		{
			case SVC_IGNORE:
				qglUniform4fvARB( interactionDirShader.colorMAD, 1, ignore );
				break;
			case SVC_MODULATE:
				qglUniform4fvARB( interactionDirShader.colorMAD, 1, modulate );
				break;
			case SVC_INVERSE_MODULATE:
				qglUniform4fvARB( interactionDirShader.colorMAD, 1, inv_modulate );
				break;
		}

		// set the constant colors
		qglUniform4fvARB( interactionDirShader.diffuseColor, 1, din->diffuseColor.ToFloatPtr() );
		qglUniform4fvARB( interactionDirShader.specularColor, 1, din->specularColor.ToFloatPtr() );
	}

	// set the textures
	// texture 0 will be the light projection texture
	GL_SelectTextureNoClient( 0 );
	din->lightImage->Bind();

	if( !din->ambientLight )
	{
		GL_SelectTextureNoClient( 16 );
		din->lightImage->Bind();
	}

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

	// draw it
	RB_DrawElementsWithCounters( din->surf->geo );
}


/*
=============
RB_GLSL_CreateDrawInteractions
=============
*/
static void RB_GLSL_CreateDrawInteractions( const drawSurf_t* surf )
{
	if( !surf )
	{
		return;
	}

	// perform setup here that will be constant for all interactions
	GL_State( GLS_SRCBLEND_ONE | GLS_DSTBLEND_ONE | GLS_DEPTHMASK | backEnd.depthFunc );

	// bind the vertex and fragment program
	if( backEnd.vLight->lightShader->IsAmbientLight() )
	{
		qglUseProgramObjectARB( interactionAmbShader.program );
	}
	else
	{
		qglUseProgramObjectARB( interactionDirShader.program );
	}

	// enable the vertex arrays
	qglEnableVertexAttribArrayARB( 8 );
	qglEnableVertexAttribArrayARB( 9 );
	qglEnableVertexAttribArrayARB( 10 );
	qglEnableVertexAttribArrayARB( 11 );
	qglEnableClientState( GL_COLOR_ARRAY );

	for( ; surf; surf = surf->nextOnLight )
	{
		// perform setup here that will not change over multiple interaction passes

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
			if( backEnd.vLight->lightShader->IsAmbientLight() )
			{
				qglUniform4fvARB( interactionAmbShader.localParms[ i ], 1, parm );
			}
			else
			{
				qglUniform4fvARB( interactionDirShader.localParms[ i ], 1, parm );
			}
		}
// <--- sikk - Custom Interaction Shaders: Local Parameters

// ---> sikk - Specular Exponent Scale/Bias
		float parm[ 4 ];
		parm[ 0 ] = surf->material->GetSpecExp( 0 );
		parm[ 1 ] = surf->material->GetSpecExp( 1 );
		parm[ 2 ] = 0.0f;
		parm[ 3 ] = 0.0f;
		if( backEnd.vLight->lightShader->IsAmbientLight() )
		{
			qglUniform4fvARB( interactionAmbShader.specExp, 1, parm );
		}
		else
		{
			qglUniform4fvARB( interactionDirShader.specExp, 1, parm );
		}
// <--- sikk - Custom Interaction Shaders: Local Parameters

		// set the vertex pointers
		idDrawVert*	ac = ( idDrawVert* )vertexCache.Position( surf->geo->ambientCache );
		qglColorPointer( 4, GL_UNSIGNED_BYTE, sizeof( idDrawVert ), ac->color );
		qglVertexAttribPointerARB( 11, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->normal.ToFloatPtr() );
		qglVertexAttribPointerARB( 10, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[1].ToFloatPtr() );
		qglVertexAttribPointerARB( 9, 3, GL_FLOAT, false, sizeof( idDrawVert ), ac->tangents[0].ToFloatPtr() );
		qglVertexAttribPointerARB( 8, 2, GL_FLOAT, false, sizeof( idDrawVert ), ac->st.ToFloatPtr() );
		qglVertexPointer( 3, GL_FLOAT, sizeof( idDrawVert ), ac->xyz.ToFloatPtr() );

		// set model matrix
		//if ( backEnd.vLight->lightShader->IsAmbientLight() ) {
		//	qglUniformMatrix4fvARB( interactionAmbShader.modelMatrix, 1, false, surf->space->modelMatrix );
		//} else {
		//	qglUniformMatrix4fvARB( interactionDirShader.modelMatrix, 1, false, surf->space->modelMatrix );
		//}

		// this may cause RB_GLSL_DrawInteraction to be executed multiple
		// times with different colors and images if the surface or light have multiple layers
		RB_CreateSingleDrawInteractions( surf, RB_GLSL_DrawInteraction );
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

	backEnd.glState.currenttmu = -1;
	GL_SelectTexture( 0 );

	qglUseProgramObjectARB( 0 );
}


/*
==================
RB_GLSL_DrawInteractions
==================
*/
void RB_GLSL_DrawInteractions( void )
{
	viewLight_t* vLight;

	GL_SelectTexture( 0 );
	qglDisableClientState( GL_TEXTURE_COORD_ARRAY );

	//
	// for each light, perform adding and shadowing
	//
	for( vLight = backEnd.viewDef->viewLights; vLight; vLight = vLight->next )
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

		// if there are no interactions, get out!
		if( !vLight->localInteractions && !vLight->globalInteractions && !vLight->translucentInteractions )
		{
			continue;
		}

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
			qglUseProgramObjectARB( stencilShadowShader.program );
			RB_StencilShadowPass( vLight->globalShadows );
			RB_GLSL_CreateDrawInteractions( vLight->localInteractions );
			qglUseProgramObjectARB( stencilShadowShader.program );
			RB_StencilShadowPass( vLight->localShadows );
			RB_GLSL_CreateDrawInteractions( vLight->globalInteractions );
			qglUseProgramObjectARB( 0 );	// if there weren't any globalInteractions, it would have stayed on
		}
		else
		{
			RB_StencilShadowPass( vLight->globalShadows );
			RB_GLSL_CreateDrawInteractions( vLight->localInteractions );
			RB_StencilShadowPass( vLight->localShadows );
			RB_GLSL_CreateDrawInteractions( vLight->globalInteractions );
		}

		// translucent surfaces never get stencil shadowed
		if( r_skipTranslucent.GetBool() )
		{
			continue;
		}

		qglStencilFunc( GL_ALWAYS, 128, 255 );

		backEnd.depthFunc = GLS_DEPTHFUNC_LESS;
		RB_GLSL_CreateDrawInteractions( vLight->translucentInteractions );
		backEnd.depthFunc = GLS_DEPTHFUNC_EQUAL;
	}

	// disable stencil shadow test
	qglStencilFunc( GL_ALWAYS, 128, 255 );

	GL_SelectTexture( 0 );
	qglEnableClientState( GL_TEXTURE_COORD_ARRAY );
}

//===================================================================================

/*
=================
R_LoadGLSLShader

loads GLSL vertex or fragment shaders
=================
*/
void R_LoadGLSLShader( const char* name, shaderProgram_t* shaderProgram, GLenum type )
{
	char*	fileBuffer;
	char*	buffer;
	idStr	fullPath = "gl2progs/";
	fullPath += name;

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

	switch( type )
	{
		case GL_VERTEX_SHADER_ARB:
			// create vertex shader
			shaderProgram->vertexShader = qglCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
			qglShaderSourceARB( shaderProgram->vertexShader, 1, ( const GLcharARB** )&buffer, 0 );
			qglCompileShaderARB( shaderProgram->vertexShader );
			break;
		case GL_FRAGMENT_SHADER_ARB:
			// create fragment shader
			shaderProgram->fragmentShader = qglCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
			qglShaderSourceARB( shaderProgram->fragmentShader, 1, ( const GLcharARB** )&buffer, 0 );
			qglCompileShaderARB( shaderProgram->fragmentShader );
			break;
		default:
			common->Printf( "R_LoadGLSLShader: no type\n" );
			return;
	}

	common->Printf( "\n" );
}

/*
=================
R_LinkGLSLShader

links the GLSL vertex and fragment shaders together to form a GLSL program
=================
*/
bool R_LinkGLSLShader( shaderProgram_t* shaderProgram, bool needsAttributes )
{
	GLint linked;

	shaderProgram->program = qglCreateProgramObjectARB( );

	qglAttachObjectARB( shaderProgram->program, shaderProgram->vertexShader );
	qglAttachObjectARB( shaderProgram->program, shaderProgram->fragmentShader );

	if( needsAttributes )
	{
		qglBindAttribLocationARB( shaderProgram->program, 8, "attr_TexCoord" );
		qglBindAttribLocationARB( shaderProgram->program, 9, "attr_Tangent" );
		qglBindAttribLocationARB( shaderProgram->program, 10, "attr_Bitangent" );
		qglBindAttribLocationARB( shaderProgram->program, 11, "attr_Normal" );
	}

	qglLinkProgramARB( shaderProgram->program );

	qglGetObjectParameterivARB( shaderProgram->program, GL_OBJECT_LINK_STATUS_ARB, &linked );
	if( !linked )
	{
		common->Printf( "R_LinkGLSLShader: program failed to link\n" );
		return false;
	}

	return true;
}

/*
=================
R_ValidateGLSLProgram

makes sure GLSL program is valid
=================
*/
bool R_ValidateGLSLProgram( shaderProgram_t* shaderProgram )
{
	GLint validProgram;

	qglValidateProgramARB( shaderProgram->program );

	qglGetObjectParameterivARB( shaderProgram->program, GL_OBJECT_VALIDATE_STATUS_ARB, &validProgram );
	if( !validProgram )
	{
		common->Printf( "R_ValidateGLSLProgram: program invalid\n" );
		return false;
	}

	return true;
}

/*
==================
RB_GLSL_InitShaders
==================
*/
static bool RB_GLSL_InitShaders( )
{
	// load interation shaders
	R_LoadGLSLShader( "interaction_Dir.vs", &interactionDirShader, GL_VERTEX_SHADER_ARB );
	R_LoadGLSLShader( "interaction_Dir.fs", &interactionDirShader, GL_FRAGMENT_SHADER_ARB );
	if( !R_LinkGLSLShader( &interactionDirShader, true ) && !R_ValidateGLSLProgram( &interactionDirShader ) )
	{
		return false;
	}
	else
	{
		// set uniform locations
		interactionDirShader.u_lightCubeTexture = qglGetUniformLocationARB( interactionDirShader.program, "u_lightCubeTexture" );
		interactionDirShader.u_lightProjectionTexture = qglGetUniformLocationARB( interactionDirShader.program, "u_lightProjectionTexture" );
		interactionDirShader.u_lightFalloffTexture = qglGetUniformLocationARB( interactionDirShader.program, "u_lightFalloffTexture" );
		interactionDirShader.u_normalTexture = qglGetUniformLocationARB( interactionDirShader.program, "u_normalTexture" );
		interactionDirShader.u_diffuseTexture = qglGetUniformLocationARB( interactionDirShader.program, "u_diffuseTexture" );
		interactionDirShader.u_specularTexture = qglGetUniformLocationARB( interactionDirShader.program, "u_specularTexture" );
		interactionDirShader.u_ssaoTexture = qglGetUniformLocationARB( interactionDirShader.program, "u_ssaoTexture" );
		interactionDirShader.u_aux0Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux0Texture" );
		interactionDirShader.u_aux1Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux1Texture" );
		interactionDirShader.u_aux2Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux2Texture" );
		interactionDirShader.u_aux3Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux3Texture" );
		interactionDirShader.u_aux4Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux4Texture" );
		interactionDirShader.u_aux5Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux5Texture" );
		interactionDirShader.u_aux6Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux6Texture" );
		interactionDirShader.u_aux7Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux7Texture" );
		interactionDirShader.u_aux8Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux8Texture" );
		interactionDirShader.u_aux9Texture = qglGetUniformLocationARB( interactionDirShader.program, "u_aux9Texture" );

		interactionDirShader.modelMatrix = qglGetUniformLocationARB( interactionDirShader.program, "u_modelMatrix" );

		interactionDirShader.localLightOrigin = qglGetUniformLocationARB( interactionDirShader.program, "u_lightOrigin" );
		interactionDirShader.localViewOrigin = qglGetUniformLocationARB( interactionDirShader.program, "u_viewOrigin" );
		interactionDirShader.lightProjectionS = qglGetUniformLocationARB( interactionDirShader.program, "u_lightProjectionS" );
		interactionDirShader.lightProjectionT = qglGetUniformLocationARB( interactionDirShader.program, "u_lightProjectionT" );
		interactionDirShader.lightProjectionQ = qglGetUniformLocationARB( interactionDirShader.program, "u_lightProjectionQ" );
		interactionDirShader.lightFalloff = qglGetUniformLocationARB( interactionDirShader.program, "u_lightFalloff" );

		interactionDirShader.bumpMatrixS = qglGetUniformLocationARB( interactionDirShader.program, "u_bumpMatrixS" );
		interactionDirShader.bumpMatrixT = qglGetUniformLocationARB( interactionDirShader.program, "u_bumpMatrixT" );
		interactionDirShader.diffuseMatrixS = qglGetUniformLocationARB( interactionDirShader.program, "u_diffuseMatrixS" );
		interactionDirShader.diffuseMatrixT = qglGetUniformLocationARB( interactionDirShader.program, "u_diffuseMatrixT" );
		interactionDirShader.specularMatrixS = qglGetUniformLocationARB( interactionDirShader.program, "u_specularMatrixS" );
		interactionDirShader.specularMatrixT = qglGetUniformLocationARB( interactionDirShader.program, "u_specularMatrixT" );
		interactionDirShader.specularMatrixT = qglGetUniformLocationARB( interactionDirShader.program, "u_specularMatrixT" );

		interactionDirShader.colorMAD = qglGetUniformLocationARB( interactionDirShader.program, "u_colorMAD" );

		interactionDirShader.diffuseColor = qglGetUniformLocationARB( interactionDirShader.program, "u_diffuseColor" );
		interactionDirShader.specularColor = qglGetUniformLocationARB( interactionDirShader.program, "u_specularColor" );

		interactionDirShader.falloffType = qglGetUniformLocationARB( interactionDirShader.program, "u_falloffType" );

		interactionDirShader.specExp = qglGetUniformLocationARB( interactionDirShader.program, "u_specExp" );

		interactionDirShader.localParms[0] = qglGetUniformLocationARB( interactionDirShader.program, "u_localParm0" );
		interactionDirShader.localParms[1] = qglGetUniformLocationARB( interactionDirShader.program, "u_localParm1" );
		interactionDirShader.localParms[2] = qglGetUniformLocationARB( interactionDirShader.program, "u_localParm2" );
		interactionDirShader.localParms[3] = qglGetUniformLocationARB( interactionDirShader.program, "u_localParm3" );
		interactionDirShader.localParms[4] = qglGetUniformLocationARB( interactionDirShader.program, "u_localParm4" );
		interactionDirShader.localParms[5] = qglGetUniformLocationARB( interactionDirShader.program, "u_localParm5" );
		interactionDirShader.localParms[6] = qglGetUniformLocationARB( interactionDirShader.program, "u_localParm6" );
		interactionDirShader.localParms[7] = qglGetUniformLocationARB( interactionDirShader.program, "u_localParm7" );

		// set texture locations
		qglUseProgramObjectARB( interactionDirShader.program );
		qglUniform1iARB( interactionDirShader.u_lightCubeTexture, 0 );
		qglUniform1iARB( interactionDirShader.u_lightProjectionTexture, 16 );
		qglUniform1iARB( interactionDirShader.u_lightFalloffTexture, 1 );
		qglUniform1iARB( interactionDirShader.u_normalTexture, 2 );
		qglUniform1iARB( interactionDirShader.u_diffuseTexture, 3 );
		qglUniform1iARB( interactionDirShader.u_specularTexture, 4 );
		qglUniform1iARB( interactionDirShader.u_ssaoTexture, 5 );
		qglUniform1iARB( interactionDirShader.u_aux0Texture, 6 );
		qglUniform1iARB( interactionDirShader.u_aux1Texture, 7 );
		qglUniform1iARB( interactionDirShader.u_aux2Texture, 8 );
		qglUniform1iARB( interactionDirShader.u_aux3Texture, 9 );
		qglUniform1iARB( interactionDirShader.u_aux4Texture, 10 );
		qglUniform1iARB( interactionDirShader.u_aux5Texture, 11 );
		qglUniform1iARB( interactionDirShader.u_aux6Texture, 12 );
		qglUniform1iARB( interactionDirShader.u_aux7Texture, 13 );
		qglUniform1iARB( interactionDirShader.u_aux8Texture, 14 );
		qglUniform1iARB( interactionDirShader.u_aux9Texture, 15 );
		qglUseProgramObjectARB( 0 );
	}

	// load ambient interation shaders
	R_LoadGLSLShader( "interaction_Amb.vs", &interactionAmbShader, GL_VERTEX_SHADER_ARB );
	R_LoadGLSLShader( "interaction_Amb.fs", &interactionAmbShader, GL_FRAGMENT_SHADER_ARB );
	if( !R_LinkGLSLShader( &interactionAmbShader, true ) && !R_ValidateGLSLProgram( &interactionAmbShader ) )
	{
		return false;
	}
	else
	{
		// set uniform locations
		interactionAmbShader.u_lightCubeTexture = qglGetUniformLocationARB( interactionAmbShader.program, "u_lightCubeTexture" );
		interactionAmbShader.u_lightFalloffTexture = qglGetUniformLocationARB( interactionAmbShader.program, "u_lightFalloffTexture" );
		interactionAmbShader.u_normalTexture = qglGetUniformLocationARB( interactionAmbShader.program, "u_normalTexture" );
		interactionAmbShader.u_diffuseTexture = qglGetUniformLocationARB( interactionAmbShader.program, "u_diffuseTexture" );
		interactionAmbShader.u_specularTexture = qglGetUniformLocationARB( interactionAmbShader.program, "u_specularTexture" );
		interactionAmbShader.u_ssaoTexture = qglGetUniformLocationARB( interactionAmbShader.program, "u_ssaoTexture" );
		interactionAmbShader.u_aux0Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux0Texture" );
		interactionAmbShader.u_aux1Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux1Texture" );
		interactionAmbShader.u_aux2Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux2Texture" );
		interactionAmbShader.u_aux3Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux3Texture" );
		interactionAmbShader.u_aux4Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux4Texture" );
		interactionAmbShader.u_aux5Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux5Texture" );
		interactionAmbShader.u_aux6Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux6Texture" );
		interactionAmbShader.u_aux7Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux7Texture" );
		interactionAmbShader.u_aux8Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux8Texture" );
		interactionAmbShader.u_aux9Texture = qglGetUniformLocationARB( interactionAmbShader.program, "u_aux9Texture" );

		interactionAmbShader.modelMatrix = qglGetUniformLocationARB( interactionAmbShader.program, "u_modelMatrix" );

		interactionAmbShader.localLightOrigin = qglGetUniformLocationARB( interactionAmbShader.program, "u_lightOrigin" );
		interactionAmbShader.localViewOrigin = qglGetUniformLocationARB( interactionAmbShader.program, "u_viewOrigin" );
		interactionAmbShader.lightProjectionS = qglGetUniformLocationARB( interactionAmbShader.program, "u_lightProjectionS" );
		interactionAmbShader.lightProjectionT = qglGetUniformLocationARB( interactionAmbShader.program, "u_lightProjectionT" );
		interactionAmbShader.lightProjectionQ = qglGetUniformLocationARB( interactionAmbShader.program, "u_lightProjectionQ" );
		interactionAmbShader.lightFalloff = qglGetUniformLocationARB( interactionAmbShader.program, "u_lightFalloff" );

		interactionAmbShader.bumpMatrixS = qglGetUniformLocationARB( interactionAmbShader.program, "u_bumpMatrixS" );
		interactionAmbShader.bumpMatrixT = qglGetUniformLocationARB( interactionAmbShader.program, "u_bumpMatrixT" );
		interactionAmbShader.diffuseMatrixS = qglGetUniformLocationARB( interactionAmbShader.program, "u_diffuseMatrixS" );
		interactionAmbShader.diffuseMatrixT = qglGetUniformLocationARB( interactionAmbShader.program, "u_diffuseMatrixT" );
		interactionAmbShader.specularMatrixS = qglGetUniformLocationARB( interactionAmbShader.program, "u_specularMatrixS" );
		interactionAmbShader.specularMatrixT = qglGetUniformLocationARB( interactionAmbShader.program, "u_specularMatrixT" );

		interactionAmbShader.colorMAD = qglGetUniformLocationARB( interactionAmbShader.program, "u_colorMAD" );

		interactionAmbShader.diffuseColor = qglGetUniformLocationARB( interactionAmbShader.program, "u_diffuseColor" );
		interactionAmbShader.specularColor = qglGetUniformLocationARB( interactionAmbShader.program, "u_specularColor" );

		interactionAmbShader.specExp = qglGetUniformLocationARB( interactionAmbShader.program, "u_specExp" );

		interactionAmbShader.localParms[0] = qglGetUniformLocationARB( interactionAmbShader.program, "u_localParm0" );
		interactionAmbShader.localParms[1] = qglGetUniformLocationARB( interactionAmbShader.program, "u_localParm1" );
		interactionAmbShader.localParms[2] = qglGetUniformLocationARB( interactionAmbShader.program, "u_localParm2" );
		interactionAmbShader.localParms[3] = qglGetUniformLocationARB( interactionAmbShader.program, "u_localParm3" );
		interactionAmbShader.localParms[4] = qglGetUniformLocationARB( interactionAmbShader.program, "u_localParm4" );
		interactionAmbShader.localParms[5] = qglGetUniformLocationARB( interactionAmbShader.program, "u_localParm5" );
		interactionAmbShader.localParms[6] = qglGetUniformLocationARB( interactionAmbShader.program, "u_localParm6" );
		interactionAmbShader.localParms[7] = qglGetUniformLocationARB( interactionAmbShader.program, "u_localParm7" );

		// set texture locations
		qglUseProgramObjectARB( interactionAmbShader.program );
		qglUniform1iARB( interactionAmbShader.u_lightCubeTexture, 0 );
		qglUniform1iARB( interactionAmbShader.u_lightFalloffTexture, 1 );
		qglUniform1iARB( interactionAmbShader.u_normalTexture, 2 );
		qglUniform1iARB( interactionAmbShader.u_diffuseTexture, 3 );
		qglUniform1iARB( interactionAmbShader.u_specularTexture, 4 );
		qglUniform1iARB( interactionAmbShader.u_ssaoTexture, 5 );
		qglUniform1iARB( interactionAmbShader.u_aux0Texture, 6 );
		qglUniform1iARB( interactionAmbShader.u_aux1Texture, 7 );
		qglUniform1iARB( interactionAmbShader.u_aux2Texture, 8 );
		qglUniform1iARB( interactionAmbShader.u_aux3Texture, 9 );
		qglUniform1iARB( interactionAmbShader.u_aux4Texture, 10 );
		qglUniform1iARB( interactionAmbShader.u_aux5Texture, 11 );
		qglUniform1iARB( interactionAmbShader.u_aux6Texture, 12 );
		qglUniform1iARB( interactionAmbShader.u_aux7Texture, 13 );
		qglUniform1iARB( interactionAmbShader.u_aux8Texture, 14 );
		qglUniform1iARB( interactionAmbShader.u_aux9Texture, 15 );
		qglUseProgramObjectARB( 0 );
	}

	// load stencil shadow extrusion shaders
	R_LoadGLSLShader( "stencilshadow.vs", &stencilShadowShader, GL_VERTEX_SHADER_ARB );
	R_LoadGLSLShader( "stencilshadow.fs", &stencilShadowShader, GL_FRAGMENT_SHADER_ARB );
	if( !R_LinkGLSLShader( &stencilShadowShader, false ) && !R_ValidateGLSLProgram( &stencilShadowShader ) )
	{
		return false;
	}
	else
	{
		// set uniform locations
		stencilShadowShader.localLightOrigin = qglGetUniformLocationARB( stencilShadowShader.program, "u_lightOrigin" );
	}

	return true;
}

/*
==================
R_FindGLSLProgram

Returns a GL identifier that can be bound to the given target, parsing
a text file if it hasn't already been loaded.
==================
*/
int R_FindGLSLProgram( GLenum target, const char* program )
{
	//int		i;
	//idStr	stripped = program;

	//stripped.StripFileExtension();

	//// see if it is already loaded
	//for ( i = 0; progs[ i ].name[ 0 ]; i++ ) {
	//	if ( progs[ i ].target != target ) {
	//		continue;
	//	}

	//	idStr compare = progs[ i ].name;
	//	compare.StripFileExtension();

	//	if ( !idStr::Icmp( stripped.c_str(), compare.c_str() ) ) {
	//		return progs[ i ].ident;
	//	}
	//}

	//if ( i == MAX_GLPROGS ) {
	//	common->Error( "R_FindARBProgram: MAX_GLPROGS" );
	//}

	//// add it to the list and load it
	//progs[ i ].ident = (program_t)0;	// will be gen'd by R_LoadARBProgram
	//progs[ i ].target = target;
	//strncpy( progs[ i ].name, program, sizeof( progs[ i ].name ) - 1 );

	//R_LoadGLSLShader( i );

	//return progs[ i ].ident;
	return 0;
}

/*
==================
R_ReloadGLSLPrograms_f
==================
*/
void R_ReloadGLSLPrograms_f( const idCmdArgs& args )
{
	common->Printf( "----- R_ReloadGLSLPrograms -----\n" );
	//for ( int i = 0; progs[ i ].name[ 0 ]; i++ ) {
	//	R_LoadGLSLShader( i );
	//}
	RB_GLSL_InitShaders();
	common->Printf( "-------------------------------\n" );
}

/*
==================
R_GLSL_Init
==================
*/
void R_GLSL_Init( void )
{
	glConfig.allowGLSLPath = false;

	common->Printf( "---------- R_GLSL_Init -----------\n" );

	if( !glConfig.GLSLAvailable )
	{
		common->Printf( "Not available.\n" );
		return;
	}
	else if( !RB_GLSL_InitShaders() )
	{
		common->Printf( "GLSL shaders failed to init.\n" );
		return;
	}

	common->Printf( "Available.\n" );

	common->Printf( "---------------------------------\n" );

	glConfig.allowGLSLPath = true;
}

