###==============================================================================
#	Feedback HDR Glare Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _hdrGlare*
#
# local[0]	= offset scale
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

PARAM 	Scale	= program.local[0];

PARAM	d0		= 0.125;
PARAM	d1		= 0.125;

# chromatic abberation disribution
PARAM	ca1		= { 0.5, 0.5, 0.5 };
PARAM	ca2		= { 0.8, 0.3, 0.3 };
PARAM	ca3		= { 1.0, 0.2, 0.2 };
PARAM	ca4		= { 0.5, 0.2, 0.8 };
PARAM	ca5		= { 0.2, 0.2, 1.0 };
PARAM	ca6		= { 0.2, 0.3, 0.9 };
PARAM	ca7		= { 0.2, 0.9, 0.2 };
PARAM	ca8		= { 0.3, 0.5, 0.3 };


# sample offsets
PARAM 	off1	= { 1.0, 1.0, 2.0, 2.0 };
PARAM 	off2	= { 3.0, 3.0, 4.0, 4.0 };
PARAM 	off3	= { 5.0, 5.0, 6.0, 6.0 };
PARAM 	off4	= { 7.0, 7.0, 8.0, 8.0 };

TEMP	uv, color1, color2, scale, weight1, weight2;
TEMP 	D0, D1, D2, D3, D4, D5, D6, D7, D8;
TEMP 	R0, R1, R2, R3, R4;
TEMP 	S0, S1, S2, S3, S4;
TEMP 	T0, T1, T2, T3, T4, T5, T6, T7, T8;
TEMP 	minUV, maxUV;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

MOV 	minUV, invRes.xyxy;
ADD 	maxUV, 1.0, -invRes.xyxy;

MUL 	scale, Scale.xyxy, invRes.xyxy;
MUL 	scale, scale, Scale.z;

# calculate weights
ADD 	R1, 4, -Scale.z;
MUL_SAT weight1, d0, R1;
MUL_SAT weight2, d1, R1;

MUL 	D1, weight1.x, ca1;
MUL 	D2, weight1.y, ca2;
MUL 	D3, weight1.z, ca3;
MUL 	D4, weight1.w, ca4;
MUL 	D5, weight2.x, ca5;
MUL 	D6, weight2.y, ca6;
MUL 	D7, weight2.z, ca7;
MUL 	D8, weight2.w, ca8;

MAD_SAT R1, off1, scale, uv;
MAD_SAT R2, off2, scale, uv;
MAD_SAT R3, off3, scale, uv;
MAD_SAT R4, off4, scale, uv;
MIN 	R1, R1, maxUV;
MIN 	R2, R2, maxUV;
MIN 	R3, R3, maxUV;
MIN 	R4, R4, maxUV;
MAX 	R1, R1, minUV;
MAX 	R2, R2, minUV;
MAX 	R3, R3, minUV;
MAX 	R4, R4, minUV;

TEX 	T0, uv.xyxy, texture[0], 2D;
TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;

MUL 	color2, T0, D1;
MAD 	color1, T1, D2, color2;
MAD 	color1, T2, D3, color1;
MAD 	color1, T3, D4, color1;
MAD 	color1, T4, D5, color1;
MAD 	color1, T5, D6, color1;
MAD 	color1, T6, D7, color1;
MAD 	color1, T7, D8, color1;

MAD_SAT R1, -off1, scale, uv;
MAD_SAT R2, -off2, scale, uv;
MAD_SAT R3, -off3, scale, uv;
MAD_SAT R4, -off4, scale, uv;
MIN 	R1, R1, maxUV;
MIN 	R2, R2, maxUV;
MIN 	R3, R3, maxUV;
MIN 	R4, R4, maxUV;
MAX 	R1, R1, minUV;
MAX 	R2, R2, minUV;
MAX 	R3, R3, minUV;
MAX 	R4, R4, minUV;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;

MAD 	color2, T1, D2, color2;
MAD 	color2, T2, D3, color2;
MAD 	color2, T3, D4, color2;
MAD 	color2, T4, D5, color2;
MAD 	color2, T5, D6, color2;
MAD 	color2, T6, D7, color2;
MAD 	color2, T7, D8, color2;

LRP 	oColor.xyz, color1, 1.0, color2;

END