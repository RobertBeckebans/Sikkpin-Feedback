###==============================================================================
#	Feedback Alpha Saturate Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= material specific texture
#
# local[0]	= RGBA
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

PARAM	RGBA	= program.local[0];

TEMP	R0;


TEX		R0, TC, texture[0], 2D;
MUL 	R0.xyz, R0, RGBA;
ADD		R0.w, R0.w, RGBA.w;
SGE 	R0.w, R0.w, 1.0;
MOV 	oColor, R0;

END