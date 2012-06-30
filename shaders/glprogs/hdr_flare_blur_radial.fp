###==============================================================================
#	Feedback HDR Flare Blur Radial Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _hdrFlare (hdr flare texture)
# 
# local[0]	= offset scale
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

# chromatic abberation disribution
PARAM	ca1		= { 1.0, 0.0, 0.5 };
PARAM	ca2		= { 1.0, 0.0, 1.0 };
PARAM	ca3		= { 0.5, 0.0, 1.0 };
PARAM	ca4		= { 0.0, 0.0, 1.0 };
PARAM	ca5		= { 0.0, 0.5, 1.0 };
PARAM	ca6		= { 0.0, 1.0, 1.0 };
PARAM	ca7		= { 0.0, 1.0, 0.5 };
PARAM	ca8		= { 0.0, 1.0, 0.0 };
PARAM	ca9		= { 0.5, 1.0, 0.0 };
PARAM	ca10	= { 1.0, 1.0, 0.0 };
PARAM	ca11	= { 1.0, 0.5, 0.0 };
PARAM	ca12	= { 1.0, 0.0, 0.0 };

PARAM	Center	= 0.5;
PARAM 	inv12 	= 0.08333333333333333333333333333333;

TEMP 	uv, color, sign, scale, dist, dir, aspect;
TEMP	R0, R1, R2, R3, R4, R5, R6;
TEMP	T0, T1, T2, T3, T4, T5, T6;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

MOV 	scale, invRes;
MOV 	scale.z, 0.0;
DP3 	scale.w, scale, scale;
RSQ 	scale.w, scale.w;
RCP 	scale, scale.w;
MUL 	scale, scale, Size;

ADD 	dir, uv, -Center;
MUL 	dir, dir.xyxy, scale.xyxy;

MAD 	R1, dir, { 6.0, 6.0, 5.0, 5.0 }, uv;
MAD 	R2, dir, { 4.0, 4.0, 3.0, 3.0 }, uv;
MAD 	R3, dir, { 2.0, 2.0, 1.0, 1.0 }, uv;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;

MUL 	color, T1, ca1;
MAD 	color, T2, ca2, color;
MAD 	color, T3, ca3, color;
MAD 	color, T4, ca4, color;
MAD 	color, T5, ca5, color;
MAD 	color, T6, ca6, color;

MAD 	R1, -dir, { 0.0, 0.0, 1.0, 1.0 }, uv;
MAD 	R2, -dir, { 2.0, 2.0, 3.0, 3.0 }, uv;
MAD 	R3, -dir, { 4.0, 4.0, 5.0, 5.0 }, uv;

TEX 	T1, R1.xyxy, texture[0], 2D;
TEX 	T2, R1.zwzw, texture[0], 2D;
TEX 	T3, R2.xyxy, texture[0], 2D;
TEX 	T4, R2.zwzw, texture[0], 2D;
TEX 	T5, R3.xyxy, texture[0], 2D;
TEX 	T6, R3.zwzw, texture[0], 2D;

MAD 	color, T1, ca7, color;
MAD 	color, T2, ca8, color;
MAD 	color, T3, ca9, color;
MAD 	color, T4, ca10, color;
MAD 	color, T5, ca11, color;
MAD 	color, T6, ca12, color;

MUL 	oColor.xyz, color, inv12;

END