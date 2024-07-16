#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Mesh.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <iostream>
#include <deque>
namespace fs = std::filesystem;

void InitWindow(); // Initial GLFW
int InitGlad(); //Initialize Glad. Glad manages function pointers for OpenGL
void Framebuffer_size_callback(GLFWwindow* window, int width, int height); //Callback function for when we resize the window
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos); //callback function for mouse inputs. Mouse X and Y 
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); // Callback function for mouse scroll
void ProcessInput(GLFWwindow* window);
void SetupLights(Shader& shader, Camera& camera); //Light parameters are set here
void InitImGui(GLFWwindow* window);
void ImGuiNewFrame();
void DrawImGuiWindow();
void DestroyImGuiWindow();

std::string rootDir = "D:\\Repositories\\spectra\\spectra";
std::string textureDirectory = rootDir + "\\Resources\\Textures";

//Screen dimensions
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_LENGTH = 720;

//Camera object with initial pos
Camera camera(glm::vec3(0.0f, 2.0f, 0.0f));

//Mouse Inputs
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_LENGTH / 2.0;
bool firstMouse = true;

//Time
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,  0.5f, 0.5f),
	glm::vec3(0.75f,  0.0f, 0.5f),
	glm::vec3(-0.75f,  0.0f, 0.5f),
	glm::vec3(-9.95f,  0.5f, 0.5f)
};

