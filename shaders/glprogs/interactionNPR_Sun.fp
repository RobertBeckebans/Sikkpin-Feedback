###==============================================================================
#	Feedback Interaction Sun Fragment Program 
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

PARAM 	nonPoT		= program.env[0];
PARAM 	invRes		= program.env[1];
PARAM 	lightColor 	= program.env[2];
PARAM 	specExp 	= program.env[3];

PARAM	const		= { 1.0,  2.0,  4.0,  5.0  };
PARAM	const2		= { 0.25, 0.5,  0.75, 0.75 };
PARAM	lumVec		= { 0.212671, 0.715160, 0.072169 };
PARAM 	M_PI		= 3.1415926535897932384626433832795;
PARAM 	M_8PI		= 25.132741228718345907701147066236;
PARAM 	M_inv2PI	= 0.15915494309189533576888376337251;
PARAM 	M_inv8PI	= 0.039788735772973833942220940843129;
PARAM 	e			= 2.7182818284590452353602874713527;
PARAM 	gamma		= { 2.4, 0.94786729857819905213270142180095, 0.41666666666666666666666666666667, 0.077399380804953560371517027863777 };

TEMP	lightVec, viewVec, reflectVec, normalVec; 
TEMP	diffuse, specular, gloss, rough, fresnel, color; 
TEMP	atten, lightCube;
TEMP	NdotL, RdotV, NdotV, VdotL;
TEMP	R1, R2;


# calculate point light tc
MAD 	R1.xyz, lightProjTC, 2.0, -1.0;

# sample projection cube map
TEX 	lightCube, R1, texture[0], CUBE;

# calculate attenuation
SGE 	R1, 1.0, lightProjTC;
SGE 	R2, lightProjTC, 0.0;
DP3 	R1.w, R1, R2;
SGE 	R1.w, R1.w, 3.0;
MUL 	atten, lightCube, R1.w;

# early out - attenuation
DP3 	atten.w, atten, const.x;
SGE 	atten.w, 0.0, atten.w;
KIL 	-atten.w;

# scale normal vector to -1.0<->1.0 range and normalize
TEX 	normalVec, normalTC, texture[2], 2D;
MAD		normalVec.xyz, normalVec, const.y, -const.x;
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
# SGE 	R1.w, 0.0, NdotL.x;
# KIL 	-R1.w;

MOV 	R1, 0.5;
MOV 	R1.x, NdotL.x;
TEX		NdotL, R1, texture[6], 2D;

# normalize the vector to the viewer
DP3 	viewVec.w, viewVecTC, viewVecTC;
RSQ 	viewVec.w, viewVec.w;
MUL 	viewVec.xyz, viewVecTC, viewVec.w;

# calculate reflection vector
DP3 	reflectVec, lightVec, normalVec;
MUL 	reflectVec, reflectVec, normalVec;
MAD 	reflectVec.xyz, reflectVec, 2.0, -lightVec;

# calculate cosines
DP3_SAT	RdotV.x, reflectVec, viewVec;
DP3_SAT	NdotV.x, normalVec, viewVec;
DP3_SAT	VdotL.x, viewVec, -lightVec;

# load texture maps
TEX		diffuse, diffspecTC.xyxy, texture[3], 2D;
TEX		gloss, diffspecTC.zwzw, texture[4], 2D;

# calculate specular term
MUL 	R1, specExp.x, 0.25;
MAD 	rough.x, gloss.w, R1, specExp.y;
ADD 	R1.x, rough.x, 8.0;
MUL 	rough.y, R1.x, M_inv8PI;
ADD_SAT fresnel.w, const.x, -NdotV.x;
MUL_SAT fresnel.w, fresnel.w, VdotL.x;
POW_SAT fresnel.w, fresnel.w, 5.0.x;
LRP_SAT fresnel, gloss, const.x, fresnel.w;
POW_SAT specular, RdotV.x, rough.x;
TEX		specular, specular.x, texture[7], 2D;
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
MOV 	oColor.w, diffuse.w;

END