R"zzz(
#version 330 core
in vec4 face_normal;
in vec4 vertex_normal;
in vec4 light_direction;
in vec4 world_position;
uniform vec3 world_displacement;
in float height;
out vec4 fragment_color;


float Fade(in float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float Lerp(in float t, in float a, in float b)
{
    return a + t * (b - a);
}

float gradient(in int hash, in float x, in float y, in float z)
{
    int h = int(hash & 15);
    float u = 0.0;
    float v = 0.0;
    if(h < 8){
        u = x;
    } else {
        u = y;
    }

    if(h < 4){
        v = y;
    } else {
        if(h == 12 || h == 14){
            v = x;
        } else {
            v = z;
        }
    }
    // float u = h < 8 ? x : y;
    // float v = h < 4 ? y : h == 12 || h == 14 ? x : z;

    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float perlin(in vec3 xyz){
    int[256] p = int[256](5, 83, 17, 241, 29, 117, 6, 11, 184, 80, 107, 94, 149, 253, 141, 162, 68, 167, 220, 26, 102, 79, 225, 177, 67, 20, 134, 230, 9, 43, 236, 30, 41, 208, 214, 144, 198, 62, 251, 154, 221, 174, 21, 4, 113, 13, 212, 52, 226, 88, 179, 227, 175, 24, 82, 213, 56, 132, 97, 234, 201, 166, 112, 19, 188, 53, 176, 238, 0, 122, 232, 109, 160, 116, 23, 216, 189, 247, 18, 39, 36, 27, 118, 239, 92, 165, 185, 14, 133, 209, 86, 173, 78, 101, 35, 57, 219, 158, 64, 164, 197, 76, 38, 194, 231, 211, 229, 170, 51, 163, 131, 135, 73, 61, 105, 250, 233, 199, 44, 128, 22, 130, 161, 153, 8, 228, 183, 47, 178, 71, 204, 37, 110, 171, 66, 114, 50, 46, 203, 95, 74, 143, 195, 152, 55, 215, 72, 69, 249, 181, 12, 31, 202, 186, 1, 121, 248, 138, 60, 65, 147, 49, 16, 124, 40, 156, 125, 140, 59, 224, 25, 77, 120, 245, 89, 252, 223, 98, 28, 169, 190, 126, 146, 200, 217, 242, 254, 237, 129, 87, 7, 84, 32, 235, 187, 222, 255, 111, 148, 123, 103, 142, 137, 210, 93, 168, 85, 180, 104, 54, 206, 207, 192, 119, 157, 151, 63, 246, 2, 218, 99, 136, 10, 182, 150, 91, 115, 139, 205, 70, 75, 58, 127, 172, 240, 81, 3, 196, 48, 191, 15, 34, 108, 90, 244, 159, 155, 106, 145, 193, 100, 45, 33, 42, 243, 96);

    float x = xyz.x;
    float y = xyz.y;
    float z = xyz.z;
    
    int X = int(floor(x)) & 255;
    int Y = int(floor(y)) & 255;
    int Z = int(floor(z)) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    float u = Fade(x);
    float v = Fade(y);
    float w = Fade(z);

    int A  = p[X & 255] + Y;
    int AA = p[A & 255] + Z;
    int AB = p[(A + 1) & 255] + Z;
    int B  = p[(X + 1) & 255] + Y;
    int BA = p[B & 255] + Z;
    int BB = p[(B + 1) & 255] + Z;

    return Lerp(w, Lerp(v, Lerp(u, gradient(p[AA & 255], x, y, z),
        gradient(p[BA & 255], x - 1, y, z)),
        Lerp(u, gradient(p[AB & 255], x, y - 1, z),
        gradient(p[BB & 255], x - 1, y - 1, z))),
        Lerp(v, Lerp(u, gradient(p[(AA + 1) & 255], x, y, z - 1),
        gradient(p[(BA + 1) & 255], x - 1, y, z - 1)),
        Lerp(u, gradient(p[(AB + 1) & 255], x, y - 1, z - 1),
        gradient(p[(BB + 1) & 255], x - 1, y - 1, z - 1))));
}

void main() {
    // fragment_color = vec4((1/world_position.x), 1.0 * (1/world_position.y), 1.0 * (1/world_position.z), 1.0);
	vec4 pos = world_position;
	float check_width = 5.0;
	// float i = floor(pos.x / check_width);
	// float j  = floor(pos.z / check_width);
    vec3 color;
    if(pos.y < 44.0){ // dirt
         
        if(pos.y < 30.0 + 10.0 * perlin(pos.xyz + check_width * floor(world_displacement/check_width) + .3)) { // cobble stone
            color = vec3(.56, .55, .49);
        } else {
            color = vec3(0.6, 0.46, 0.32) +  .2 * perlin(pos.xyz + check_width * floor(world_displacement/check_width) + .3);
        }
    } else { // floor will be white
        color = vec3(.12, .19, .08) +  .1 * perlin(pos.xyz + check_width * floor(world_displacement/check_width) + .3); // green floor
    }
	// vec3 color = mod(i + j, 2) * vec3(1.0, 1.0, 1.0);
	float dot_nl = 1.0 * dot(normalize(light_direction), normalize(face_normal));
	dot_nl = clamp(dot_nl, 0.0, 1.0);
	color = clamp(dot_nl * color , 0.0, 1.0);
    if(true){
        fragment_color = vec4(color, 1.0);
    } else {
        fragment_color = vec4(1.0, height, 1.0, 1.0);
    }
	fragment_color = vec4(1.0, 1.0, 1.0, 0.5);
}
)zzz"
