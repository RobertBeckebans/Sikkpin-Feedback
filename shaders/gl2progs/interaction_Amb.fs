#version 120

varying vec3		var_Position;
varying vec3		var_LightDir;
varying vec3		var_ViewDir;
varying vec2		var_TexDiffuse;
varying vec2		var_TexNormal;
varying vec2		var_TexSpecular;
varying vec4		var_TexLight;
varying mat3		var_ModelToWorldMatrix;
varying vec4		var_Color;

uniform samplerCube	u_lightCubeTexture;
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

uniform vec4		u_localParm0;
uniform vec4		u_localParm1;

#define M_inv8PI	0.039788735772973833942220940843129
#define M_inv128	0.018315638888734180293718021273241

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

void main( void ) {
	gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
	
	// calculate point light tc
	vec3 atten;
	atten.x = texture2D( u_lightFalloffTexture, vec2( var_TexLight.x, 0.5 ) ).x;
	atten.y = texture2D( u_lightFalloffTexture, vec2( var_TexLight.y, 0.5 ) ).y;
	atten.z = texture2D( u_lightFalloffTexture, vec2( var_TexLight.z, 0.5 ) ).z;
	atten = atten.x * atten.y * atten.z;
	if ( var_TexLight.x < 0 || var_TexLight.x > 1 ||
		 var_TexLight.y < 0 || var_TexLight.y > 1 ||
		 var_TexLight.z < 0 || var_TexLight.z > 1 ||
		 atten.x <= 0 ) {
		return;
	}
	 
	// normalize the vector to the viewer
	vec3 wV = normalize( var_ViewDir ); 
	
	// scale normal vector to -1.0<->1.0 range and normalize
	vec3 N = normalize( texture2D( u_normalTexture, var_TexNormal.st ).xyz * 2.0 - 1.0 );
	vec3 wN = N.xyz * var_ModelToWorldMatrix;
	vec3 nSquared = wN * wN;
	vec3 nSign;
	if ( wN.x < 0.0 ) nSign.x = -1.0; else nSign.x = 1.0;
	if ( wN.y < 0.0 ) nSign.y = -1.0; else nSign.y = 1.0;
	if ( wN.z < 0.0 ) nSign.z = -1.0; else nSign.z = 1.0;
	
	// sample ambient cube map
	vec3 C1 = textureCube( u_lightCubeTexture, vec3( nSign.x, 0.0, 0.0 ) ).xyz;
	vec3 C2 = textureCube( u_lightCubeTexture, vec3( 0.0, nSign.y, 0.0 ) ).xyz;
	vec3 C3 = textureCube( u_lightCubeTexture, vec3( 0.0, 0.0, nSign.z ) ).xyz;
	vec3 ambient = C1 * nSquared.x + C2 * nSquared.y + C3 * nSquared.z;
	
	vec4 specular = textureCube( u_lightCubeTexture, reflect( -wV, wN ) );
	specular.xyz *= specular.w;
	
	ambient.xyz = SrgbToLinear( ambient.xyz );
	specular.xyz = SrgbToLinear( specular.xyz );
	
	// load texture maps   
	vec4 diffuse = texture2D( u_diffuseTexture, var_TexDiffuse );
	diffuse.xyz = SrgbToLinear( diffuse.xyz );
	vec4 gloss = texture2D( u_specularTexture, var_TexSpecular );

	// calculate specular term
	float NdotV = clamp( dot( wN, wV ), 0.0, 1.0 ); 
	float roughPow = gloss.w * u_specExp.x + u_specExp.y;
	float roughMul = clamp( ( roughPow + 2.0 ) * M_inv8PI, 0.0, 1.0 );
	float fresnel = pow( clamp( 1.0 - NdotV, 0.0, 1.0 ), 5.0 );
	gloss.xyz = ( gloss * fresnel + gloss ) * roughMul;
	
	// final modulation
	vec3 color = ( gloss * specular + diffuse * ambient ) * u_diffuseColor * atten;
	color.xyz = LinearToSrgb( color.xyz ) * var_Color.xyz;
	
	// output final color     
	gl_FragColor.xyz = color; 
	gl_FragColor.w = diffuse.w;
}