// Basic Texture Shader

#type vertex
#version 320 es

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) uniform mat4 u_ViewProjection;
layout(location = 1) uniform mat4 u_Transform;


layout(location = 0) out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 320 es
#ifdef GL_ES
precision mediump float;
#endif

layout(location = 0) out vec4 color;

layout (location = 0) in vec2 v_TexCoord;

layout(location = 2) uniform vec4 u_Color;
layout(location = 3) uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture, v_TexCoord * 10.0) * u_Color;
}