// Vertices coordinates
Vertex vertices[] =
{
	{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
	{glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	2, 1, 0,
	3, 2, 0
};

//Instanced cubes
//Vertices of a cube with pos, normals, color and tex
Vertex instancedVertices[] = {
	// positions			  //Normals				// colors         // texture 
//																		// coords
	// Front face
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
	// Back face
	{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
	// Left face
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
	// Right face
	{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
	// Top face
	{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
	// Bottom face
	{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
	{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
	{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)}
};

//indices of the vertices defined above
GLuint instancedIndices[] = 
{  
	// Back face
	0, 2, 1,
	2, 0, 3,
	// Front face
	4, 5, 6,
	6, 7, 4,
	// Left face
	8, 10, 9,
	10, 8, 11,
	// Right face
	12, 13, 14,
	14, 15, 12,
	// Top face
	16, 18, 17,
	18, 16, 19,
	// Bottom face
	20, 21, 22,
	22, 23, 20
};


//Light Cube
Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.5f, -0.5f,  0.5f)},
	Vertex{glm::vec3(0.5f, -0.5f,  0.5f)},
	Vertex{glm::vec3(0.5f,  0.5f,  0.5f)},
	Vertex{glm::vec3(-0.5f,  0.5f,  0.5f)},

	Vertex{glm::vec3(-0.5f, -0.5f, -0.5f)},
	Vertex{glm::vec3(0.5f, -0.5f, -0.5f)},
	Vertex{glm::vec3(0.5f,  0.5f, -0.5f)},
	Vertex{glm::vec3(-0.5f,  0.5f, -0.5f)}
};

GLuint lightIndices[] =
{
	//Front face
	0, 1, 2,
	0, 2, 3,

	//Left Face
	4, 0, 3,
	4, 3, 7,

	//Back face
	5, 4, 7,
	5, 7, 6,

	//Top Face
	3, 2, 6,
	3, 6, 7,

	//Right face
	1, 5, 6,
	1, 6, 2,

	//Bottom face
	1, 0, 4,
	1, 4, 5
};

// Constants
const int MAX_CUBES = 12;
const int MAX_POINTLIGHTS = 4;

std::deque<glm::vec3> cubePositions;
glm::vec3 ambientDir = glm::vec3(1.0f, -1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(1.0f, 2.0f, -0.5f);

class PointLight 
{
public:
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Color = glm::vec3(0.250f);
};

std::deque<PointLight> pointLights;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;


int main() 
{
	
	//Instantiate GLFW Window
	InitWindow();

	//Create Window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_LENGTH, "Test", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Set the current window as context for OpenGL
	glfwMakeContextCurrent(window);
	//Allow cursor but keep it disabled
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//On Window Resize, callback to update viewport
	glfwSetFramebufferSizeCallback(window, Framebuffer_size_callback);
	// Set mouse button callback
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//On mouse movement, callback to update camera
	glfwSetCursorPosCallback(window, mouse_callback);
	//On scroll input, callback to update camera zoom (FOV)
	glfwSetScrollCallback(window, scroll_callback);

	//Initialize GLAD
	InitGlad();

	//Enable Depth Buffer
	glEnable(GL_DEPTH_TEST);

	//Face Culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	camera.SetScreenDimensions(SCR_WIDTH, SCR_LENGTH);

#pragma region Init Shaders

	std::string vs = "\\VertexShader.vs";
	std::string fs = "\\FragmentShader.fs";
	std::string l_vs = "\\LightShader.vs";
	std::string l_fs = "\\LightShader.fs";
	std::string depth_vs = "\\ShadowDepthVertShader.vs";
	std::string depth_fs = "\\ShadowDepthFragShader.fs";

	Shader mainShader((rootDir + vs).c_str(), (rootDir + fs).c_str());
	Shader lightShader((rootDir + l_vs).c_str(), (rootDir + l_fs).c_str());
	Shader depthShader((rootDir + depth_vs).c_str(), (rootDir + depth_fs).c_str());
#pragma endregion

#pragma region Plank

	mainShader.setInt("num_pointLights", 0);
	Texture textures[]
	{
		Texture(textureDirectory, "planks.png", "diffuse", 0, GL_UNSIGNED_BYTE),
		Texture(textureDirectory, "planksSpec.png", "specular", 1, GL_UNSIGNED_BYTE)
	};

	// Store mesh data in vectors for the mesh
	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

	glm::vec3 plankPosition = glm::vec3(0.0f);
	glm::vec3 plankRotation = glm::vec3(0.0f, 0.0f, 0.0f);;
	glm::vec3 plankScale = glm::vec3(1.0f);

	Mesh plank(verts, ind, tex);

	plank.UpdateBoundingBoxScale(plankScale);

#pragma endregion

#pragma region Instanced Cube

	Texture instancedTextures[]
	{
		Texture(textureDirectory, "container2.png", "diffuse", 0, GL_UNSIGNED_BYTE),
		Texture(textureDirectory, "container2_specular.png", "specular", 1, GL_UNSIGNED_BYTE)
	};

	// Store mesh data in vectors for the mesh
	std::vector <Vertex> cubeVerts(instancedVertices, instancedVertices + sizeof(instancedVertices) / sizeof(Vertex));	
	std::vector <GLuint> cubeInd(instancedIndices, instancedIndices + sizeof(instancedIndices) / sizeof(GLuint));
	std::vector <Texture> cubeTex(instancedTextures, instancedTextures + sizeof(instancedTextures) / sizeof(Texture));

	//glm::vec3 cubePosition = cubePos;
	glm::vec3 cubeRotation = glm::vec3(0.0f);
	glm::vec3 cubeScale = glm::vec3(0.2f);

	Mesh cube(cubeVerts, cubeInd, cubeTex);

	cube.UpdateBoundingBoxScale(cubeScale);

#pragma endregion

#pragma region Light Cube
	
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	std::vector <Texture> lightTex;

	//glm::vec3 lightPosition = glm::vec3(0.0f);
	glm::vec3 lightRotation = glm::vec3(0.0f);
	glm::vec3 lightScale = glm::vec3(0.1f);

	Mesh lightCube(lightVerts, lightInd, lightTex);

	lightCube.UpdateBoundingBoxScale(lightScale);

#pragma endregion	

#pragma region Shadow Map
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	

	GLuint shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	

#pragma endregion

	//Set Light Color, the value is fed into LightShader.fs
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 instancedCubeColor = glm::vec3(0.5f, 1.0f, 0.75f);

	//ImGui
	InitImGui(window);

	//Render Loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Input
		ProcessInput(window);

		//Render Call
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGuiNewFrame();		


		//Setup lights
		SetupLights(mainShader, camera);



		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 25.0f;
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		depthShader.Activate();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

#pragma region Scene Objects
#pragma region Plank Draw

		plank.SetMeshProperties(depthShader, camera, plankPosition, plankRotation, plankScale);
		plank.Draw(depthShader);

		/*cube.SetMeshProperties(mainShader, camera, cubePos, cubeRotation, cubeScale);
		cube.Draw(mainShader);*/

#pragma endregion 

#pragma region Light Cube draw


		mainShader.setInt("num_pointLights", pointLights.size());
		for (unsigned int i = 0; i < pointLights.size(); i++)
		{
			glm::mat4 lightModel = glm::mat4(1.0f);

			/*pointLightPositions[i].x = radius * cos(speed * currentFrame);
			pointLightPositions[i].y = radius * sin(speed * currentFrame);*/
			lightCube.SetMeshProperties(lightShader, camera, pointLights[i].Position, lightRotation, lightScale);
			lightShader.setVec3("lightColor", pointLights[i].Color);
			lightCube.Draw(lightShader);
		}

#pragma endregion

#pragma region Instanced Cube Draw

		for (int i = 0; i < cubePositions.size(); i++)
		{
			cube.SetMeshProperties(depthShader, camera, cubePositions[i], cubeRotation, cubeScale);
			cube.Draw(depthShader);
		}

#pragma endregion 
#pragma endregion

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, SCR_WIDTH, SCR_LENGTH);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		mainShader.Activate();		
		mainShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		mainShader.Activate();
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		mainShader.setInt("shadowMap", 2);

#pragma region Scene Objects
#pragma region Plank Draw

		plank.SetMeshProperties(mainShader, camera, plankPosition, plankRotation, plankScale);
		plank.Draw(mainShader);

		/*cube.SetMeshProperties(mainShader, camera, cubePos, cubeRotation, cubeScale);
		cube.Draw(mainShader);*/

#pragma endregion 

#pragma region Light Cube draw


		mainShader.setInt("num_pointLights", pointLights.size());
		for (unsigned int i = 0; i < pointLights.size(); i++)
		{
			glm::mat4 lightModel = glm::mat4(1.0f);

			/*pointLightPositions[i].x = radius * cos(speed * currentFrame);
			pointLightPositions[i].y = radius * sin(speed * currentFrame);*/
			lightCube.SetMeshProperties(lightShader, camera, pointLights[i].Position, lightRotation, lightScale);
			lightShader.setVec3("lightColor", pointLights[i].Color);
			lightCube.Draw(lightShader);
		}

#pragma endregion

#pragma region Instanced Cube Draw

		for (int i = 0; i < cubePositions.size(); i++)
		{
			cube.SetMeshProperties(mainShader, camera, cubePositions[i], cubeRotation, cubeScale);
			cube.Draw(mainShader);
		}

#pragma endregion 
#pragma endregion
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		DrawImGuiWindow();

		//check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

	mainShader.Delete();
	lightShader.Delete();
	depthShader.Delete();

	DestroyImGuiWindow();

	//Terminate call to clean up all resources
	glfwTerminate();
	return 0;
}

void InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

int InitGlad()
{
	//GLAD manages functions pointers for OpenGL. We initialize GLAD before we call any OpenGL function
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
}

void Framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	camera.SetScreenDimensions(width, height);
}

// Callback function for mouse button press
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
	if (ImGui::GetIO().WantCaptureMouse) 
	{
		// ImGui is handling the mouse input, don't process it in the game
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		float xNormalized = (xpos / width) * 2.0f - 1.0f;
		float yNormalized = 1.0f - (ypos / height) * 2.0f;

		// Get the view and projection matrices
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = camera.GetProjectionMatrix();

		// Calculate the inverse of the view-projection matrix
		glm::mat4 viewProjectionInverse = glm::inverse(projection * view);

		// Create a NDC coordinate with z set to 0.0 for near plane or 1.0 for far plane
		glm::vec4 screenPos = glm::vec4(xNormalized, yNormalized, 0.9f, 1.0f);

		// Transform NDC to world coordinates
		glm::vec4 worldPos = viewProjectionInverse * screenPos;

		// Perform perspective divide (important step)
		worldPos /= worldPos.w;

		glm::vec3 finalPos = glm::vec3(worldPos);

		// Add the new position to the deque and maintain the max capacity
		if (cubePositions.size() >= MAX_CUBES) {
			cubePositions.pop_front();
		}

		cubePositions.push_back(finalPos);
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		float xNormalized = (xpos / width) * 2.0f - 1.0f;
		float yNormalized = 1.0f - (ypos / height) * 2.0f;

		// Get the view and projection matrices
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = camera.GetProjectionMatrix();

		// Calculate the inverse of the view-projection matrix
		glm::mat4 viewProjectionInverse = glm::inverse(projection * view);

		// Create a NDC coordinate with z set to 0.0 for near plane or 1.0 for far plane
		glm::vec4 screenPos = glm::vec4(xNormalized, yNormalized, 0.9f, 1.0f);

		// Transform NDC to world coordinates
		glm::vec4 worldPos = viewProjectionInverse * screenPos;

		// Perform perspective divide (important step)
		worldPos /= worldPos.w;

		glm::vec3 finalPos = glm::vec3(worldPos);

		// Add the new position to the deque and maintain the max capacity
		if (pointLights.size() >= MAX_POINTLIGHTS) {
			pointLights.pop_front();
		}

		PointLight p;
		p.Color = glm::vec3(1.0f);
		p.Position = finalPos;

		pointLights.push_back(p);
	}
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		// ImGui is handling the mouse input, don't process it in the game
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
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

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		// ImGui is handling the mouse input, don't process it in the game
		return;
	}

	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void SetupLights(Shader& shader, Camera& camera)
{
	shader.Activate();

	//ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	shader.setFloat("material.shininess", 32.0f);

	//Directional Light
	shader.setVec3("dirLight.direction", ambientDir);
	shader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
	shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

	for (int i = 0; i < pointLights.size(); ++i) 
	{
		shader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLights[i].Position);
		shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLights[i].Color);
		shader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
		shader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
		shader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
	}

	/*for (int i = 0; i < 4; ++i)
	{
		shader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
		shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.05f, 0.05f, 0.05f);
		shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
		shader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
		shader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
		shader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
		shader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
	}*/

	// spotLight
	shader.setVec3("spotLight.position", camera.Position);
	shader.setVec3("spotLight.direction", camera.Front);
	shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.09f);
	shader.setFloat("spotLight.quadratic", 0.032f);
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
}

#pragma region ImGUI
void InitImGui(GLFWwindow* window)
{
	//ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiNewFrame() 
{
	//Let Imgui know that its a new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void DrawImGuiWindow() 
{
	//Imgui Window
	ImGui::Begin("Window, ImGui Window");
	ImGui::DragFloat3("Ambient light Dir", &ambientDir[0], 0.1f);
	ImGui::DragFloat3("Ambient light Pos", &lightPos[0], 0.1f);

	for (int i = 0; i < pointLights.size(); ++i) 
	{
		std::string label = "Point Light " + std::to_string(i + 1);	

		if (ImGui::CollapsingHeader(label.c_str())) 
		{
			PointLight& light = pointLights[i];
			ImGui::PushID(i);
			ImGui::SliderFloat3("Position", &light.Position.x, -10, 10);
			ImGui::ColorEdit3("Color", &light.Color.x);
			ImGui::PopID();
		}		
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DestroyImGuiWindow() 
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
#pragma endregion