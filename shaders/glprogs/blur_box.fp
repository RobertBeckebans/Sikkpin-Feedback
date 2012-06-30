###==============================================================================
#	Feedback Blur Box Fragment Program 
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

PARAM	Scale	= program.local[0];

# Gaussian distribution in 2D, standard deviation = 0.59
# PARAM	d0		= 0.45721041;
# PARAM	d1		= 0.10871990;
# PARAM	d2		= 0.10871990;
# PARAM	d3		= 0.10871990;
# PARAM	d4		= 0.10871990;
# PARAM	d5		= 0.02585247;
# PARAM	d6		= 0.02585247;
# PARAM	d7		= 0.02585247;
# PARAM	d8		= 0.02585247;

# linear distribution
# PARAM	d0		= 0.2500;
# PARAM	d1		= 0.1250;
# PARAM	d2		= 0.1250;
# PARAM	d3		= 0.1250;
# PARAM	d4		= 0.1250;
# PARAM	d5		= 0.0625;
# PARAM	d6		= 0.0625;
# PARAM	d7		= 0.0625;
# PARAM	d8		= 0.0625;

# mean distribution
PARAM 	d0		= 0.11111111;
PARAM 	d1		= 0.11111111;
PARAM 	d2		= 0.11111111;
PARAM 	d3		= 0.11111111;
PARAM 	d4		= 0.11111111;
PARAM 	d5		= 0.11111111;
PARAM 	d6		= 0.11111111;
PARAM 	d7		= 0.11111111;
PARAM 	d8		= 0.11111111;

# sample offsets
PARAM 	off1	= { -1.0,  0.0,  0.0, -1.0 };
PARAM 	off2	= {  1.0,  0.0,  0.0,  1.0 };
PARAM 	off3	= { -1.0, -1.0, -1.0,  1.0 };
PARAM 	off4	= {  1.0, -1.0,  1.0,  1.0 };

TEMP	uv, invres, color, scale;
TEMP 	R0, R1, R2, R3, R4;
TEMP 	T0, T1, T2, T3, T4, T5, T6, T7, T8;


# calculate the non-power-of-two inverse resolution
CMP 	invres, Scale.w, nonPoT.zwzw, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

MUL 	scale, Scale.xyxy, invres.xyxy;

MAD_SAT R1, off1, scale, uv.xyxy;
MAD_SAT R2, off2, scale, uv.xyxy;
MAD_SAT R3, off3, scale, uv.xyxy;
MAD_SAT R4, off4, scale, uv.xyxy;

TEX 	T0, R0.xyxy, texture[0], 2D;
TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;
TEX 	T8, R4.zwzw, texture[0], 2D;

MUL 	color, T0, d0;
MAD 	color, T1, d1, color;
MAD 	color, T2, d2, color;
MAD 	color, T3, d3, color;
MAD 	color, T4, d4, color;
MAD 	color, T5, d5, color;
MAD 	color, T6, d6, color;
MAD 	color, T7, d7, color;
MAD 	color, T8, d8, color;

MOV 	oColor.xyz, color;

END