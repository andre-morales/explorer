#version 110
varying vec2 texCoord;
varying vec3 normal;
varying vec3 _vertex;
varying vec3 fragPos;
varying vec3 fragPosSun;
varying float diffuseFact;
varying float brightness;
varying float fogCoord;
uniform bool shadowed;

#vert
attribute vec3 Vertex;
attribute vec2 TexCoords;
attribute float Brightness;
attribute vec3 Normal;
uniform mat4 modelMat;
uniform mat4 transformMat;
uniform mat4 sunTransformMat;


void main(){
	texCoord = TexCoords;
	vec4 vertex = vec4(Vertex, 1);
	
	#if LIGHTING
		normal = Normal;
		vec3 sunDir = gl_LightSource[0].position.xyz;
		diffuseFact = (gl_FrontLightProduct[0].diffuse.r * max(dot(normal, sunDir), 0.0));
	#endif
	
	#if SHADOWS
		if(shadowed){
			fragPos = vec3(modelMat * vertex);
			fragPosSun = (sunTransformMat * vec4(fragPos, 1.0)).xyz;
		}
	#endif
	
	#if AMBIENT_OCCLUSION
		brightness = Brightness;
	#endif	
	
	#if FOG
		vec4 _vertexView = gl_ModelViewMatrix * modelMat * vertex;
		fogCoord = clamp((-_vertexView.z / _vertexView.w - gl_Fog.start) * gl_Fog.scale, 0.0, 1.0);
	#endif

	_vertex = Vertex;
	gl_Position = transformMat * vertex;
}

#frag
uniform sampler2D texture;
uniform sampler2D shadowMap;
uniform vec2 shadowMapTexelSize;

float shadows();

void main(){
	vec4 texColor = texture2D(texture, texCoord);
	if(texColor.a < 0.95) discard;
	
	#if AMBIENT_OCCLUSION
		float facts = brightness;
	#else
		float facts = 1.0;
	#endif
	
	#if LIGHTING
		float ambientFact = gl_FrontLightProduct[0].ambient.r;
	
		#if SHADOWS
			if(shadowed){
				facts *= ambientFact + diffuseFact * (1.0 - shadows());
			} else {
				facts *= ambientFact + diffuseFact;
			}
		#else 
			facts *= ambientFact + diffuseFact;
		#endif
	#else
		#if SHADOWS
			if(shadowed){
				facts *= shadows() * -0.40 + 1.0;
			}
		#endif
	#endif
	
	#if FOG
		gl_FragColor = mix(texColor * facts, gl_Fog.color, fogCoord);
	#else
		gl_FragColor = texColor * facts;
	#endif
}

