###==============================================================================
#	Feedback Interaction Parallax 3D Vertex/Fragment Program 
#
#	Normalized Blinn-Phong Lighting Model (Isotropic)
###==============================================================================
#
# --------------------------------------
# vertex attributes:
# 
# attrib[8]		= texture coordinates
# attrib[9]		= global tangent
# attrib[10]	= global bitangent
# attrib[11]	= global normal
#
# --------------------------------------
# environment variables:
#
# env[0]  		= 1.0 to _currentRender conversion
# env[1]  		= fragment.position to 0.0 - 1.0 conversion
# env[2]  		= diffuse light color
# env[3]  		= specular exponent scale/bias
# env[4]  		= localLightOrigin
# env[5]		= localViewOrigin
# env[6]		= lightProjection S
# env[7]		= lightProjection T
# env[8]		= lightFalloff S
# env[9]		= lightProjection Q
# env[10]		= bumpMatrix S
# env[11]		= bumpMatrix T
# env[12]		= diffuseMatrix S
# env[13]		= diffuseMatrix T
# env[14]		= specularMatrix S
# env[15]		= specularMatrix T
# env[16]		= modelMatrix[0]
# env[17]		= modelMatrix[1]
# env[18]		= modelMatrix[2]
# env[19]		= vertex color modulate/add
#
#--------------------------------------
# textures binds:
# 
# texture 0		= 2D/cubemap light projection texture
# texture 1		= 1D ambient light falloff texture
# texture 2		= per-surface normal map
# texture 3		= per-surface diffuse map
# texture 4		= per-surface specular map
# texture 5		= global ssao buffer
# texture 6		= aux interaction map 0
# texture 7		= aux interaction map 1
# texture 8		= aux interaction map 2
# texture 9		= aux interaction map 3
# texture 10	= aux interaction map 4
# texture 11	= aux interaction map 5
# texture 12	= aux interaction map 6
# texture 13	= aux interaction map 7
# texture 14	= aux interaction map 8
# texture 15	= aux interaction map 9
#
#--------------------------------------
###==============================================================================

!!ARBvp1.0
OPTION	ARB_position_invariant;

# Instruction Count: 21

TEMP	R0;

# calculate vector to light in R0
ADD		R0, program.env[4], -vertex.position;
DP3		result.texcoord[0].x, vertex.attrib[9], R0;
DP3		result.texcoord[0].y, vertex.attrib[10], R0;
DP3		result.texcoord[0].z, vertex.attrib[11], R0;

# calculate vector to viewer in R0
ADD		R0, program.env[5], -vertex.position;
DP3		result.texcoord[1].x, vertex.attrib[9], R0;
DP3		result.texcoord[1].y, vertex.attrib[10], R0;
DP3		result.texcoord[1].z, vertex.attrib[11], R0;

DP3		R0.w, R0, R0;
RSQ		R0.w, R0.w;
RCP		result.texcoord[1].w, R0.w;

# light projection texture coordinates
DP4		result.texcoord[2].x, vertex.position, program.env[6];
DP4		result.texcoord[2].y, vertex.position, program.env[7];
DP4		result.texcoord[2].z, vertex.position, program.env[8];

# normal map texture coordinates
DP4 	result.texcoord[3].x, vertex.attrib[8], program.env[10];
DP4 	result.texcoord[3].y, vertex.attrib[8], program.env[11];

# diffuse and specular map texture coordinates
DP4		result.texcoord[4].x, vertex.attrib[8], program.env[12];
DP4		result.texcoord[4].y, vertex.attrib[8], program.env[13];
DP4		result.texcoord[4].z, vertex.attrib[8], program.env[14];
DP4		result.texcoord[4].w, vertex.attrib[8], program.env[15];

# ssao buffer texcoords
DP4		result.texcoord[5].x, vertex.position, state.matrix.mvp.row[0];
DP4		result.texcoord[5].y, vertex.position, state.matrix.mvp.row[1];
DP4		result.texcoord[5].w, vertex.position, state.matrix.mvp.row[3];	# need w to divide

# generate the vertex color, which can be 1.0, color, or 1.0 - color
# for 1.0 			: env[19].x =  0.0, env[19].y = 1.0
# for color			: env[19].x =  1.0, env[19].y = 0.0
# for 1.0 - color	: env[19].x = -1.0, env[19].y = 1.0
MAD		result.color, vertex.color, program.env[19].x, program.env[19].y;

END

#==================================================================================

!!ARBfp1.0 
OPTION	ARB_precision_hint_fastest;

# Instruction Count: ALU: 97 TEX: 6 Total: 103

OUTPUT 	oColor 		= result.color;
ATTRIB 	vColor 		= fragment.color;
ATTRIB 	fPos 		= fragment.position;

ATTRIB 	lightVecTC 	= fragment.texcoord[0];
ATTRIB 	viewVecTC 	= fragment.texcoord[1];
ATTRIB 	lightProjTC = fragment.texcoord[2];
ATTRIB 	normalTC	= fragment.texcoord[3];
ATTRIB 	diffspecTC 	= fragment.texcoord[4];
ATTRIB 	ssaoTC		= fragment.texcoord[5];

PARAM 	nonPoT		= program.env[0];
PARAM 	invRes		= program.env[1];
PARAM 	lightColor 	= program.env[2];
PARAM 	specExp 	= program.env[3];

PARAM	heightScale	= program.local[0];

PARAM	const		= { 1.0,  2.0,  4.0,  5.0  };
PARAM	const2		= { 0.25, 0.5,  0.75, 0.75 };
PARAM	lumVec		= { 0.212671, 0.715160, 0.072169 };
PARAM 	M_PI		= 3.1415926535897932384626433832795;
PARAM 	M_8PI		= 25.132741228718345907701147066236;
PARAM 	M_inv2PI	= 0.15915494309189533576888376337251;
PARAM 	M_inv8PI	= 0.039788735772973833942220940843129;
PARAM 	e			= 2.7182818284590452353602874713527;
PARAM 	gamma		= { 2.4, 0.94786729857819905213270142180095, 0.41666666666666666666666666666667, 0.077399380804953560371517027863777 };

