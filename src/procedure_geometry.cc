#include "procedure_geometry.h"
#include "config.h"
#include <glm/gtx/vector_angle.hpp>

#include <iostream>


void generate_geometry_helper(std::vector<glm::vec4>& obj_vertices,
                        std::vector<glm::uvec3>& obj_faces,
						glm::vec4 origin,
						  int nesting_level,
						  int depth,
						  int vertex_offset, GUI& gui)
{

    glm::vec4 p0 = glm::vec4(origin.x, origin.y, origin.z, 1);
    glm::vec4 p1 = glm::vec4(origin.x + kTileLen, origin.y, origin.z, 1);
    glm::vec4 p2 = glm::vec4(origin.x + kTileLen, origin.y + kTileLen, origin.z, 1);
    glm::vec4 p3 = glm::vec4(origin.x, origin.y + kTileLen, origin.z, 1);

    glm::vec4 p4 = glm::vec4(origin.x + kTileLen, origin.y, origin.z + kTileLen, 1);
    glm::vec4 p5 = glm::vec4(origin.x, origin.y, origin.z + kTileLen, 1);
    glm::vec4 p6 = glm::vec4(origin.x + kTileLen, origin.y + kTileLen, origin.z + kTileLen, 1);
    glm::vec4 p7 = glm::vec4(origin.x, origin.y + kTileLen, origin.z + kTileLen, 1);

    float angle_offset = (kFov*3.14)/180;
    bool cull = true;
    // View frustrum culling   
    for(int i = 8; i > 0; --i){
        if(glm::angle(glm::vec3(glm::normalize(p0 - glm::vec4(gui.eye_, 0.0f))), glm::vec3(gui.look_)) > angle_offset){
            cull = false;
            break;
        } else if(glm::angle(glm::vec3(glm::normalize(p1 - glm::vec4(gui.eye_, 0.0f))), glm::vec3(gui.look_)) > angle_offset){
            cull = false;
            break;
        } else if(glm::angle(glm::vec3(glm::normalize(p2 - glm::vec4(gui.eye_, 0.0f))), glm::vec3(gui.look_)) > angle_offset){
            cull = false;
            break;
        } else if(glm::angle(glm::vec3(glm::normalize(p3 - glm::vec4(gui.eye_, 0.0f))), glm::vec3(gui.look_)) > angle_offset){
            cull = false;
            break;
        } else if(glm::angle(glm::vec3(glm::normalize(p4 - glm::vec4(gui.eye_, 0.0f))), glm::vec3(gui.look_)) > angle_offset){
            cull = false;
            break;
        } else if(glm::angle(glm::vec3(glm::normalize(p5 - glm::vec4(gui.eye_, 0.0f))), glm::vec3(gui.look_)) > angle_offset){
            cull = false;
            break;
        } else if(glm::angle(glm::vec3(glm::normalize(p6 - glm::vec4(gui.eye_, 0.0f))), glm::vec3(gui.look_)) > angle_offset){
            cull = false;
            break;
        } else if(glm::angle(glm::vec3(glm::normalize(p7 - glm::vec4(gui.eye_, 0.0f))), glm::vec3(gui.look_)) > angle_offset){
            cull = false;
            break;
        }
    }

    /*0*/ obj_vertices.push_back(p0);
    /*1*/ obj_vertices.push_back(p1);
    /*2*/ obj_vertices.push_back(p2);
    /*3*/ obj_vertices.push_back(p3);

    /*4*/ obj_vertices.push_back(p4);
    /*5*/ obj_vertices.push_back(p5);
    /*6*/ obj_vertices.push_back(p6);
    /*7*/ obj_vertices.push_back(p7);
    if(true || cull){
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
    }
}


// x, y, z
// 40, 12, 40 cubes
void create_floor(std::vector<glm::vec4>& floor_vertices, std::vector<glm::uvec3>& floor_faces, const glm::vec3& displacements, PerlinNoise *pn, GUI& gui)
{
    double d_x = displacements[0];
    double d_y = displacements[1];
    double d_z = displacements[2];

    floor_vertices.clear();
    floor_faces.clear();

    int counter = 0;
    for(float z = 0.0f; z < 100.0f; z += (float)kTileLen){
        for(float y = 0.0f; y < 50.0f; y += (float)kTileLen){
            for(float x = 0.0f; x < 100.0f; x += (float)kTileLen){
                double height = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) y + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
                // std::cout << "This is the height: " << height << std::endl;
                if(height > 0.0f || y == 0.0f){
                    
                    // std::cout << "x: " << x << " y: " << y << " z: " << z << " with faces " <<  counter << " total " << floor_faces.size() << std::endl;
                    generate_geometry_helper(floor_vertices, floor_faces, glm::vec4(x, y, z, 1), 0, 0, counter, gui);
                    counter += 8;
                }
            }
        }
    }
}

