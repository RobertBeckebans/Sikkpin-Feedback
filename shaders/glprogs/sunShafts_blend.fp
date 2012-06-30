###==============================================================================
#	Feedback Sun Shafts Blend Fragment Program 
###==============================================================================
#
# input:
#--------------------------------
# texture 0	= _currentRender
# texture 1	= _sunShafts
#
# local[0]	= x = sun shafts strength, y = shadow strength
# local[1]	= sun color
#
# env[0]	= 1.0 to _currentRender conversion
# env[1]	= fragment.position to 0.0 - 1.0 conversion
#
###==============================================================================

!!ARBfp1.0 
OPTION	ARB_precision_hint_fastest;

OUTPUT 	oColor 		= result.color;

ATTRIB	fPos		= fragment.position;
ATTRIB 	TC			= fragment.texcoord[0];

PARAM 	nonPoT		= program.env[0];
PARAM 	invRes		= program.env[1];

PARAM 	sunStrength	= program.local[0];
PARAM 	sunColor	= program.local[1];

TEMP 	uv, invres, color, sunShafts, shaftsMask, if;
TEMP 	R0, R1, R2, R3, R4;


# calculate the non-power-of-two inverse resolution
MUL 	invres, invRes.xyxy, nonPoT.xyxy;
MOV 	invres.zw, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

# TEX 	color, uv, texture[0], 2D;
# TEX		sunShafts, uv.zwzw, texture[1], 2D;

# MOV 	shaftsMask.w, shaftsMask.z;
# ADD_SAT	shaftsMask.x, 1.00001, -sunShafts.w;
# MUL 	shaftsMask.x, shaftsMask.x, sunStrength.y;
# ADD 	shaftsMask.x, shaftsMask.x, shaftsMask.x;
# MUL 	shaftsMask.xyz, shaftsMask.x, sunColor;
# MAD 	shaftsMask.xyz, shaftsMask, 0.5, 0.5;

# MUL 	sunShafts, sunShafts, sunStrength.x;
# MUL 	sunShafts, sunShafts, sunColor;

# screen blend
# LRP 	color.xyz, color, 1.0, sunShafts;

# soft light blend
# MUL 	R1, color, shaftsMask;
# MUL 	R1, R1, 2.0;
# MAD 	R2, shaftsMask, -2.0, 1.0;
# MUL 	R3, color, color;
# MAD 	R4, R2, R3, R1;
# RSQ 	R1.x, color.x;
# RSQ 	R1.y, color.y;
# RSQ 	R1.z, color.z;
# RCP 	R1.x, R1.x;
# RCP 	R1.y, R1.y;
# RCP 	R1.z, R1.z;
# MAD 	R2, shaftsMask, 2.0, -1.0;
# ADD 	R3, 1.0, -shaftsMask;
# MUL 	R3, R3, color;
# ADD 	R3, R3, R3;
# MAD 	R1, R1, R2, R3;
# SLT 	if, shaftsMask, 0.5;
# CMP 	oColor.xyz, -if, R4, R1;

TEX		oColor, uv.zwzw, texture[1], 2D;

END