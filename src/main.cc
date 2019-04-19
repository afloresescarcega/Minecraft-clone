#include <GL/glew.h>

#include "procedure_geometry.h"
#include "render_pass.h"
#include "config.h"
#include "gui.h"

#include <algorithm>// clamp
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <debuggl.h>

#include "PerlinNoise.hpp"
#include <chrono> // for gravity 

PerlinNoise *pn = new PerlinNoise(3000);

int window_width = 800, window_height = 600;
const std::string window_title = "Skinning";

const char* vertex_shader =
#include "shaders/default.vert"
;
const char* satelite_vertex_shader =
#include "shaders/satelite.vert"
;
const char* blending_shader =
#include "shaders/blending.vert"
;

const char* geometry_shader =
#include "shaders/default.geom"
;

const char* fragment_shader =
#include "shaders/default.frag"
;

const char* floor_fragment_shader =
#include "shaders/floor.frag"
;
const char* cloud_fragment_shader = 
#include "shaders/cloud.frag"
;
const char* satelite_fragment_shader = 
#include "shaders/satelite.frag"
;
const char* bone_vertex_shader =
#include "shaders/bone.vert"
;

const char* bone_fragment_shader =
#include "shaders/bone.frag"
;

// FIXME: Add more shaders here.

void ErrorCallback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << "\n";
}

GLFWwindow* init_glefw()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwSetErrorCallback(ErrorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	auto ret = glfwCreateWindow(window_width, window_height, window_title.data(), nullptr, nullptr);
	CHECK_SUCCESS(ret != nullptr);
	glfwMakeContextCurrent(ret);
	glewExperimental = GL_TRUE;
	CHECK_SUCCESS(glewInit() == GLEW_OK);
	glGetError();  // clear GLEW's error for it
	glfwSwapInterval(1);
	const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
	const GLubyte* version = glGetString(GL_VERSION);    // version as a string
	std::cout << "Renderer: " << renderer << "\n";
	std::cout << "OpenGL version supported:" << version << "\n";

	return ret;
}

