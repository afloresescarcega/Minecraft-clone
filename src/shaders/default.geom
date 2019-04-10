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
out float height;

float fade(in float x){
    return x * x * x * (x * (x * 6 - 15) + 10);
}

float my_lerp(in float a, in float b, in float t){
    return a + t * (b - a);
}

float gradient(int hash_v, vec3 xyz){
    int h = hash_v & 15;
    float x = xyz.x;
    float y = xyz.y;
    float z = xyz.z;
    switch(h){
        case  0: { return  x + y; }
        case  1: { return -x + y; }
        case  2: { return  x - y; }
        case  3: { return -x - y; }
        case  4: { return  x + x; }
        case  5: { return -x + x; }
        case  6: { return  x - x; }
        case  7: { return -x - x; }
        case  8: { return  y + x; }
        case  9: { return -y + x; }
        case 10: { return  y - x; }
        case 11: { return -y - x; }
        case 12: { return  y + z; }
        case 13: { return -y + x; }
        case 14: { return  y - x; }
        case 15: { return -y - z; }
    }
}

float perlin(in vec3 xyz){
    int[256] hash = int[256](151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180);

    float x = xyz.x;
    float y = xyz.y;
    float z = xyz.z;

    // We need to index into a 256 x 256 hash table
    int x_256 = int(mod(x, 256));
    int y_256 = int(mod(y, 256));
    int z_256 = int(mod(z, 256));

    // Coordinates within the block should range from 0.0 to 1.0
    float local_x = x - floor(x);
    float local_y = y - floor(y);
    float local_z = z - floor(z);

    

    int h_x   = hash[        x_256 ];
    int h_x_1 = hash[int(mod(x_256 + 1, 256))];
    int h_y = y_256;
    int h_y_1 = int(mod(y_256 + 1, 256)) & 255;
    int h_z = z_256  & 255;
    int h_z_1 = int(mod(z_256 + 1, 256)) & 255;

    

    int h0 = hash[hash[(h_x  +  h_y) & 255]+   h_z ];
    int h1 = hash[hash[(h_x  +  h_y) & 255]+   h_z_1];
    int h2 = hash[hash[(h_x  +  h_y_1) & 255]+ h_z];
    int h3 = hash[hash[(h_x  +  h_y_1) & 255]+ h_z_1];
    int h4 = hash[hash[(h_x_1 + h_y) & 255]+   h_z];
    int h5 = hash[hash[(h_x_1 + h_y_1) & 255]+ h_z];
    int h6 = hash[hash[(h_x_1 + h_y) & 255]+   h_z_1];
    int h7 = hash[hash[(h_x_1 + h_y_1) & 255]+ h_z_1];


    float u = fade(local_x);
    float v = fade(local_y);
    float w = fade(local_z);

    float x1_avg = my_lerp(gradient(h0, vec3(local_x, local_y  , local_z)), gradient(h4, vec3(local_x-1, local_y  , local_z)),u);							
    float x2_avg = my_lerp(gradient(h2, vec3(local_x, local_y-1, local_z)), gradient(h5, vec3(local_x-1, local_y-1, local_z)),u);
    float y1_avg = my_lerp(x1_avg, x2_avg, v);
          x1_avg = my_lerp(gradient(h1, vec3(local_x, local_y  , local_z-1)), gradient(h6, vec3(local_x-1, local_y  , local_z-1)), u);
          x2_avg = my_lerp(gradient(h3, vec3(local_x, local_y-1, local_z-1)), gradient(h7, vec3(local_x-1, local_y-1, local_z-1)), u);
    float y2_avg = my_lerp (x1_avg, x2_avg, v);
            
    return (my_lerp(y1_avg, y2_avg, w)+1)/2;
    // return xyz.y / 30.0;
    unsigned int ux = uint(x);
    unsigned int uy = uint(y);
    unsigned int uz = uint(z);

    unsigned int my_h = ux;
    my_h = my_h ^ (my_h << 13);
    my_h = my_h ^ (my_h >> 17);
    my_h = my_h ^ (my_h << 5);
    return my_h & uint(255); 

}
void main() {
	int n = 0;

    // Vertices of the triangle ------
	vec3 a = gl_in[0].gl_Position.xyz;
	vec3 b = gl_in[1].gl_Position.xyz;
	vec3 c = gl_in[2].gl_Position.xyz;

    // Normals -----------------------
	vec3 u = normalize(b - a);
	vec3 v = normalize(c - a);

    /*
    * Consistent manner of choosing a same point for two triangles on a plane
    */
    float max_x = max(a.x, max(b.x, c.x));
    float max_y = max(a.y, max(b.y, c.y));
    float max_z = max(a.z, max(b.z, c.z));

    float check_width = 5.0;
    float i = floor(max_x / check_width);
	float j = floor(max_z / check_width);
    // temp.y = temp.y + (mod(i + j, 2) * check_width);

    // end of checkerboarding

    // vec3[3] temp;
    // temp[0] = vec3(gl_in[0].gl_Position.x, perlin(gl_in[0].gl_Position.xyz), gl_in[0].gl_Position.z);
    // temp[1] = vec3(gl_in[1].gl_Position.x, perlin(gl_in[1].gl_Position.xyz), gl_in[1].gl_Position.z);
    // temp[2] = vec3(gl_in[2].gl_Position.x, perlin(gl_in[2].gl_Position.xyz), gl_in[2].gl_Position.z);
    
    vec4[3] temp;

    temp[0] = vec4(a, 1.0);
    temp[1] = vec4(b, 1.0);
    temp[2] = vec4(c, 1.0);
    
    // float new_height = perlin(vec3(max_x, max_y, max_z));

    
    float new_height;
    if(max_x < 1.0){
        new_height = a.x;    
    } else {
        new_height = 1 / a.x;
    }
    temp[0].y = temp[0].y;
    temp[1].y = temp[1].y;
    temp[2].y = temp[2].y;

	face_normal = normalize(vec4(normalize(cross(u, v)), 0.0));
	for (n = 0; n < gl_in.length(); n++) {
        light_direction = normalize(vs_light_direction[n]);
        camera_direction = normalize(vs_camera_direction[n]);
        world_position = temp[n];
        vertex_normal = vs_normal[n];
        uv_coords = vs_uv[n];
        gl_Position = projection * view * model * temp[n];
        height = new_height;
        // conditionally render a whole cube if one of
        // the max_x or max_y is odd
        //if( perlin(vec3(max_x, max_y, max_z)) <= 0.5){
            EmitVertex();
        //}
	}
	EndPrimitive();
}
)zzz"
