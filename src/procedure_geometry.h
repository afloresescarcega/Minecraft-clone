#ifndef PROCEDURE_GEOMETRY_H
#define PROCEDURE_GEOMETRY_H

#include <vector>
#include <glm/glm.hpp>

#include "PerlinNoise.hpp"
#include "gui.h"

struct LineMesh {
	std::vector<glm::vec4> vertices;
	std::vector<glm::uvec2> indices;
};

class Chunk {
    std::vector<glm::vec4> vertices;
    std::vector<glm::uvec3> faces;
    float bMin;
    float bMax;
    bool dirty;
};

void create_floor(std::vector<glm::vec4>& floor_vertices, std::vector<glm::uvec3>& floor_faces, const glm::vec3& displacements,  PerlinNoise *pn, GUI& gui);
void create_clouds(std::vector<glm::vec4>& clouds_vertices, std::vector<glm::uvec3>& clouds_faces, const glm::vec3& displacements,  PerlinNoise *pn, GUI& gui);
void create_satelite(std::vector<glm::vec4>& satelite_vertices, std::vector<glm::uvec3>& satelite_faces, const glm::vec3& displacements,  PerlinNoise *pn, GUI& gui);
// void update_heights(std::vector<glm::vec4>& floor_vertices, const glm::vec3& heights);
void create_cylinder_mesh(LineMesh& cylinder_mesh);
void create_axes_mesh(LineMesh& axes_mesh);

#endif
