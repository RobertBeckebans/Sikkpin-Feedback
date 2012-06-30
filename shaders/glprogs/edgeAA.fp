###==============================================================================
#	Feedback EdgeAA Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
#
# local[0]	= sample offset scale
# local[1]	= filter offset scale
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0
OPTION	ARB_precision_hint_nicest;

OUTPUT	oColor	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB 	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

PARAM	sScale	= program.local[0];
PARAM	fScale	= program.local[1];

# sample offsets
PARAM	off1	= {  0.0,  1.0,  0.0, -1.0 };
PARAM	off2	= { -1.0,  0.0,  1.0,  0.0 };

PARAM	lumVec	= { 0.212671, 0.715160, 0.072169 };
PARAM	inv5	= 0.2;

TEMP 	uv, invres, color, lum, scale, vec, sign;
TEMP 	R0, R1, R2, R3, R4;
TEMP 	S0, S1, S2, S3, S4;
TEMP 	T0, T1, T2, T3, T4;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, nonPoT.zwzw;

MUL 	scale, sScale, nonPoT.zwzw;

MAD_SAT R1, off1, scale, uv;	# top / bottom
MAD_SAT R2, off2, scale, uv;	# left / right

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;

# RGB-based distance
DP3 	sign.x, T1, T1;
DP3 	sign.y, T2, T2;
DP3 	sign.z, T3, T3;
DP3 	sign.w, T4, T4;
SLT 	sign.xyzw, sign.xzxz, sign.ywyw;
CMP 	sign.xy, -sign, -1.0, 1.0;

ADD 	T1.xyz, T1, -T2;
DP3 	T1.w, T1, T1;
RSQ 	T1.w, T1.w;
RCP 	vec.x, T1.w;
ADD 	T2.xyz, T3, -T4;
DP3 	T2.w, T2, T2;
RSQ 	T2.w, T2.w;
RCP 	vec.y, T2.w;

# Luminance-based distance
# DP3 	T0.w, T0, lumVec;
# DP3 	lum.x, T1, lumVec;
# DP3 	lum.y, T2, lumVec;
# DP3 	lum.z, T3, lumVec;
# DP3 	lum.w, T4, lumVec;
# SLT 	sign.xyzw, lum.xzxz, lum.ywyw;
# CMP 	sign.xy, -sign, -1.0, 1.0;

# ADD 	vec, lum.xzxz, -lum.ywyw;
# MUL 	vec.xy, vec, 2.0;

MUL 	vec, vec.xyxy, sign.xyxy;

MUL 	vec, vec, nonPoT.zwzw;
MAD_SAT R1, vec.xyxy, fScale.xxyy, uv;
MAD_SAT R2, vec.xyxy, fScale.zzww, uv;

TEX 	T0, uv.xyxy, texture[0], 2D;
TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;

ADD 	color, T0, T1;
ADD 	color, color, T2;
ADD 	color, color, T3;
ADD 	color, color, T4;
MUL 	color, color, inv5;

MOV 	oColor.xyz, color;

# for visualizing the vector
# MAD 	oColor, vec, 0.5, 0.5;
# MOV 	oColor, vec;
# MOV 	oColor.z, 1;

END