TEMP	lightVec, viewVec, halfVec, normalVec, detailNormalVec; 
TEMP	diffuse, specular, gloss, rough, fresnel, ssao, color; 
TEMP	atten, lightCube;
TEMP	NdotL, NdotH, NdotV, VdotL;
TEMP	R1, R2;


# calculate point light tc
MAD 	R1.xyz, lightProjTC, 2.0, -1.0;

# sample projection cube map
TEX 	lightCube, R1, texture[0], CUBE;

# calculate attenuation
DP3 	R1.w, R1, R1;
RSQ 	R1.w, R1.w;
RCP 	R1.w, R1.w;
ADD_SAT R1.w, 1.0, -R1.w;
MUL 	R1.w, R1.w, R1.w;
MUL 	atten, lightCube, R1.w;

# early out - attenuation
DP3 	atten.w, atten, const.x;
SGE 	atten.w, 0.0, atten.w;
# KIL 	-atten.w;

# normalize the vector to the viewer
DP3 	viewVec.w, viewVecTC, viewVecTC;
RSQ 	viewVec.w, viewVec.w;
MUL 	viewVec.xyz, viewVecTC, viewVec.w;

# load height map then scale & bias
TEX 	normalVec, normalTC, texture[2], 2D;
MAD 	R2.w, normalVec.w, heightScale.x, heightScale.y;

# calculate new texcoords
MAD 	R1, R2.w, viewVec.xyxy, normalTC;
MAD 	R2, R2.w, viewVec.xyxy, diffspecTC;
MUL 	R1.zw, R1.xyxy, program.local[1].xyxy;

# scale normal vector to -1.0<->1.0 range and normalize
TEX 	normalVec, R1, texture[2], 2D;
TEX 	detailNormalVec, R1.zwzw, texture[6], 2D;
MAD		detailNormalVec.xyz, detailNormalVec, const.y, -const.x;

ADD_SAT R1.x, viewVecTC.w, -program.local[1].z;
RCP 	R1.w, program.local[1].w;
MAD_SAT detailNormalVec.w, R1.x, -R1.w, 1.0;

MAD		normalVec.xyz, normalVec, const.y, -const.x;
MAD		normalVec.xyz, detailNormalVec, detailNormalVec.w, normalVec;
DP3		normalVec.w, normalVec, normalVec;
RSQ		normalVec.w, normalVec.w;
MUL		normalVec.xyz, normalVec, normalVec.w;

# normalize the vector to the light
DP3 	lightVec.w, lightVecTC, lightVecTC;
RSQ 	lightVec.w, lightVec.w;
MUL 	lightVec.xyz, lightVecTC, lightVec.w;

# calculate diffuse cosine
DP3 	NdotL.x, normalVec, lightVec;

# early out - NdotL
SGE 	R1.w, 0.0, NdotL.x;
# KIL 	-R1.w;

# calculate the half angle vector and normalize
ADD 	halfVec, lightVec, viewVec;
DP3 	halfVec.w, halfVec, halfVec;
RSQ 	halfVec.w, halfVec.w;
MUL 	halfVec.xyz, halfVec, halfVec.w;

# calculate cosines
DP3_SAT	NdotH.x, normalVec, halfVec;
DP3_SAT	NdotV.x, normalVec, viewVec;
DP3_SAT	VdotL.x, viewVec, -lightVec;

# load texture maps
TEX		diffuse, R2.xyxy, texture[3], 2D;
TEX		gloss, R2.zwzw, texture[4], 2D;

# calculate specular term
MAD 	rough.x, gloss.w, specExp.x, specExp.y;
ADD 	R1.x, rough.x, 6.0;
MUL 	rough.y, R1.x, M_inv8PI;
ADD_SAT fresnel.w, const.x, -NdotV.x;
MUL_SAT fresnel.w, fresnel.w, VdotL.x;
POW_SAT fresnel.w, fresnel.w, const.w;
LRP_SAT fresnel, gloss, const.x, fresnel.w;
POW_SAT specular, NdotH.x, rough.x;
MUL 	specular, specular, rough.y;

# sRGB -> Linear
MUL 	R1, diffuse, gamma.w;
ADD 	R2, diffuse, 0.055;
MUL 	R2, R2, gamma.y;
POW 	R2.x, R2.x, gamma.x;
POW 	R2.y, R2.y, gamma.x;
POW 	R2.z, R2.z, gamma.x;
SGE 	diffuse.xyz, 0.04045, diffuse;
CMP 	diffuse.xyz, -diffuse, R1, R2;

# final modulation
MAD 	color, specular, fresnel, diffuse;
MUL 	color, color, lightColor;
MUL 	color, color, NdotL.x;
MUL 	color, color, atten;

# ssao
MUL 	R1, fPos, nonPoT.zwzw;
TEX		R1, R1, texture[5], 2D;
MUL 	color, color, R1.w;

# Linear -> sRGB
MUL 	R1, color, 12.92;
POW 	R2.x, color.x, gamma.z;
POW 	R2.y, color.y, gamma.z;
POW 	R2.z, color.z, gamma.z;
MAD 	R2, R2, 1.055, -0.055;
SGE 	color, 0.0031308, color;
CMP 	color.xyz, -color, R1, R2;

# modify by the vertex color
MUL 	oColor.xyz, color, vColor;
MOV 	oColor.xyz, detailNormalVec.w;
MOV 	oColor.w, diffuse.w;

END