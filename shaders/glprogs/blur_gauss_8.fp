###==============================================================================
#	Feedback Blur Gaussian 8 Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
#
# local[0]	= offset scale (w = holds whether we're working with a cropped or full res texture)
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0
OPTION	ARB_precision_hint_fastest;

OUTPUT	oColor	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB 	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

PARAM 	Scale	= program.local[0];

# Gaussian distribution in 1D, standard deviation = 1.5
PARAM	d0		= 0.26596152;
PARAM	d1		= 0.21296534;
PARAM	d2		= 0.10934005;
PARAM	d3		= 0.03599398;
PARAM	d4		= 0.00759732;

# linear distribution
# PARAM	d0		= 0.20;
# PARAM	d1		= 0.16;
# PARAM	d2		= 0.12;
# PARAM	d3		= 0.08;
# PARAM	d4		= 0.04;

# sample offsets
PARAM 	off1	= { 1.0, 1.0, 2.0, 2.0 };
PARAM 	off2	= { 3.0, 3.0, 4.0, 4.0 };

TEMP 	uv, invres, color, scale;
TEMP 	R0, R1, R2, R3, R4;
TEMP 	T0, T1, T2, T3, T4;


# calculate the non-power-of-two inverse resolution
MUL 	invres, invRes.xyxy, nonPoT.xyxy;
CMP 	invres, Scale.w, invres, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

MUL 	scale, Scale.xyxy, invres;

MAD_SAT R1, off1, scale, uv;
MAD_SAT R2, off2, scale, uv;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;

MUL 	color, T1, d1;
MAD 	color, T2, d2, color;
MAD 	color, T3, d3, color;
MAD 	color, T4, d4, color;

MAD_SAT R1, -off1, scale, uv;
MAD_SAT R2, -off2, scale, uv;

TEX 	T0, uv.xyxy, texture[0], 2D;
TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;

MAD 	color, T0, d0, color;
MAD 	color, T1, d1, color;
MAD 	color, T2, d2, color;
MAD 	color, T3, d3, color;
MAD 	color, T4, d4, color;

MOV 	oColor.xyz, color;

END