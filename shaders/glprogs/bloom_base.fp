###==============================================================================
#	Feedback Bloom Base Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
#
# local[0]	= Gamma curve
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

PARAM	Gamma	= program.local[0];

PARAM	lumVec	= { 0.212671, 0.715160, 0.072169 };

TEMP	uv, color;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

TEX		color, uv, texture[0], 2D;

DP3 	color.w, color, lumVec;
POW		color.w, color.w, Gamma.x;
MUL		oColor.xyz, color, color.w;

END