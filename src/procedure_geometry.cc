#include "procedure_geometry.h"
#include "config.h"

void generate_geometry_helper(std::vector<glm::vec4>& obj_vertices,
                          std::vector<glm::uvec3>& obj_faces, 
						  glm::vec4 origin,
						  int nesting_level,
						  int depth,
						  int faces_offset)
{
	float cube_size = 0.5f / powf(3.0f, (float)depth);

	// If our recursion level is 0, just draw a cube WITH the scale we need.
	if (nesting_level == 0) {

		obj_vertices.push_back(glm::vec4(origin.x - kTileLen, origin.y - kTileLen, origin.z - kTileLen, 1));
		obj_vertices.push_back(glm::vec4(origin.x - kTileLen, origin.y + kTileLen, origin.z - kTileLen, 1));
		obj_vertices.push_back(glm::vec4(origin.x + kTileLen, origin.y + kTileLen, origin.z - kTileLen, 1));
		obj_vertices.push_back(glm::vec4(origin.x + kTileLen, origin.y - kTileLen, origin.z - kTileLen, 1));

		obj_vertices.push_back(glm::vec4(origin.x - kTileLen, origin.y - kTileLen, origin.z + kTileLen, 1));
		obj_vertices.push_back(glm::vec4(origin.x - kTileLen, origin.y + kTileLen, origin.z + kTileLen, 1));
		obj_vertices.push_back(glm::vec4(origin.x + kTileLen, origin.y + kTileLen, origin.z + kTileLen, 1));
		obj_vertices.push_back(glm::vec4(origin.x + kTileLen, origin.y - kTileLen, origin.z + kTileLen, 1));

		obj_faces.push_back(glm::uvec3(faces_offset, faces_offset+1, faces_offset+2));
		obj_faces.push_back(glm::uvec3(faces_offset, faces_offset+2, faces_offset+3));

		obj_faces.push_back(glm::uvec3(faces_offset+6, faces_offset+5, faces_offset+4));
		obj_faces.push_back(glm::uvec3(faces_offset+7, faces_offset+6, faces_offset+4));

		obj_faces.push_back(glm::uvec3(faces_offset+3, faces_offset+2, faces_offset+6));
		obj_faces.push_back(glm::uvec3(faces_offset+3, faces_offset+6, faces_offset+7));

		obj_faces.push_back(glm::uvec3(faces_offset+4, faces_offset+5, faces_offset+1));
		obj_faces.push_back(glm::uvec3(faces_offset+4, faces_offset+1, faces_offset));

		obj_faces.push_back(glm::uvec3(faces_offset+1, faces_offset+5, faces_offset+2));
		obj_faces.push_back(glm::uvec3(faces_offset+5, faces_offset+6, faces_offset+2));

		obj_faces.push_back(glm::uvec3(faces_offset+4, faces_offset,   faces_offset+3));
		obj_faces.push_back(glm::uvec3(faces_offset+4, faces_offset+3, faces_offset+7));
	} else {
		// x
		for (int i = 0; i < 3; i++) {
			// y
			for (int j = 0; j < 3; j++) {
				// z
				for (int k = 0; k < 3; k++) {
					if ((i == 1 && i == j) || (j == 1 && j == k) || (k == 1 && i == k)) {
						// Skip inner cube.
					} else {
						std::vector<glm::vec4> obj_verticesR;
						std::vector<glm::uvec3> obj_facesR;

						float signX = i - 1;
						float signY = j - 1;
						float signZ = k - 1;
						float originX = origin.x + (kTileLen - (0.5f / powf(3.0f, (float)depth+1.0f))) * signX;
						float originY = origin.y + (kTileLen - (0.5f / powf(3.0f, (float)depth+1.0f))) * signY;
						float originZ = origin.z + (kTileLen - (0.5f / powf(3.0f, (float)depth+1.0f))) * signZ;

						int new_nesting_level = nesting_level - 1;
						int new_depth = depth + 1;
						glm::vec4 originR(originX, originY, originZ, 1);
						generate_geometry_helper(obj_verticesR,
												obj_facesR, 
												originR,
												new_nesting_level,
												new_depth,
												faces_offset);

						copy(obj_verticesR.begin(), obj_verticesR.end(), back_inserter(obj_vertices));
						copy(obj_facesR.begin(), obj_facesR.end(), back_inserter(obj_faces));
						faces_offset = faces_offset + obj_verticesR.size();
					}
				}
			}
		}
	}
}

void create_floor(std::vector<glm::vec4>& floor_vertices, std::vector<glm::uvec3>& floor_faces)
{
    for(float x = kFloorXMin; x < kFloorXMax; x += (float)kTileLen){
        for(float z = kFloorZMin; z < kFloorZMax; z += (float)kTileLen){
            generate_geometry_helper(floor_vertices, floor_faces, glm::vec4(x, 0, z, 1), 0, 0, floor_faces.size());
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
