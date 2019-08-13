
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;


#define GLM_ENABLE_EXPERIMENTAL

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void errorCallbackGLFW(int error, const char* description);
void hintsGLFW();
void endProgram();

void update(double currentTime);
void setupRender();
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
void debugGL();
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);
string readShader(string name);
void  checkErrorShader(GLuint shader);

void readTexture(string name, GLuint texture);

//for animation


double previousTime = 0;
bool directionforward = true;
float animationAdd = -0.4;

//for tree color change
int treeColor = 0;


// VARIABLES
GLFWwindow*		window;
int				windowWidth = 640;
int				windowHeight = 480;
bool			running = true;

struct modelObject {
	GLuint          program;
	GLuint          vao;
	GLuint          buffer[3];
	GLint           mv_location;
	GLint           proj_location;
	GLint			tex_location;
	std::vector < glm::vec3 > out_vertices;
	std::vector < glm::vec2 > out_uvs;
	std::vector < glm::vec3 > out_normals;
	GLuint*	texture;
	GLuint ID;
	float			angleX = 0.0f;
	float			angleY = 0.0f;
	GLuint		matColor_location;
	GLuint		lightColor_location;
}cube_world, platformer_happy, grass, tree, rock, cube_map, platformer_sad, platformer_angry, platformer_annoyed, treeSmart_Happy, treeSmart_Angry, treeSmart_Sad, treeSmart_Annoyed, lightModel;



void startup(modelObject *obj, string filename);
void render(double currentTime);
void renderPrep(modelObject *obj);
void readObj(string name, modelObject *obj);
void startupLight();

float           aspect;
glm::mat4		proj_matrix;

GLfloat radius = 5.0f;
float			disp = 0.0;
float			fovy = 45.0f;


glm::vec3		cameraPosition = glm::vec3(1.0f, 4.0f, 1.0f);
glm::vec3		cameraFront = glm::vec3(0.0f, -0.2f, -1.0f);
glm::vec3		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool			keyStatus[1024];
GLfloat			deltaTime = 0.0f;
GLfloat			lastTime = 0.0f;

bool			movingLight = false;
glm::vec3		lightDisp = glm::vec3(0.0f, 5.0f, 0.0f);
glm::vec3		ia = glm::vec3(0.5f, 0.5f, 0.5f);
GLfloat			ka = 1.0f;
glm::vec3		id = glm::vec3(0.93f, 0.75f, 0.32f);
GLfloat			kd = 1.0f;
glm::vec3		is = glm::vec3(1.00f, 1.00f, 1.0f);
GLfloat			ks = 0.01f;




