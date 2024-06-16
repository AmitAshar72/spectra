#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Mesh.h"
#include <GLFW/glfw3.h>

void InitWindow(); // Initial GLFW
int InitGlad(); //Initialize Glad. Glad manages function pointers for OpenGL
void Framebuffer_size_callback(GLFWwindow* window, int width, int height); //Callback function for when we resize the window
void mouse_callback(GLFWwindow* window, double xpos, double ypos); //callback function for mouse inputs. Mouse X and Y 
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); // Callback function for mouse scroll
void ProcessInput(GLFWwindow* window);
void SetupLights(Shader& shader, Camera& camera); //Light parameters are set here
void InitImGui(GLFWwindow* window);
void ImGuiNewFrame();
void DrawImGuiWindow();
void DestroyImGuiWindow();


//Screen dimensions
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_LENGTH = 1200;

//Camera object with initial pos
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));

//Mouse Inputs
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_LENGTH / 2.0;
bool firstMouse = true;

//Time
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(9.95f,  2.1f,  -95.0f),
	glm::vec3(-9.95f,  2.1f, -25.0f),
	glm::vec3(9.95f,  2.1f, 25.0f),
	glm::vec3(-9.95f,  2.1f, 95.0f)
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

#pragma region Light Cube
	////Initialize Light Shader
	Shader lightShader("LightShader.vs", "LightShader.fs");

	// Store mesh data in vectors for the mesh
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
	std::vector <Texture> lightTex;

	//glm::vec3 lightPosition = glm::vec3(0.0f);
	glm::vec3 lightRotation = glm::vec3(0.0f);
	glm::vec3 lightScale = glm::vec3(0.2f);

	Mesh lightCube(lightVerts, lightInd, lightTex);

	lightCube.UpdateBoundingBoxScale(lightScale);

#pragma endregion Light Cube

	//Set Light Color, the value is fed into LightShader.fs
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

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

#pragma region Light Cube draw

		for (unsigned int i = 0; i < 4; i++)
		{
			glm::mat4 lightModel = glm::mat4(1.0f);

			/*pointLightPositions[i].x = radius * cos(speed * currentFrame);
			pointLightPositions[i].y = radius * sin(speed * currentFrame);*/
			lightCube.SetMeshProperties(lightShader, camera, pointLightPositions[i], lightRotation, lightScale);
			lightShader.setVec3("lightColor", lightColor);
			lightCube.Draw(lightShader);
		}
#pragma endregion

		DrawImGuiWindow();

		//check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

	lightShader.Delete();

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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

	//camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void SetupLights(Shader& shader, Camera& camera)
{
	shader.Activate();

	//ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	shader.setFloat("material.shininess", 32.0f);

	//Directional Light
	shader.setVec3("dirLight.direction", 1.0f, -1.0f, 0.0f);
	shader.setVec3("dirLight.ambient", 0.25f, 0.25f, 0.25f);
	shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

	// point light 1
	shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);
	// point light 2
	shader.setVec3("pointLights[1].position", pointLightPositions[1]);
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09f);
	shader.setFloat("pointLights[1].quadratic", 0.032f);
	// point light 3
	shader.setVec3("pointLights[2].position", pointLightPositions[2]);
	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[2].constant", 1.0f);
	shader.setFloat("pointLights[2].linear", 0.09f);
	shader.setFloat("pointLights[2].quadratic", 0.032f);
	// point light 4
	shader.setVec3("pointLights[3].position", pointLightPositions[3]);
	shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[3].constant", 1.0f);
	shader.setFloat("pointLights[3].linear", 0.09f);
	shader.setFloat("pointLights[3].quadratic", 0.032f);
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