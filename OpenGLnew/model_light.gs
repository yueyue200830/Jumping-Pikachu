#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
    vec2 texCoords;
} gs_in[];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
	vec3 direction;
	vec4 temp;
	if (time > 0.0) {
		direction = normal * (-log(1.0- time)) * magnitude; 
		temp = position + vec4(direction, 0.0);
	} else {
		temp = position;
	}
    return temp;
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {    
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
	Normal = gs_in[0].Normal;
	FragPos = gs_in[0].FragPos;
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
	Normal = gs_in[1].Normal;
	FragPos = gs_in[1].FragPos;
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
	Normal = gs_in[2].Normal;
	FragPos = gs_in[2].FragPos;
    TexCoords = gs_in[2].texCoords;
    EmitVertex();
    EndPrimitive();
}