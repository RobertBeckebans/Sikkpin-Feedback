###==============================================================================
#	Feedback Color Process Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
# 
# local[0]	= RGB = target color, A = fraction from the source color to the target color to map
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

PARAM 	blendColor	= program.local[0];

PARAM	lumVec 		= { 0.212671, 0.715160, 0.072169 };

TEMP	uv, R0, R1;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, nonPoT.zwzw;

# load the screen render
TEX		R0, uv, texture[0], 2D;

# calculate the grey scale version of the color
DP3 	R1, R0, lumVec;

# scale by the target color
MUL		R1, R1, blendColor;

# lerp between the source color and the target color
LRP 	oColor.xyz, blendColor.w, R1, R0;

END