int main(int argc, char* argv[])
{
	// if (argc < 2) {
	// 	std::cerr << "Input model file is missing" << std::endl;
	// 	std::cerr << "Usage: " << argv[0] << " <PMD file>" << std::endl;
	// 	return -1;
	// }
	GLFWwindow *window = init_glefw();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	GUI gui(window, pn);
	GLuint floor_vao;
	glGenVertexArrays(1, (GLuint*)&floor_vao);

	std::vector<glm::vec4> floor_vertices;
	std::vector<glm::uvec3> floor_faces;

    std::vector<glm::vec4> cloud_vertices;
	std::vector<glm::uvec3> cloud_faces;

    std::vector<glm::vec4> satelite_vertices;
	std::vector<glm::uvec3> satelite_faces;

    auto t0 = std::chrono::system_clock::now().time_since_epoch();
	auto t1 = std::chrono::system_clock::now().time_since_epoch();


	glm::vec4 light_position = glm::vec4(0.0f, 100.0f, 0.0f, 1.0f);
    glm::vec3 world_displace_copy = glm::vec3(0.0f, 0.0f, 0.0f);
    float time = 0;
	MatrixPointers mats; // Define MatrixPointers here for lambda to capture

	// FIXME: add more lambdas for data_source if you want to use RenderPass.
	//        Otherwise, do whatever you like here
	std::function<const glm::mat4*()> model_data = [&mats]() {
		return mats.model;
	};
	std::function<glm::mat4()> view_data = [&mats]() { return *mats.view; };
	std::function<glm::mat4()> proj_data = [&mats]() { return *mats.projection; };
	std::function<glm::mat4()> identity_mat = [](){ return glm::mat4(1.0f); };
	std::function<glm::vec3()> cam_data = [&gui](){ return gui.getCamera(); };
	std::function<glm::vec4()> lp_data = [&light_position]() { return light_position; };
    std::function<glm::vec3()> world_dis_data = [&world_displace_copy]() {return world_displace_copy; };
    std::function<float()> time_data = [&time]() {return time; };

	auto std_model = std::make_shared<ShaderUniform<const glm::mat4*>>("model", model_data);
	auto floor_model = make_uniform("model", identity_mat);
	auto std_view = make_uniform("view", view_data);
	auto std_camera = make_uniform("camera_position", cam_data);
	auto std_proj = make_uniform("projection", proj_data);
	auto std_light = make_uniform("light_position", lp_data);
    auto std_world_dis = make_uniform("world_displacement", world_dis_data);
    auto std_time = make_uniform("time", time_data);

	std::function<float()> alpha_data = [&gui]() {
		static const float transparet = 0.5; // Alpha constant goes here
		static const float non_transparet = 1.0;
		if (gui.isTransparent())
			return transparet;
		else
			return non_transparet;
	};
	auto object_alpha = make_uniform("alpha", alpha_data);

	float aspect = 0.0f;
	// std::cout << "center = " << mesh.getCenter() << "\n";

	bool draw_floor = true;
	bool draw_cylinder = true;

    // Then draw floor.
    world_displace_copy = gui.getDisplacement();

    // Floor render pass
    create_floor(floor_vertices, floor_faces, gui.getDisplacement(), pn, gui);
    RenderDataInput floor_pass_input;
    floor_pass_input.assign(0, "vertex_position", floor_vertices.data(), floor_vertices.size(), 4, GL_FLOAT);
    floor_pass_input.assignIndex(floor_faces.data(), floor_faces.size(), 3);
    RenderPass floor_pass(floor_vao,
            floor_pass_input,
            { vertex_shader, geometry_shader, floor_fragment_shader},
            { floor_model, std_view, std_proj, std_light, std_world_dis },
            { "fragment_color" }
            );

    

    // Clouds render pass
    create_clouds(cloud_vertices, cloud_faces, gui.getDisplacement(), pn, gui);
    RenderDataInput clouds_pass_input;
    clouds_pass_input.assign(0, "vertex_position", cloud_vertices.data(), cloud_vertices.size(), 4, GL_FLOAT);
    clouds_pass_input.assignIndex(cloud_faces.data(), cloud_faces.size(), 3);
    RenderPass clouds_pass(-1,
            clouds_pass_input,
            { vertex_shader, geometry_shader, cloud_fragment_shader},
            { floor_model, std_view, std_proj, std_light, std_world_dis },
            { "fragment_color" }
            );

    // sun/moon render pass
    create_satelite(satelite_vertices, satelite_faces, gui.getDisplacement(), pn, gui);
    RenderDataInput satelite_pass_input;
    satelite_pass_input.assign(0, "vertex_position", satelite_vertices.data(), satelite_vertices.size(), 4, GL_FLOAT);
    satelite_pass_input.assignIndex(satelite_faces.data(), satelite_faces.size(), 3);
    RenderPass satelite_pass(-1,
            satelite_pass_input,
            { satelite_vertex_shader, geometry_shader, satelite_fragment_shader},
            { floor_model, std_view, std_proj, std_light, std_world_dis, std_time },
            { "fragment_color" }
            );

	while (!glfwWindowShouldClose(window)) {
		// Setup some basic window stuff.
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		// glClearColor(.52f, .8f, .92f, 0.0f); // sky color
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		gui.updateMatrices();
		mats = gui.getMatrixPointers();

        world_displace_copy = gui.getDisplacement();
		// Then draw floor.
        create_floor(floor_vertices, floor_faces, gui.getDisplacement(), pn, gui);

        // Floor render pass
        floor_pass.updateVBO(0, floor_vertices.data(), floor_vertices.size());
		if (draw_floor) {
			floor_pass.setup();
			// Draw our triangles.
			CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES,
			                              floor_faces.size() * 3,
			                              GL_UNSIGNED_INT, 0));
		}

        // Draw clouds
        clouds_pass.setup();
        // Draw our triangles.
        CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES,
                                        cloud_faces.size() * 3,
                                        GL_UNSIGNED_INT, 0));

        // Draw clouds
        satelite_pass.setup();
        // Draw our triangles.
        CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES,
                                        satelite_faces.size() * 3,
                                        GL_UNSIGNED_INT, 0));
        float time_diff = time - floor(time);
        glm::vec3 sky_color = glm::vec3(1.0f);
        if(fmod(floor(time), 2.0) == 0.0){
            sky_color = glm::mix(glm::vec3(0.0f, 0.0f, .14f), glm::vec3(.52f, .8f, .92f), time_diff);
        } else {
            sky_color = glm::mix(glm::vec3(.52f, .8f, .92f), glm::vec3(0.0f, 0.0f, .14f),time_diff);
        }
        glClearColor(sky_color.x, sky_color.y, sky_color.z, 1.0f);


        // Apply gravity to character ----------------------------------
        t0 = t1;
        t1 = std::chrono::system_clock::now().time_since_epoch();
		
		std::chrono::duration<double, std::milli> fp_ms = t1 - t0;

		float dt = (float) (fp_ms.count() / 1000);
        time += .01f;

        // find the y of the nearest block underneath the player that exists
        float max_y_under_player =  kTileLen * floor(gui.eye_.y/kTileLen) - 7.5f;
        // std::cout << "actual y: " << gui.eye_.y << "floored: " << max_y_under_player << std::endl; 
        for(float i = kTileLen * floor(gui.eye_.y/kTileLen) - 7.5f; i > 0.0f; i -= kTileLen){
            // if block exists, capture it's y and break loop
            float x = gui.eye_.x;
            float y = i;
            float z = gui.eye_.z;
            float d_x = world_displace_copy[0];
            float d_y = world_displace_copy[1];
            float d_z = world_displace_copy[2];
            double height = pn->octaveNoise(1/30.0f * ((double) x  + kTileLen* floor(d_x/kTileLen)) + .01, 1/30.0f * ((double) y + kTileLen* floor(d_y/kTileLen))+ .01, 1/30.0f * ((double) z +kTileLen * floor(d_z/kTileLen))+ .01, 3);
            if(height > 0.0f){ // - kTileLen and offset because that's how the 
                // block exists,
                max_y_under_player = y + kTileLen;
                break;
            }

            if(i == 0.0f){
                max_y_under_player = -999999;
            }
        }
        if(max_y_under_player < 5.0f){
            max_y_under_player = 5.0f;  
        } else if(max_y_under_player > 50.0f){
            max_y_under_player = 55.0f;
        }
        // std::cout << "the y of the nearest block underneath the player that exists is: " << max_y_under_player << std::endl;

        if(gui.eye_.y > max_y_under_player + 7.5f){
            std::cout << "y + : " << 9.8 * 500 * (dt) * (dt) << std::endl;
            gui.eye_.y += -9.8 * 250 * (dt) * (dt);
            gui.feet_above_ground = true;
        } else {
            gui.feet_above_ground = false;
        }

        if(gui.just_jumped){
            gui.just_jumped = false;
            gui.eye_.y += 10.0f;
        }

        // float zoom_speed_ = .5f;
        // if(gui.moving_forward){
        //     gui.displacement_ += zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * gui.look_;
        //     gui.eye_ += zoom_speed_ * glm::vec3(1.0f, 0.0f, 1.0f) * gui.look_;
        // }
        // end of gravity code -----------------------------------------

		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
        
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
