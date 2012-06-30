###==============================================================================
#	Feedback Simplex Noise Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
#
# local[0]	= offset scale (w = holds whether we're working with a cropped or full res texture)
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0 
OPTION	ARB_precision_hint_fastest;

OUTPUT 	oColor 		= result.color;

ATTRIB	fPos		= fragment.position;
ATTRIB	TC			= fragment.texcoord[0];

PARAM 	nonPoT		= program.env[0];
PARAM 	invRes		= program.env[1];

PARAM 	size		= program.local[0];
PARAM 	time		= program.local[1];

PARAM 	scale		= { 4.0, 4.0, 0.0, 0.0 };
PARAM 	bias		= { 2.0, 2.0, 0.0, 0.0 };

PARAM 	col			= { 1, 0.5, 0, 0 };
PARAM 	F2			= 0.366025403784;
PARAM 	G2			= 0.211324865405;

TEMP	uv, invres, color, mask;
TEMP 	R0, R1, R2;

TEMP 	intg, frac, frac_1, tmp;
TEMP 	g00, g10, g01, g11, n0, n1, n2, o1, noise;
TEMP 	s, Pi, t, P0, Pf0, Pf1, Pf2, grad0, grad1, grad2, t0, t1, t2;


# calculate the non-power-of-two inverse resolution
MUL 	invres, invRes.xyxy, nonPoT.xyxy;
MOV 	invres.zw, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

#PARAM size = 64.0;
#PARAM invSize = 0.015625;
#PARAM invHalfSize = 0.0078125;

ADD		s, fPos.x, fPos.y;
MUL 	s, s, F2;

ADD 	Pi, fPos, s;
FLR 	Pi, Pi;

ADD		t, Pi.x, Pi.y;
MUL 	t, t, G2;

ADD 	P0, Pi, -t;
MOV 	P0.zw, 0.0;

MAD 	Pi, Pi, size.y, size.z;
MOV 	Pi.zw, 0.0;

ADD 	Pf0, fPos, P0;
MOV 	Pf0.zw, 0.0;

SLT 	o1.w, Pf0.y, Pf0.x;
CMP 	o1, -o1.w, { 1.0, 0.0 }, { 0.0, 1.0 };

TEX		grad0, Pi, texture[0], 2D;
MAD		grad0, grad0, scale, -bias;
MOV 	grad0.zw, 0.0;
DP3 	grad0.x, grad0, Pf0;
DP3 	t0, Pf0, Pf0;
ADD 	t0, 0.5, -t0;
SLT 	R1, 0.0, t0;
MUL 	t0, t0, t0;
MUL 	t0, t0, t0;
MUL 	n0.x, t0, grad0.x;
MUL 	n0.x, n0, R1;


ADD 	Pf1, Pf0, -o1;
ADD 	Pf1, Pf1, G2;

MAD 	R1, o1, size.y, Pi;
TEX		grad1, R1, texture[0], 2D;
MAD		grad1, grad1, scale, -bias;
MOV 	grad1.zw, 0.0;
DP3 	grad1.x, grad1, Pf1;
DP3 	t1, Pf1, Pf1;
ADD 	t1, 0.5, -t1;
SLT 	R1, 0.0, t1;
MUL 	t1, t1, t1;
MUL 	t1, t1, t1;
MUL 	n0.y, t1, grad1.x;
MUL 	n0.y, n0, R1;


MAD 	R1, G2, -2.0, 1.0;
ADD 	Pf2, Pf0, -R1;

ADD 	R1, Pi, size.y;
TEX		grad2, R1, texture[0], 2D;
MAD		grad2, grad2, scale, -bias;
MOV 	grad2.zw, 0.0;
DP3 	grad2.x, grad2, Pf2;
DP3 	t2, Pf2, Pf2;
ADD 	t2, 0.5, -t2;
SLT 	R1, 0.0, t2;
MUL 	t2, t2, t2;
MUL 	t2, t2, t2;
MUL 	n0.z, t2, grad1.x;
MUL 	n0.z, n0, R1;

DP3 	noise, n0, 1.0;
MUL 	noise, noise, 100.0;

MAD		oColor, noise, 0.5, 0.5;
  
END 