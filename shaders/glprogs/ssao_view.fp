###==============================================================================
#	Feedback SSAO View Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _ssao (xyz = depth (24-bit encoded), w = ssao)
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0
OPTION	ARB_precision_hint_fastest;

# texture 0	= x = _ssao (ssao buffer), yz = depth (16-bit ecoded)
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion

OUTPUT 	oColor 	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB 	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

TEMP 	R0;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	R0, fPos.xyxy, nonPoT;

# scale by the screen non-power-of-two-adjust
# MUL 	R0, R0, nonPoT;
# RCP 	R0.z, nonPoT.x;
# RCP 	R0.w, nonPoT.y;
# MUL 	R0, R0, R0.zwzw;

# scale by the screen non-power-of-two-adjust
# MUL 	R0, TC, nonPoT;

TEX 	R0, R0.zwzw, texture[0], 2D;

# MOV 	oColor.xyz, R0.w;
MOV 	oColor.xyz, R0;
# MOV 	oColor.z, 0;

END