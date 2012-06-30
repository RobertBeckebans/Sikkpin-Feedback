###==============================================================================
#	Feedback SSAO Blur Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _ssao (xyz = depth (24-bit encoded), w = ssao)
#
# local[0]	= blur scale
# local[1]	= depth threshold
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0
OPTION	ARB_precision_hint_nicest;

OUTPUT 	oColor 	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB 	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];

PARAM 	Scale	= program.local[0];
PARAM 	Thresh	= program.local[1];

# sample offsets
PARAM	off1	= {  0.5,  1.0,  1.0, -0.5 };
PARAM	off2	= { -0.5, -1.0, -1.0,  0.5 };
# PARAM	off1	= {  0.0,  1.0 };
# PARAM	off2	= {  1.0,  0.0 };
# PARAM	off3	= {  0.0, -1.0 };
# PARAM	off4	= { -1.0,  0.0 };

PARAM	const	= { 0.25, 0.5, 0.75, 1.0 };
PARAM	dcode24	= { 1.0, 0.003921568627450980392156862745098, 0.000015259021896696421759365224689097  };
PARAM 	farClip = 8192.0;

TEMP	uv, color, diff, weight, sumWeight, scale;
TEMP 	R0, R1, R2, R3, R4;
TEMP 	S0, S1, S2, S3, S4;
TEMP 	D0, D1, D2;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;

MUL 	scale, Scale, invRes.xyxy;

MAD_SAT R1, off1, scale, uv.xyxy;
MAD_SAT R2, off2, scale, uv.xyxy;

TEX 	S0, uv, texture[0], 2D;
TEX 	S1, R1.xyxy, texture[0], 2D;
TEX 	S2, R1.zwzw, texture[0], 2D;
TEX 	S3, R2.xyxy, texture[0], 2D;
TEX 	S4, R2.zwzw, texture[0], 2D;

DP4 	D0, S0, dcode24;
DP4 	D1.x, S1, dcode24;
DP4 	D1.y, S2, dcode24;
DP4 	D1.z, S3, dcode24;
DP4 	D1.w, S4, dcode24;

MOV 	S1.x, S1.w;
MOV 	S1.y, S2.w;
MOV 	S1.z, S3.w;
MOV 	S1.w, S4.w;
MOV 	sumWeight.x, 0.0125;

RCP 	D0.x, D0.x;
MUL 	D1, D1, D0.x;
ADD 	D1, 1.0, -D1;
MUL 	diff, D1, Thresh;
ABS 	D1, diff;
MAD 	D1, D1, -const.z, const.y;
MUL 	D2, diff, const.x;
ADD_SAT weight, D1, -D2;

DP4 	color.x, S1, weight;
MAD 	color.x, S0.x, sumWeight.x, color.x;

DP4 	weight.x, weight, const.w;
ADD 	sumWeight.x, sumWeight, weight;
RCP 	sumWeight.x, sumWeight.x;

MOV 	oColor, S0;
MUL 	oColor.w, color.x, sumWeight.x;

END