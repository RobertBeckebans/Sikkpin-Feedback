###==============================================================================
#	Feedback Interaction Translucent 2D Fragment Program 
#
#	Normalized Blinn-Phong Lighting Model (Isotropic)
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

TEMP	lightVec, viewVec, halfVec, normalVec; 
TEMP	diffuse, specular, gloss, rough, fresnel, color; 
TEMP	atten, lightFall, lightProj;
TEMP	NdotL, NdotH, NdotV, VdotL;
TEMP	R1, R2;


# load falloff/projection maps
KIL 	lightProjTC;
TXP 	lightProj, lightProjTC, texture[0], 2D;
ADD_SAT lightFall, 1.0, -lightProjTC.z;
MUL 	lightFall, lightFall, lightFall;

# calculate attenuation
MUL 	atten, lightProj, lightFall;

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
MUL 	NdotL.x, NdotL.x, NdotL.x;

# early out - NdotL
# SGE 	R1.w, 0.0, NdotL.x;
# KIL 	-R1.w;

# normalize the vector to the viewer
DP3 	viewVec.w, viewVecTC, viewVecTC;
RSQ 	viewVec.w, viewVec.w;
MUL 	viewVec.xyz, viewVecTC, viewVec.w;

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
TEX		diffuse, diffspecTC.xyxy, texture[3], 2D;
TEX		gloss, diffspecTC.zwzw, texture[4], 2D;

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
MOV 	oColor.w, diffuse.w;

END