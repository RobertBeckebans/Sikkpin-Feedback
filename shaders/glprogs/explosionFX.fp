###==============================================================================
#	Feedback Explosion FX Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
#
# local[0]	= screen-space location of center
# local[1]	= blur scale
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0 
OPTION	ARB_precision_hint_fastest;

OUTPUT	oColor	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

PARAM 	Center	= program.local[0];
PARAM 	Scale	= program.local[1];

# sample offsets
PARAM 	off1	= { 0.0, 1.0, 2.0, 3.0 };
PARAM 	off2	= { 4.0, 5.0, 6.0, 7.0 };

PARAM 	inv8 	= 0.125;

TEMP 	uv, invres, color, center, scale, dir, dist, aspect;
TEMP	R0, R1, R2, R3, R4;
TEMP 	T0, T1, T2, T3, T4, T5, T6, T7, T8;


# calculate the non-power-of-two inverse resolution
MUL 	invres, invRes.xyxy, nonPoT.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

# calculate aspect
RCP 	aspect.x, invres.x;
MUL 	aspect.x, aspect.x, invres.y;
MOV 	aspect.y, 1.0;
MOV 	aspect.z, 0.0;

MUL 	center, Center, nonPoT;
ADD 	dir, center, -uv;
MUL 	dir, dir, aspect;
DP3 	dist.w, dir, dir;
RSQ 	dist.w, dist.w;
RCP_SAT dist.w, dist.w;

MUL 	scale, Scale, invres;
MUL 	dir, dir.xyxy, scale.xyxy;

MAD_SAT R1, dir, off1.xxyy, uv;
MAD_SAT R2, dir, off1.zzww, uv;
MAD_SAT R3, dir, off2.xxyy, uv;
MAD_SAT R4, dir, off2.zzww, uv;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;
TEX 	T8, R4.zwzw, texture[0], 2D;

ADD 	color, T1, T2;
ADD 	color, T3, color;
ADD 	color, T4, color;
ADD 	color, T5, color;
ADD 	color, T6, color;
ADD 	color, T7, color;
ADD 	color, T8, color;
MUL 	color, color, inv8;

LRP 	oColor.xyz, dist.w, T1, color;

END