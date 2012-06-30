###==============================================================================
#	Feedback HDR Glare Blend Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _hdrBloom
# texture 1	= _hdrGlare
# texture 2	= _hdrFlare
#
# local[0]	= Intensity scale
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

PARAM 	Scale	= program.local[0];

TEMP 	uv, color, R0, R1, R2, R3;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

TEX 	R1, uv, texture[0], 2D;
TEX 	R2, uv, texture[1], 2D;
TEX 	R3, uv, texture[2], 2D;

MUL 	R1.xyz, R1, Scale.x;
MUL 	R2.xyz, R2, Scale.y;
MUL 	R3.xyz, R3, Scale.z;

ADD 	R0, 1.0, -R2;
MAD 	color, R1, R0, R2;
ADD 	R0, 1.0, -R3;
MAD 	color, color, R0, R3;

MOV 	oColor.xyz, color;

END