int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

												// Start a window using GLFW
	// Start a window using GLFW
	string title = "My OpenGL Application";
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}


	glfwMakeContextCurrent(window);				// making the OpenGL context current

												// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

												// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);		// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);					// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);	// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);		// Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);		// Set callback for mouse wheel.
																//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor FPS.

	setupRender();
	// setup some render variables.

	startup(&cube_world, "cube_world_dark.obj");			// Setup all necessary information for startup (aka. load texture, shaders, models, etc).
	startup(&platformer_happy, "Platformer_Happy.obj");
	startup(&grass, "grass_row.obj");
	startup(&tree, "treeSmart.obj");
	startup(&rock, "rock_multi.obj");
	startup(&cube_map, "cubemap_simplestar.obj");
	startup(&platformer_sad, "Platformer_Sad.obj");
	startup(&platformer_angry, "Platformer_Angry.obj");
	startup(&platformer_annoyed, "Platformer_Annoyed.obj");
	startup(&treeSmart_Happy, "treeSmart_Happy.obj");
	startup(&treeSmart_Angry, "treeSmart_Angry.obj");
	startup(&treeSmart_Sad, "treeSmart_Sad.obj");
	startup(&treeSmart_Annoyed, "treeSmart_Annoyed.obj");
	startupLight();


	do {										// run until the window is closed

		double currentTime = glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime - currentTime;					// Save for next frame calculations.
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		//render(currentTime, &platformer_happy, 1);
		render(currentTime);// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)


		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...


	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.

						// tidy heap memory
	delete[] cube_world.texture;
	delete[] platformer_happy.texture;
	delete[] grass.texture;
	delete[] tree.texture;
	delete[] rock.texture;
	delete[] cube_map.texture;
	delete[] platformer_sad.texture;
	delete[] platformer_angry.texture;
	delete[] platformer_annoyed.texture;
	delete[] treeSmart_Happy.texture;
	delete[] treeSmart_Sad.texture;
	delete[] treeSmart_Angry.texture;
	delete[] treeSmart_Annoyed.texture;
	delete[] lightModel.texture;
}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void startup(modelObject *obj, string filename) {

	(*obj).program = glCreateProgram();

	string vs_text = readShader("vs_model.glsl");
	const char* vs_source = vs_text.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	checkErrorShader(vs);
	glAttachShader((*obj).program, vs);

	string fs_text = readShader("fs_model.glsl");
	const char* fs_source = fs_text.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	checkErrorShader(fs);
	glAttachShader((*obj).program, fs);

	glLinkProgram((*obj).program);
	glUseProgram((*obj).program);

	readObj(filename, obj);

	glGenVertexArrays(1, &((*obj).vao));			// Create Vertex Array Object
	glBindVertexArray((*obj).vao);				// Bind VertexArrayObject

	glGenBuffers(3, (*obj).buffer);			// Create new buffer
	glBindBuffer(GL_ARRAY_BUFFER, (*obj).buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER,
		(*obj).out_vertices.size() * sizeof(glm::vec3),
		&((*obj).out_vertices[0]),
		GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, (*obj).buffer[1]);	// Bind Buffer uv coordinates
	glBufferStorage(GL_ARRAY_BUFFER,
		(*obj).out_uvs.size() * sizeof(glm::vec2),
		&((*obj).out_uvs[0]),
		GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, (*obj).buffer[2]);	// Bind Buffer normals coordinates
	glBufferStorage(GL_ARRAY_BUFFER,
		(*obj).out_normals.size() * sizeof(glm::vec3),
		&((*obj).out_normals[0]),
		GL_DYNAMIC_STORAGE_BIT);

	glVertexAttribFormat(0,			// Atrib index 
		3,							// Size
		GL_FLOAT,					// Type
		GL_FALSE,					// Normalised
		0);							// Offset					
	glBindVertexBuffer(				// Bind buffer to the vao and format. 
		0,							// Binding Index
		(*obj).buffer[0],					// buffer
		0,							// offset
		sizeof(GLfloat) * 3);		// Stride
	glVertexAttribBinding(0, 0);	// Atrib Index, binding Index 
	glEnableVertexAttribArray(0);	// Enable Vertex Array Attribute

	glVertexAttribFormat(1,			// Atrib index 
		2,							// Size
		GL_FLOAT,					// Type
		GL_FALSE,					// Normalised
		0);							// Offset					
	glBindVertexBuffer(				// Bind buffer to the vao and format. 
		1,							// Binding Index
		(*obj).buffer[1],					// buffer
		0,							// offset
		sizeof(GLfloat) * 2);		// Stride
	glVertexAttribBinding(1, 1);	// Atrib Index, binding Index 
	glEnableVertexAttribArray(1);	// Enable Color Array Attribute

	glVertexAttribFormat(2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0);
	glBindVertexBuffer(
		2,
		(*obj).buffer[2], 0,
		sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);



	(*obj).mv_location = glGetUniformLocation((*obj).program, "mv_matrix");
	(*obj).proj_location = glGetUniformLocation((*obj).program, "proj_matrix");
	(*obj).tex_location = glGetUniformLocation((*obj).program, "tex");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void startupLight() {
	lightModel.program = glCreateProgram();

	string vs_textLight = readShader("vs_light.glsl"); static const char* vs_sourceLight = vs_textLight.c_str();
	GLuint vsLight = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsLight, 1, &vs_sourceLight, NULL);
	glCompileShader(vsLight);
	checkErrorShader(vsLight);
	glAttachShader(lightModel.program, vsLight);

	string fs_textLight = readShader("fs_light.glsl"); static const char* fs_sourceLight = fs_textLight.c_str();
	GLuint fsLight = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsLight, 1, &fs_sourceLight, NULL);
	glCompileShader(fsLight);
	checkErrorShader(fsLight);
	glAttachShader(lightModel.program, fsLight);

	glLinkProgram(lightModel.program);

	readObj("sphere.obj", &lightModel);

	glGenVertexArrays(1, &lightModel.vao);			// Create Vertex Array Object
	glBindVertexArray(lightModel.vao);				// Bind VertexArrayObject

	glGenBuffers(3, lightModel.buffer);			// Create new buffers (Vertices, Texture Coordinates, Normals
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[0]);	// Bind Buffer Vertex
	glBufferStorage(GL_ARRAY_BUFFER, lightModel.out_vertices.size() * sizeof(glm::vec3), &lightModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[1]);	// Bind Buffer UV
	glBufferStorage(GL_ARRAY_BUFFER, lightModel.out_uvs.size() * sizeof(glm::vec2), &lightModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[2]);	// Bind Buffer Normals
	glBufferStorage(GL_ARRAY_BUFFER, lightModel.out_normals.size() * sizeof(glm::vec3), &lightModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);		// Vertices									
	glBindVertexBuffer(0, lightModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 2, GL_FLOAT, GL_FALSE, 0);			// UV									
	glBindVertexBuffer(1, lightModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexAttribBinding(1, 1);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);			// Normals									
	glBindVertexBuffer(2, lightModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexAttribBinding(2, 2);
	glEnableVertexAttribArray(2);


	lightModel.mv_location = glGetUniformLocation(lightModel.program, "mv_matrix");
	lightModel.proj_location = glGetUniformLocation(lightModel.program, "proj_matrix");
	lightModel.tex_location = glGetUniformLocation(lightModel.program, "tex");

}

