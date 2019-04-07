R"zzz(#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec4 light_position;
in vec4 vs_light_direction[];
in vec4 vs_camera_direction[];
in vec4 vs_normal[];
in vec2 vs_uv[];
out vec4 face_normal;
out vec4 light_direction;
out vec4 camera_direction;
out vec4 world_position;
out vec4 vertex_normal;
out vec2 uv_coords;
void main() {
	int n = 0;
	vec3 a = gl_in[0].gl_Position.xyz;
	vec3 b = gl_in[1].gl_Position.xyz;
	vec3 c = gl_in[2].gl_Position.xyz;
	vec3 u = normalize(b - a);
	vec3 v = normalize(c - a);

    // checkerboarding
    float max_x = max(a.x, max(b.x, c.x));
    float max_z = max(a.z, max(b.z, c.z));

    float check_width = 5.0;
    float i = floor(max_x / check_width);
	float j = floor(max_z / check_width);
    // temp.y = temp.y + (mod(i + j, 2) * check_width);

    // end of checkerboarding

	face_normal = normalize(vec4(normalize(cross(u, v)), 0.0));
	for (n = 0; n < gl_in.length(); n++) {
        light_direction = normalize(vs_light_direction[n]);
        camera_direction = normalize(vs_camera_direction[n]);
        world_position = gl_in[n].gl_Position;
        vertex_normal = vs_normal[n];
        uv_coords = vs_uv[n];
        gl_Position = projection * view * model * gl_in[n].gl_Position;

        // conditionally render a whole cube if one of
        // the max_x or max_y is odd
        if(mod(max_x, 2) != 0 && mod(max_z, 2) != 0){
            EmitVertex();
        }
	}
	EndPrimitive();
}
)zzz"
