###==============================================================================
#	Feedback DOF Manual Focus Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _depth
#
# local[0]	= near distance
# local[1]	= focus distance
# local[2]	= far distance
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
PARAM 	pos2view	= program.env[21];

PARAM 	Near	= program.local[0];
PARAM 	Focus	= program.local[1];
PARAM 	Far		= program.local[2];

PARAM	dcode24	= { 1.0, 0.003921568627450980392156862745098, 0.000015259021896696421759365224689097 };
PARAM	farClip	= 131072.0;

TEMP	uv, depth, dist, dof, near, focus, far;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos, invRes;
# MUL 	uv.xy, uv, nonPoT;
# MUL 	uv, fPos, nonPoT.zwzw;
# MUL 	uv, fPos, { 0.0006103515625, 0.0013726128472222222222222222222222 };

# sample depth texture
TEX 	depth, uv, texture[0], 2D;
# calculate the view space z by deriving w from depth.  This will be negative.
MAD 	depth, depth.z, 2.0, -1.0;
ADD		depth.z, -depth.z, -proj2view.x;
RCP		depth.z, depth.z;
MUL		depth, depth.z, proj2view.y;
ADD		depth, 1.0, depth.z;
RCP		depth, depth.z;
# MAD		depth, depth.z, pos2view.x, -pos2view.z;

ADD 	dist, depth, -Focus;

ADD 	dof.x, Near, -Focus;
ADD 	dof.y, Far, -Focus;
RCP 	dof.x, dof.x;
RCP 	dof.y, dof.y;
MUL_SAT dof.xy,	dof, dist;

# CMP 	oColor.xyz, dist, dof.x, dof.y

ADD 	dof.x, dof.x, dof.y;
MUL 	oColor.xyz, dof.x, dof.x;
# MOV 	oColor.xyz, depth;

END