void update(double currentTime) {

	// calculate movement
	GLfloat cameraSpeed = 0.003f * deltaTime;
	if (keyStatus[GLFW_KEY_W] && radius > 3) radius -= 0.1;
	if (keyStatus[GLFW_KEY_S] && radius < 8
		) radius += 0.1;

}

void renderPrep(modelObject *obj) {
	glUseProgram((*obj).program);
	glBindVertexArray((*obj).vao);
	glUniformMatrix4fv((*obj).proj_location, 1, GL_FALSE, &(proj_matrix[0][0]));

	glUniform4f(glGetUniformLocation((*obj).program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation((*obj).program, "lightPosition"), lightDisp.x, lightDisp.y, lightDisp.z, 1.0);
	glUniform4f(glGetUniformLocation((*obj).program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation((*obj).program, "ka"), ka);
	glUniform4f(glGetUniformLocation((*obj).program, "id"), id.r, id.g, id.b, 1.0);
	glUniform1f(glGetUniformLocation((*obj).program, "kd"), 1.0f);
	glUniform4f(glGetUniformLocation((*obj).program, "is"), is.r, is.g, is.b, 1.0);
	glUniform1f(glGetUniformLocation((*obj).program, "ks"), 1.0f);
	glUniform1f(glGetUniformLocation((*obj).program, "shininess"), 32.0f);

	// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, (*obj).texture[0]);
	glUniform1i((*obj).tex_location, 0);
}

void render(double currentTime) {
	glViewport(0, 0, windowWidth, windowHeight);
	// Clear colour buffer
	static const GLfloat silver[] = { 0.0f, 0.0f, 0.0f, 0.0f }; glClearBufferfv(GL_COLOR, 0, silver);
	// Clear Deep buffer
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);

	glm::vec3 modelPositions[] = {
		glm::vec3(0.0f,  0.2f,  0.0f), //cube map
		glm::vec3(0.6f, 0.58f, -0.40f),  //4 platformers
		glm::vec3(-0.6f,  0.58f, -0.40f),
		glm::vec3(-0.4f, 0.58f, -0.6f),
		glm::vec3(-0.4f,  0.58f, 0.6f),
		glm::vec3(0.00f,  0.67f, 0.00f), //grass
		glm::vec3(0.00f,  0.67f, 0.05f),
		glm::vec3(0.00f,  0.67f, 0.10f),
		glm::vec3(0.00f,  0.67f, 0.15f),
		glm::vec3(0.00f,  0.67f, 0.20f),
		glm::vec3(0.00f,  0.67f, 0.25f),
		glm::vec3(0.00f,  0.67f, 0.30f),
		glm::vec3(0.00f,  0.67f, -0.05f),
		glm::vec3(0.00f,  0.67f, -0.10f),
		glm::vec3(0.00f,  0.67f, -0.15f),
		glm::vec3(0.00f,  0.67f, -0.20f),
		glm::vec3(0.00f,  0.67f, -0.25f),
		glm::vec3(0.00f,  0.67f, -0.30f),
		glm::vec3(0.00f,  0.67f, 0.00f),
		glm::vec3(0.05f,  0.67f, 0.00f),
		glm::vec3(0.10f,  0.67f, 0.00f),
		glm::vec3(0.15f,  0.67f, 0.00f),
		glm::vec3(0.20f,  0.67f, 0.00f),
		glm::vec3(0.25f,  0.67f, 0.00f),
		glm::vec3(0.30f,  0.67f, 0.00f),
		glm::vec3(-0.05f,  0.67f, 0.00f),
		glm::vec3(-0.10f,  0.67f, 0.00f),
		glm::vec3(-0.15f,  0.67f, 0.00f),
		glm::vec3(-0.20f,  0.67f, 0.00f),
		glm::vec3(-0.25f,  0.67f, 0.00f),
		glm::vec3(-0.30f,  0.67f, 0.00f),
		glm::vec3(0.0f,  0.68f, 0.0f), //tree
		glm::vec3(0.22f,  0.68f, 0.22f), //rocks
		glm::vec3(-0.22f,  0.68f, -0.22f),
		glm::vec3(-0.22f,  0.68f, 0.22f),
		glm::vec3(0.22f,  0.68f, -0.22f),
		glm::vec3(0.0f,  0.0f,  0.0f), //cubemap
	};
	glm::vec3 modelRotations[] = {
		glm::vec3(0.0f, 0.0f,  0.0f), //cube world
		glm::vec3(0.0f, 1.55f,  0.0f), //4 platformers
		glm::vec3(0.0f, -1.55f,  0.0f),
		glm::vec3(0.0f, 3.10f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f), //grass 
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f), //grass rotated 90
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 1.55f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f), //tree
		glm::vec3(0.0f, 0.0f,  0.0f), //rocks
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.0f,  0.0f),
		glm::vec3(0.0f, -1.55f,  0.0f), //cubemap
	};



	GLfloat camX = sin(currentTime / 4) * radius;
	GLfloat camZ = cos(currentTime / 4) * radius;
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(camX, 1.5, camZ),
		glm::vec3(0.0, 0.0, 0.0),
		cameraUp);


	double timeDifference = currentTime - previousTime;

	if (animationAdd > 0.39) {
		cout << animationAdd;
		directionforward = false;
	}
	if (animationAdd < -0.39) {
		directionforward = true;
	}

	if (directionforward == true) {
		animationAdd += (timeDifference / 10);
	}
	else {
		animationAdd -= (timeDifference / 10);
	}

	previousTime = currentTime;

	glm::vec3 characterPositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(0.6f, 0.58f, animationAdd),
		glm::vec3(-0.6f,  0.58f, -animationAdd),
		glm::vec3(animationAdd, 0.58f, -0.6f),
		glm::vec3(-animationAdd,  0.58f, 0.6f),

	};

	renderPrep(&cube_world);

	for (int i = 0; i < 1; i++) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, disp));
		modelMatrix = glm::translate(modelMatrix, modelPositions[i]);
		modelMatrix = glm::rotate(modelMatrix, cube_world.angleX + modelRotations[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, cube_world.angleY + modelRotations[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.12f, 0.12f, 0.12f));
		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(cube_world.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(cube_world.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, cube_world.out_vertices.size());
	}

	renderPrep(&platformer_happy);
	for (int i = 1; i < 5; i++) {
		//different model textures
		if (i == 2) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, platformer_sad.texture[0]);
			glUniform1i(platformer_sad.tex_location, 0);
		}
		if (i == 3) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, platformer_angry.texture[0]);
			glUniform1i(platformer_angry.tex_location, 0);
		}
		if (i == 4) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, platformer_annoyed.texture[0]);
			glUniform1i(platformer_annoyed.tex_location, 0);
		}
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, disp));
		modelMatrix = glm::translate(modelMatrix, characterPositions[i]);
		modelMatrix = glm::rotate(modelMatrix, platformer_happy.angleX + modelRotations[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, platformer_happy.angleY + modelRotations[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 0.15f));
		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(platformer_happy.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(platformer_happy.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, platformer_happy.out_vertices.size());
	}

	renderPrep(&grass);
	for (int i = 5; i < 31; i++) {		

		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, disp));
		modelMatrix = glm::translate(modelMatrix, modelPositions[i]);
		modelMatrix = glm::rotate(modelMatrix, grass.angleX + modelRotations[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, grass.angleY + modelRotations[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.030f, 0.030f, 0.030f));
		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(grass.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(grass.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, grass.out_vertices.size());
	}
	
	renderPrep(&tree);

	for (int i = 31; i < 32; i++) {

		if (treeColor == 1 || treeColor == 2) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, treeSmart_Happy.texture[0]);
			glUniform1i(treeSmart_Happy.tex_location, 0);
		}
		if (treeColor == 3 || treeColor == 4) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, treeSmart_Angry.texture[0]);
			glUniform1i(treeSmart_Angry.tex_location, 0);
		}
		if (treeColor == 5 || treeColor == 6) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, treeSmart_Sad.texture[0]);
			glUniform1i(treeSmart_Sad.tex_location, 0);
		}
		if (treeColor == 7 || treeColor == 8) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, treeSmart_Annoyed.texture[0]);
			glUniform1i(treeSmart_Annoyed.tex_location, 0);
		}
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, disp));
		modelMatrix = glm::translate(modelMatrix, modelPositions[i]);
		modelMatrix = glm::rotate(modelMatrix, tree.angleX + modelRotations[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, tree.angleY + modelRotations[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.035f, 0.035f, 0.035f));
		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(tree.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(tree.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, tree.out_vertices.size());
	}

	renderPrep(&rock);
	for (int i = 32; i < 36; i++) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, disp));
		modelMatrix = glm::translate(modelMatrix, modelPositions[i]);
		modelMatrix = glm::rotate(modelMatrix, rock.angleX + modelRotations[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, rock.angleY + modelRotations[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f, 0.025f, 0.02f));
		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(rock.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(rock.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, rock.out_vertices.size());
	}
	renderPrep(&cube_map);

	for (int i = 36; i < 37; i++) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, disp));
		modelMatrix = glm::translate(modelMatrix, modelPositions[i]);
		modelMatrix = glm::rotate(modelMatrix, cube_map.angleX + modelRotations[i].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, cube_map.angleY + modelRotations[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(cube_map.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(cube_map.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, cube_map.out_vertices.size());
	}
	//render light
	glUseProgram(lightModel.program);
	glBindVertexArray(lightModel.vao);
	glUniformMatrix4fv(lightModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);
	// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lightModel.texture[0]);
	glUniform1i(lightModel.tex_location, 0);

	glm::mat4 modelMatrixLight = glm::translate(glm::mat4(1.0f), glm::vec3(lightDisp.x, lightDisp.y, lightDisp.z));
	modelMatrixLight = glm::scale(modelMatrixLight, glm::vec3(0.2f, 0.2f, 0.2f));
	glm::mat4 mv_matrixLight = viewMatrix * modelMatrixLight;

	glUniformMatrix4fv(lightModel.mv_location, 1, GL_FALSE, &mv_matrixLight[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, lightModel.out_vertices.size());

}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);	

}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

	if (button == 0) {
		if (treeColor == 8 || treeColor ==10) {
			treeColor = 1;
		}
		else
		{
			treeColor++;
		}
	}
	if (button == 1) {
		treeColor = 0;
	}
	cout << treeColor;
}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
}

