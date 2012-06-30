###==============================================================================
#	Feedback Blur Radial Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
#
# local[0]	= offset scale (w = holds whether we're working with a cropped or full res texture)
# local[1]	= screen-space location of center
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

PARAM	Scale	= program.local[0];
PARAM	Center	= program.local[1];

# linear distribution
# PARAM	d0		= 0.20000000;
# PARAM	d1		= 0.17777778;
# PARAM	d2		= 0.15555556;
# PARAM	d3		= 0.13333333;
# PARAM	d4		= 0.11111111;
# PARAM	d5		= 0.08888889;
# PARAM	d6		= 0.06666667;
# PARAM	d7		= 0.04444444;
# PARAM	d8		= 0.02222222;

# mean distribution
PARAM	d0		= 0.11111111;
PARAM	d1		= 0.11111111;
PARAM	d2		= 0.11111111;
PARAM	d3		= 0.11111111;
PARAM	d4		= 0.11111111;
PARAM	d5		= 0.11111111;
PARAM	d6		= 0.11111111;
PARAM	d7		= 0.11111111;
PARAM	d8		= 0.11111111;

PARAM	const	= 1.0;

TEMP 	color, center, scale;
TEMP	R0, R1, R2, R3, R4, R5, R6, R7, R8;


# calculate the screen texcoord in the 0.0 to 1.0 range
# MUL 	R0, fPos, invRes;

# scale by the screen non-power-of-two-adjust
# MUL 	R0, R0, nonPoT;

# scale by the screen non-power-of-two-adjust
MUL 	R0, TC, nonPoT;
CMP 	R0, Scale.w, R0, TC;

# load the screen render
TEX 	color, R0, texture[0], 2D;

MUL 	center, Center, nonPoT;
ADD 	R0, R0, -center;
# ADD 	R0, R0, -Center;

MUL 	scale, Scale, invRes;

MOV		R1, const;
ADD		R2, R1, -scale;
ADD		R3, R2, -scale;
ADD		R4, R3, -scale;
ADD		R5, R4, -scale;
ADD		R6, R5, -scale;
ADD		R7, R6, -scale;
ADD		R8, R7, -scale;

MAD_SAT R1, R1, R0, center;
MAD_SAT R2, R2, R0, center;
MAD_SAT R3, R3, R0, center;
MAD_SAT R4, R4, R0, center;
MAD_SAT R5, R5, R0, center;
MAD_SAT R6, R6, R0, center;
MAD_SAT R7, R7, R0, center;
MAD_SAT R8, R8, R0, center;

TEX		R1, R1, texture[0], 2D;
TEX		R2, R2, texture[0], 2D;
TEX		R3, R3, texture[0], 2D;
TEX		R4, R4, texture[0], 2D;
TEX		R5, R5, texture[0], 2D;
TEX		R6, R6, texture[0], 2D;
TEX		R7, R7, texture[0], 2D;
TEX		R8, R8, texture[0], 2D;

MUL 	color, color, d0;
MAD 	color, R1, d1, color;
MAD 	color, R2, d2, color;
MAD 	color, R3, d3, color;
MAD 	color, R4, d4, color;
MAD 	color, R5, d5, color;
MAD 	color, R6, d6, color;
MAD 	color, R7, d7, color;
MAD 	color, R8, d8, color;

MOV 	oColor.xyz, color;

END