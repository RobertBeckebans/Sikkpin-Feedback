###==============================================================================
#	Feedback Perlin Noise Fragment Program 
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

PARAM 	halfTexel	= { 0.001953125, 0.03125 };
PARAM 	invWidth	= 0.0625;

PARAM 	scale		= { 4.0, 4.0, 0.0, 0.0 };
PARAM 	bias		= { 2.0, 2.0, 0.0, 0.0 };

PARAM 	col			= { 1, 0.5, 0, 0 };

TEMP	uv, invres, color, mask;
TEMP 	R0, R1, R2;

TEMP 	intg, frac, frac_1, tmp;
TEMP 	g00, g10, g01, g11, n00, n10, n01, n11, u, v, noise;


# calculate the non-power-of-two inverse resolution
MUL 	invres, invRes.xyxy, nonPoT.xyxy;
MOV 	invres.zw, invRes.xyxy;

# calculate the screen texcoord in the 0.0 to 1.0 range
MUL 	uv, fPos.xyxy, invres;

#PARAM size = 64.0;
#PARAM invSize = 0.015625;
#PARAM invHalfSize = 0.0078125;

MAD 	R0, TC.xyxy, 1.0, 0.0;
MUL		R0, R0, size.x;

FLR		intg, R0;
FRC		frac, R0;
MOV 	frac.zw, 0.0;

MAD		R0, intg, size.y, size.z;

TEX		g00, R0, texture[0], 2D;          # Sample X values from the four surrounding texels
MAD		g00, g00, scale, -bias;                # Expand [0, 1] to [-1, 1]
DP3 	n00.x, g00, frac;

MOV 	R1, R0;
ADD 	R1.x, R0, size.y;
TEX		g10, R1, texture[0], 2D;          # Sample Y values from the four surrounding texels
MAD		g10, g10, scale, -bias;
ADD 	R1, frac, { -1.0, 0.0, 0.0, 0.0 };
DP3 	n10.x, g10, R1;

MOV 	R1, R0;
ADD 	R1.y, R0, size.y;
TEX		g01, R1, texture[0], 2D;          # Sample Y values from the four surrounding texels
MAD		g01, g01, scale, -bias;
ADD 	R1, frac, { 0.0, -1.0, 0.0, 0.0 };
DP3 	n00.y, g01, R1;

ADD 	R1.xy, R0, size.y;
TEX		g11, R1, texture[0], 2D;          # Sample Y values from the four surrounding texels
MAD		g11, g11, scale, -bias;
ADD 	R1, frac, { -1.0, -1.0, 0.0, 0.0 };
DP3 	n10.y, g11, R1;

MAD		tmp, frac, 6, -15;                 # s_curve(t) = t * t * (3 - 2 * t)
MAD		tmp, frac, tmp, 10;
MUL		R1, frac, frac;
MUL		R1, R1, frac;
MUL		frac, R1, tmp;

# //Smooth polynomial bi-lerp
# float interp0 = val00 + 	( 3 * pow( fraction.x, 2 ) - 2 * pow( fraction.x, 3 ) ) * ( val10 - val00 );
# float interp1 = val01 + 	( 3 * pow( fraction.x, 2 ) - 2 * pow( fraction.x, 3 ) ) * ( val11 - val01 );
# float interp2 = interp0 + ( 3 * pow( fraction.y, 2 ) - 2 * pow( fraction.y, 3 ) ) * ( interp1 - interp0 );
# ADD 	R1.x, n00.y, -n00.x;
# MAD 	noise.x, R1.x, frac.x, n00.x;

# ADD 	R1.x, n01.y, -n01.x;
# MAD 	noise.y, R1.x, frac.x, n01.x;

# ADD 	R1.x, noise.y, -noise.x;
# MAD 	noise, R1.x, frac.y, noise.x;

# MAD 	noise, n10, frac.z, noise;

  # // Blend contributions along x
  # vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));

  # // Blend contributions along y
  # float n_xy = mix(n_x.x, n_x.y, fade(Pf.y));

#mix(x, y, a) == x * ( 1.0 - a ) + y * a
LRP 	noise, frac.x, n10, n00;
LRP 	noise, frac.y, noise.y, noise.x;


MAD		noise, noise, 6.23, time;
COS		noise, noise.x;
# MUL		noise, noise, 1.7;

MAD		oColor, noise, 0.5, 0.5;
  
END 