#include <iostream>
#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include "Shader.h"
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

GLenum glCheckError_(const char* file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void generateUVSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, int rings);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void GenerateShape(unsigned int VAO, int modelLocation, unsigned int texture, int vertexAmount,
	glm::vec3 translate, float rotateDegrees,
	glm::vec3 rotateDirection, glm::vec3 scale);

void Mesh(string path, vector<unsigned int>& indexList, vector<float>& vertexList);

bool roofFlip = false;

bool fillFlip = false;



glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 linkPos = glm::vec3(0.0f, -0.5f, 0.0f);
glm::vec3 lightPos1 = glm::vec3(-3.0f,0.0f,1.0f);
glm::vec3 lightPos2 = glm::vec3(3.0f,0.0f,1.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

glm::mat4 model;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW\n";
		return -1;
	}

	GLFWwindow* window;
	window = glfwCreateWindow(640, 480, "Snok Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glCheckError();

	glViewport(0, 0, 640, 480);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	Shader snokShader("Vertex.vert", "Fragment.frag");

	int modelLocation = glGetUniformLocation(snokShader.ID, "model");
	int viewLocation = glGetUniformLocation(snokShader.ID, "view");
	int projectionLocation = glGetUniformLocation(snokShader.ID, "projection");
	int lightLocation = glGetUniformLocation(snokShader.ID, "lightPosition");
	
	glCheckError();

	float verticesC[] =
	{
		// Positions				// Color				//Texture
		//
		// Front face
		-0.5f,	-0.5f,	0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		0.5f,	-0.5f,	0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
		0.5f,	0.5,	0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		-0.5f,	0.5,	0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		
		// Back face 
		-0.5f,	-0.5f,	-0.5f,		0.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		-0.5f,	0.5,	-0.5,		0.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		0.5,	0.5,	-0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		0.5f,	-0.5,	-0.5f,		0.0f, 1.0f, 1.0f,		0.0f, 1.0f,

		// Bottom face
		-0.5f,	-0.5f,	-0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		0.5f,	-0.5,	-0.5,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		0.5,	-0.5,	0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
		-0.5f,	-0.5,	0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
		
		// Top face
		-0.5f,	0.5f,	-0.5f,		1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f,	0.5,	0.5,		1.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		0.5,	0.5,	0.5f,		1.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		0.5f,	0.5,	-0.5f,		1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		
		// Left face
		-0.5f,	-0.5f,	-0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f,	-0.5,	0.5,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		-0.5,	0.5,	0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		-0.5f,	0.5,	-0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		
		// Right face
		0.5f,	-0.5f,	-0.5f,		1.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		0.5f,	0.5,	-0.5,		1.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		0.5,	0.5,	0.5f,		1.0f, 0.0f, 1.0f,		0.0f, 0.0f,
		0.5f,	-0.5,	0.5f,		1.0f, 0.0f, 1.0f,		0.0f, 1.0f,
	};

	unsigned int indicesC[] =
	{
		0, 1, 2, 2, 3, 0,			//bottom face
		4, 5, 6, 6, 7, 4,			//top face
		8, 9, 10, 10, 11, 8,		//+x
		12, 13, 14, 14, 15, 12,		//-x
		16, 17, 18, 18, 19, 16,		//-y
		20, 21, 22, 22, 23, 20		//+y
	};

	unsigned int VBO;

	unsigned int VAOC;
	glGenVertexArrays(1, &VAOC);

	unsigned int VAOM;
	int indicesM;
	glGenVertexArrays(1, &VAOM);

	unsigned int EBO;

	glBindVertexArray(VAOC); //START CUBE
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesC), verticesC, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesC), indicesC, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); 
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);  //END CUBE

	glBindVertexArray(0);

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	Mesh("resources/objects/cl.obj", indices, vertices);

	glBindVertexArray(VAOM); //START MODEL
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1); 
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); 
	glEnableVertexAttribArray(2); //END MODEL

	glBindVertexArray(0);

	// load and create a texture 
	// -------------------------
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load("resources/textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load image, create texture and generate mipmaps
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load("resources/textures/linktexS3TC.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	float T = 0.0f;
	
	while (!glfwWindowShouldClose(window))
	{

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCheckError();

		if (fillFlip)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		glCheckError();

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glCheckError();

		processInput(window);
		glCheckError();

		snokShader.use();
		glCheckError();

		if(!roofFlip)
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(glm::perspective(glm::radians(70.0f), (640.0f / 480.0f), 0.1f, 100.0f)));
		else
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(glm::ortho(-6.4f,6.4f,-4.8f,4.8f, 0.1f, 100.0f)));
		glCheckError();
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
		glCheckError();
		
		glm::vec3 lightLerp;
		
		if (glm::distance(linkPos, lightPos1) >= glm::distance(linkPos, lightPos2))
		{
			T += deltaTime*2;
			T = glm::max(T,0.0f);
		}
		else
		{
			T -= deltaTime*2;
			T = glm::min(T,1.0f);
		}
		lightLerp = glm::mix(lightPos1, lightPos2, T);
		glUniform3f(lightLocation, lightLerp.x, lightLerp.y, lightLerp.z);
		glCheckError();

		GenerateShape(VAOC, modelLocation, texture, 36, lightPos1, 0, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.2)); //Cube 1
		GenerateShape(VAOC, modelLocation, texture, 36, lightPos2,	0, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.2)); //Cube 2
		GenerateShape(VAOM, modelLocation, texture2, indices.size(), linkPos, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		glCheckError();

		glfwSwapBuffers(window);
		glCheckError();
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAOC);
	glDeleteVertexArrays(1, &VAOM);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(snokShader.ID);

	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}

void generateUVSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, int rings) 
{

	if (rings < 3)
		rings = 3;
	int slices = rings / 2;

	float PI = 3.14159f;

	float deltaLatitude = PI / slices;
	float deltaLongitude = 2 * PI / rings;

	for (int i = 0; i <= slices; ++i)
	{
		float latitudeAngle = PI / 2 - i * deltaLatitude;
		for (int j = 0; j <= rings; ++j)
		{
			float longitudeAngle = j * deltaLongitude;
			float x = cosf(latitudeAngle) * cosf(longitudeAngle);
			float y = cosf(latitudeAngle) * sinf(longitudeAngle);
			float z = sinf(latitudeAngle);

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z); 
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	unsigned int k1, k2;
	for (int i = 0; i < slices; ++i)
	{
		k1 = i * (rings + 1);
		k2 = k1 + rings + 1;
		for (int j = 0; j < rings; ++j, ++k1, ++k2)
		{
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
			if (i != (slices - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		roofFlip = !roofFlip;
	}
		

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
	{
		fillFlip = !fillFlip;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

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

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window)
{
	float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	cameraPos.y = 0;
	
	float linkSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		linkPos.z += linkSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		linkPos.z -= linkSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		linkPos.x += linkSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		linkPos.x -= linkSpeed;
		
}

void GenerateShape(unsigned int VAO, int modelLocation, unsigned int texture, int vertexAmount, 
	glm::vec3 translate = glm::vec3(0.0f, 0.0f, 0.0f), float rotateDegrees = 0.0f,
	glm::vec3 rotateDirection = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f))
{
	model = glm::mat4(1.0f);

	model = glm::translate(model, translate);
	model = glm::rotate(model, glm::radians(rotateDegrees), rotateDirection);
	model = glm::scale(model, scale);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, vertexAmount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh(string path, vector<unsigned int> &indexList, vector<float> &vertexList)
{
	cout << "Loading model: " << path << endl;
	//auto start = chrono::high_resolution_clock::now();
	tinyobj::ObjReaderConfig readerConfig;
	readerConfig.mtl_search_path = "./";
	readerConfig.triangulate = true;
	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(path, readerConfig)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error() << std::endl;
		}
		exit(1);
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning() << std::endl;
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	//update to unordered_map for faster lookup?
	std::map<std::tuple<int, int, int>, unsigned int> uniqueVertices;
	unsigned int newIndex = 0;
	int dupecount = 0, vertexcount = 0;
	for (auto& shape : shapes) {
		size_t index_offset = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
			size_t fv = size_t(shape.mesh.num_face_vertices[f]);

			for (size_t v = 0; v < fv; v++) {
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
				auto key = std::make_tuple(idx.vertex_index, idx.normal_index, idx.texcoord_index);

				//Check if this index combination already exists, if we get the default return we didn't get a match
				//and need to create new vertex attributes
				if (uniqueVertices.find(key) == uniqueVertices.end()) {
					uniqueVertices[key] = newIndex;
					newIndex++;
					//Vertex
					vertexList.push_back(attrib.vertices[3 * size_t(idx.vertex_index) + 0]);
					vertexList.push_back(attrib.vertices[3 * size_t(idx.vertex_index) + 1]);
					vertexList.push_back(attrib.vertices[3 * size_t(idx.vertex_index) + 2]);

					//Normals default to 0 if not defined in .obj
					if (idx.normal_index >= 0) {
						vertexList.push_back(attrib.normals[3 * size_t(idx.normal_index) + 0]);
						vertexList.push_back(attrib.normals[3 * size_t(idx.normal_index) + 1]);
						vertexList.push_back(attrib.normals[3 * size_t(idx.normal_index) + 2]);
					}
					else {
						vertexList.push_back(0.0f);
						vertexList.push_back(0.0f);
						vertexList.push_back(0.0f);
					}

					//UV also set to 0 if missing
					if (idx.texcoord_index >= 0) {
						vertexList.push_back(attrib.texcoords[2 * size_t(idx.texcoord_index) + 0]);
						vertexList.push_back(attrib.texcoords[2 * size_t(idx.texcoord_index) + 1]);
					}
					else {
						vertexList.push_back(0.0f);
						vertexList.push_back(0.0f);
					}
				}
				else
				{
					dupecount++;
				}
				vertexcount++;
				//Push the index
				indexList.push_back(uniqueVertices[key]);
			}
			index_offset += fv;
		}
	}
	
	cout << "tinyobj loader done" << endl;
	cout << "dupe count: " << dupecount << endl;
	cout << "vertex count: " << vertexcount << endl;
}