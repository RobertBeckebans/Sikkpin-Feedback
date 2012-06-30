###==============================================================================
#	Feedback Screen Wipe Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0 = _currentRender
# texture 1 = normal map that we will use to deform texture 0
#
# local[0]	= vector scale
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

PARAM	scale	= program.local[0];

TEMP	uv, normalVec;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, nonPoT.zwzw;

TEX 	normalVec, TC, texture[1], 2D;
MAD 	uv.y, normalVec, scale, uv;
TEX		oColor.xyz, uv, texture[0], 2D;

END