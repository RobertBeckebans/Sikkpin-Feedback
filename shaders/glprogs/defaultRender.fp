###==============================================================================
#	Feedback Default Render Fragment Program 
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
OPTION	ARB_precision_hint_fastest;

OUTPUT 	oColor 	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB 	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

TEMP 	uv;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, nonPoT.zwzw;

TEX 	oColor.xyz, uv, texture[0], 2D;
# MOV 	oColor.w, 1.0;

END