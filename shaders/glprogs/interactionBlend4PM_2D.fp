###==============================================================================
#	Feedback Interaction Blend4 2D Fragment Program 
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

PARAM 	maskTCScale = program.local[0];
PARAM 	rgbBias 	= program.local[1];
PARAM 	rgbScale 	= program.local[2];
PARAM 	heightScale = program.local[3];
PARAM 	heightBias 	= program.local[4];

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
TEMP	diffuse2, gloss2, normalVec2, blend; 
TEMP	diffuse3, gloss3, normalVec3; 
TEMP	diffuse4, gloss4, normalVec4; 
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

# normalize the vector to the viewer
DP3 	viewVec.w, viewVecTC, viewVecTC;
RSQ 	viewVec.w, viewVec.w;
MUL 	viewVec.xyz, viewVecTC, viewVec.w;

# calculate blend coefficient
MUL 	R1.xy, normalTC.xyxy, maskTCScale.xyxy;
TEX		blend, R1.xyxy, texture[6], 2D;
ADD_SAT blend, blend, -rgbBias;
MUL_SAT blend, blend, rgbScale;

# sample height maps (normal map alpha)
TEX 	normalVec, normalTC, texture[2], 2D;
TEX 	normalVec2, normalTC, texture[7], 2D;
TEX 	normalVec3, normalTC, texture[10], 2D;
TEX 	normalVec4, normalTC, texture[13], 2D;
MOV 	R2.x, normalVec.w;
MOV 	R2.y, normalVec2.w;
MOV 	R2.z, normalVec3.w;
MOV 	R2.w, normalVec4.w;
MAD 	R2, R2, heightScale, -heightBias;
LRP		R2.w, blend.z, R2.z, R2.w;
LRP		R2.w, blend.y, R2.y, R2.w;
LRP		R2.w, blend.x, R2.x, R2.w;

# calculate new texcoords
MAD 	R1, R2.w, viewVec.xyxy, normalTC;
MAD 	R2, R2.w, viewVec.xyxy, diffspecTC;

# do normal map blending
TEX 	normalVec, R1, texture[2], 2D;
TEX 	normalVec2, R1, texture[7], 2D;
TEX 	normalVec3, R1, texture[10], 2D;
TEX 	normalVec4, R1, texture[13], 2D;

LRP		normalVec3.xyz, blend.z, normalVec3, normalVec4;
LRP		normalVec2.xyz, blend.y, normalVec2, normalVec3;
LRP		normalVec.xyz, blend.x, normalVec, normalVec2;

# scale normal vector to -1.0<->1.0 range and normalize
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
SGE 	R1.w, 0.0, NdotL.x;
KIL 	-R1.w;

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
TEX		diffuse2, R2.xyxy, texture[8], 2D;
TEX		gloss2, R2.zwzw, texture[9], 2D;
TEX		diffuse3, R2.xyxy, texture[11], 2D;
TEX		gloss3, R2.zwzw, texture[12], 2D;
TEX		diffuse4, R2.xyxy, texture[14], 2D;
TEX		gloss4, R2.zwzw, texture[15], 2D;

# do diffuse/spec blending
LRP		diffuse3, blend.z, diffuse3, diffuse4;
LRP		diffuse2, blend.y, diffuse2, diffuse3;
LRP		diffuse, blend.x, diffuse, diffuse2;
LRP		gloss3, blend.z, gloss3, gloss4;
LRP		gloss2, blend.y, gloss2, gloss3;
LRP		gloss, blend.x, gloss, gloss2;

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