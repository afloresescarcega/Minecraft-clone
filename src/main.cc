#include <GL/glew.h>

#include "procedure_geometry.h"
#include "render_pass.h"
#include "config.h"
#include "gui.h"

#include <algorithm>
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

siv::PerlinNoise *pn = new siv::PerlinNoise(3000);

int window_width = 800, window_height = 600;
const std::string window_title = "Skinning";

const char* vertex_shader =
#include "shaders/default.vert"
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

	GUI gui(window);
	GLuint floor_vao;
	glGenVertexArrays(1, (GLuint*)&floor_vao);

	std::vector<glm::vec4> floor_vertices;
	std::vector<glm::uvec3> floor_faces;


	LineMesh cylinder_mesh;
	LineMesh axes_mesh;

	// FIXME: we already created meshes for cylinders. Use them to render
	//        the cylinder and axes if required by the assignment.
	create_cylinder_mesh(cylinder_mesh);
	create_axes_mesh(axes_mesh);

	// Mesh mesh;
	// mesh.loadPmd(argv[1]);
	// std::cout << "Loaded object  with  " << mesh.vertices.size()
	// 	<< " vertices and " << mesh.faces.size() << " faces.\n";

	// glm::vec4 mesh_center = glm::vec4(0.0f);
	// for (size_t i = 0; i < mesh.vertices.size(); ++i) {
	// 	mesh_center += mesh.vertices[i];
	// }
	// mesh_center /= mesh.vertices.size();

	/*
	 * GUI object needs the mesh object for bone manipulation.
	 */
	// gui.assignMesh(&mesh);

	glm::vec4 light_position = glm::vec4(0.0f, 100.0f, 0.0f, 1.0f);
    glm::vec3 world_displace_copy = glm::vec3(0.0f, 0.0f, 0.0f);
	MatrixPointers mats; // Define MatrixPointers here for lambda to capture

	/*
	 * In the following we are going to define several lambda functions as
	 * the data source of GLSL uniforms
	 *
	 * Introduction about lambda functions:
	 *      http://en.cppreference.com/w/cpp/language/lambda
	 *      http://www.stroustrup.com/C++11FAQ.html#lambda
	 *
	 * Note: lambda expressions cannot be converted to std::function directly
	 *       Hence we need to declare the data function explicitly.
	 *
	 * CAVEAT: DO NOT RETURN const T&, which compiles but causes
	 *         segfaults.
	 *
	 * Do not worry about the efficient issue, copy elision in C++ 17 will
	 * minimize the performance impact.
	 *
	 * More details about copy elision:
	 *      https://en.cppreference.com/w/cpp/language/copy_elision
	 */

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

	auto std_model = std::make_shared<ShaderUniform<const glm::mat4*>>("model", model_data);
	auto floor_model = make_uniform("model", identity_mat);
	auto std_view = make_uniform("view", view_data);
	auto std_camera = make_uniform("camera_position", cam_data);
	auto std_proj = make_uniform("projection", proj_data);
	auto std_light = make_uniform("light_position", lp_data);
    auto std_world_dis = make_uniform("world_displacement", world_dis_data);

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
    create_floor(floor_vertices, floor_faces, gui.getDisplacement(), pn);

    // Floor render pass
    RenderDataInput floor_pass_input;
    floor_pass_input.assign(0, "vertex_position", floor_vertices.data(), floor_vertices.size(), 4, GL_FLOAT);
    floor_pass_input.assignIndex(floor_faces.data(), floor_faces.size(), 3);
    RenderPass floor_pass(floor_vao,
            floor_pass_input,
            { vertex_shader, geometry_shader, floor_fragment_shader},
            { floor_model, std_view, std_proj, std_light, std_world_dis },
            { "fragment_color" }
            );

	while (!glfwWindowShouldClose(window)) {
		// Setup some basic window stuff.
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		glClearColor(.52f, .8f, .92f, 0.0f); // sky color
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
        create_floor(floor_vertices, floor_faces, gui.getDisplacement(), pn);

        // Floor render pass
        floor_pass.updateVBO(0, floor_vertices.data(), floor_vertices.size());
		if (draw_floor) {
			floor_pass.setup();
			// Draw our triangles.
			CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES,
			                              floor_faces.size() * 3,
			                              GL_UNSIGNED_INT, 0));
		}

		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
        
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
