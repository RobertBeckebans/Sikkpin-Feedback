###==============================================================================
#	Feedback HDR Flare Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _hdrFlare (hdr flare texture)
#
# local[0]	= flare offsets
# local[1]	= flare Gamma exponent
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

PARAM 	Offset	= program.local[0];
PARAM 	Gamma	= program.local[1];

PARAM	Center	= 0.5;
PARAM	lumVec	= { 0.212671, 0.715160, 0.072169 };

TEMP	uv, color, dist, dir, scale, aspect, tc;
TEMP 	R0, R1, R2;
TEMP 	S0, S1, S2;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

MAD 	dist, uv, { 2.0, 2.0, 0.0 }, { -1.0, -1.0, 0.0 };
DP3 	dist.w, dist, dist;
RSQ 	dist.w, dist.w;
MUL 	dir.xy, dist, dist.w;
RCP 	dist.w, dist.w;
MUL 	dir, dir.xyxy, dist.w;

MAD 	R1, dir, Offset.x, uv.xyxy;
MAD 	R2, dir, Offset.y, uv.xyxy;
SLT 	S1.xy, R1.xyxy, 0.0;
SLT 	S2.xy, R2.xyxy, 0.0;
SLT 	S1.zw, 1.0, R1.xyxy;
SLT 	S2.zw, 1.0, R2.xyxy;
DP4 	S1.w, S1, 1.0;
DP4 	S2.w, S2, 1.0;

TEX 	R0.xyz, uv, texture[0], 2D;
TEX 	R1.xyz, R1, texture[0], 2D;
TEX 	R2.xyz, R2, texture[0], 2D;

CMP 	R1, -S1.w, 0.0, R1;
CMP 	R2, -S2.w, 0.0, R2;

LRP 	color, R0, 1.0, R1;
LRP 	color, color, 1.0, R2;

DP3 	color.w, color, lumVec;
POW 	color.w, color.w, Gamma.x;
MUL 	oColor.xyz, color, color.w;

END