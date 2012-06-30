###==============================================================================
#	Feedback Sun Shafts Blur Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _sunShafts
#
# local[0]	= blur parameters
# local[1]	= Sun screen space position
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0 
OPTION	ARB_precision_hint_fastest;

OUTPUT 	oColor	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

PARAM	Size	= program.local[0];
PARAM	SunPos	= program.local[1];
PARAM	Sign	= program.local[2];

# sample offsets
PARAM 	off1	= { 0.0, 1.0, 2.0, 3.0 };
PARAM 	off2	= { 4.0, 5.0, 6.0, 7.0 };

PARAM	D1	 	= { 1.0, 0.875, 0.75, 0.625 };
PARAM	D2	 	= { 0.5, 0.375, 0.25, 0.125 };
# linear distribution
# PARAM	D1		= { 0.22222222, 0.19444444, 0.16666667, 0.13888889 };
# PARAM	D2		= { 0.11111111, 0.08333333, 0.05555556, 0.02777778 };

PARAM 	inv9 	= 0.11111111;
PARAM 	inv8 	= 0.125;

TEMP 	uv, color, sign, scale, dist, dir, aspect;
TEMP	R0, R1, R2, R3, R4;
TEMP 	T0, T1, T2, T3, T4, T5, T6, T7, T8;


# calculate the screen texcoord in the 0.0 to 1.0 range
# MUL 	uv, fPos.xyxy, invRes.xyxy;
MUL 	uv, fPos.xyxy, nonPoT.zwzw;

# calculate aspect
# RCP 	aspect.x, invRes.x;
# MUL 	aspect.x, aspect.x, invRes.y;
# MOV 	aspect.y, 1.0;
# MOV 	aspect.z, 0.0;

# CMP 	sign.x, Sign, -1.0, 1.0;

# MUL 	scale, Size.x, invRes.xyxy;

# ADD 	dir, SunPos, -uv.xyxy;
# MUL 	dir, dir, aspect;
# DP3 	dist.w, dir, dir;
# RSQ 	dist.w, dist.w;
# RCP 	dist.w, dist.w;
# ADD_SAT dist.w, 1.0, -dist.w;
# MUL_SAT dist.w, dist.w, dist.w;

# MUL_SAT dist.w, dist.w, Sign.x;

# MUL 	dir.xy, dir, scale;
# MUL 	dir.xy, dir, sign.x;

# MAD_SAT R1, dir.xyxy, off1.xxyy, uv;
# MAD_SAT R2, dir.xyxy, off1.zzww, uv;
# MAD_SAT R3, dir.xyxy, off2.xxyy, uv;
# MAD_SAT R4, dir.xyxy, off2.zzww, uv;

# TEX 	T1, R1.xyxy, texture[0], 2D;
# TEX 	T2, R1.zwzw, texture[0], 2D;
# TEX 	T3, R2.xyxy, texture[0], 2D;
# TEX 	T4, R2.zwzw, texture[0], 2D;
# TEX 	T5, R3.xyxy, texture[0], 2D;
# TEX 	T6, R3.zwzw, texture[0], 2D;
# TEX 	T7, R4.xyxy, texture[0], 2D;
# TEX 	T8, R4.zwzw, texture[0], 2D;

# MUL 	color, T1, D1.x;
# MAD 	color, T2, D1.y, color;
# MAD 	color, T3, D1.z, color;
# MAD 	color, T4, D1.w, color;
# MAD 	color, T5, D2.x, color;
# MAD 	color, T6, D2.y, color;
# MAD 	color, T7, D2.z, color;
# MAD 	color, T8, D2.w, color;
# MUL 	color, color, inv8;

# LRP_SAT color.xyz, color, 1.0, T1;
# ADD 	color.w, color, color;
# MUL 	oColor.xyz, color, dist.w;
# MAD_SAT sign.x, Sign, 0.1, 0.9;
# ADD 	sign.x, 1.0, -sign.x;
# ADD 	oColor.w, color.w, sign.x;

TEX 	oColor, TC, texture[0], 2D;

END