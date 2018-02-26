/*
use input image of six directions to generate fisheye output
*/
#ifndef FISHEYE_H__
#define FISHEYE_H__

#include<string>

#define GLEW_STATIC
#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<SOIL2/SOIL2.h>

#include"shader.h"
#include"camera.h"
#include"model.h"
#include"texture.h"

class Fisheye {
public:
	const GLuint WIDTH = 512, HEIGHT = 512;
	int SCREEN_WIDTH, SCREEN_HEIGHT;

	Camera camera;

	bool keys[1024];
	GLfloat lastX = 400, lastY = 300;
	bool firstMouse = true;
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	GLFWwindow *window;
	vector<Shader> shaders;
	GLuint cubemapVAO, cubemapVBO;
	/*const string postfix[6] = {
		"_90_0.jpg",
		"_270_0.jpg",
		"_0_90.jpg",
		"_0_270.jpg",
		"_0_0.jpg",
		"_180_0.jpg"
	};*/

	/*const string postfix[6] = {
		"_90_0.bin",
		"_270_0.bin",
		"_0_90.bin",
		"_0_270.bin",
		"_0_0.bin",
		"_180_0.bin"
	};*/

	const string postfix[6] = {
		"_90_0.jpg",
		"_270_0.jpg",
		"_0_90.jpg",
		"_0_270.jpg",
		"_0_0.jpg",
		"_180_0.jpg"
	};

	//vector<const GLchar*> faces;

	void KeyCallBack(GLFWwindow *window, int key, int scancode
		, int action, int mode) {
		if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key >= 0 && key < 1024) {
			if (action == GLFW_PRESS) {
				keys[key] = true;
			}
			else if (action == GLFW_RELEASE) {
				keys[key] = false;
			}
		}
	}

	void MouseCallback(GLFWwindow *window, double xPos, double yPos) {
		if (firstMouse) {
			lastX = xPos;
			lastY = yPos;
			firstMouse = false;
		}
		GLfloat xOffset = xPos - lastX;
		GLfloat yOffset = lastY - yPos;
		lastX = xPos;
		lastY = yPos;

		camera.ProcessMouseMovement(xOffset, yOffset);
	}

	void DoMovement() {
		// Camera controls
		if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
			camera.ProcessKeyboard(FORWARD, deltaTime);
		}

		if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		}

		if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
			camera.ProcessKeyboard(LEFT, deltaTime);
		}

		if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
			camera.ProcessKeyboard(RIGHT, deltaTime);
		}
	}

public:
	int init() {
		cout << "Initialize gl" << endl;
		camera = (glm::vec3(0.0f, 0.0f, 3.0f));
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		//create glfw window
		window = glfwCreateWindow(WIDTH, HEIGHT, "cubemap"
			, nullptr, nullptr);
		if (nullptr == window) {
			std::cerr << "failed to create glfw window" << std::endl;
			glfwTerminate();
			return EXIT_FAILURE;
		}

		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
		//glfwSetKeyCallback(window, KeyCallBack);
		//glfwSetCursorPosCallback(window, MouseCallback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glewExperimental = GL_TRUE;
		//initialize glew
		if (GLEW_OK != glewInit()) {
			std::cerr << "failed to initialize glew!" << std::endl;
			return EXIT_FAILURE;
		}

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glEnable(GL_DEPTH_TEST);

		shaders.push_back(Shader("shaders/cubemap.vs", "shaders/cubemap_top.frag"));
		shaders.push_back(Shader("shaders/cubemap.vs", "shaders/cubemap_bottom.frag"));
		shaders.push_back(Shader("shaders/cubemap.vs", "shaders/cubemap_front.frag"));
		shaders.push_back(Shader("shaders/cubemap.vs", "shaders/cubemap_back.frag"));
		shaders.push_back(Shader("shaders/cubemap.vs", "shaders/cubemap_left.frag"));
		shaders.push_back(Shader("shaders/cubemap.vs", "shaders/cubemap_right.frag"));

		GLfloat cubemapVertices[] = {
			// Positions
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
		};

		glGenVertexArrays(1, &cubemapVAO);
		glGenBuffers(1, &cubemapVBO);
		glBindVertexArray(cubemapVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices),
			&cubemapVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE
			, 3 * sizeof(GLfloat), (GLvoid *)0);
		glBindVertexArray(0);

		return 0;
	}

public:
	int draw(const string & filename, const string &path) {
		//faces.clear();
		/*
		_90_0
		_270_0
		_0_90
		_0_270
		_0_0
		_180_0
		*/
		/*faces.push_back("data\\right.jpg");
		faces.push_back("data\\left.jpg");
		faces.push_back("data\\top.jpg");
		faces.push_back("data\\bottom.jpg");
		faces.push_back("data\\front.jpg");
		faces.push_back("data\\back.jpg");
		faces.push_back("data\\39.973401414928_-75.16560625035436_90_0.jpg");
		faces.push_back("data\\39.973401414928_-75.16560625035436_270_0.jpg");
		faces.push_back("data\\39.973401414928_-75.16560625035436_0_90.jpg");
		faces.push_back("data\\39.973401414928_-75.16560625035436_0_270.jpg");
		faces.push_back("data\\39.973401414928_-75.16560625035436_0_0.jpg");
		faces.push_back("data\\39.973401414928_-75.16560625035436_180_0.jpg");*/
		/*string test[6] = { "data\\right.jpg",
			"data\\left.jpg",
			"data\\top.jpg",
			"data\\bottom.jpg",
			"data\\front.jpg",
			"data\\back.jpg"};*/
		vector<string> faces;
		for (int i = 0; i < 6; i++) {
			string s = filename + postfix[i];
			//const string name = test[i];
			//GLchar* name = &test[i][0];
			//GLchar* gpuchar = &name[i][0];
			faces.push_back(s);
			//faces.push_back(name);
			//std::cout << "face: "<<faces[i] << std::endl;
		}
		//std::cout << "loading texture" << std::endl;
		GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

		/*
		std::string fileNames[6] = { "cubemap_top", "cubemap_bottom", "cubemap_left"
			, "cubemap_right", "cubemap_front", "cubemap_back" };
			*/

		glm::mat4 projection = glm::perspective(
			camera.GetZoom()
			, static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT)
			, 0.1f
			, 1000.0f
		);

		glm::mat4 model;
		glm::mat4 view = camera.GetViewMatrix();

		// Change depth function so depth test passes when values 
		// are equal to depth buffer's content
		glDepthFunc(GL_LEQUAL);
		shaders[0].Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

		glUniformMatrix4fv(glGetUniformLocation(shaders[0].Program, "view")
			, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(
			glGetUniformLocation(shaders[0].Program, "projection")
			, 1, GL_FALSE, glm::value_ptr(projection)
		);

			glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(cubemapVAO);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS);

			unsigned char* data =
				(unsigned char*)malloc(WIDTH*HEIGHT * 4 * sizeof(unsigned char));

			glReadnPixels(0, 0, WIDTH, HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE
				, WIDTH*HEIGHT * 4 * sizeof(unsigned char), data);

			SOIL_save_image_quality(
				&path[0],
				SOIL_SAVE_TYPE_JPG,
				WIDTH, HEIGHT, 4,
				data,
				100
			);
			std::free(data);
			glfwSwapBuffers(window);
			glDeleteTextures(1, &cubemapTexture);
		return 0;
	}

	Fisheye() {
		init();
	}

	~Fisheye() {
		glfwTerminate();
	}
};

#endif // !FISHEYE_H__