void create_clouds(std::vector<glm::vec4>& clouds_vertices, std::vector<glm::uvec3>& clouds_faces, const glm::vec3& displacements,  PerlinNoise *pn, GUI& gui){
    double d_x = displacements[0];
    double d_y = displacements[1];
    double d_z = displacements[2];

    clouds_vertices.clear();
    clouds_faces.clear();
    int counter = 0;
    for(float z = -1000.0f; z < 1000.0f; z += (float)kTileLen){
        for(float y = 200.0f; y < 250.0f; y += (float)kTileLen){
            for(float x = -1000.0f; x < 1000.0f; x += (float)kTileLen){
                double height = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) y + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
                // std::cout << "This is the height: " << height << std::endl;
                if(height > 0.5f){
                    
                    // std::cout << "x: " << x << " y: " << y << " z: " << z << " with faces " <<  counter << " total " << floor_faces.size() << std::endl;
                    generate_geometry_helper(clouds_vertices, clouds_faces, glm::vec4(x, y, z, 1), 0, 0, counter, gui);
                    counter += 8;
                }
            }
        }
    }
}

void create_satelite(std::vector<glm::vec4>& satelite_vertices, std::vector<glm::uvec3>& satelite_faces, const glm::vec3& displacements, PerlinNoise *pn, GUI& gui){
    satelite_vertices.clear();
    satelite_faces.clear();

    float satelite_width = 100.0f;

    float init_x = 500.0f;
    float init_y = 50.0f;
    float init_z = 0.0f;
    float vertex_offset = 0.0f;

    /*0*/ satelite_vertices.push_back(glm::vec4(init_x, init_y, init_z, 1));
    /*1*/ satelite_vertices.push_back(glm::vec4(init_x + satelite_width, init_y, init_z, 1));
    /*2*/ satelite_vertices.push_back(glm::vec4(init_x + satelite_width, init_y + satelite_width, init_z, 1));
    /*3*/ satelite_vertices.push_back(glm::vec4(init_x, init_y + satelite_width, init_z, 1));

    /*4*/ satelite_vertices.push_back(glm::vec4(init_x + satelite_width, init_y, init_z + satelite_width, 1));
    /*5*/ satelite_vertices.push_back(glm::vec4(init_x, init_y, init_z + satelite_width, 1));
    /*6*/ satelite_vertices.push_back(glm::vec4(init_x + satelite_width, init_y + satelite_width, init_z + satelite_width, 1));
    /*7*/ satelite_vertices.push_back(glm::vec4(init_x, init_y + satelite_width, init_z + satelite_width, 1));

    // Arbitrarily chosen front
    satelite_faces.push_back(glm::uvec3(vertex_offset + 2, vertex_offset + 1, vertex_offset));
    satelite_faces.push_back(glm::uvec3(vertex_offset + 3, vertex_offset + 2, vertex_offset));

    // Arbitrarily chosen back
    satelite_faces.push_back(glm::uvec3(vertex_offset + 7, vertex_offset + 5, vertex_offset + 4));
    satelite_faces.push_back(glm::uvec3(vertex_offset + 6, vertex_offset + 7, vertex_offset + 4));

    // Arbitrarily chosen top
    satelite_faces.push_back(glm::uvec3(vertex_offset + 6, vertex_offset+2, vertex_offset+3));
    satelite_faces.push_back(glm::uvec3(vertex_offset + 7, vertex_offset+6, vertex_offset+3));

    // Arbitrarily chosen bottom
    satelite_faces.push_back(glm::uvec3(vertex_offset+1, vertex_offset+4, vertex_offset+5));
    satelite_faces.push_back(glm::uvec3(vertex_offset, vertex_offset+1, vertex_offset+5));

    // Arbitrarily chosen right
    satelite_faces.push_back(glm::uvec3(vertex_offset + 6, vertex_offset+4, vertex_offset+1));
    satelite_faces.push_back(glm::uvec3(vertex_offset + 2, vertex_offset+6, vertex_offset+1));

    // Arbitrarily chosen left 
    satelite_faces.push_back(glm::uvec3(vertex_offset + 3, vertex_offset, vertex_offset + 5));
    satelite_faces.push_back(glm::uvec3(vertex_offset + 7, vertex_offset + 3, vertex_offset + 5));
}

