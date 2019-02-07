#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

//GLM library
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<SOIL2/SOIL2.h>

using namespace std;

int width, height;
const double PI = 3.14159;
const float toRadians = PI / 180.0f;

//input function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//declare view matrix
glm::mat4 viewMatrix;

//initalize field of view
GLfloat fov = 45.f;

void initCamera();

//difine camera attributes
glm::vec3 cameraPosition = glm::vec3(0.f, 0.f, 3.f);
glm::vec3 target = glm::vec3(0.f, 0.f, 0.f);
glm::vec3 cameraDirection = glm::normalize(cameraPosition - target);
glm::vec3 worldUp = glm::vec3(0.f, 1.f, 0.f);
glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.f, 0.f, -1.f));

//declare target prototype
glm::vec3 getTarget();

//camera transformation prototype
void TransformCamera();

//booleen for keys and mouse buttons
bool keys[1024], mouseButtons[3];

//booleen to check camera transformation
bool isPanning = false, isOrbiting = false;

//radius pitch and yaw
GLfloat radius = 3.f, rawYaw = 0.f, rawPitch = 0.f, degYaw, degPitch;

GLfloat deltaTime = 0.f, lastFrame = 0.f;
GLfloat lastX = 320, lastY = 240, xChange, yChange;

bool firstMouseMove = true;

void initCamera();

//light source position
glm::vec3 lightPosition(0.0f, 1.0f, 0.0f);

//Draw Primitive
void draw()
{
	GLenum mode = GL_QUADS;
	GLsizei indices = 144;

	glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);

}

//Create and compile shaders
static GLuint CompileShader(const string& source, GLuint shaderType)
{
	//Create shader object
	GLuint shaderID = glCreateShader(shaderType);
	const char* src = source.c_str();

	//Attach source code to shader object
	glShaderSource(shaderID, 1, &src, nullptr);

	//compile shader
	glCompileShader(shaderID);

	//return ID of Compiled shader
	return shaderID;
}

//Create program object
static GLuint CreateShaderProgram(const string& vertexShader, const string& fragmentShader)
{
	//Compile vertex shader
	GLuint vertexShaderComp = CompileShader(vertexShader, GL_VERTEX_SHADER);

	//Compile fragment shader
	GLuint fragmentShaderComp = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	//Create Program object
	GLuint shaderProgram = glCreateProgram();

	//Attach vertex and fragment shaders to program object
	glAttachShader(shaderProgram, vertexShaderComp);
	glAttachShader(shaderProgram, fragmentShaderComp);

	//Link shaders to create executable
	glLinkProgram(shaderProgram);

	//Delete compiled shaders
	glDeleteShader(vertexShaderComp);
	glDeleteShader(fragmentShaderComp);

	//Return shader program
	return shaderProgram;

}

