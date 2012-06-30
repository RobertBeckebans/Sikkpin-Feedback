###==============================================================================
#	Feedback HDR Lum Base Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0 
OPTION	ARB_precision_hint_nicest;

OUTPUT 	oColor 		= result.color;

ATTRIB	fPos		= fragment.position;
ATTRIB	TC			= fragment.texcoord[0];

PARAM 	nonPoT		= program.env[0];
PARAM 	invRes		= program.env[1];

PARAM	dcodeHDR	= { 16.0, 2.0 };
PARAM	ncodeHDR	= { 0.5, 0.0625 };
PARAM	ncode24		= { 1.0, 255.0, 65535.0, 0.003921568627450980392156862745098 };
PARAM	lumVec		= { 0.212671, 0.715160, 0.072169 };
PARAM 	e			= 2.7182818284590452353602874713527;

TEMP	uv, color, lum, R0;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

# grab texture
TEX		color, uv, texture[0], 2D;

# convert RGB to luminance
DP3		lum.x, color, lumVec;

# log luminance
ADD 	lum.x, lum.x, 1.0;
LG2 	lum.x, lum.x;

MOV 	oColor.xyz, lum.x;

END