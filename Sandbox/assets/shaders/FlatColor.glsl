// Flat Color Shader

#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

layout(binding = 0) uniform Matrices
{
	mat4 u_ViewProjection;
	mat4 u_Transform;
} mats;

void main()
{
	gl_Position = mats.u_ViewProjection * mats.u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 color;

layout(binding = 1) uniform Color
{
	vec4 u_Color;
} col;

void main()
{
	color = col.u_Color;
}