int main(void)
{
	width = 640;
	height = 480;

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Main Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//set input callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
		cout << "Error!" << endl;

	//define vertex data for light
	GLfloat lampVertices[] = {
		//top
		-.12, .50, -.50,
		-.12, .50, -.25,
		.12, .50, -.25,
		.12, .50, -.50,
		//S1
		.12, .50, -.25,
		.12, .25, -.25,
		.12, .25, -.50,
		.12, .50, -.50,
		//S2
		-.12, .50, -.50,
		.12, .50, -.50,
		.12, .25, -.50,
		-.12, .25, -.50,
		//S3
		-.12, .50, -.50,
		-.12, .50, -.25,
		-.12, .25, -.25,
		-.12, .25, -.50,
		//S4
		-.12, .50, -.25,
		.12, .50, -.25,
		.12, .25, -.25,
		-.12, .25, -.25,
		//bottom
		-.12, .25, -.50,
		.12, .25, -.50,
		.12, .25, -.25,
		-.12, .25, -.25
	};

	//Define vertex data for table (GLfloat array)
	GLfloat vertices[] = {
		//position attributes (x,y,z)
		//leg 1 front
		//top
		-.25, -.25, .25,//index
		1.0, 0.0, 0.0,//color
		1.0, 0.0,//UV
		0.0f, 0.0f, 1.0f,//normal psitive z

		-.25, -.25, 0.0,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, .25,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,
		//side 1
		-.25, -.25, .25,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, .25,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.25, -.25, 0.0,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 2
		-.25, -.25, 0.0,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, 0.0,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,
		//side 3
		-.50, -.25, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, 0.0,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, .25,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, .25,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 4
		-.50, -.25, .25,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.25, -.25, .25,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, .25,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, .25,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,
		//bottom
		-.25, -1, .25,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, 0.0,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, .25,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		//leg 2 front
		//top
		.25, -.25, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.25, -.25, .25,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, .25,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, 0.0,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 1
		.50, -.25, .25,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, .25,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, 0.0,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, 0.0,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 2
		.50, -.25, 0.0,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, 0.0,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.25, -1, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.25, -.25, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 3
		.25, -.25, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, .25,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.25, -.25, .25,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 4
		.25, -.25, .25,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.25, -1, .25,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, .25,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, .25,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//bottom
		.25, -1, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.25, -1, .25,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, .25,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, 0.0,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		//leg 3 back
		//top
		-.25, -.25, -1,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.25, -.25, -.75,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, -.75,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, -1,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,
		//side 1
		-.25, -.25, -.75,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, -.75,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, -1,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.25, -.25, -1,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 2
		-.25, -.25, -1,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, -1,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, -1,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, -1,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,
		//side 3
		-.50, -.25, -.75,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, -.75,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, -1,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, -1,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 4
		-.50, -.25, -.75,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.25, -.25, -.75,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, -.75,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, -.75,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,
		//bottom
		-.25, -1, -.75,
		1.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.25, -1, -1,
		1.0, 0.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, -1,
		1.0, 0.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -1, -.75,
		1.0, 0.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		//leg 4 back
		//top
		.25, -.25, -1,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.25, -.25, -.75,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, -.75,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, -1,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 1
		.50, -.25, -.75,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, -.75,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, -1,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, -1,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 2
		.50, -.25, -1,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, -1,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.25, -1, -1,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.25, -.25, -1,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 3
		.25, -.25, -1,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, -1,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, -.75,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.25, -.25, -.75,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side 4
		.25, -.25, -.75,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.25, -1, -.75,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, -.75,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, -.75,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//bottom
		.25, -1, -1,
		0.0, 1.0, 0.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.25, -1, -.75,
		0.0, 1.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, -.75,
		0.0, 1.0, 0.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -1, -1,
		0.0, 1.0, 0.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//Table top
		//top
		-.50, -.12, .25,
		0.0, 0.0, 1.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.12, .25,
		0.0, 0.0, 1.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.12, -1,
		0.0, 0.0, 1.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.12, -1,
		0.0, 0.0, 1.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side1
		.50, -.12, .25,
		0.0, 0.0, 1.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, .25,
		0.0, 0.0, 1.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, -1,
		0.0, 0.0, 1.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.12, -1,
		0.0, 0.0, 1.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side2
		.50, -.12, -1,
		0.0, 0.0, 1.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, -1,
		0.0, 0.0, 1.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, -1,
		0.0, 0.0, 1.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.12, -1,
		0.0, 0.0, 1.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side3
		-.50, -.12, .25,
		0.0, 0.0, 1.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, .25,
		0.0, 0.0, 1.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, -1,
		0.0, 0.0, 1.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.12, -1,
		0.0, 0.0, 1.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//side4
		.50, -.12, .25,
		0.0, 0.0, 1.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, .25,
		0.0, 0.0, 1.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, .25,
		0.0, 0.0, 0.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.12, .25,
		0.0, 0.0, 1.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,
		//bottom
		-.50, -.25, .25,
		0.0, 0.0, 1.0,
		0.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, .25,
		0.0, 0.0, 1.0,
		1.0, 0.0,
		0.0f, 0.0f, 1.0f,

		.50, -.25, -1,
		0.0, 0.0, 1.0,
		1.0, 1.0,
		0.0f, 0.0f, 1.0f,

		-.50, -.25, -1,
		0.0, 0.0, 1.0,
		0.0, 1.0,
		0.0f, 0.0f, 1.0f,
		
	};

	//Define element indices
	GLubyte indices[] = {
		0, 1, 2, 3
	};

	//enable depth buffer
	glEnable(GL_DEPTH_TEST);

	//wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint VBO, EBO, VAO, lampVBO, lampEBO, lampVAO;
	glGenBuffers(1, &VBO); //Create VBO
	glGenBuffers(1, &EBO); //Create EBO

	glGenBuffers(1, &lampVBO); //Create VBO
	glGenBuffers(1, &lampEBO); //Create EBO

	glGenVertexArrays(1, &VAO);//Create VAO
	glGenVertexArrays(1, &lampVAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Select EBO

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Load vertex attributes
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); //Load indices

	//specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	//lamp VAO
	glBindVertexArray(lampVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lampVBO); // Select VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO); // Select EBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW); // Load vertex attributes
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indice
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//load textures
	int tableTexWidth, tableTexHeight;
	unsigned char* tableImage = SOIL_load_image("Wood.png", &tableTexWidth, &tableTexHeight, 0, SOIL_LOAD_RGB);

	//generate textures
	GLuint tableTexture;
	glGenTextures(1, &tableTexture);
	glBindTexture(GL_TEXTURE_2D, tableTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tableTexWidth, tableTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tableImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(tableImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Vertex shader source code
	string vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 FragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal = mat3(transpose(inverse(model))) * normal;"
		"FragPos = vec3(model * vec4(vPosition, 1.0f));"
		"}\n";

	//Fragment shader source code
	string fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 FragPos;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 objectColor;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"//Ambient\n"
		"float ambientStrength = 0.5f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"//Diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - FragPos);"
		"float diff = max(dot(norm, lightDir), 0.0);"
		"vec3 diffuse = diff * lightColor;"
		"//Specularity\n"
		"float specularStrength = 1.5f;"
		"vec3 viewDir = normalize(viewPos - FragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 result = (ambient + diffuse + specular) * objectColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"}\n";

	// lamp Vertex shader source code
	string lampVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"}\n";

	// lamp Fragment shader source code
	string lampFragmentShaderSource =
		"#version 330 core\n"
		"out vec4 fragColor;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(1.0f);"
		"}\n";

	//Create shader program
	GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	GLuint lampShaderProgram = CreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource);

	//Use shader program exe
	glUseProgram(shaderProgram);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//set delta time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Resize window and graphics simultaneously
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		//declare identity matrix
		glm::mat4 projectionMatrix;

		//initialize transforms

		viewMatrix = glm::lookAt(cameraPosition, getTarget(), worldUp);
		//viewMatrix = glm::rotate(viewMatrix, (GLfloat)glfwGetTime() * 45.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

		//select uniform shader and variable
		GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

		//get light and object color location and light position
		GLint ObjectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

		//assign light and object colors
		glUniform3f(ObjectColorLoc, 0.28f, 0.12f, 0.04f);//numbers change based on image color
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

		//set light position
		glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);

		//specify view position
		glUniform3f(viewPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		//pass transform to shader

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindTexture(GL_TEXTURE_2D, tableTexture);
		glBindVertexArray(VAO);

		//draw table
		for (GLuint i = 0; i < 4; i++)
		{
			glm::mat4 modelMatrix;
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glDrawArrays(GL_QUADS, 0, 120);
		}

		glBindVertexArray(0);
		glUseProgram(0);

		glUseProgram(lampShaderProgram);
		GLint lampModelLoc = glGetUniformLocation(lampShaderProgram, "model");
		GLint lampViewLoc = glGetUniformLocation(lampShaderProgram, "view");
		GLint lampProjLoc = glGetUniformLocation(lampShaderProgram, "projection");
		glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(lampProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(lampVAO); // User-defined VAO must be called before draw. 

		// Transform planes to form lamp
		for (GLuint i = 0; i < 6; i++)
		{
			glm::mat4 modelMatrix;
			if (i >= 4)
			glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glDrawArrays(GL_QUADS, 0, 24);
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after
		glUseProgram(0);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		//Poll camera transformations
		TransformCamera();

	}

	//Clear GPU resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

//define input callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//clamp fov
	if (fov >= 1.f && fov <= 45.f)
		fov -= yoffset * 0.01f;

	//default fov
	if (fov < 1.f)
		fov = 1.f;
	if (fov > 45.f)
		fov = 45.f;

}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	//calculate cursor offset
	xChange = xpos - lastX;
	yChange = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	//pan camera
	if (isPanning)
	{
		if (cameraPosition.z < 0.f)
			cameraFront.z = 1.f;
		else
			cameraFront.z = -1.f;

		GLfloat cameraSpeed = xChange * deltaTime;
		cameraPosition += cameraSpeed * cameraRight;

		cameraSpeed = yChange * deltaTime;
		cameraPosition += cameraSpeed * cameraUp;
	}

	// orbit camera
	if (isOrbiting)
	{
		rawYaw += xChange;
		rawPitch += yChange;

		//convert yaw and pitch to degrees
		degYaw = glm::radians(rawYaw);
		degPitch = glm::radians(rawPitch);

		//azimuth altitude formula
		cameraPosition.x = target.x + radius * cosf(degPitch) * sinf(degYaw);
		cameraPosition.y = target.y + radius * sinf(degPitch);
		cameraPosition.z = target.z + radius * cosf(degPitch) * cosf(degYaw);
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		mouseButtons[button] = true;
	else if (action == GLFW_RELEASE)
		mouseButtons[button] = false;
}

//define getTarget function
glm::vec3 getTarget()
{
	if (isPanning)
		target = cameraPosition + cameraFront;


	return target;
}

//define transformCamera function
void TransformCamera()
{
	//pan camera
	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_RIGHT])
		isPanning = true;
	else
		isPanning = false;

	//orbit camera
	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_LEFT])
		isOrbiting = true;
	else
		isOrbiting = false;

	//reset camera
	if (keys[GLFW_KEY_F])
		initCamera();
}

void initCamera()
{
	cameraPosition = glm::vec3(0.f, 0.f, 3.f);
	target = glm::vec3(0.f, 0.f, 0.f);
	cameraDirection = glm::normalize(cameraPosition - target);
	worldUp = glm::vec3(0.f, 1.f, 0.f);
	cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
	cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));
	cameraFront = glm::normalize(glm::vec3(0.f, 0.f, -1.f));
}