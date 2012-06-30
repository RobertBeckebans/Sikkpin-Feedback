###==============================================================================
#	Feedback Encode Depth Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentDepth
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

PARAM 	size	= program.local[0];

TEMP 	R0, R1, dist, uv;

PARAM	ncode24	= { 1.0, 255.0, 65535.0, 16777216.0 };
PARAM 	inv255	= 0.003921568627450980392156862745098;
PARAM 	near 	= { -3.0, 3.0 };
PARAM 	near2 	= 6.0;
PARAM 	far 	= 131072.0;
# PARAM	invfar	= 1.0;
# PARAM	invfar	= 0.5;
PARAM	invfar	= 0.0009765625;
# PARAM	invfar	= 0.0001220703125;
# PARAM	invfar	= 0.00006103515625;
# PARAM	invfar	= 0.0000152587890625;
# PARAM	invfar	= 0.000030517578125;
# PARAM	invfar	= 0.00000762939453125;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos, invRes;
# MUL 	R0.xy, fPos, { 0.00097751710654936461388074291300098, 0.0019569471624266144814090019569472 };
# MUL 	R0.xy, fPos, size.zwzw;
# MUL 	R0, fPos, { 0.0006103515625, 0.0013726128472222222222222222222222 };
# MUL 	R0, fPos, { 0.0015625, 0.0020833333333333333333333333333333 };

# scale by the screen non-power-of-two-adjust
# RCP 	R0.z, nonPoT.x;
# RCP 	R0.w, nonPoT.y;
# MUL 	R0, R0, R0.zwzw;
# MUL 	R1, 2, size;
# MUL 	R0, R0, { 1.25, 1.40625 };

# scale by the screen non-power-of-two-adjust
MUL 	uv, uv, nonPoT;

# MAD 	R0, R0, { 1024.0, 512.0 }, 0.5;
# MAD 	R0, R0, { 1280.0, 720.0 }, 0.5;
# FLR 	R0, R0;
# MUL_SAT R0, fPos.xyxy, { 0.0009765625, 0.001953125 };
# MUL_SAT R0, fPos.xyxy, { 0.00048828125, 0.0009765625 };

# MOV 	uv, TC;
# ADD 	uv.y, 1.0, -uv;
TEX 	R0, uv, texture[0], 2D;

# (2.0 * Near) / (Far + Near - depth * (Far - Near)
MAD 	R0, R0.x, 2.0, -1.0;
ADD 	R1, far, near;
MAD 	R1.x, R1.x, -R0.x, R1.y;
RCP 	R1.x, R1.x;
MUL 	R0, near2, R1.x;
MUL 	R0, R0, far;

# convert to -1.0 to 1.0 NDC
# ADD		R0, depth.x, depth.x;
# SUB		R0, R0, 1;
#MAD		R0, depth.x, 2, -1;

# calculate the view space z by deriving w from depth.  This will be negative.
# ADD		R0.z, -R0.z, -program.env[20].x;
# RCP		R0.z, R0.z;
# MUL		R0, R0.z, -program.env[20].y;

# MAD 	R1, 1, 2.0, -1.0;
# ADD		R1.z, -R1, -program.env[20].x;
# RCP		R1.z, R1.z;
# MUL		R1, R1.z, -program.env[20].y;

# ADD 	R0, -R0, R1;
# RCP 	R0, R0.x;
# ADD 	R0, R0, -1;
# ADD 	R0, 1, -R0;
# RCP 	R0, R0.x;
# ADD 	R0, R0, -1;

# now make it a full xyz in view space
# MAD		R0.xy, uv, program.env[21].zwzw, program.env[21].xyxy;
# MUL		R0.xy, R0.xyxy, -R0.z;
# MOV		R0.w, 1;

# DP3		dist, R0, R0;
# RSQ		dist.x, dist.x;
# MUL		R1, R0, dist.x;
# RCP		dist.x, dist.x;

# MUL		R0, R0, 256;
MUL		R0, R0, invfar;

MUL 	R0.w, R0.w, ncode24.y;
FLR 	R0.x, R0.w;
FRC 	R0.w, R0.w;
MUL 	R0.w, R0.w, ncode24.y;
FLR 	R0.y, R0.w;
FRC 	R0.w, R0.w;
MUL 	R0.w, R0.w, ncode24.y;
FLR 	R0.z, R0.w;
MUL 	R0.xyzw, R0, inv255;

MOV 	oColor.xyzw, R0;
# MUL 	oColor.xyzw, dist.x, 1;

END