static void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);

	fovy += yoffsetInt;
	if (fovy >= 1.0f && fovy <= 45.0f) fovy -= yoffset;
	if (fovy <= 1.0f) fovy = 1.0f;
	if (fovy >= 45.0f) fovy = 45.0f;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void debugGL() {
	//Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

void  checkErrorShader(GLuint shader) {
	// Get log lenght
	GLint maxLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// Init a string for it
	std::vector<GLchar> errorLog(maxLength);

	if (maxLength > 1) {
		// Get the log file
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		cout << "--------------Shader compilation error-------------\n";
		cout << errorLog.data();
	}

}

void readObj(string name, modelObject *obj) {
	cout << "Loading model " << name << "\n";

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< string > materials, textures;
	std::vector< glm::vec3 > obj_vertices;
	std::vector< glm::vec2 > obj_uvs;
	std::vector< glm::vec3 > obj_normals;

	std::ifstream dataFile(name);

	string rawData;		// store the raw data.
	int countLines = 0;
	if (dataFile.is_open()) {
		string dataLineRaw;
		while (getline(dataFile, dataLineRaw)) {
			rawData += dataLineRaw;
			rawData += "\n";
			countLines++;
		}
		dataFile.close();
	}

	cout << "Finished reading model file " << name << "\n";

	istringstream rawDataStream(rawData);
	string dataLine;
	int linesDone = 0;
	while (std::getline(rawDataStream, dataLine)) {
		if (dataLine.find("v ") != string::npos) {	// does this line have a vector?
			glm::vec3 vertex;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			vertex.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_vertices.push_back(vertex);
		}
		else if (dataLine.find("vt ") != string::npos) {	// does this line have a uv coordinates?
			glm::vec2 uv;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			uv.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			uv.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_uvs.push_back(uv);
		}
		else if (dataLine.find("vn ") != string::npos) { // does this line have a normal coordinates?
			glm::vec3 normal;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			normal.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_normals.push_back(normal);
		}
		else if (dataLine.find("f ") != string::npos) { // does this line defines a triangle face?
			string parts[3];

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			parts[0] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[1] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[2] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			for (int i = 0; i < 3; i++) {		// for each part

				vertexIndices.push_back(stoul(parts[i].substr(0, parts[i].find("/"))));

				int firstSlash = parts[i].find("/"); int secondSlash = parts[i].find("/", firstSlash + 1);

				if ((firstSlash + 1) != (secondSlash)) {	// there are texture coordinates.
					uvIndices.push_back(stoul(parts[i].substr(firstSlash + 1, secondSlash - firstSlash + 1)));
				}


				normalIndices.push_back(stoul(parts[i].substr(secondSlash + 1)));

			}
		}
		else if (dataLine.find("mtllib ") != string::npos) { // does this object have a material?
			materials.push_back(dataLine.substr(dataLine.find(" ") + 1));
		}

		linesDone++;

		if (linesDone % 50000 == 0) {
			cout << "Parsed " << linesDone << " of " << countLines << " from model...\n";
		}

	}

	for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i + 2] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i + 2] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i + 2] - 1]);

		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i + 1] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i + 1] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i + 1] - 1]);

		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i + 0] - 1]);
	}

	// Load Materials
	for (unsigned int i = 0; i < materials.size(); i++) {

		std::ifstream dataFileMat(materials[i]);

		string rawDataMat;		// store the raw data.
		int countLinesMat = 0;
		if (dataFileMat.is_open()) {
			string dataLineRawMat;
			while (getline(dataFileMat, dataLineRawMat)) {
				rawDataMat += dataLineRawMat;
				rawDataMat += "\n";
			}
			dataFileMat.close();
		}

		istringstream rawDataStreamMat(rawDataMat);
		string dataLineMat;
		while (std::getline(rawDataStreamMat, dataLineMat)) {
			if (dataLineMat.find("map_Kd ") != string::npos) {	// does this line have a texture map?
				textures.push_back(dataLineMat.substr(dataLineMat.find(" ") + 1));
			}
		}
	}

	(*obj).texture = new GLuint[textures.size()];		// Warning possible memory leak here - there is a new here, where is your delete?
	glGenTextures(textures.size(), (*obj).texture);

	for (int i = 0; i < textures.size(); i++) {
		readTexture(textures[i], (*obj).texture[i]);
	}

	cout << "done";
}

void readTexture(string name, GLuint textureName) {

	// Flip images as OpenGL expects 0.0 coordinates on the y-axis to be at the bottom of the image.
	stbi_set_flip_vertically_on_load(true);

	// Load image Information.
	int iWidth, iHeight, iChannels;
	unsigned char *iData = stbi_load(name.c_str(), &iWidth, &iHeight, &iChannels, 0);

	// Load and create a texture 
	glBindTexture(GL_TEXTURE_2D, textureName); // All upcoming operations now have effect on this texture object
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, iWidth, iHeight);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGBA, GL_UNSIGNED_BYTE, iData);

	// This only works for 2D Textures...
	// Set the texture wrapping parameters 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Generate mipmaps 
	glGenerateMipmap(GL_TEXTURE_2D);

}
