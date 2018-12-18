// shader version and custom defines are inserted on compilation

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec3 in_tangent;

uniform mat4 MVP;
uniform mat3 MVI;
uniform mat4 model;

out vec3 normal;
out vec3 position;
out vec2 texcoord;
out mat3 tbn;

void main() {
	gl_Position = MVP * vec4(in_vertex, 1);
	
	position = vec3(model * vec4(in_vertex, 1.0));
	normal = normalize(MVI * in_normal);
	vec3 tangent = normalize(MVI * in_tangent);
	vec3 bitangent = cross(normal, tangent);

	tbn = mat3(tangent, bitangent, normal);

	texcoord = in_texcoord;
}