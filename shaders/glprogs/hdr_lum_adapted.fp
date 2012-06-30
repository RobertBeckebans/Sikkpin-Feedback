###==============================================================================
#	Feedback HDR Lum Adapted Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _hdrLumAvg	(current scene luminance)
# texture 1	= _hdrLumAdpt	(previous adapted scene luminance)
#
# local[0]	= adaptation rate, elapsed time, min/max luminance threshold
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

PARAM	adaptParm	= program.local[0];

PARAM	lumTC		= { 0.5, 0.5 };
PARAM	dcode24		= { 1.0, 0.003921568627450980392156862745098, 0.000015259021896696421759365224689097 };
PARAM	ncode24 	= { 1.0, 255.0, 65535.0, 0.003921568627450980392156862745098 };
PARAM 	e			= 2.7182818284590452353602874713527;

TEMP	lumAvg, lumAdpt, R0;


TEX		lumAvg, lumTC, texture[0], 2D;
TEX		lumAdpt, lumTC, texture[1], 2D;

# decode 24-bit luminance
# DP3 	lumAvg.x, lumAvg, dcode24;
DP3 	lumAdpt.x, lumAdpt, dcode24;

# exponentiate log2 luminance
# EX2 	lumAvg.x, lumAvg.x;
# ADD 	lumAvg.x, lumAvg.x, -1.0;

# calculate adapted luminance
ADD 	R0.x, lumAvg.x, -lumAdpt.x;
MUL 	R0.y, adaptParm.x, adaptParm.y;
POW 	R0.y, 0.98.x, R0.y;
ADD 	R0.y, 1.0, -R0.y;
MAD 	lumAdpt.x, R0.x, R0.y, lumAdpt.x;

# clamp luminance to desired limits
MIN 	lumAdpt.x, lumAdpt.x, adaptParm.w;
MAX 	lumAdpt.x, lumAdpt.x, adaptParm.z;

# encode 24-bit luminance
MUL 	lumAdpt.w, lumAdpt.x, ncode24.y;
FLR 	lumAdpt.x, lumAdpt.w;
FRC 	lumAdpt.w, lumAdpt.w;
MUL 	lumAdpt.w, lumAdpt.w, ncode24.y;
FLR 	lumAdpt.y, lumAdpt.w;
FRC 	lumAdpt.w, lumAdpt.w;
MUL 	lumAdpt.w, lumAdpt.w, ncode24.y;
FLR 	lumAdpt.z, lumAdpt.w;
MUL 	lumAdpt.xyz, lumAdpt, ncode24.w;

MOV		oColor.xyz, lumAdpt;

END