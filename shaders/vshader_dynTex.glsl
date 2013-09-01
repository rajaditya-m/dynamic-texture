#version 110 

attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec2 vTextureCoods;

varying vec3 fNormal;
varying vec3 fPosition;
varying vec2 fTextureCoods;
//varying vec3 fL;

uniform mat4 modelView;
uniform mat4 projection;
//uniform vec4 lightPos;

void main()
{
	fNormal = vNormal;
	fPosition = vPosition.xyz;
	fTextureCoods = vTextureCoods;
	
	gl_Position = projection * modelView * vPosition;

}

