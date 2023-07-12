R"zzz(
#version 330 core
uniform vec4 light_position;
uniform vec3 camera_position;
in vec4 vertex_position;
in vec4 normal;
in vec2 uv;
out vec4 vs_light_direction;
out vec4 vs_normal;
out vec2 vs_uv;
out vec4 vs_camera_direction;
void main() {
    // float check_width = 5.0;
    vec4 temp = vertex_position;
    // float i = floor(temp.x / check_width);
	// float j  = floor(temp.z / check_width);
    // temp.y = temp.y + (mod(i + j, 2) * 5);
	gl_Position = temp;
	vs_light_direction = light_position - gl_Position;
	vs_camera_direction = vec4(camera_position, 1.0) - gl_Position;
	vs_normal = normal;
	vs_uv = uv;
}
)zzz"
