###==============================================================================
#	Feedback Bloom Blend Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
# texture 1	= _bloom
#
# local[0]	= bloom scale
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

PARAM 	Scale	= program.local[0];

TEMP	uv, invres, color, bloom, R0;


# calculate the non-power-of-two inverse resolution
MOV 	invres, nonPoT.zwzw;
MOV 	invres.zw, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

# sample textures
TEX		color, uv.xyxy, texture[0], 2D;
TEX		bloom, uv.zwzw, texture[1], 2D;

MUL 	bloom, bloom, Scale;
LRP		oColor.xyz, color, 1.0, bloom;

END