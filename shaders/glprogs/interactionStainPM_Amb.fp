###==============================================================================
#	Feedback Interaction Stain Parallax Ambient Fragment Program 
#
#	Valve's Basis Function Ambient Lighting
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

PARAM	heightScale	= program.local[0];
PARAM 	maskTCScale = program.local[1];
PARAM 	blendParm 	= program.local[2];

PARAM	const		= { 1.0,  2.0,  4.0,  5.0  };
PARAM	const2		= { 0.25, 0.5,  0.75, 0.75 };
PARAM	lumVec		= { 0.212671, 0.715160, 0.072169 };
PARAM 	M_PI		= 3.1415926535897932384626433832795;
PARAM 	M_8PI		= 25.132741228718345907701147066236;
PARAM 	M_inv2PI	= 0.15915494309189533576888376337251;
PARAM 	M_inv8PI	= 0.039788735772973833942220940843129;
PARAM 	e			= 2.7182818284590452353602874713527;
PARAM 	gamma		= { 2.4, 0.94786729857819905213270142180095, 0.41666666666666666666666666666667, 0.077399380804953560371517027863777 };

TEMP	viewVec, normalVec, wViewVec, wNormalVec, reflectVec; 
TEMP	diffuse, specular, gloss, rough, color, ambient, atten, stain; 
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

# normalize tangent space view vector
DP3 	viewVec.w, viewVecTC, viewVecTC;
RSQ 	viewVec.w, viewVec.w;
MUL 	viewVec.xyz, viewVecTC, viewVec.w;

# normalize world space view vector
DP3 	wViewVec.w, wViewVecTC, wViewVecTC;
RSQ 	wViewVec.w, wViewVec.w;
MUL 	wViewVec.xyz, wViewVecTC, wViewVec.w;

# load height map then scale & bias
TEX 	normalVec, normalTC, texture[2], 2D;
MAD 	R2.w, normalVec.w, heightScale.x, heightScale.y;

# calculate new texcoords
MAD 	R1, R2.w, viewVec.xyxy, normalTC;
MAD 	R2, R2.w, viewVec.xyxy, diffspecTC;
MUL 	R1.zw, R1.xyxy, maskTCScale.xyxy;

# load texture maps
TEX 	normalVec, R1.xyxy, texture[2], 2D;
TEX		diffuse, R2.xyxy, texture[3], 2D;
TEX		gloss, R2.zwzw, texture[4], 2D;
TEX		stain, R1.zwzw, texture[6], 2D;

# do diffuse blending
MUL 	stain.xyz, diffuse, stain;
ADD_SAT stain.w, stain.w, blendParm.x;
MUL_SAT stain.w, stain.w, blendParm.y;
LRP 	diffuse.xyz, stain.w, stain, diffuse;

# scale normal vector to -1.0<->1.0 range and normalize
MAD 	normalVec.xyz, normalVec, const.y, -const.x;

# transform normal to world space
DP3 	wNormalVec.x, normalVec, tangent;
DP3 	wNormalVec.y, normalVec, bitangent;
DP3 	wNormalVec.z, normalVec, normal;

# normalize world space normal
DP3 	wNormalVec.w, wNormalVec, wNormalVec;
RSQ 	wNormalVec.w, wNormalVec.w;
MUL 	wNormalVec.xyz, wNormalVec, wNormalVec.w;

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