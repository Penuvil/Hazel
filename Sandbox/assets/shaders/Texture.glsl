// Basic Texture Shader

#type vertex
#version 450
#extension GL_KHR_vulkan_glsl :enable
			
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;

layout(binding = 0) uniform Matrices
{
	mat4 u_ViewProjection;
	mat4 u_Transform;
} mats;

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out float v_TexIndex;
layout(location = 3) out float v_TilingFactor;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;
	v_TilingFactor = a_TilingFactor;

	gl_Position = mats.u_ViewProjection * vec4(a_Position, 1.0);

}

#type fragment
#version 450
#extension GL_KHR_vulkan_glsl : enable
			
layout(location = 0) out vec4 color;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in float v_TexIndex;
layout(location = 3) in float v_TilingFactor;

layout(binding = 1) uniform Color
{
	vec4 u_Color;
	float u_TilingFactor;
} col;

layout(set = 1, binding = 2) uniform sampler2D u_Textures[32];

void main()
{
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord * v_TilingFactor) * v_Color;
}