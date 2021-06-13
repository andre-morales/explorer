#version 110
varying vec2 texCoord;

#vert
void main(){
	texCoord = gl_MultiTexCoord0.xy;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

#frag
uniform sampler2D texture;

void main(){
	//if(texture2D(texture, texCoord).a < 0.5) discard;
}