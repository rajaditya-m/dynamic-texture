#version 110

varying vec3 fNormal;
varying vec3 fPosition;
varying vec2 fTextureCoods;

uniform vec4 ambientPdk,diffusePdk,specularPdk;
uniform mat4 modelView;
uniform vec4 lightPos;
uniform float shininess;

uniform sampler2D texture;

void main()
{
	gl_FragColor = texture2D(texture,fTextureCoods);	
}