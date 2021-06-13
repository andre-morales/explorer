#version 110
varying vec4 color;

#vert
void main(){
	color = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

#frag
void main(){
	gl_FragColor = color; 
}