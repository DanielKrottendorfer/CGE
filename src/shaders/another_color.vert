#version 330


uniform mat4 pvm;

in vec4 position;
in vec4 normal;
in vec2 uvs;

out vec2 uv;

void main()
{
	gl_Position = pvm * position ;
	uv = uvs;

}
