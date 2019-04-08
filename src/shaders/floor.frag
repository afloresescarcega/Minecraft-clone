R"zzz(
#version 330 core
in vec4 face_normal;
in vec4 vertex_normal;
in vec4 light_direction;
in vec4 world_position;
in float height;
out vec4 fragment_color;
void main() {
    // fragment_color = vec4((1/world_position.x), 1.0 * (1/world_position.y), 1.0 * (1/world_position.z), 1.0);
	vec4 pos = world_position;
	// float check_width = 5.0;
	// float i = floor(pos.x / check_width);
	// float j  = floor(pos.z / check_width);
    vec3 color;
    if(pos.y < 9.0){ // elevated will squares will be black
        color = vec3(0.6, 0.46, 0.32);
    } else { // floor will be white
        color = vec3(.12, .19, .08); // green floor
    }
	// vec3 color = mod(i + j, 2) * vec3(1.0, 1.0, 1.0);
	float dot_nl = dot(normalize(light_direction), normalize(face_normal));
	dot_nl = clamp(dot_nl, 0.0, 1.0);
	color = clamp(dot_nl * color, 0.0, 1.0);
    if(false){
        fragment_color = vec4(color, 1.0);
    } else {
        fragment_color = vec4(1.0, height, 1.0, 1.0);
    }
	
}
)zzz"
