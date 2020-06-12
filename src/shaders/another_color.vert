#version 330


uniform mat4 pvm;

in vec4 position;
in vec4 normal;
in vec4 color;

out vec4 c;

void main()
{
	gl_Position = pvm * position ;
	c = color;

} 
