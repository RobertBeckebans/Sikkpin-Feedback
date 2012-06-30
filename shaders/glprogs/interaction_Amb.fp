###==============================================================================
#	Feedback Interaction Ambient Fragment Program 
#
#	Valve's Basis Function Ambient Lighting
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

# Instruction Count: ALU: 12 TEX: 4 Total: 16

OUTPUT 	oColor 		= result.color;
ATTRIB 	vColor 		= fragment.color;
ATTRIB 	fPos 		= fragment.position;

ATTRIB 	wViewVecTC 	= fragment.texcoord[0];
ATTRIB 	viewVecTC 	= fragment.texcoord[1];
ATTRIB 	lightProjTC = fragment.texcoord[2];
ATTRIB 	normalTC	= fragment.texcoord[3];
ATTRIB 	diffspecTC 	= fragment.texcoord[4];
ATTRIB 	tangent		= fragment.texcoord[5];
ATTRIB 	bitangent	= fragment.texcoord[6];
ATTRIB 	normal	 	= fragment.texcoord[7];

PARAM 	nonPoT		= program.env[0];
PARAM 	invRes		= program.env[1];
PARAM 	lightColor 	= program.env[2];
PARAM 	specExp 	= program.env[3];

PARAM 	ambColXNeg 	= program.env[20];
PARAM 	ambColXPos 	= program.env[21];
PARAM 	ambColYNeg 	= program.env[22];
PARAM 	ambColYPos 	= program.env[23];
PARAM 	ambColZNeg 	= program.env[24];
PARAM 	ambColZPos 	= program.env[25];

PARAM	const		= { 1.0,  2.0,  4.0,  5.0  };
PARAM	const2		= { 0.25, 0.5,  0.75, 0.75 };
PARAM	lumVec		= { 0.212671, 0.715160, 0.072169 };
PARAM	vecX		= { 1.0, 0.0, 0.0 };
PARAM	vecY		= { 0.0, 1.0, 0.0 };
PARAM	vecZ		= { 0.0, 0.0, 1.0 };
PARAM 	M_PI		= 3.1415926535897932384626433832795;
PARAM 	M_8PI		= 25.132741228718345907701147066236;
PARAM 	M_inv2PI	= 0.15915494309189533576888376337251;
PARAM 	M_inv8PI	= 0.039788735772973833942220940843129;
PARAM 	e			= 2.7182818284590452353602874713527;
PARAM 	gamma		= { 2.4, 0.94786729857819905213270142180095, 0.41666666666666666666666666666667, 0.077399380804953560371517027863777 };

TEMP	normalVec, wViewVec, wNormalVec, reflectVec; 
TEMP	diffuse, specular, gloss, rough, color, ambient, ssao, atten; 
TEMP	R1, R2, R3;
TEMP	S1, S2, S3;
TEMP 	C1, C2, C3;


# load falloff map
TEX		atten.x, lightProjTC.xxxx, texture[1], 2D;
TEX		atten.y, lightProjTC.yyyy, texture[1], 2D;
TEX		atten.z, lightProjTC.zzzz, texture[1], 2D;
MUL 	atten.w, atten.x, atten.y;
MUL 	atten, atten.w, atten.z;

SLT 	R1, 0.0, lightProjTC;
SLT 	R2, lightProjTC, 1.0;
DP3 	R1.w, R1, R2;
SGE 	R1.w, R1.w, 3.0;
MUL 	atten.w, atten.w, R1.w;

# early out - attenuation 
SGE 	atten.w, 0.0, atten.w;
KIL 	-atten.w;

# load texture maps
TEX 	normalVec, normalTC, texture[2], 2D;
TEX		diffuse, diffspecTC.xyxy, texture[3], 2D;
TEX		gloss, diffspecTC.zwzw, texture[4], 2D;

# normalize world space view vector
DP3 	wViewVec.w, wViewVecTC, wViewVecTC;
RSQ 	wViewVec.w, wViewVec.w;
MUL 	wViewVec.xyz, wViewVecTC, wViewVec.w;

