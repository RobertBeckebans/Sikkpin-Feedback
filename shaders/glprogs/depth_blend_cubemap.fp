!!ARBfp1.0 
OPTION ARB_precision_hint_nicest;

# texture 0	= surface texture
# texture 1	= _currentDepth
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion

OUTPUT 	oColor 		= result.color;

ATTRIB	fPos		= fragment.position;
ATTRIB 	TC			= fragment.texcoord[0];

PARAM 	nonPoT		= program.env[0];
PARAM 	invRes		= program.env[1];
PARAM 	proj2view	= program.env[20];

PARAM	scroll		= program.local[0];	# scroll parms
PARAM	contrast	= program.local[1];	# depth fade
PARAM	tint		= program.local[2];	# depth fade

TEMP	uv, color, depth, alpha;
TEMP 	R0, R1;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL		uv, fPos.xyxy, invRes.xyxy;
TEX		depth, uv, texture[1], 2D;

# calculate the view space z by deriving w from depth.  This will be negative.
MAD 	depth, depth.z, 2.0, -1.0;
SUB		depth.z, -depth.z, proj2view.x;
RCP		depth.z, depth.z;
MUL		depth, depth.z, proj2view.y;

# calculate the view space z by deriving w from depth.  This will be negative.
MAD 	R1, fPos.z, 2.0, -1.0;
SUB 	R1.x, -R1.z, proj2view.x;
RCP 	R1.x, R1.x;
MUL 	R1.z, R1.x, proj2view.y;

# basically, we start the fade when depth difference is scale input
# value and fade to 0 using the contrast function below
ADD 	alpha, R1.z, -depth.z;
MAX 	alpha.x, alpha.x, 0;
MUL_SAT alpha.x, alpha.x, contrast.x;

# contrast function (from nVidia's "Soft Particles" paper by Tristan Lorach)
SGE 	alpha.w, alpha.x, 0.5;
ADD 	alpha.x, alpha.w, -alpha.x;
ABS 	alpha.x, alpha.x;
ADD_SAT alpha.x, alpha.x, alpha.x;
POW 	alpha.x, alpha.x, contrast.y;
MAD 	alpha.x, alpha.x, -0.5, alpha.w;
ABS_SAT alpha.x, alpha.x;

# calculate the screen texcoord in the 0.0 to 1.0 range
# MUL 	uv.zw, fPos.xyxy, nonPoT.zwzw;

# sample current render (refraction)
ADD 	R1, TC, scroll;
TEX 	color, R1, texture[0], 2D;
MUL 	oColor.xyz, color, tint;
ADD_SAT color.w, color.w, tint.w;
MUL 	oColor.w, color.w, alpha.x;


END