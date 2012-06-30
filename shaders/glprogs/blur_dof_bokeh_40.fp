###==============================================================================
#	Feedback Blur DoF Bokeh 40 Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
# texture 1	= mask
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

PARAM	radOff	= { 0.0, 0.78539816339744830961566084581988, 1.57079632679489661923132169156, 2.356194490192344928846982537457 };	# 45 deg
PARAM	DEG2RAD	= 0.017453292519943295769236907684886;
PARAM 	PI		= 3.1415926535897932384626433832795;
PARAM 	deg22_5	= 0.39269908169872415480783042289;

PARAM 	randUV	= 0.0625;	# 0.25 = 4x4; 0.0625 = 16x16; 0.015625 = 64x64

TEMP	uv, invres, color, mask, dither, scale, angle, aspect, maxUV, radius;
TEMP 	R0, R1, R2, R3, R4, R5, R6, R7, R8;
TEMP 	T0, T1, T2, T3, T4, T5, T6, T7, T8;


# calculate the non-power-of-two inverse resolution
MUL 	invres, invRes.xyxy, nonPoT.xyxy;
CMP 	invres, Scale.w, invres, invRes.xyxy;
MOV 	invres.zw, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

# sample mask texture
TEX 	mask, uv.zwzw, texture[1], 2D;

# load random vector map
MUL 	dither.xy, fPos, randUV;
TEX 	dither, dither, texture[2], 2D;
MAD 	R0, dither, 0.5, 0.5;

# multiply the offset scale by the mask
MUL 	scale, Scale.xyxy, invres.xyxy;
MUL 	scale, scale, mask.x;
# MUL 	scale, scale, R0;
MOV 	scale.zw, -scale.xyxy;
MOV 	radius, scale;

# calculate starting angle
MUL 	angle.xyzw, dither, radOff.y;

ADD 	T1, angle.x, radOff;
SCS 	R1, T1.x;
SCS 	R2, T1.y;
SCS 	R3, T1.z;
SCS 	R4, T1.w;
MAD 	R1, R1.xyxy, radius, uv.xyxy;
MAD 	R2, R2.xyxy, radius, uv.xyxy;
MAD 	R3, R3.xyxy, radius, uv.xyxy;
MAD 	R4, R4.xyxy, radius, uv.xyxy;

# TEX 	T0, R0.xyxy, texture[0], 2D;
TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;
TEX 	T8, R4.zwzw, texture[0], 2D;

ADD		T0, T1, T2;
ADD		T0, T0, T3;
ADD		T0, T0, T4;
ADD		T0, T0, T5;
ADD		T0, T0, T6;
ADD		T0, T0, T7;
ADD		T0, T0, T8;
MUL		color, T0, 0.25;

ADD 	radius, scale, scale;
ADD 	T1, angle.y, radOff;
SCS 	R1, T1.x;
SCS 	R2, T1.y;
SCS 	R3, T1.z;
SCS 	R4, T1.w;
MAD 	R1, R1.xyxy, radius, uv.xyxy;
MAD 	R2, R2.xyxy, radius, uv.xyxy;
MAD 	R3, R3.xyxy, radius, uv.xyxy;
MAD 	R4, R4.xyxy, radius, uv.xyxy;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;
TEX 	T8, R4.zwzw, texture[0], 2D;

ADD		T0, T1, T2;
ADD		T0, T0, T3;
ADD		T0, T0, T4;
ADD		T0, T0, T5;
ADD		T0, T0, T6;
ADD		T0, T0, T7;
ADD		T0, T0, T8;
MAD		color, T0, 0.75, color;

MUL 	radius, scale, 3.0;
ADD 	T1, angle.z, radOff;
SCS 	R1, T1.x;
SCS 	R2, T1.y;
SCS 	R3, T1.z;
SCS 	R4, T1.w;
MAD 	R1, R1.xyxy, radius, uv.xyxy;
MAD 	R2, R2.xyxy, radius, uv.xyxy;
MAD 	R3, R3.xyxy, radius, uv.xyxy;
MAD 	R4, R4.xyxy, radius, uv.xyxy;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;
TEX 	T8, R4.zwzw, texture[0], 2D;

ADD		T0, T1, T2;
ADD		T0, T0, T3;
ADD		T0, T0, T4;
ADD		T0, T0, T5;
ADD		T0, T0, T6;
ADD		T0, T0, T7;
ADD		T0, T0, T8;
ADD 	color, color, T0;

MUL 	radius, scale, 4.0;
ADD 	T1, angle.w, radOff;
ADD 	T2, T1, deg22_5;
SCS 	R1, T1.x;
SCS 	R2, T1.y;
SCS 	R3, T1.z;
SCS 	R4, T1.w;
SCS 	R5, T2.x;
SCS 	R6, T2.y;
SCS 	R7, T2.z;
SCS 	R8, T2.w;
MAD 	R1, R1.xyxy, radius, uv.xyxy;
MAD 	R2, R2.xyxy, radius, uv.xyxy;
MAD 	R3, R3.xyxy, radius, uv.xyxy;
MAD 	R4, R4.xyxy, radius, uv.xyxy;
MAD 	R5, R5.xyxy, radius, uv.xyxy;
MAD 	R6, R6.xyxy, radius, uv.xyxy;
MAD 	R7, R7.xyxy, radius, uv.xyxy;
MAD 	R8, R8.xyxy, radius, uv.xyxy;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;
TEX 	T7, R4.xyxy, texture[0], 2D;
TEX 	T8, R4.zwzw, texture[0], 2D;

ADD		T0, T1, T2;
ADD		T0, T0, T3;
ADD		T0, T0, T4;
ADD		T0, T0, T5;
ADD		T0, T0, T6;
ADD		T0, T0, T7;
ADD		T0, T0, T8;

TEX 	T1, R5.xyxy, texture[0], 2D;
TEX 	T2, R5.zwzw, texture[0], 2D;
TEX 	T3, R6.xyxy, texture[0], 2D;
TEX 	T4, R6.zwzw, texture[0], 2D;
TEX 	T5, R7.xyxy, texture[0], 2D;
TEX 	T6, R7.zwzw, texture[0], 2D;
TEX 	T7, R8.xyxy, texture[0], 2D;
TEX 	T8, R8.zwzw, texture[0], 2D;

ADD		T0, T0, T1;
ADD		T0, T0, T2;
ADD		T0, T0, T3;
ADD		T0, T0, T4;
ADD		T0, T0, T5;
ADD		T0, T0, T6;
ADD		T0, T0, T7;
ADD		T0, T0, T8;

MAD		color, T0, 2.0, color;

MUL 	oColor.xyz, color, 0.020833333333333333333333333333333;

END