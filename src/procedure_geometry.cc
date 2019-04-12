#include "procedure_geometry.h"
#include "config.h"

#include <iostream>

#include "PerlinNoise.hpp"

siv::PerlinNoise *pn = new siv::PerlinNoise(3000);


float fade(float x){
    return x * x * x * (x * (x * 6 - 15) + 10);
}

float my_lerp(float a, float b, float t){
    return a + t * (b - a);
}

float gradient(int hash_v, glm::vec3 xyz){
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

float perlin(glm::vec3 xyz){
    int hash[256] = {151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

    float x = xyz[0];
    float y = xyz[1];
    float z = xyz[2];

    // // We need to index into a 256 x 256 hash table
    // int x_256 = ((int) x) & 255;
    // std::cout << "This is x_256: " << x_256 << std::endl;
    // int y_256 = ((int) y) & 255;
    // std::cout << "This is y_256: " << y_256 << std::endl;
    // int z_256 = ((int) z) & 255;
    // std::cout << "This is z_256: " << z_256 << std::endl;

    // // Coordinates within the block should range from 0.0 to 1.0
    // float local_x = x - floor(x);
    // float local_y = y - floor(y);
    // float local_z = z - floor(z);

    

    // int h_x   = hash[        x_256 ];
    // int h_x_1 = hash[(x_256 + 1) & 255];
    // int h_y = y_256;
    // int h_y_1 = ((y_256 + 1) & 255) & 255;
    // int h_z = z_256 & 255;
    // int h_z_1 = ((z_256 + 1) &  255) & 255;

    

    // int h0 = hash[hash[(h_x  +  h_y) & 255]+   h_z ];
    // int h1 = hash[hash[(h_x  +  h_y) & 255]+   h_z_1];
    // int h2 = hash[hash[(h_x  +  h_y_1) & 255]+ h_z];
    // int h3 = hash[hash[(h_x  +  h_y_1) & 255]+ h_z_1];
    // int h4 = hash[hash[(h_x_1 + h_y) & 255]+   h_z];
    // int h5 = hash[hash[(h_x_1 + h_y_1) & 255]+ h_z];
    // int h6 = hash[hash[(h_x_1 + h_y) & 255]+   h_z_1];
    // int h7 = hash[hash[(h_x_1 + h_y_1) & 255]+ h_z_1];


    // float u = fade(local_x);
    // float v = fade(local_y);
    // float w = fade(local_z);

    // float x1_avg = my_lerp(gradient(h0, glm::vec3(local_x, local_y  , local_z)), gradient(h4, glm::vec3(local_x-1, local_y  , local_z)),u);							
    // float x2_avg = my_lerp(gradient(h2, glm::vec3(local_x, local_y-1, local_z)), gradient(h5, glm::vec3(local_x-1, local_y-1, local_z)),u);
    // float y1_avg = my_lerp(x1_avg, x2_avg, v);
    //       x1_avg = my_lerp(gradient(h1, glm::vec3(local_x, local_y  , local_z-1)), gradient(h6, glm::vec3(local_x-1, local_y  , local_z-1)), u);
    //       x2_avg = my_lerp(gradient(h3, glm::vec3(local_x, local_y-1, local_z-1)), gradient(h7, glm::vec3(local_x-1, local_y-1, local_z-1)), u);
    // float y2_avg = my_lerp (x1_avg, x2_avg, v);
            
    // return (my_lerp(y1_avg, y2_avg, w)+1)/2;
    // // return xyz.y / 30.0;
    unsigned int ux = (uint) x;
    unsigned int uy = (uint) y;
    unsigned int uz = (uint) z;

    unsigned int my_h = ux + (uy << 8) ^ (uy >> 24);
    my_h += (my_h ^ uz) + (uz ^ 0x72);
    my_h = my_h ^ (my_h << 13);
    my_h = my_h ^ (my_h >> 17);
    my_h = my_h ^ (my_h << 5);
    return (float) (my_h & 0xFF) / 255.0f;

}

void generate_geometry_helper(std::vector<glm::vec4>& obj_vertices,
                        std::vector<glm::uvec3>& obj_faces,
						glm::vec4 origin,
						  int nesting_level,
						  int depth,
						  int vertex_offset)
{
	// float cube_size = 0.5f / powf(3.0f, (float)depth);

	// If our recursion level is 0, just draw a cube WITH the scale we need.
	// if (nesting_level == 0) {

		/*0*/ obj_vertices.push_back(glm::vec4(origin.x, origin.y, origin.z, 1));
		/*1*/ obj_vertices.push_back(glm::vec4(origin.x + kTileLen, origin.y, origin.z, 1));
		/*2*/ obj_vertices.push_back(glm::vec4(origin.x + kTileLen, origin.y + kTileLen, origin.z, 1));
		/*3*/ obj_vertices.push_back(glm::vec4(origin.x, origin.y + kTileLen, origin.z, 1));

		/*4*/ obj_vertices.push_back(glm::vec4(origin.x + kTileLen, origin.y, origin.z + kTileLen, 1));
		/*5*/ obj_vertices.push_back(glm::vec4(origin.x, origin.y, origin.z + kTileLen, 1));
		/*6*/ obj_vertices.push_back(glm::vec4(origin.x + kTileLen, origin.y + kTileLen, origin.z + kTileLen, 1));
		/*7*/ obj_vertices.push_back(glm::vec4(origin.x, origin.y + kTileLen, origin.z + kTileLen, 1));

        // Arbitrarily chosen front
		obj_faces.push_back(glm::uvec3(vertex_offset + 2, vertex_offset + 1, vertex_offset));
		obj_faces.push_back(glm::uvec3(vertex_offset + 3, vertex_offset + 2, vertex_offset));

        // Arbitrarily chosen back
		obj_faces.push_back(glm::uvec3(vertex_offset + 7, vertex_offset + 5, vertex_offset + 4));
		obj_faces.push_back(glm::uvec3(vertex_offset + 6, vertex_offset + 7, vertex_offset + 4));

        // Arbitrarily chosen top
		obj_faces.push_back(glm::uvec3(vertex_offset + 6, vertex_offset+2, vertex_offset+3));
		obj_faces.push_back(glm::uvec3(vertex_offset + 7, vertex_offset+6, vertex_offset+3));

        // Arbitrarily chosen bottom
		obj_faces.push_back(glm::uvec3(vertex_offset+1, vertex_offset+4, vertex_offset+5));
		obj_faces.push_back(glm::uvec3(vertex_offset, vertex_offset+1, vertex_offset+5));

        // Arbitrarily chosen right
		obj_faces.push_back(glm::uvec3(vertex_offset + 6, vertex_offset+4, vertex_offset+1));
		obj_faces.push_back(glm::uvec3(vertex_offset + 2, vertex_offset+6, vertex_offset+1));

        // Arbitrarily chosen left 
		obj_faces.push_back(glm::uvec3(vertex_offset + 3, vertex_offset, vertex_offset + 5));
		obj_faces.push_back(glm::uvec3(vertex_offset + 7, vertex_offset + 3, vertex_offset + 5));
	// } 
    // else {
	// 	// x
	// 	for (int i = 0; i < 3; i++) {
	// 		// y
	// 		for (int j = 0; j < 3; j++) {
	// 			// z
	// 			for (int k = 0; k < 3; k++) {
	// 				// if ((i == 1 && i == j) || (j == 1 && j == k) || (k == 1 && i == k)) {
	// 				// 	// Skip inner cube.
	// 				// } else {
	// 					// std::vector<glm::vec4> obj_verticesR;
	// 					// std::vector<glm::uvec3> obj_facesR;

	// 					// float signX = i - 1;
	// 					// float signY = j - 1;
	// 					// float signZ = k - 1;
	// 					// float originX = origin.x + (kTileLen - (0.5f / powf(3.0f, (float)depth+1.0f))) * signX;
	// 					// float originY = origin.y + (kTileLen - (0.5f / powf(3.0f, (float)depth+1.0f))) * signY;
	// 					// float originZ = origin.z + (kTileLen - (0.5f / powf(3.0f, (float)depth+1.0f))) * signZ;

	// 					// // int new_nesting_level = nesting_level - 1;
	// 					// // int new_depth = depth + 1;
	// 					// // glm::vec4 originR(originX, originY, originZ, 1);
	// 					// // generate_geometry_helper(obj_verticesR,
	// 					// // 						obj_facesR, 
	// 					// // 						originR,
	// 					// // 						new_nesting_level,
	// 					// // 						new_depth,
	// 					// // 						faces_offset);

	// 					// copy(obj_verticesR.begin(), obj_verticesR.end(), back_inserter(obj_vertices));
	// 					// copy(obj_facesR.begin(), obj_facesR.end(), back_inserter(obj_faces));
	// 					// faces_offset = faces_offset + obj_verticesR.size();
	// 				// }
	// 			}
	// 		}
	// 	}
	// }
}


// x, y, z
// 40, 12, 40 cubes
void create_floor(std::vector<glm::vec4>& floor_vertices, std::vector<glm::uvec3>& floor_faces)
{
    // for(float y = -30.0f; y < 30.0f; y += (float)kTileLen){
    //     for(float x = kFloorXMin; x < kFloorXMax; x += (float)kTileLen){
    //         for(float z = kFloorZMin; z < kFloorZMax; z += (float)kTileLen){
    //             // double height = pn->octaveNoise((double) x + .553, (double) y + .553, (double) z + .553, 9);
    //             // std::cout << "This is the height: " << height << std::endl;
    //             if(perlin(glm::vec3(x,y, z)) < .5){
    //                 generate_geometry_helper(floor_vertices, floor_faces, glm::vec4(x, y, z, 1), 0, 0, floor_faces.size());
    //             }
    //         }
    //     }
    // }

    // debugging single cube
    int counter = 0;
    for(float z = 0.0f; z < 100.0f; z += (float)kTileLen){
        for(float y = 0.0f; y < 50.0f; y += (float)kTileLen){
            for(float x = 0.0f; x < 100.0f; x += (float)kTileLen){
                double height = pn->octaveNoise((double) x * (1/30.0) + .001, (double) y * (1/30.0) + .001, (double) z * (1/30.0) + .001, 3);
                // std::cout << "This is the height: " << height << std::endl;
                if(height > 0.0f){
                    
                    // std::cout << "x: " << x << " y: " << y << " z: " << z << " with faces " <<  counter << " total " << floor_faces.size() << std::endl;
                    generate_geometry_helper(floor_vertices, floor_faces, glm::vec4(x, y, z, 1), 0, 0, counter);
                    counter += 8;
                }
            }
        }
    }
 

	// floor_vertices.push_back(glm::vec4(kFloorXMin, kFloorY, kFloorZMax, 1.0f));
	// floor_vertices.push_back(glm::vec4(kFloorXMax, kFloorY, kFloorZMax, 1.0f));
	// floor_vertices.push_back(glm::vec4(kFloorXMax, kFloorY, kFloorZMin, 1.0f));
	// floor_vertices.push_back(glm::vec4(kFloorXMin, kFloorY, kFloorZMin, 1.0f));
	// floor_faces.push_back(glm::uvec3(0, 1, 2));
	// floor_faces.push_back(glm::uvec3(2, 3, 0));
}

/*
 * Create Cylinder from x = -0.5 to x = 0.5
 */
void create_cylinder_mesh(LineMesh& cylinder_mesh)
{
	constexpr int kCylinderGridSizeX = 16;  // number of points in each direction
	constexpr int kCylinderGridSizeY = 3;   // number of points in each direction
	float step_x = 1.0f / (kCylinderGridSizeX - 1);
	float step_y = 1.0f / (kCylinderGridSizeY - 1);
	glm::vec3 p = glm::vec3(-0.5f, 0.0f, 0.0f);

	// Setup the vertices of the lattice.
	// Note: vertex shader is used to generate the actual cylinder
	// Extra Credit: Optionally you can use tessellation shader to draw the
	//               cylinder
	for (int i = 0; i < kCylinderGridSizeY; ++i) {
		p.x = -0.5f;
		for (int j = 0; j < kCylinderGridSizeX; ++j) {
			cylinder_mesh.vertices.push_back(glm::vec4(p, 1.0f));
			p.x += step_x;
		}
		p.y += step_y;
	}

	// Compute the indices, this is just column / row indexing for the
	// vertical line segments and linear indexing for the horizontal
	// line segments.
	for (int n = 0; n < kCylinderGridSizeX * kCylinderGridSizeY; ++n) {
		int row = n / kCylinderGridSizeX;
		int col = n % kCylinderGridSizeX;
		if (col > 0) {
			cylinder_mesh.indices.emplace_back(n - 1, n);
		}
		if (row > 0) {
			cylinder_mesh.indices.emplace_back((row - 1) * kCylinderGridSizeX + col, n);
		}
	}
}

void create_axes_mesh(LineMesh& axes_mesh)
{
	axes_mesh.vertices.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	axes_mesh.vertices.push_back(glm::vec4(2.0f, 0.0f, 0.0f, 1.0f));
	axes_mesh.vertices.push_back(glm::vec4(0.0f, 0.0f, 2.0f, 1.0f));
	axes_mesh.indices.push_back(glm::uvec2(0, 1));
	axes_mesh.indices.push_back(glm::uvec2(0, 2));
}
