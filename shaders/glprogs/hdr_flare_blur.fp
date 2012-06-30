###==============================================================================
#	Feedback HDR Flare Blur Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _hdrFlare (hdr flare texture)
# 
# local[0]	= offset scale
# local[1]	= flare scale factor
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0 
OPTION	ARB_precision_hint_nicest;

OUTPUT 	oColor 	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

PARAM	Size	= program.local[0];
PARAM	Scale	= program.local[1];

# Gaussian distribution in 1D, standard deviation = 3
PARAM	d0		= 0.13298076;
PARAM	d1		= 0.12579441;
PARAM	d2		= 0.10648267;
PARAM	d3		= 0.08065691;
PARAM	d4		= 0.05467002;
PARAM	d5		= 0.03315905;
PARAM	d6		= 0.01799699;
PARAM	d7		= 0.00874063;
PARAM	d8		= 0.00379866;

PARAM	Center	= 0.5;
PARAM 	pi		= 3.1415926535897932384626433832795;
PARAM 	halfpi	= 1.5707963267948966192313216916398;

TEMP 	uv, color, center, radius, angle, dist, aspect;
TEMP	R0, R1, R2, R3, R4, R5, R6, R7, R8;
TEMP 	D0, D1, D2, D3, D4, D5, D6, D7, D8;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

# calculate radius
ADD		dist, uv, -Center;
MOV 	dist.z, 0.0;
DP3 	radius.x, dist, dist;
RSQ 	radius.x, radius.x;
RCP 	radius.x, radius.x;

# calculate angle - atan2()
ABS 	R2, dist;
MAX 	R1.x, R2.x, R2.y;
MIN 	R1.y, R2.x, R2.y;
RCP 	R1.x, R1.x;
MUL 	R1.x, R1.x, R1.y;
MUL 	R1.y, R1.x, R1.x;
MAD 	R1.w, R1.y, -0.013480470, 0.057477314;
MAD 	R1.w, R1.w, R1.y, -0.121239071;
MAD 	R1.w, R1.w, R1.y,  0.195635925;
MAD 	R1.w, R1.w, R1.y, -0.332994597;
MAD 	R1.w, R1.w, R1.y, 0.999995630;
MUL 	R1.x, R1.w, R1.x;
ADD 	R2.w, R2.x, -R2.y;
ADD 	R2.x, halfpi, -R1.x;
CMP 	R1.x, R2.w, R2.x, R1.x;
ADD 	R2.x, pi, -R1.x;
CMP 	R1.x, dist.x, R2.x, R1.x;
CMP 	R1.x, dist.y, -R1.x, R1.x;
MOV 	angle.xy, R1.x;

# calculate offsets
ADD 	angle.x, angle.x, Size;
SCS 	R1, angle.x;
MAD 	R1, R1, radius.x, Center;
ADD 	angle.x, angle.x, Size;
SCS 	R2, angle.x;
MAD 	R2, R2, radius.x, Center;
ADD 	angle.x, angle.x, Size;
SCS 	R3, angle.x;
MAD 	R3, R3, radius.x, Center;
ADD 	angle.x, angle.x, Size;
SCS 	R4, angle.x;
MAD 	R4, R4, radius.x, Center;
ADD 	angle.x, angle.x, Size;
SCS 	R5, angle.x;
MAD 	R5, R5, radius.x, Center;
ADD 	angle.x, angle.x, Size;
SCS 	R6, angle.x;
MAD 	R6, R6, radius.x, Center;
ADD 	angle.x, angle.x, Size;
SCS 	R7, angle.x;
MAD 	R7, R7, radius.x, Center;
ADD 	angle.x, angle.x, Size;
SCS 	R8, angle.x;
MAD 	R8, R8, radius.x, Center;

# do samples
TEX		R1, R1, texture[0], 2D;
TEX		R2, R2, texture[0], 2D;
TEX		R3, R3, texture[0], 2D;
TEX		R4, R4, texture[0], 2D;
TEX		R5, R5, texture[0], 2D;
TEX		R6, R6, texture[0], 2D;
TEX		R7, R7, texture[0], 2D;
TEX		R8, R8, texture[0], 2D;

MAD 	color, R1, d1, color;
MAD 	color, R2, d2, color;
MAD 	color, R3, d3, color;
MAD 	color, R4, d4, color;
MAD 	color, R5, d5, color;
MAD 	color, R6, d6, color;
MAD 	color, R7, d7, color;
MAD 	color, R8, d8, color;

# calculate offsets
ADD 	angle.y, angle.y, -Size;
SCS 	R1, angle.y;
MAD 	R1, R1, radius.x, Center;
ADD 	angle.y, angle.y, -Size;
SCS 	R2, angle.y;
MAD 	R2, R2, radius.x, Center;
ADD 	angle.y, angle.y, -Size;
SCS 	R3, angle.y;
MAD 	R3, R3, radius.x, Center;
ADD 	angle.y, angle.y, -Size;
SCS 	R4, angle.y;
MAD 	R4, R4, radius.x, Center;
ADD 	angle.y, angle.y, -Size;
SCS 	R5, angle.y;
MAD 	R5, R5, radius.x, Center;
ADD 	angle.y, angle.y, -Size;
SCS 	R6, angle.y;
MAD 	R6, R6, radius.x, Center;
ADD 	angle.y, angle.y, -Size;
SCS 	R7, angle.y;
MAD 	R7, R7, radius.x, Center;
ADD 	angle.y, angle.y, -Size;
SCS 	R8, angle.y;
MAD 	R8, R8, radius.x, Center;

TEX		R0, uv, texture[0], 2D;
TEX		R1, R1, texture[0], 2D;
TEX		R2, R2, texture[0], 2D;
TEX		R3, R3, texture[0], 2D;
TEX		R4, R4, texture[0], 2D;
TEX		R5, R5, texture[0], 2D;
TEX		R6, R6, texture[0], 2D;
TEX		R7, R7, texture[0], 2D;
TEX		R8, R8, texture[0], 2D;

MAD 	color, R0, d0, color;
MAD 	color, R1, d1, color;
MAD 	color, R2, d2, color;
MAD 	color, R3, d3, color;
MAD 	color, R4, d4, color;
MAD 	color, R5, d5, color;
MAD 	color, R6, d6, color;
MAD 	color, R7, d7, color;
MAD 	color, R8, d8, color;

MAD		radius.x, radius.x, -radius.x, radius.x;
MAD 	radius.x, radius.x, -4.0, 1.0;
MAD		radius.x, radius.x, -radius.x, 1.0;
MUL 	color, color, radius.x;

MOV 	oColor.xyz, color;

END