# scale normal vector to -1.0<->1.0 range and normalize
MAD 	normalVec.xyz, normalVec, const.y, -const.x;

# transform normal to world space
DP3 	wNormalVec.x, normalVec, tangent;
DP3 	wNormalVec.y, normalVec, bitangent;
DP3 	wNormalVec.z, normalVec, normal;

# normalize world space normal
DP3		wNormalVec.w, wNormalVec, wNormalVec;
RSQ		wNormalVec.w, wNormalVec.w;
MUL		wNormalVec.xyz, wNormalVec, wNormalVec.w;

# Valve's basis function
CMP 	C1, wNormalVec.x, ambColXNeg, ambColXPos;
CMP 	C2, wNormalVec.y, ambColYNeg, ambColYPos;
CMP 	C3, wNormalVec.z, ambColZNeg, ambColZPos;
MUL 	R1, wNormalVec, wNormalVec;
MUL 	ambient.xyz, C1, R1.x;
MAD 	ambient.xyz, C2, R1.y, ambient;
MAD 	ambient.xyz, C3, R1.z, ambient;

# calculate reflection vector
DP3 	reflectVec.w, wNormalVec, wViewVec;
MUL 	reflectVec.xyz, reflectVec.w, wNormalVec;
MAD 	reflectVec.xyz, reflectVec, const.y, -wViewVec;

# calculate specular term
TEX 	specular, reflectVec, texture[0], CUBE;
MUL 	specular, specular, specular.w;

MAD 	rough.x, gloss.w, specExp.x, specExp.y;
MUL_SAT rough.y, rough.x, 0.0078125;
ADD_SAT rough.z, 1.0, -reflectVec.w;
POW 	rough.z, rough.z, 5.0.x;
MUL 	R1, gloss, rough.y;
LRP 	gloss, rough.z, gloss, R1;

# crude method to get variable "roughness"
# MAD 	rough.x, rough, 31.0, 1.0;
# MUL 	R3, reflectVec, reflectVec;
# MOV 	S1, reflectVec;
# MOV 	S2, reflectVec;
# MOV 	S3, reflectVec;
# MUL 	S1.x, S1.x, rough.x;
# MUL 	S2.y, S2.y, rough.x;
# MUL 	S3.z, S3.z, rough.x;
# TEX 	S1, S1, texture[3], CUBE;
# TEX 	S2, S2, texture[3], CUBE;
# TEX 	S3, S3, texture[3], CUBE;
# MUL 	specular.xyz, S1, R3.x;
# MAD 	specular.xyz, S2, R3.y, specular;
# MAD 	specular.xyz, S3, R3.z, specular;

# sRGB -> Linear
MUL 	R1, specular, gamma.w;
ADD 	R2, specular, 0.055;
MUL 	R2, R2, gamma.y;
POW 	R2.x, R2.x, gamma.x;
POW 	R2.y, R2.y, gamma.x;
POW 	R2.z, R2.z, gamma.x;
SGE 	specular.xyz, 0.04045, specular;
CMP 	specular.xyz, -specular, R1, R2;

# sRGB -> Linear
MUL 	R1, diffuse, gamma.w;
ADD 	R2, diffuse, 0.055;
MUL 	R2, R2, gamma.y;
POW 	R2.x, R2.x, gamma.x;
POW 	R2.y, R2.y, gamma.x;
POW 	R2.z, R2.z, gamma.x;
SGE 	diffuse.xyz, 0.04045, diffuse;
CMP 	diffuse.xyz, -diffuse, R1, R2;

# modulate specular by fresnel
MUL 	specular, specular, gloss;

# final modulation
MAD 	color, diffuse, ambient, specular;
# lightColor can be used to tint the ambient color but it's best to
# leave it grayscale and use it to just scale the ambient contribution
MUL 	color, color, lightColor;
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

# modulate by the vertex color
MUL 	oColor.xyz, color, vColor;
MOV 	oColor.w, diffuse.w;

END