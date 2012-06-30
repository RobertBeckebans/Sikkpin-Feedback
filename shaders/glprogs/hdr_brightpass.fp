###==============================================================================
#	Feedback HDR Brightpass Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
# texture 1	= _hdrLumAdpt (adapted luminance)
#
# local[0]	= middle gray
# local[1]	= white point
# local[2]	= lum threshold
# local[3]	= lum offset
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

PARAM	middleGray	= program.local[0];
PARAM	whitePoint	= program.local[1];
PARAM	lumThresh	= program.local[2];
PARAM	lumOffset	= program.local[3];

PARAM 	lumTC		= { 0.5, 0.5 };
PARAM	lumVec	 	= { 0.212671, 0.715160, 0.072169 };
PARAM	dcode24		= { 1.0, 0.003921568627450980392156862745098, 0.000015259021896696421759365224689097 };
PARAM 	e			= 2.7182818284590452353602874713527;

TEMP	uv, color, lumAdpt, R0, R1;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

# sample textures
TEX 	color, uv, texture[0], 2D;
TEX 	lumAdpt, lumTC, texture[1], 2D;

DP3		color.w, color, lumVec;
MUL 	R0.x, color.w, color.w;
POW 	R0.y, color.w, 0.5.x;
LRP 	color.w, color.w, R0.x, R0.y;
MAD		color.w, color.w, 15.0, 1.0;
MUL 	color, color, color.w;

# decode HDR
# ADD 	color, 1.0, -color;
# MAD 	color, color, -color, 2.0;
# MAD 	color, color, color, -1.0;
# EX2 	color.x, color.x;
# EX2 	color.y, color.y;
# EX2 	color.z, color.z;
# ADD 	color, color, -1.0;

# decode 24-bit luminance
DP3 	lumAdpt.x, lumAdpt, dcode24;
# decode HDR
# ADD 	lumAdpt.x, 1.0, -lumAdpt.x;
# MAD 	lumAdpt.x, lumAdpt.x, -lumAdpt.x, 2.0;
# MAD 	lumAdpt.x, lumAdpt.x, lumAdpt.x, -1.0;
EX2 	lumAdpt.x, lumAdpt.x;
ADD 	lumAdpt.x, lumAdpt.x, -1.0;
MUL 	lumAdpt.x, lumAdpt.x, 5.0;

# tonemap scene and filter out dark pixels
ADD 	R0.w, lumAdpt.x, 0.001;
RCP 	R0.w, R0.w;
MUL 	R0.w, middleGray.x, R0.w;
MUL 	R0, color, R0.w;
MAD 	R1, R0, whitePoint, 1.0;
MAD 	R0, R0, R1, -lumThresh;
MAX 	R0, R0, 0.0;
ADD 	R0, R0, -lumOffset;
POW 	color.x, e.x, -R0.x;
POW 	color.y, e.x, -R0.y;
POW 	color.z, e.x, -R0.z;
ADD 	color, 1.0, -color;

MOV 	oColor.xyz, color;

END