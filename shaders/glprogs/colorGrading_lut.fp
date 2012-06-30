###==============================================================================
#	Feedback Color Grading LUT Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
# texture 1	= _blurRender
#
# local[0]	= Unsharp Mask level
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0 
OPTION	ARB_precision_hint_fastest;

OUTPUT 	oColor 		= result.color;

ATTRIB	fPos		= fragment.position;
ATTRIB	TC			= fragment.texcoord[0];

PARAM 	nonPoT		= program.env[0];
PARAM 	invRes		= program.env[1];

PARAM	sharpness	= program.local[0];

PARAM 	halfTexel	= { 0.001953125, 0.03125 };
PARAM 	invWidth	= 0.0625;

TEMP	uv, invres, color, mask;
TEMP 	R0, R1, R2;


# calculate the non-power-of-two inverse resolution
MOV 	invres, nonPoT.zwzw;
MOV 	invres.zw, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

# load screen texture
TEX 	color, uv, texture[0], 2D;

#====================================
# unsharp mask
TEX 	mask, uv.zwzw, texture[1], 2D;
LRP_SAT	color, sharpness.x, color, mask;

#====================================
# lut color correction
MUL 	R0, color, 0.9375;
MUL 	R0.w, color.z, 15.0;
FLR 	R0.z, R0.w;
FRC 	R0.w, R0.w;
MUL 	R0.z, R0.z, invWidth;

MAD 	R0.x, R0.x, invWidth, R0.z;
ADD 	R0.xy, R0, halfTexel;
TEX 	R1, R0, texture[2], 2D;
ADD 	R0.x, R0.x, invWidth;
TEX 	R2, R0, texture[2], 2D;

LRP 	color, R0.w, R2, R1;

#====================================
# output
MOV 	oColor.xyz, color;

END