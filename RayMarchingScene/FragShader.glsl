#version 430

layout(binding = 0) uniform sampler2D rayMarchTexture;

in vec2 tc;
out vec4 color;

void main(void) {
	color = texture(rayMarchTexture , tc);
	//color = vec4(1.0, 0, 0, 1);
}