###==============================================================================
#	Feedback HDR Lum Average Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= scene luminance
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0
OPTION	ARB_precision_hint_nicest;

OUTPUT	oColor		= result.color;

ATTRIB	fPos		= fragment.position;
ATTRIB 	TC			= fragment.texcoord[0];

PARAM 	nonPoT		= program.env[0];
PARAM 	invRes		= program.env[1];

# sample offsets
PARAM	off1		= {  0.0,  0.5,  0.5,  0.0 };
PARAM	off2		= {  0.0, -0.5, -0.5,  0.0 };
PARAM 	off3		= { -0.5, -0.5, -0.5,  0.5 };
PARAM 	off4		= {  0.5, -0.5,  0.5,  0.5 };

PARAM	dcodeHDR	= { 2.0, 8.0 };
PARAM	ncodeHDR	= { 0.125, 0.5, 0.5 };
PARAM	dcode24		= { 1.0, 0.003921568627450980392156862745098, 0.000015259021896696421759365224689097 };
PARAM	ncode24 	= { 1.0, 255.0, 65535.0, 0.003921568627450980392156862745098 };
PARAM 	inv9		= 0.11111111111111111111111111111111;

TEMP 	uv, lumAvg;
TEMP	R0, R1, R2, R3, R4;
TEMP 	T0, T1, T2, T3, T4, T5, T6, T7, T8;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

MAD 	R1, off1, invRes.xyxy, uv;
MAD 	R2, off2, invRes.xyxy, uv;
MAD 	R3, off3, invRes.xyxy, uv;
MAD 	R4, off4, invRes.xyxy, uv;

TEX 	T0.x, uv.xyxy, texture[0], 2D;
TEX 	T0.y, R1.xyxy, texture[0], 2D;
TEX 	T0.z, R1.zwzw, texture[0], 2D;
TEX 	T1.x, R2.xyxy, texture[0], 2D;
TEX 	T1.y, R2.zwzw, texture[0], 2D;
TEX 	T1.z, R3.xyxy, texture[0], 2D;
TEX 	T2.x, R3.zwzw, texture[0], 2D;
TEX 	T2.y, R4.xyxy, texture[0], 2D;
TEX 	T2.z, R4.zwzw, texture[0], 2D;

DP3 	lumAvg.x, T0, inv9;
DP3 	lumAvg.y, T1, inv9;
DP3 	lumAvg.z, T2, inv9;
DP3 	oColor.xyz, lumAvg, 1.0;

END