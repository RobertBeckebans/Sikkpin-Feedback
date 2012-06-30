###==============================================================================
#	Feedback Blur Gaussian 16 Mask Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
# texture 1	= mask
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

# Gaussian distribution in 1D, standard deviation = 3
PARAM	d0		= 0.13298076;
PARAM	d1		= 0.12579441;
PARAM	d2		= 0.10648267;
PARAM	d3		= 0.08065691;
PARAM	d4		= 0.05467002;
PARAM	d5		= 0.03315905;
PARAM	d6		= 0.01799699;
PARAM	d7		= 0.00874063;
PARAM	d8		= 0.00379866;

# linear distribution
# PARAM	d0		= 0.111111111;
# PARAM	d1		= 0.098765432;
# PARAM	d2		= 0.086419753;
# PARAM	d3		= 0.074074074;
# PARAM	d4		= 0.061728395;
# PARAM	d5		= 0.049382716;
# PARAM	d6		= 0.037037037;
# PARAM	d7		= 0.024691358;
# PARAM	d8		= 0.012345679;

# sample offsets
PARAM 	off1	= { 1.0, 1.0, 2.0, 2.0 };
PARAM 	off2	= { 3.0, 3.0, 4.0, 4.0 };
PARAM 	off3	= { 5.0, 5.0, 6.0, 6.0 };
PARAM 	off4	= { 7.0, 7.0, 8.0, 8.0 };


TEMP	uv, invres, color, mask, scale;
TEMP 	R0, R1, R2, R3, R4;
TEMP 	T0, T1, T2, T3, T4, T5, T6, T7, T8;


# calculate the non-power-of-two inverse resolution
MUL 	invres, invRes.xyxy, nonPoT.xyxy;
CMP 	invres, Scale.w, invres, invRes.xyxy;
MOV 	invres.zw, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

# sample mask texture
TEX 	mask, uv.zwzw, texture[1], 2D;

# multiply the offset scale by the mask
MUL 	scale, Scale.xyxy, invres.xyxy;
MUL 	scale, scale, mask.x;

MAD_SAT R1, off1, scale, uv.xyxy;
MAD_SAT R2, off2, scale, uv.xyxy;
MAD_SAT R3, off3, scale, uv.xyxy;
MAD_SAT R4, off4, scale, uv.xyxy;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;
TEX 	T8, R4.zwzw, texture[0], 2D;

MUL 	color, T1, d1;
MAD 	color, T2, d2, color;
MAD 	color, T3, d3, color;
MAD 	color, T4, d4, color;
MAD 	color, T5, d5, color;
MAD 	color, T6, d6, color;
MAD 	color, T7, d7, color;
MAD 	color, T8, d8, color;

MAD_SAT R1, -off1, scale, uv.xyxy;
MAD_SAT R2, -off2, scale, uv.xyxy;
MAD_SAT R3, -off3, scale, uv.xyxy;
MAD_SAT R4, -off4, scale, uv.xyxy;

TEX 	T0, uv.xyxy, texture[0], 2D;
TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;
TEX 	T8, R4.zwzw, texture[0], 2D;

MAD 	color, T0, d0, color;
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