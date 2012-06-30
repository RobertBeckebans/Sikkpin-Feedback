###==============================================================================
#	Feedback Sun Shafts Gen Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _depth
#
# local[0]	= offset scale (w = holds whether we're working with a cropped or full res texture)
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0
OPTION	ARB_precision_hint_nicest;

OUTPUT 	oColor	= result.color;

ATTRIB	fPos	= fragment.position;
ATTRIB	TC		= fragment.texcoord[0];

PARAM 	nonPoT	= program.env[0];
PARAM 	invRes	= program.env[1];
PARAM 	proj2view	= program.env[20];

PARAM	dcode24	= { 1.0, 0.003921568627450980392156862745098, 0.000015259021896696421759365224689097 };
PARAM 	farClip	= 0.0001220703125;

PARAM 	near 	= { -3.0, 3.0 };
PARAM 	near2 	= 6.0;
PARAM 	far 	= 131072.0;
PARAM	invfar	= 0.0001220703125;

TEMP	uv, color, depth;
TEMP 	R0, R1;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;
MUL 	uv.xy, uv, nonPoT;

# sample depth texture
ADD 	uv, 1.0, -TC;
MOV 	uv.x, TC;
TEX 	depth, uv, texture[0], 2D;
DP3 	depth, depth, dcode24;
# MAD 	depth, depth.z, 2.0, -1.0;
# SUB		depth.z, -depth.z, proj2view.x;
# RCP		depth, depth.z;
# MUL		depth, depth.z, proj2view.y;

# (2.0 * Near) / (Far + Near - depth * (Far - Near)
# MAD 	R0, depth.x, 2.0, -1.0;
# ADD 	R1, far, near;
# MAD 	R1.x, R1.x, -R0.x, R1.y;
# RCP 	R1.x, R1.x;
# MUL 	R0, near2, R1.x;
# MUL 	depth, R0, far;

# MAD 	R1, 0, 2.0, -1.0;
# SUB 	R1.x, -R1.z, proj2view.x;
# RCP 	R1, R1.x;
# MUL 	R1.z, R1.x, proj2view.y;

# ADD 	depth, R1.z, -depth.z;
# MAX 	depth.x, depth.x, 0;
# MUL_SAT depth.x, depth.x, farClip;

# sample color texture
TEX 	color, uv, texture[1], 2D;

MUL 	oColor.xyz, color, depth.x;
ADD 	oColor.w, 1.0, -depth.x;
MOV 	oColor.xyz, color;
# MOV 	oColor.xyz, depth.x;

END