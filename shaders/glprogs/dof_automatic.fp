###==============================================================================
#	Feedback DOF Automatic Focus Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _depth
# texture 1	= mask texture
#
# local[0]	= focus distance
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

PARAM 	focus	= program.local[0];

PARAM	dcode24	= { 1.0, 0.003921568627450980392156862745098, 0.000015259021896696421759365224689097 };
PARAM 	dofParm	= { 0.125, 4.0, -64.0, 0.0 };	# xy = Near/Far distance scale, zw = Near/Far distance bias
PARAM	farClip	= 131072.0;
PARAM	DEG2RAD	= 0.017453292519943295769236907684886;

TEMP	uv, depth, dist, dof, mask;


# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invRes.xyxy;
# MUL 	uv.xy, uv, nonPoT;

# sample textures
TEX 	depth, uv, texture[0], 2D;
DP3 	depth, depth, dcode24;
MUL		depth, depth, farClip;

ADD 	dist, depth, -focus;

MAD 	dof.xy, focus, dofParm.xyxy, dofParm.zwzw;
ADD 	dof.xy, dof, -focus;
RCP 	dof.x, dof.x;
RCP 	dof.y, dof.y;
MUL_SAT dof.xy, dist, dof;
ADD 	dof.x, dof.x, dof.y;
MAD 	dof.x, dof.x, dof.x, -0.0125;
MOV 	oColor.xyz, dof.x;

END