#if SHADOWS
float shadows(){
	if(fragPosSun.z > 1.0) return 0.0;
	
	vec3 projCoords = fragPosSun * 0.5 + 0.5;
	float bias = 0.003;
	//float bias = max(0.02 * (1.0 - dot(normal, gl_LightSource[0].position.xyz)), 0.001); 
	float zDepth = projCoords.z - bias;

	float shadow = 0.0;
	#define SHFC (1.0 / SHADOWS_PCF_F)
	#if SHADOWS_PCF == 2
		float pcfDepth1 = texture2D(shadowMap, projCoords.xy).r; 
		float pcfDepth2 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, 0.0) * shadowMapTexelSize).r; 
		shadow += (zDepth > pcfDepth1) ? SHFC : 0.0;        		
		shadow += (zDepth > pcfDepth2) ? SHFC : 0.0;
	#elif SHADOWS_PCF == 3
		float pcfDepth1 = texture2D(shadowMap, projCoords.xy).r; 
		float pcfDepth2 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth3 = texture2D(shadowMap, projCoords.xy + vec2(0.0, 1.0) * shadowMapTexelSize).r;
		shadow += (zDepth > pcfDepth1) ? SHFC : 0.0;        		
		shadow += (zDepth > pcfDepth2) ? SHFC : 0.0;  		
		shadow += (zDepth > pcfDepth3) ? SHFC : 0.0; 
	#elif SHADOWS_PCF == 4
		float pcfDepth1 = texture2D(shadowMap, projCoords.xy).r; 
		float pcfDepth2 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth3 = texture2D(shadowMap, projCoords.xy + vec2(0.0, 1.0) * shadowMapTexelSize).r;
		float pcfDepth4 = texture2D(shadowMap, projCoords.xy + vec2(1.0, 0.0) * shadowMapTexelSize).r;
		shadow += (zDepth > pcfDepth1) ? SHFC : 0.0;        		
		shadow += (zDepth > pcfDepth2) ? SHFC : 0.0;  		
		shadow += (zDepth > pcfDepth3) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth4) ? SHFC : 0.0; 
	#elif SHADOWS_PCF == 5
		float pcfDepth1 = texture2D(shadowMap, projCoords.xy).r; 
		float pcfDepth2 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth3 = texture2D(shadowMap, projCoords.xy + vec2(0.0, 1.0) * shadowMapTexelSize).r;
		float pcfDepth4 = texture2D(shadowMap, projCoords.xy + vec2(1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth5 = texture2D(shadowMap, projCoords.xy + vec2(0.0, -1.0) * shadowMapTexelSize).r;
		shadow += (zDepth > pcfDepth1) ? SHFC : 0.0;        		
		shadow += (zDepth > pcfDepth2) ? SHFC : 0.0;  		
		shadow += (zDepth > pcfDepth3) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth4) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth5) ? SHFC : 0.0; 
	#elif SHADOWS_PCF == 6
		float pcfDepth1 = texture2D(shadowMap, projCoords.xy).r; 
		float pcfDepth2 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth3 = texture2D(shadowMap, projCoords.xy + vec2(0.0, 1.0) * shadowMapTexelSize).r;
		float pcfDepth4 = texture2D(shadowMap, projCoords.xy + vec2(1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth5 = texture2D(shadowMap, projCoords.xy + vec2(0.0, -1.0) * shadowMapTexelSize).r;
		float pcfDepth6 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, -1.0) * shadowMapTexelSize).r;
		shadow += (zDepth > pcfDepth1) ? SHFC : 0.0;        		
		shadow += (zDepth > pcfDepth2) ? SHFC : 0.0;  		
		shadow += (zDepth > pcfDepth3) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth4) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth5) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth6) ? SHFC : 0.0; 
	#elif SHADOWS_PCF == 7
		float pcfDepth1 = texture2D(shadowMap, projCoords.xy).r; 
		float pcfDepth2 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth3 = texture2D(shadowMap, projCoords.xy + vec2(0.0, 1.0) * shadowMapTexelSize).r;
		float pcfDepth4 = texture2D(shadowMap, projCoords.xy + vec2(1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth5 = texture2D(shadowMap, projCoords.xy + vec2(0.0, -1.0) * shadowMapTexelSize).r;
		float pcfDepth6 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, -1.0) * shadowMapTexelSize).r;
		float pcfDepth7 = texture2D(shadowMap, projCoords.xy + vec2(1.0, -1.0) * shadowMapTexelSize).r;
		shadow += (zDepth > pcfDepth1) ? SHFC : 0.0;        		
		shadow += (zDepth > pcfDepth2) ? SHFC : 0.0;  		
		shadow += (zDepth > pcfDepth3) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth4) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth5) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth6) ? SHFC : 0.0; 		
		shadow += (zDepth > pcfDepth7) ? SHFC : 0.0; 		
	#elif SHADOWS_PCF == 8
		float pcfDepth1 = texture2D(shadowMap, projCoords.xy).r; 
		float pcfDepth2 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth3 = texture2D(shadowMap, projCoords.xy + vec2(0.0, 1.0) * shadowMapTexelSize).r;
		float pcfDepth4 = texture2D(shadowMap, projCoords.xy + vec2(1.0, 0.0) * shadowMapTexelSize).r;
		float pcfDepth5 = texture2D(shadowMap, projCoords.xy + vec2(0.0, -1.0) * shadowMapTexelSize).r;
		float pcfDepth6 = texture2D(shadowMap, projCoords.xy + vec2(-1.0, -1.0) * shadowMapTexelSize).r;
		float pcfDepth7 = texture2D(shadowMap, projCoords.xy + vec2(1.0, -1.0) * shadowMapTexelSize).r;
		float pcfDepth8 = texture2D(shadowMap, projCoords.xy + vec2(1.0, 1.0) * shadowMapTexelSize).r;
		shadow += (zDepth > pcfDepth1) ? SHFC : 0.0;        		
		shadow += (zDepth > pcfDepth2) ? SHFC : 0.0;  		
		shadow += (zDepth > pcfDepth3) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth4) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth5) ? SHFC : 0.0; 
		shadow += (zDepth > pcfDepth6) ? SHFC : 0.0; 	
		shadow += (zDepth > pcfDepth7) ? SHFC : 0.0; 			
		shadow += (zDepth > pcfDepth8) ? SHFC : 0.0; 	
	#elif SHADOWS_PCF == 9
		for(int x = -1; x <= 1; ++x){
			for(int y = -1; y <= 1; ++y){
				float pcfDepth = texture2D(shadowMap, projCoords.xy + vec2(x, y) * shadowMapTexelSize).r; 
				shadow += zDepth > pcfDepth ? SHFC : 0.0;        
			}    
		}
	#else
		float texDepth = texture2D(shadowMap, projCoords.xy).r; 
		shadow = zDepth > texDepth ? 1.0 : 0.0;
	#endif
	return shadow;
}
#endif