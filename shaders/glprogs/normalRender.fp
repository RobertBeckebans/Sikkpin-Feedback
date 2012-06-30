###==============================================================================
#	Feedback View-space Normals Render Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _depth
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
PARAM 	proj2view	= program.env[20];
PARAM 	pos2view	= program.env[21];

PARAM 	FOV		= program.local[0];

PARAM	dcode24	= { 1.0, 0.003921568627450980392156862745098, 0.000015259021896696421759365224689097, 0.000000059604644775390625 };
PARAM	radOff	= { 0.0, 0.78539816339744830961566084581988, 1.57079632679489661923132169156, 2.356194490192344928846982537457 };	# 45 deg
PARAM	DEG2RAD	= 0.017453292519943295769236907684886;
PARAM 	PI		= 3.1415926535897932384626433832795;
PARAM 	halfPI	= 1.5707963267948966192313216916398;
PARAM 	near	= 3.0;
# PARAM	farClip	= 8192.0;
PARAM	farClip	= 131072.0;
# PARAM	farClip	= 32768.0;
# PARAM	farClip	= 65536.0;
# PARAM	farClip	= 131072.0;
PARAM 	randUV	= 0.0625;	# 0.25 = 4x4; 0.0625 = 16x16; 0.015625 = 64x64
PARAM 	uvScale	= {  2.0,  2.0,  0.0 };
PARAM 	uvBias	= { -1.0, -1.0,  1.0 };

TEMP	depthEnc, depth, normal, pos, random, ssao, focalLen, invFocalLen, invres, aspect;
TEMP 	angle, radius, dist, contrast, res, uv, invNPoT;
TEMP 	D0, D1, D2, D3, D4, D5, D6, D7, D8;
TEMP	R0, R1, R2, R3, R4, R5, R6, R7, R8;
TEMP	P0, P1, P2, P3, P4, P5, P6, P7, P8;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;
# MUL 	uv.xy, fPos.xyxy, nonPoT.zwzw;

# calculate screen width/height
MOV 	invres, invRes;
MOV 	invres.zw, -invres.xyxy;

# MUL 	aspect.z, invRes.z, invRes.y;

# calculate fov focal length
# SCS 	R0.xy, FOV.x;
# RCP 	R0.x, R0.x;
# MUL 	invFocalLen.y, R0.y, R0.x;
# MUL 	invFocalLen.x, invFocalLen.y, aspect.z;

# load the depth render
TEX 	depthEnc, uv, texture[0], 2D;
# DP3 	depth, depthEnc, dcode24;
# MUL 	depth.x, depth, farClip;
# RCP 	depth.w, depth.x;

# calculate the view space z by deriving w from depth.  This will be negative.
MAD 	depth, depthEnc.z, 2.0, -1.0;
ADD		depth.z, -depth.z, -proj2view.x;
RCP		depth.z, depth.z;
MUL		depth, depth.z, proj2view.y;

# now make it a full xyz in view space
# MUL 	pos, fPos, invRes.zwzw;
# MUL 	pos, uv, nonPoT.xyxy;
# MUL 	pos, uv, nonPoT.xyxy;
MAD 	pos, uv, 2.0, -1.0;
# MAD 	pos, pos, 2.0, 0;
# MAD		pos.xy, pos, pos2view.zwzw, pos2view.xyxy;
MUL		pos.xy, pos.xyxy, -depth.z;
MOV		pos.z, -depth.z;
MOV		pos.w, 1;
# MUL		pos, pos, 128;

# calculate pixel view-space position
# MAD 	pos.xyz, uv.xyxy, uvScale, uvBias;
# MUL 	pos.xy, pos, invFocalLen;
# MUL 	pos.xyz, pos, depth.x;
 
# calculate face normal
MOV 	P1.xy, uv;
MOV 	P2.xy, uv;
MOV 	P3.xy, uv;
MOV 	P4.xy, uv;
# ADD 	P1.x, P1, nonPoT.z;
# ADD 	P2.x, P2, -nonPoT.z;
# ADD 	P3.y, P3, nonPoT.w;
# ADD 	P4.y, P4, -nonPoT.w;
ADD 	P1.x, P1, invres.x;
ADD 	P2.x, P2, -invres.x;
ADD 	P3.y, P3, invres.y;
ADD 	P4.y, P4, -invres.y;

TEX 	D1, P1, texture[0], 2D;
TEX 	D2, P2, texture[0], 2D;
TEX 	D3, P3, texture[0], 2D;
TEX 	D4, P4, texture[0], 2D;
# DP3 	D0.x, D1, dcode24;
# DP3 	D0.y, D2, dcode24;
# DP3 	D0.z, D3, dcode24;
# DP3 	D0.w, D4, dcode24;
MOV 	D0.x, D1.x;
MOV 	D0.y, D2.x;
MOV 	D0.z, D3.x;
MOV 	D0.w, D4.x;
# MUL 	D0, D0, farClip;

MAD 	D0, D0, 2.0, -1.0;
ADD		D0, -D0, -proj2view.x;
RCP		D0.x, D0.x;
RCP		D0.y, D0.y;
RCP		D0.z, D0.z;
RCP		D0.w, D0.w;
MUL		D0, D0, proj2view.y;

# MOV 	P1.xy, uv.zwzw;
# MOV 	P2.xy, uv.zwzw;
# MOV 	P3.xy, uv.zwzw;
# MOV 	P4.xy, uv.zwzw;
# ADD 	P1.x, P1, invres.x;
# ADD 	P2.x, P2, -invres.x;
# ADD 	P3.y, P3, invres.y;
# ADD 	P4.y, P4, -invres.y;

MAD 	P1.xyz, P1, uvScale, uvBias;
MAD 	P2.xyz, P2, uvScale, uvBias;
MAD 	P3.xyz, P3, uvScale, uvBias;
MAD 	P4.xyz, P4, uvScale, uvBias;
# MUL 	P1.xy, P1, invFocalLen;
# MUL 	P2.xy, P2, invFocalLen;
# MUL 	P3.xy, P3, invFocalLen;
# MUL 	P4.xy, P4, invFocalLen;
MUL 	P1.xyz, P1, -D0.x;
MUL 	P2.xyz, P2, -D0.y;
MUL 	P3.xyz, P3, -D0.z;
MUL 	P4.xyz, P4, -D0.w;

ADD 	P1, P1, -pos;
ADD 	P2, pos, -P2;
ADD 	P3, P3, -pos;
ADD 	P4, pos, -P4;
DP3 	P1.w, P1, P1;
DP3 	P2.w, P2, P2;
DP3 	P3.w, P3, P3;
DP3 	P4.w, P4, P4;
SLT 	P1.w, P1.w, P2.w;
SLT 	P3.w, P3.w, P4.w;
CMP 	P1, -P1.w, P1, P2;
CMP 	P2, -P3.w, P3, P4;

XPD 	normal.xyz, P1, P2;
DP3 	normal.w, normal, normal;
RSQ 	normal.w, normal.w;
MUL 	normal.xyz, normal, normal.w;
# MOV 	normal.y, -normal.y;

MAD 	oColor.xyzw, normal, 0.5, 0.5;
MOV 	oColor, normal;
# MOV 	oColor, pos;
MOV 	oColor.w, 1.0;

END