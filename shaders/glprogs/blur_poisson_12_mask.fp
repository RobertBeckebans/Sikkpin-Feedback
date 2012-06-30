###==============================================================================
#	Feedback Blur Poisson 12 Mask Fragment Program 
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

OUTPUT 	oColor 	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

PARAM	Scale	= program.local[0];

PARAM	off01	= { -0.326212, -0.405805, -0.840144, -0.073580 };
PARAM	off02	= { -0.695914,  0.457137, -0.203345,  0.620716 };
PARAM	off03	= {  0.962340, -0.194983,  0.473434, -0.480026 };
PARAM	off04	= {  0.519456,  0.767022,  0.185461, -0.893124 };
PARAM	off05	= {  0.507431,  0.064425,  0.896420,  0.412458 };
PARAM	off06	= { -0.321940, -0.932615, -0.791559, -0.597705 };

PARAM	inv13	= 0.076923077;

TEMP	uv, invres, color, mask, scale;
TEMP 	R0, R1, R2, R3, R4, R5, R6;
TEMP 	T0, T1, T2, T3, T4, T5, T6;


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

# load the screen render
TEX 	color, uv, texture[0], 2D;

MAD_SAT R1, off01, scale, uv.xyxy;
MAD_SAT R2, off02, scale, uv.xyxy;
MAD_SAT R3, off03, scale, uv.xyxy;
MAD_SAT R4, off04, scale, uv.xyxy;
MAD_SAT R5, off05, scale, uv.xyxy;
MAD_SAT R6, off06, scale, uv.xyxy;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;

ADD		color, color, T1;
ADD		color, color, T2;
ADD		color, color, T3;
ADD		color, color, T4;
ADD		color, color, T5;
ADD		color, color, T6;

TEX 	T1, R4.xyxy, texture[0], 2D;
TEX 	T2, R4.zwzw, texture[0], 2D;
TEX 	T3, R5.xyxy, texture[0], 2D;
TEX 	T4, R5.zwzw, texture[0], 2D;
TEX 	T5, R6.xyxy, texture[0], 2D;
TEX 	T6, R6.zwzw, texture[0], 2D;

ADD		color, color, T1;
ADD		color, color, T2;
ADD		color, color, T3;
ADD		color, color, T4;
ADD		color, color, T5;
ADD		color, color, T6;

MUL 	oColor.xyz, color, inv13;

END