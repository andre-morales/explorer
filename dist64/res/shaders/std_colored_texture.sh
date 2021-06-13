#version 110
varying vec2 texCoord;
varying vec4 color;

#vert
void main(){
	texCoord = gl_MultiTexCoord0.xy;
	color = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

#frag
uniform sampler2D texture;

void main(){
	gl_FragColor = color * texture2D(texture, texCoord); 
}