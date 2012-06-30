###==============================================================================
#	Feedback FXAA Fragment Program 
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
PARAM	off1	= { -1.0,  1.0,  1.0,  1.0 };
PARAM	off2	= { -1.0, -1.0,  1.0, -1.0 };

PARAM	lumVec	= { 0.212671, 0.715160, 0.072169 };
PARAM	vecX	= { -1.0, -1.0,  1.0,  1.0 };
PARAM	vecY	= {  1.0, -1.0,  1.0, -1.0 };

PARAM	EDGE_SHARPNESS	= 0.03125;
PARAM	MAX_OFFSET		= 8.0;

TEMP 	uv, invres, color, scale, vec, lum, lumMinMax;
TEMP 	R0, R1, R2, R3, R4;
TEMP 	T0, T1, T2, T3, T4;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, nonPoT.zwzw;

MUL 	scale, sScale, nonPoT.zwzw;

MAD_SAT R1, off1, scale, uv;	# top / bottom
MAD_SAT R2, off2, scale, uv;	# left / right

TEX 	T0, uv.xyxy, texture[0], 2D;
TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;

DP3 	T0.w, T0, lumVec;
DP3 	lum.x, T1, lumVec;
DP3 	lum.y, T2, lumVec;
DP3 	lum.z, T3, lumVec;
DP3 	lum.w, T4, lumVec;

MIN 	lumMinMax.xy, lum.xzxz, lum.ywyw;
MIN 	lumMinMax.x, lumMinMax.x, lumMinMax.y;
MIN 	lumMinMax.x, lumMinMax.x, T0.w;

MAX 	lumMinMax.zw, lum.xzxz, lum.ywyw;
MAX 	lumMinMax.y, lumMinMax.z, lumMinMax.w;
MAX 	lumMinMax.y, lumMinMax.y, T0.w;

ADD 	R1, lum.xzxy, lum.ywzw;
ADD 	vec, R1.xzxz, -R1.ywyw;
ABS 	vec.zw, vec.xyxy;

DP4 	R1.w, lum, 1.0;
MUL 	R1.w, R1, EDGE_SHARPNESS;

MIN 	R1.x, vec.z, vec.w;
ADD 	R1.x, R1.x, R1.w;
RCP 	R1.x, R1.x;
MUL 	vec, vec, R1.x;
MAX 	vec, vec, -MAX_OFFSET;
MIN 	vec, vec, MAX_OFFSET;

MUL 	vec, vec, nonPoT.zwzw;
MAD_SAT R1, vec.xyxy, fScale.xxyy, uv;
MAD_SAT R2, vec.xyxy, fScale.zzww, uv;

# TEX 	T0, uv.xyxy, texture[0], 2D;
TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;

ADD 	R1, T1, T2;
MUL 	R1, R1, 0.5;

ADD 	R2, T3, T4;
MUL 	R2, R2, 0.25;
MAD 	R2, R1, 0.5, R2;

DP3 	R2.w, R2, lumVec;
SLT 	R3.x, R2.w, lumMinMax.x;
SLT 	R3.y, lumMinMax.y, R2.w;
ADD 	R3.x, R3.x, R3.y;
CMP 	color, -R3.x, R1, R2;

MOV 	oColor.xyz, color;

# for visualizing the vector
# MAD 	oColor, vec, 0.5, 0.5;
# MOV 	oColor, vec;
# MOV 	oColor.z, 0;

END