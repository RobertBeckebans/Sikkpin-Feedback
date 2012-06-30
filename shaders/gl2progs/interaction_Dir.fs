#version 120

varying vec3		var_Position;
varying vec3		var_LightDir;
varying vec3		var_ViewDir;
varying vec2		var_TexDiffuse;
varying vec2		var_TexNormal;
varying vec2		var_TexSpecular;
varying vec4		var_TexLight;
varying mat3		var_TangentBitangentNormalMatrix;
varying vec4		var_Color;

uniform samplerCube	u_lightCubeTexture;
uniform sampler2D	u_lightProjectionTexture;
uniform sampler2D	u_lightFalloffTexture;
uniform sampler2D	u_normalTexture;
uniform sampler2D	u_diffuseTexture;
uniform sampler2D	u_specularTexture;
uniform sampler2D	u_ssaoTexture;
uniform sampler2D	u_aux0Texture;
uniform sampler2D	u_aux1Texture;
uniform sampler2D	u_aux2Texture;
uniform sampler2D	u_aux3Texture;
uniform sampler2D	u_aux4Texture;
uniform sampler2D	u_aux5Texture;
uniform sampler2D	u_aux6Texture;
uniform sampler2D	u_aux7Texture;
uniform sampler2D	u_aux8Texture;
uniform sampler2D	u_aux9Texture;

uniform vec4		u_lightOrigin;
uniform vec4		u_viewOrigin;
uniform vec4		u_diffuseColor;
uniform vec4		u_specularColor;
uniform vec4		u_specExp;
uniform int			u_falloffType;

uniform vec4		u_localParm0;
uniform vec4		u_localParm1;

#define	M_inv8PI	0.039788735772973833942220940843129

// Gamma correction
vec3 SrgbToLinear( vec3 val ) {
	vec3 ret;
	
	if ( val.x <= 0.0 ) ret.x = 0.0;
	else if ( val.x <= 0.04045 ) ret.x = val.x / 12.92;
	else ret.x = pow( ( val.x + 0.055 ) / 1.055, 2.4 );

	if ( val.y <= 0.0 ) ret.y = 0.0;
	else if ( val.y <= 0.04045 ) ret.y = val.y / 12.92;
	else ret.y = pow( ( val.y + 0.055 ) / 1.055, 2.4 );

	if ( val.z <= 0.0 ) ret.z = 0.0;
	else if ( val.z <= 0.04045 ) ret.z = val.z / 12.92;
	else ret.z = pow( ( val.z + 0.055 ) / 1.055, 2.4 );

	return ret;
}

vec3 LinearToSrgb( vec3 val ) {
	vec3 ret;
	
	if ( val.x <= 0.0 ) ret.x = 0.0;
	else if ( val.x <= 0.0031308 ) ret.x = val.x * 12.92;
	else ret.x = pow( val.x, 0.41666 ) * 1.055 - 0.055;

	if ( val.y <= 0.0 ) ret.y = 0.0;
	else if ( val.y <= 0.0031308 ) ret.y = val.y * 12.92;
	else ret.y = pow( val.y, 0.41666 ) * 1.055 - 0.055;

	if ( val.z <= 0.0 ) ret.z = 0.0;
	else if ( val.z <= 0.0031308 ) ret.z = val.z * 12.92;
	else ret.z = pow( val.z, 0.41666 ) * 1.055 - 0.055;

	return ret;
}

vec3 CalcAttenuation( void ) {
	vec3 temp;
	float f;

	if ( u_falloffType == 2 ) {	// projected light
		if ( var_TexLight.z < 0 ) {
			temp = 0.0;
		} else {
			f = clamp( 1.0 - var_TexLight.z, 0.0, 1.0 );
			temp = f * f * texture2DProj( u_lightProjectionTexture, var_TexLight ).xyz;
		}
	} else {	// point light
		if ( u_falloffType == 1 ) {	// parallel
			if ( var_TexLight.x < 0 || var_TexLight.x > 1 ||
				 var_TexLight.y < 0 || var_TexLight.y > 1 ||
				 var_TexLight.z < 0 || var_TexLight.z > 1 ) {
				temp = 0.0;
			} else {
				temp = textureCube( u_lightCubeTexture, var_TexLight.xyz * 2.0 - 1.0 ).xyz;
			}
		} else {	// default
			temp = var_TexLight.xyz * 2.0 - 1.0;
			f = clamp( 1.0 - length( temp ), 0.0, 1.0 );
			temp = f * f * textureCube( u_lightCubeTexture, temp ).xyz;
		}
	}
	
	return temp;
}
	

void main( void ) {
	gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
	
	vec3 atten = CalcAttenuation();
	
	// early out - attenuation
	if ( atten.x <= 0 && atten.y <= 0 && atten.z <= 0 ) {
		return;
	}
 
	// scale normal vector to -1.0<->1.0 range and normalize
	vec3 N = normalize( texture2D( u_normalTexture, var_TexNormal.st ).xyz * 2.0 - 1.0 );
	
	// normalize the vector to the light
	vec3 L = normalize( var_LightDir );

	// calculate diffuse cosine
	float NdotL = dot( N, L ); 
	
	// early out - NdotL
	if ( NdotL <= 0 ) {
		return;
	}

	// normalize the vector to the viewer
	vec3 V = normalize( var_ViewDir ); 
	// calculate the half angle vector and normalize
	vec3 H = normalize( L + V ); 

	// calculate cosines
	float NdotH = clamp( dot( N, H ), 0.0, 1.0 ); 
	float NdotV = clamp( dot( N, V ), 0.0, 1.0 ); 
	float VdotL = clamp( dot( V, -L ), 0.0, 1.0 ); 
	
	// load texture maps   
	vec4 diffuse = texture2D( u_diffuseTexture, var_TexDiffuse );
	diffuse.xyz = SrgbToLinear( diffuse.xyz );
	vec4 gloss = texture2D( u_specularTexture, var_TexSpecular );

	// calculate specular term
	float roughPow = gloss.w * u_specExp.x + u_specExp.y;
	float roughMul = ( roughPow + 6.0 ) * M_inv8PI;
	float fresnel = pow( ( 1.0 - NdotV ) * VdotL, 5.0 );
	gloss.xyz = mix( gloss, vec3( 1.0, 1.0, 1.0 ), fresnel );
	float specular = pow( NdotH, roughPow ) * roughMul;
	
	// final modulation
	vec3 color = ( specular * gloss + diffuse ) * u_diffuseColor * NdotL * atten;
	color.xyz = LinearToSrgb( color.xyz ) * var_Color.xyz;
	
	// output final color     
	gl_FragColor.xyz = color; 
	gl_FragColor.w = diffuse.w;
}