#version 110

varying vec3 fNormal;
varying vec3 fPosition;
varying vec2 fTextureCoods;

uniform sampler2D texture;

void main()
{
	gl_FragColor = texture2D(texture,fTextureCoods);	
}