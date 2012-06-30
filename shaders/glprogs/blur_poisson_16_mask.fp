###==============================================================================
#	Feedback Blur Poisson 16 Mask Fragment Program 
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
ATTRIB 	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

PARAM	Scale	= program.local[0];

PARAM	off1	= { -0.94201624, -0.39906216,  0.94558609, -0.76890725 };
PARAM	off2	= { -0.09418410, -0.92938870,  0.34495938,  0.29387760 };
PARAM	off3	= { -0.91588581,  0.45771432, -0.81544232, -0.87912464 };
PARAM	off4	= { -0.38277543,  0.27676845,  0.97484398,  0.75648379 };
PARAM	off5	= {  0.44323325, -0.97511554,  0.53742981, -0.47373420 };
PARAM	off6	= { -0.26496911, -0.41893023,  0.79197514,  0.19090188 };
PARAM	off7	= { -0.24188840,  0.99706507, -0.81409955,  0.91437590 };
PARAM	off8	= {  0.19974126,  0.78641367,  0.14383161, -0.14100790 };

PARAM	inv17	= 0.0588235294117647;

TEMP	uv, invres, color, mask, scale;
TEMP	R0, R1, R2, R3, R4, R5, R6, R7, R8;
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

# load the screen render
TEX 	color, uv, texture[0], 2D;

MAD_SAT R1, off1, scale, uv.xyxy;
MAD_SAT R2, off2, scale, uv.xyxy;
MAD_SAT R3, off3, scale, uv.xyxy;
MAD_SAT R4, off4, scale, uv.xyxy;
MAD_SAT R5, off5, scale, uv.xyxy;
MAD_SAT R6, off6, scale, uv.xyxy;
MAD_SAT R7, off7, scale, uv.xyxy;
MAD_SAT R8, off8, scale, uv.xyxy;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;
TEX 	T8, R4.zwzw, texture[0], 2D;

ADD		color, color, T1;
ADD		color, color, T2;
ADD		color, color, T3;
ADD		color, color, T4;
ADD		color, color, T5;
ADD		color, color, T6;
ADD		color, color, T7;
ADD		color, color, T8;

TEX 	T1, R5.xyxy, texture[0], 2D;
TEX 	T2, R5.zwzw, texture[0], 2D;
TEX 	T3, R6.xyxy, texture[0], 2D;
TEX 	T4, R6.zwzw, texture[0], 2D;
TEX 	T5, R7.xyxy, texture[0], 2D;
TEX 	T6, R7.zwzw, texture[0], 2D;
TEX 	T7, R8.xyxy, texture[0], 2D;
TEX 	T8, R8.zwzw, texture[0], 2D;

ADD		color, color, T1;
ADD		color, color, T2;
ADD		color, color, T3;
ADD		color, color, T4;
ADD		color, color, T5;
ADD		color, color, T6;
ADD		color, color, T7;
ADD		color, color, T8;

MUL 	oColor.xyz, color, inv17;

END