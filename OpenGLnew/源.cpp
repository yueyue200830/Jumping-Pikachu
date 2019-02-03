#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <math.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//jump
float jumpTime = -5.0f;
float jumppositionz = 0.0f;
float jumplength = 0.0f;
bool isjumping = false;
bool goingtojump = false;
bool isdying = false;
float dying = 0.0f;

//ground
float vertices[800000];

//floating
float floatingTime = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Jumping Pikachu", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("model_light.vs", "model_light.fs", "model_light.gs");
	Shader groundShader("ground_light.vs", "ground_light.fs");
	Shader asteroidShader("asteroids.vs", "asteroids.fs");

	// load models
	// -----------
	Model ourModel("Pikachu/PikachuM_ColladaMax.DAE");
	Model rock("rock/rock.obj");


	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// positions          // normals           // texture coords
	float pi = acos(-1);
	//圆面
	for (float i = 0; i < 200; i++) {
		//the first point
		vertices[int(i) * 8 * 3] = cos(2 * pi / 200 * i);
		vertices[int(i) * 8 * 3 + 1] = sin(2 * pi / 200 * i);
		vertices[int(i) * 8 * 3 + 2] = 0.0f;
		vertices[int(i) * 8 * 3 + 3] = 0.0f;
		vertices[int(i) * 8 * 3 + 4] = 0.0f;
		vertices[int(i) * 8 * 3 + 5] = 1.0f;
		vertices[int(i) * 8 * 3 + 6] = cos(2 * pi / 200 * i) / 2.0f + 0.5f;
		vertices[int(i) * 8 * 3 + 7] = sin(2 * pi / 200 * i) / 2.0f + 0.5f;
		//the second point
		vertices[int(i) * 8 * 3 + 8] = cos(2 * pi / 200 * (i + 1));
		vertices[int(i) * 8 * 3 + 9] = sin(2 * pi / 200 * (i + 1));
		vertices[int(i) * 8 * 3 + 10] = 0.0f;
		vertices[int(i) * 8 * 3 + 11] = 0.0f;
		vertices[int(i) * 8 * 3 + 12] = 0.0f;
		vertices[int(i) * 8 * 3 + 13] = 1.0f;
		vertices[int(i) * 8 * 3 + 14] = cos(2 * pi / 200 * (i + 1)) / 2.0f + 0.5f;
		vertices[int(i) * 8 * 3 + 15] = sin(2 * pi / 200 * (i + 1)) / 2.0f + 0.5f;
		//the center point
		vertices[int(i) * 8 * 3 + 16] = 0.0f;
		vertices[int(i) * 8 * 3 + 17] = 0.0f;
		vertices[int(i) * 8 * 3 + 18] = 0.0f;
		vertices[int(i) * 8 * 3 + 19] = 0.0f;
		vertices[int(i) * 8 * 3 + 20] = 0.0f;
		vertices[int(i) * 8 * 3 + 21] = 1.0f;
		vertices[int(i) * 8 * 3 + 22] = 0.5f;
		vertices[int(i) * 8 * 3 + 23] = 0.5f;
	}
	//圆柱面
	for (float i = 0; i < 200; i++) {
		int j = 200 * 24 + int(i) * 8 * 6;
		//the first point
		vertices[j++] = cos(2 * pi / 200 * i);
		vertices[j++] = sin(2 * pi / 200 * i);
		vertices[j++] = 0.0f;
		vertices[j++] = cos(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = sin(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = 0.0f;
		vertices[j++] = 0.0f;
		vertices[j++] = 1.0f;
		//the second point
		vertices[j++] = cos(2 * pi / 200 * (i + 1));
		vertices[j++] = sin(2 * pi / 200 * (i + 1));
		vertices[j++] = 0.0f;
		vertices[j++] = cos(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = sin(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = 0.0f;
		vertices[j++] = 1.0f;
		vertices[j++] = 1.0f;
		//the third point
		vertices[j++] = cos(2 * pi / 200 * i);
		vertices[j++] = sin(2 * pi / 200 * i);
		vertices[j++] = -0.2f;
		vertices[j++] = cos(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = sin(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = 0.0f;
		vertices[j++] = 0.0f;
		vertices[j++] = 0.0f;

		//another
		//the first point
		vertices[j++] = cos(2 * pi / 200 * i);
		vertices[j++] = sin(2 * pi / 200 * i);
		vertices[j++] = -0.2f;
		vertices[j++] = cos(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = sin(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = 0.0f;
		vertices[j++] = 0.0f;
		vertices[j++] = 0.0f;
		//the second point
		vertices[j++] = cos(2 * pi / 200 * (i + 1));
		vertices[j++] = sin(2 * pi / 200 * (i + 1));
		vertices[j++] = 0.0f;
		vertices[j++] = cos(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = sin(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = 0.0f;
		vertices[j++] = 1.0f;
		vertices[j++] = 1.0f;
		//the third point
		vertices[j++] = cos(2 * pi / 200 * (i + 1));
		vertices[j++] = sin(2 * pi / 200 * (i + 1));
		vertices[j++] = -0.2f;
		vertices[j++] = cos(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = sin(2 * pi / 200 * (i + 0.5f));
		vertices[j++] = 0.0f;
		vertices[j++] = 1.0f;
		vertices[j++] = 0.0f;
	}
	//球
	for (float i = 0; i < 200; i++) {
		for (float j = 0; j < 50; j++) {
			int k = 200 * 24 * 3 + int(i) * 50 * 48 + int(j) * 48;
			float ball_x = pi / 100 * (i + 0.5);
			float ball_y = pi / 100 * (j + 0.5);
			//the first point
			float ball_a = pi / 100 * i;
			float ball_b = pi / 100 * j;
			vertices[k++] = cos(ball_a) * cos(ball_b);
			vertices[k++] = sin(ball_a) * cos(ball_b);
			vertices[k++] = -0.2f - sin(ball_b);
			vertices[k++] = cos(ball_x) * cos(ball_y);
			vertices[k++] = sin(ball_x) * cos(ball_y);
			vertices[k++] = -sin(ball_y);
			vertices[k++] = cos(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			vertices[k++] = sin(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			//the second point
			ball_a = pi / 100 * (i + 1);
			vertices[k++] = cos(ball_a) * cos(ball_b);
			vertices[k++] = sin(ball_a) * cos(ball_b);
			vertices[k++] = -0.2f - sin(ball_b);
			vertices[k++] = cos(ball_x) * cos(ball_y);
			vertices[k++] = sin(ball_x) * cos(ball_y);
			vertices[k++] = -sin(ball_y);
			vertices[k++] = cos(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			vertices[k++] = sin(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			//the third point
			ball_b = pi / 100 * (j + 1);
			vertices[k++] = cos(ball_a) * cos(ball_b);
			vertices[k++] = sin(ball_a) * cos(ball_b);
			vertices[k++] = -0.2f - sin(ball_b);
			vertices[k++] = cos(ball_x) * cos(ball_y);
			vertices[k++] = sin(ball_x) * cos(ball_y);
			vertices[k++] = -sin(ball_y);
			vertices[k++] = cos(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			vertices[k++] = sin(ball_a) * cos(ball_b) / 2.0f + 0.5f;

			//another 
			//the first point
			vertices[k++] = cos(ball_a) * cos(ball_b);
			vertices[k++] = sin(ball_a) * cos(ball_b);
			vertices[k++] = -0.2f - sin(ball_b);
			vertices[k++] = cos(ball_x) * cos(ball_y);
			vertices[k++] = sin(ball_x) * cos(ball_y);
			vertices[k++] = -sin(ball_y);
			vertices[k++] = cos(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			vertices[k++] = sin(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			//the second point
			ball_a = pi / 100 * i;
			vertices[k++] = cos(ball_a) * cos(ball_b);
			vertices[k++] = sin(ball_a) * cos(ball_b);
			vertices[k++] = -0.2f - sin(ball_b);
			vertices[k++] = cos(ball_x) * cos(ball_y);
			vertices[k++] = sin(ball_x) * cos(ball_y);
			vertices[k++] = -sin(ball_y);
			vertices[k++] = cos(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			vertices[k++] = sin(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			//the third point
			ball_b = pi / 100 * j;
			vertices[k++] = cos(ball_a) * cos(ball_b);
			vertices[k++] = sin(ball_a) * cos(ball_b);
			vertices[k++] = -0.2f - sin(ball_b);
			vertices[k++] = cos(ball_x) * cos(ball_y);
			vertices[k++] = sin(ball_x) * cos(ball_y);
			vertices[k++] = -sin(ball_y);
			vertices[k++] = cos(ball_a) * cos(ball_b) / 2.0f + 0.5f;
			vertices[k++] = sin(ball_a) * cos(ball_b) / 2.0f + 0.5f;
		}
	}

	//ground position
	glm::vec3 groundPositions[] = {
		glm::vec3(0.0f, -0.95f, 0.0f),
		glm::vec3(0.0f, -0.95f, 3.0f)
	};

	// ground VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture("pokeball.png");
	unsigned int diffuseMap2 = loadTexture("red.png");
	//unsigned int specularMap = loadTexture("container2_specular.png");
	
	// shader configuration
	// --------------------
	ourShader.use();
	ourShader.setInt("material.diffuse", 0);
	ourShader.setInt("material.specular", 1);
	
	groundShader.use();
	groundShader.setInt("material.diffuse", 0);
	groundShader.setInt("material.specular", 1);

	//Shadertest.use();

	// generate a large list of semi-random model transformation matrices
	// ------------------------------------------------------------------
	unsigned int amount = 10000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	float radius = 25.0f;
	float offset = 18.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = displacement * 0.4f + (float)i / 10.0f - 30.0f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 200.0f + 0.02;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	// configure instanced array
	// -------------------------
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	// set transformation matrices as an instance vertex attribute (with divisor 1)
	// note: we're cheating a little by taking the, now publicly declared, VAO of the model's mesh(es) and adding new vertexAttribPointers
	// normally you'd want to do this in a more organized fashion, but for learning purposes this will do.
	// -----------------------------------------------------------------------------------------------------------------------------------
	for (unsigned int i = 0; i < rock.meshes.size(); i++)
	{
		unsigned int VAO = rock.meshes[i].VAO;
		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float floating = 0.0f;

		//jumping
		float jumpheight = 0.0f;
		float jumprotate = 0.0f;
		if (isdying) {
			if (dying > 1.0f) {
				groundPositions[0].z = 0.0f;
				groundPositions[1].z = 3.0f;
				floatingTime = 0.0f;
				jumppositionz = 0.0f;
				camera.Reset();
				dying = 0.0f;
				isdying = false;
			}
			else {
				dying += deltaTime;
			}
		}
		if (currentFrame - jumpTime > 1.0f && isjumping) {
			isjumping = false;
			srand((int)time(0));
			int dis = rand() % 1000;
			groundPositions[0] = groundPositions[1];
			groundPositions[1].z += 2.0f + float(dis) / 200.0f;
			//std::cout << "over" << std::endl;
			if (jumppositionz >= groundPositions[0].z + 0.73f || jumppositionz <= groundPositions[0].z - 0.90) {
				isdying = true;
			}
		}
		if (isjumping) {
			jumpheight = 1.0f - 4 * (currentFrame - jumpTime - 0.5f)*(currentFrame - jumpTime - 0.5f);
			if (jumplength > 1.0f)
				jumpheight *= jumplength;
			jumpheight *= 4.0f;
			jumppositionz += deltaTime * jumplength * 4;
			jumprotate = -(currentFrame - jumpTime) * pi * 2;
		}
		else {
			floatingTime += deltaTime;
			floating = sin(floatingTime) / 4.0f;
			groundPositions[0].y = -0.95f + floating;
			groundPositions[1].y = -0.95f + floating;
			//std::cout << floatingTime << "," << floating << std::endl;
		}


		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// don't forget to enable shader before setting uniforms
		ourShader.use();
		ourShader.setFloat("material.shininess", 32.0f);
		ourShader.setVec3("viewPos", camera.Position);

		// directional light
		ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f); // 光源
		ourShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f); // 环境光照
		ourShader.setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f); // 漫反射
		ourShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f); // 镜面光照，最亮点
		
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		
		
		// render the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -0.9f + jumpheight + floating, jumppositionz)); // 移动
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));	// 缩小
		model = glm::rotate(model, 1.3f + jumprotate, glm::vec3(-1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);
		ourShader.setFloat("time", dying);
		ourModel.Draw(ourShader);
		

		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// 缩小

		// don't forget to enable shader before setting uniforms
		groundShader.use();
		groundShader.setFloat("material.shininess", 32.0f);
		groundShader.setVec3("viewPos", camera.Position);

		// directional light
		groundShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f); // 光源
		groundShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f); // 环境光照
		groundShader.setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f); // 漫反射
		groundShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f); // 镜面光照，最亮点


		groundShader.setMat4("projection", projection);
		groundShader.setMat4("view", view);

		
		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glBindVertexArray(cubeVAO);
		for (int i = 0; i < 2; i++) {
			glm::mat4 model1;
			model1 = glm::translate(model1, groundPositions[i]); // 移动
			model1 = glm::rotate(model1, (float)acos(-1) / 2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
			groundShader.setMat4("model", model1);
			glDrawArrays(GL_TRIANGLES, 0, 600);
			glDrawArrays(GL_TRIANGLES, 1800, 60000);
		}

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap2);

		glBindVertexArray(cubeVAO);
		for (int i = 0; i < 2; i++) {
			glm::mat4 model1;
			model1 = glm::translate(model1, groundPositions[i]); // 移动
			model1 = glm::rotate(model1, (float)acos(-1) / 2.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
			groundShader.setMat4("model", model1);
			glDrawArrays(GL_TRIANGLES, 600, 1200);
		}

		asteroidShader.use();
		asteroidShader.setMat4("projection", projection);
		asteroidShader.setMat4("view", view);


		// draw meteorites
		asteroidShader.use();
		asteroidShader.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id); 
		for (unsigned int i = 0; i < rock.meshes.size(); i++)
		{
			glBindVertexArray(rock.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (!isjumping) {
			if (!goingtojump) {
				goingtojump = true;
				jumplength = 0.0f;
			}
			else {
				jumplength += deltaTime;
			}
		}
	}
	else {
		if (goingtojump) {
			goingtojump = false;
			isjumping = true;
			jumpTime = glfwGetTime();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}