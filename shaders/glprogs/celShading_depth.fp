!!ARBfp1.0
OPTION ARB_precision_hint_nicest;

# texture 0	= _currentRender
# texture 1	= _depth
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion

OUTPUT	oColor	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB 	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];
PARAM 	proj2view	= program.env[20];
PARAM 	pos2view	= program.env[21];

PARAM	Scale	= program.local[0];
PARAM	Thresh	= program.local[1];

# sample offsets
PARAM 	off1	= { -1.0, -1.0,  0.0, -1.0 };
PARAM 	off2	= {  1.0, -1.0, -1.0,  0.0 };
PARAM 	off3	= {  1.0,  0.0, -1.0,  1.0 };
PARAM 	off4	= {  0.0,  1.0,  1.0,  1.0 };

PARAM	dcode24	= { 1.0, 0.003921568627450980392156862745098, 0.000015259021896696421759365224689097 };
PARAM 	invFar	= 0.0001220703125;
PARAM 	farClip	= 8192.0;

TEMP 	uv, invres, color, depth, scale, threshold, edge, res, sky;
TEMP 	R0, R1, R2, R3, R4;
TEMP 	T0, T1, T2, T3, T4, T5, T6, T7, T8;


# calculate the non-power-of-two inverse resolution
MUL 	invres, invRes.xyxy, nonPoT.xyxy;
MOV 	invres.zw, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

# calculate screen width/height
RCP 	res.x, invRes.x;
RCP 	res.y, invRes.y;

TEX 	color, uv.xyxy, texture[0], 2D;
TEX 	depth, uv.zwzw, texture[1], 2D;
SGE 	sky, depth, 1.0;

# calculate the view space z by deriving w from depth.  This will be negative.
# MAD 	depth.w, -depth.z, 2.0, -1.0;
# ADD		depth.w, depth.w, proj2view.x;
# RCP		depth.w, depth.w;
# MUL		depth.w, depth.w, proj2view.y;
# RCP		depth.w, depth.w;
# MUL		depth.w, depth.w, depth.z;

# MAD		depth.xy, fPos, pos2view.zwzw, pos2view.xyxy;
# MUL		depth.xy, depth.xyxy, -depth.z;
# MOV		depth.w, 1;
# DP3		depth.w, depth, depth;
# RSQ		depth.w, depth.w;
# RCP		depth.w, depth.w;

MUL 	scale, Scale.xyxy, invres.zwzw;

# calculate thresh0ld
MUL_SAT threshold.x, depth.x, 0.0078125;		# make area smaller if distance is less than 128 units
MAD 	threshold.y, depth.x, 0.001953125, 1.0;	# make area larger if distance is more than 512 units
MUL 	threshold.w, threshold.x, threshold.y;
MUL 	threshold, Thresh, threshold.w;
MOV 	threshold, Thresh;

MAD 	R1, off1, scale, uv.zwzw;
MAD 	R2, off2, scale, uv.zwzw;
MAD 	R3, off3, scale, uv.zwzw;
MAD 	R4, off4, scale, uv.zwzw;

TEX 	T1.x, R1.xyxy, texture[1], 2D;
TEX 	T1.y, R1.zwzw, texture[1], 2D;
TEX 	T1.z, R2.xyxy, texture[1], 2D;

TEX 	T1.w, R2.zwzw, texture[1], 2D;
TEX 	T2.x, R3.xyxy, texture[1], 2D;

TEX 	T2.y, R3.zwzw, texture[1], 2D;
TEX 	T2.z, R4.xyxy, texture[1], 2D;
TEX 	T2.w, R4.zwzw, texture[1], 2D;

MAD 	T1, T1, 2.0, -1.0;
ADD		T1, -T1, -proj2view.x;
RCP		T1.x, T1.x;
RCP		T1.y, T1.y;
RCP		T1.z, T1.z;
RCP		T1.w, T1.w;
MUL		T1, T1, proj2view.y;

MAD 	T2, T2, 2.0, -1.0;
ADD		T2, -T2, -proj2view.x;
RCP		T2.x, T2.x;
RCP		T2.y, T2.y;
RCP		T2.z, T2.z;
RCP		T2.w, T2.w;
MUL		T2, T2, proj2view.y;

MOV 	R1, T1;
MOV 	R2.x, T1.w;
MOV 	R2.z, T2.x;
MOV 	R3, T2.yzwx;

ADD 	edge.x, 0.0, -R1.x;
MUL 	R1.w, R1.y, 2.0;
ADD 	edge.x, edge.x, -R1.w;
ADD 	edge.x, edge.x, -R1.z;
ADD 	edge.x, edge.x, R3.x;
MAD 	edge.x, R3.y, 2.0, edge.x;
ADD 	edge.x, edge.x, R3.z;

ADD 	edge.y, 0.0, -R1.x;
ADD 	edge.y, edge.y, R1.z;
MUL 	R1.w, R2.x, 2.0;
ADD 	edge.y, edge.y, -R1.w;
MAD 	edge.y, R2.z, 2.0, edge.y;
ADD 	edge.y, edge.y, -R3.x;
ADD 	edge.y, edge.y, R3.z;

MOV 	edge.z, 0.0;
DP3 	edge.w, edge, edge;
POW 	edge.w, edge.w, 0.5.x;

SLT 	edge.xyz, edge.w, threshold;

MUL 	oColor.xyz, color, edge;

# RCP 	depth.w, depth.w;
# MUL 	depth.z, depth.z, depth.w;
# RCP 	depth.z, depth.z;
MUL 	oColor, 1, depth.w;
# MOV 	oColor, depth.w;

END