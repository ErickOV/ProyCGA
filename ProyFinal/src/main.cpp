//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"

// Texture include
#include "Headers/Texture.h"

#include "Headers\CubemapTexture.h"

// Camera include
#include "Headers/CameraFPS.h"

#include "Headers/Model.h"

// Sphere include
#include "Headers/Sphere.h"

#include "Headers\collision.h"

// OpenAL include
#include <AL/alut.h>

#define NUM_BUFFERS 3
#define NUM_SOURCES 3
#define NUM_ENVIRONMENTS 1


Sphere sp(1.5, 50, 50, MODEL_MODE::VERTEX_COLOR);
Sphere sp2(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);

Shader lightingShader;
Shader lampShader;
Shader cubemapShader;
Shader envCubeShader;
Shader ShaderM;
Shader shader;
Shader shaderP;

Texture * texture1;
Texture * texture2;

Model king, king2,
		  queen, queen2,
	      pawn, pawn2,
		  rook, rook2,
		  bishop, bishop2,
		  knight, knight2,
			snitch;

std::vector<GLuint> rays;

Texture textureDifuse(GL_TEXTURE_2D, "../Textures/container2.png");
Texture textureSpecular(GL_TEXTURE_2D, "../Textures/container2_specular.png");
CubemapTexture * cubeMaptexture = new CubemapTexture("../Textures/mp_amh", "amh_ft.tga", "amh_bk.tga", "amh_up.tga", "amh_dn.tga", "amh_rt.tga", "amh_lf.tga");

GLuint VBO, VAO, EBO, texture, shader_compute;

struct VertexColorTexture {
	glm::vec3 m_Pos;
	glm::vec3 m_Color;
	glm::vec2 m_TexCoord;
};


int screenWidth;
int screenHeight;

GLFWwindow * window;
InputManager inputManager;
double deltaTime;

// OpenAL config
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };

ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };

ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };

ALfloat source2Pos[] = { 0.0, 0.0, -4.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };

ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];

ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable test depth, must be clear depth buffer bit
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	// Set up vertex data (and buffer(s)) and attribute pointers
	VertexColorTexture vertices[36] = {
		// Back face
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.15f, 0.44f, 0.18f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.15f, 0.31f, 0.25f), glm::vec2(1.0f, 0.0f)},
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.21f, 0.61f, 0.25f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.21f, 0.61f, 0.25f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.15f, 0.31f, 0.25f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.15f, 0.44f, 0.18f), glm::vec2(0.0f, 0.0f) },
		// Front face
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.15f, 0.44f, 0.18f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.15f, 0.31f, 0.25f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.21f, 0.61f, 0.25f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.21f, 0.61f, 0.25f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.15f, 0.31f, 0.25f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.15f, 0.44f, 0.18f), glm::vec2(0.0f, 0.0f) },
		// Left face
		{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.99f, 0.22f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.79f, 0.22f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.89f, 0.69f, 0.22f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.89f, 0.69f, 0.22f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.79f, 0.22f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.99f, 0.22f), glm::vec2(1.0f, 0.0f) },
		// Right face
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.99f, 0.22f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.79f, 0.22f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.89f, 0.69f, 0.22f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.89f, 0.69f, 0.22f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.79f, 0.22f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.99f, 0.22f), glm::vec2(1.0f, 0.0f) },
		// Bottom face
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.96f, 0.58f, 0.94f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.66f, 0.58f, 0.94f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.36f, 0.38f, 0.94f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.36f, 0.38f, 0.94f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.66f, 0.58f, 0.94f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.96f, 0.58f, 0.94f), glm::vec2(0.0f, 1.0f) },
		// Top face
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.96f, 0.58f, 0.94f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.66f, 0.58f, 0.94f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.36f, 0.38f, 0.64f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.36f, 0.38f, 0.64f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.66f, 0.58f, 0.94f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.96f, 0.58f, 0.94f), glm::vec2(0.0f, 1.0f) }
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 2, // First Triangle
		3, 4, 5,  // Second Triangle
		6, 7, 8,
		9,10,11,
		12,13,14,
		15,16,17,
		18,19,20,
		21,22,23,
		24,25,26,
		27,28,29,
		30,31,32,
		33,34,35
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*) sizeof(vertices[0].m_Pos));
	glEnableVertexAttribArray(1);
	// Texture Mapping attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*)(sizeof(vertices[0].m_Color) + sizeof(vertices[0].m_Pos)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	const char* shaderOutputs[4] = {
	"outPosition",
	"outColor",
	"outVelocity",
	"outOthers"
	};
	glTransformFeedbackVaryings(shader_compute, 4, shaderOutputs, GL_INTERLEAVED_ATTRIBS);


	sp.init();
	sp.load();

	sp2.init();
	sp2.load();

	king.loadModel("../objects/CGA/King/10580_Chess_King_v2_Iterations-2.obj");
	queen.loadModel("../objects/CGA/Queen/10581_Chess_Queen_v2_Iterations-2.obj");
	rook.loadModel("../objects/CGA/Rook/10583_Chess Rook_v2_Iterations-2.obj");
	pawn.loadModel("../objects/CGA/Pawn/10585_Chess Pawn_v1_Iterations-2.obj");
	bishop.loadModel("../objects/CGA/Bishop/10582_Chess Bishop_v2_Iterations-2.obj");
	knight.loadModel("../objects/CGA/Knight/10584_Chess Knight_v2_Iterations-2.obj");

	king2.loadModel("../objects/CGA/King2/10580_Chess_King_v2_Iterations-2.obj");
	queen2.loadModel("../objects/CGA/Queen2/10581_Chess_Queen_v2_Iterations-2.obj");
	rook2.loadModel("../objects/CGA/Rook2/10583_Chess Rook_v2_Iterations-2.obj");
	pawn2.loadModel("../objects/CGA/Pawn2/10585_Chess Pawn_v1_Iterations-2.obj");
	bishop2.loadModel("../objects/CGA/Bishop2/10582_Chess Bishop_v2_Iterations-2.obj");
	knight2.loadModel("../objects/CGA/Knight2/10584_Chess Knight_v2_Iterations-2.obj");

	snitch.loadModel("../objects/CGA/Snitch/Snitch.obj");

	lightingShader.initialize("../Shaders/loadModelLighting.vs", "../Shaders/loadModelLighting.fs");
	lampShader.initialize("../Shaders/lampShader.vs", "../Shaders/lampShader.fs");
	cubemapShader.initialize("../Shaders/cubemapTexture.vs", "../Shaders/cubemapTexture.fs");
	envCubeShader.initialize("../Shaders/envRefCubemapTexture.vs", "../Shaders/envRefCubemapTexture.fs");
	ShaderM.initialize("../Shaders/model.vs", "../Shaders/model.fs");
	shader.initialize("../Shaders/mvpTexture.vs", "../Shaders/mixTexture.fs");
	textureDifuse.load();

	texture1 = new Texture(GL_TEXTURE_2D, "../Textures/Emblema.jpg");
	texture1->load();

	texture2 = new Texture(GL_TEXTURE_2D, "../Textures/tablero.jpg");
	texture2->load();

	cubeMaptexture->Load();

	// OpenAL init
	alutInit(0, NULL);

	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	alGetError(); // clear any error messages

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}

	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);

	buffer[0] = alutCreateBufferFromFile("../sounds/Dracarys.wav");
	//buffer[0] = alutCreateBufferHelloWorld();

	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}

	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 1.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 1200);

	inputManager.getCameraFPS()->Position = glm::vec3(0, 0, 0);





}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();
	lightingShader.destroy();
	lampShader.destroy();
	cubemapShader.destroy();
	envCubeShader.destroy();
	delete cubeMaptexture;



	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	inputManager.keyPressed(inputManager.toApplicationKey(key), deltaTime * 10.0f,
		inputManager.toApplicationState(action));
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	inputManager.mouseMoved(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	inputManager.mouseClicked(inputManager.toMouseButtonIndex(button), xpos,
		ypos, inputManager.toApplicationState(state));
}

bool processInput(bool continueApplication) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
		|| glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	inputManager.do_movement(deltaTime);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::vec3 lightPos(1.2f, 20.0f, 2.0f);
	double lastTime = TimeManager::Instance().GetTime();

	glm::vec3 cubePositions[] =
	{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-2.0f, -5.0f, -15.0f),
	  glm::vec3(-1.5f, 2.2f, -2.5f), glm::vec3(1.8f, 1.0f, -12.3f),
	  glm::vec3(4.4f, -0.4f, -3.5f), glm::vec3(-6.7f, 3.0f, -7.5f),
	  glm::vec3(-4.3f, -3.0f, -2.5f), glm::vec3(3.5f, 8.0f, -2.5f),
	  glm::vec3(-1.7f, -0.7f, -1.5f), glm::vec3(3.3f, 3.0f, -1.5f) };

	SBB sbb1 = getSBB(king.getMeshes());
	SBB sbb2 = getSBB(queen.getMeshes());
	SBB sbb3 = getSBB(knight.getMeshes());
	SBB sbb4 = getSBB(bishop.getMeshes());
	SBB sbb5 = getSBB(rook.getMeshes());
	SBB sbb6 = getSBB(pawn.getMeshes());

	while (psi) {
		psi = processInput(true);
		// This is new, need clear depth buffer bit
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create camera transformations
		glm::mat4 view = inputManager.getCameraFPS()->GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);


		cubemapShader.turnOn();

		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;

		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

		GLint viewLoc = cubemapShader.getUniformLocation("view");
		GLint projLoc = cubemapShader.getUniformLocation("projection");
		GLint modelLoc = cubemapShader.getUniformLocation("model");

		view = glm::mat3(inputManager.getCameraFPS()->GetViewMatrix());
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 cubeModel;
		cubeModel = glm::scale(cubeModel, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeModel));

		cubeMaptexture->Bind(GL_TEXTURE0);
		GLuint cubeTextureId = cubemapShader.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);

		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sp2.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		cubemapShader.turnOff();

		envCubeShader.turnOn();

		view = inputManager.getCameraFPS()->GetViewMatrix();
		viewLoc = envCubeShader.getUniformLocation("view");
		projLoc = envCubeShader.getUniformLocation("projection");
		modelLoc = envCubeShader.getUniformLocation("model");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		cubeMaptexture->Bind(GL_TEXTURE0);
		cubeTextureId = envCubeShader.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);

		GLuint viewPosLoc = envCubeShader.getUniformLocation("viewPos");
		glUniform3fv(viewPosLoc,1, glm::value_ptr(inputManager.getCameraFPS()->Position));

		ShaderM.turnOn();

		glm::mat4 model1, model3, model5, model7, model9, model11, model13, model15,
			            model17, model19, model21, model23, model25, model27, model29, model31;
		glm::vec3 mov1 = glm::vec3(-2.0f, 0.0f, -17.5f);

		model9 = glm::scale(model9, glm::vec3(2.0f, 2.0f, 2.0f));
		model9 = glm::translate(model9, glm::vec3(-17.25f, 0.0f, -17.5f));
		model9 = glm::rotate(model9, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));
		rook.render(&ShaderM);

		model5 = glm::scale(model5, glm::vec3(2.0f, 2.0f, 2.0f));
		model5 = glm::translate(model5, glm::vec3(-12.25f, 0.0f, -17.5f));
		model5 = glm::rotate(model5, 55.0f, glm::vec3(10.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));
		knight.render(&ShaderM);

		model7 = glm::scale(model7, glm::vec3(2.0f, 2.0f, 2.0f));
		model7 = glm::translate(model7, glm::vec3(-7.25f, 0.0f, -17.5f));
		model7 = glm::rotate(model7, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));
		bishop.render(&ShaderM);

		model1 = glm::scale(model1, glm::vec3(2.0f, 2.0f, 2.0f));
		model1 = glm::translate(model1, glm::vec3(-2.0f, 0.0f, -17.5f));
		model1 = glm::rotate(model1, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
		king.render(&ShaderM);

		model3 = glm::scale(model3, glm::vec3(2.0f, 2.0f, 2.0f));
		model3 = glm::translate(model3, glm::vec3(3.0f, 0.0f, -17.5f));
		model3 = glm::rotate(model3, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
		queen.render(&ShaderM);

		model13 = glm::scale(model13, glm::vec3(2.0f, 2.0f, 2.0f));
		model13 = glm::translate(model13, glm::vec3(8.0f, 0.0f, -17.5f));
		model13 = glm::rotate(model13, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model13));
		bishop.render(&ShaderM);

		model11 = glm::scale(model11, glm::vec3(2.0f, 2.0f, 2.0f));
		model11 = glm::translate(model11, glm::vec3(13.0f, 0.0f, -17.5f));
	    model11 = glm::rotate(model11, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model11));
		knight.render(&ShaderM);

		model15 = glm::scale(model15, glm::vec3(2.0f, 2.0f, 2.0f));
		model15 = glm::translate(model15, glm::vec3(18.0f, 0.0f, -17.5f));
		model15 = glm::rotate(model15, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model15));
		rook.render(&ShaderM);

		model17 = glm::scale(model17, glm::vec3(2.0f, 2.0f, 2.0f));
		model17 = glm::translate(model17, glm::vec3(-17.25f, 0.0f, -12.5f));
		model17 = glm::rotate(model17, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model17));
		pawn.render(&ShaderM);

		model19 = glm::scale(model19, glm::vec3(2.0f, 2.0f, 2.0f));
		model19 = glm::translate(model19, glm::vec3(-12.25f, 0.0f, -12.5f));
		model19 = glm::rotate(model19, 55.0f, glm::vec3(10.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model19));
		pawn.render(&ShaderM);

		model21 = glm::scale(model21, glm::vec3(2.0f, 2.0f, 2.0f));
		model21 = glm::translate(model21, glm::vec3(-7.25f, 0.0f, -12.5f));
		model21 = glm::rotate(model21, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));
		pawn.render(&ShaderM);

		model23 = glm::scale(model23, glm::vec3(2.0f, 2.0f, 2.0f));
		model23 = glm::translate(model23, glm::vec3(-2.0f, 0.0f, -12.5f));
		model23 = glm::rotate(model23, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model23));
		pawn.render(&ShaderM);

		model25 = glm::scale(model25, glm::vec3(2.0f, 2.0f, 2.0f));
		model25 = glm::translate(model25, glm::vec3(3.0f, 0.0f, -12.5f));
		model25 = glm::rotate(model25, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model25));
		pawn.render(&ShaderM);

		model27 = glm::scale(model27, glm::vec3(2.0f, 2.0f, 2.0f));
		model27 = glm::translate(model27, glm::vec3(8.0f, 0.0f, -12.5f));
		model27 = glm::rotate(model27, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model27));
		pawn.render(&ShaderM);

		model29 = glm::scale(model29, glm::vec3(2.0f, 2.0f, 2.0f));
		model29 = glm::translate(model29, glm::vec3(13.0f, 0.0f, -12.5f));
		model29 = glm::rotate(model29, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model29));
		pawn.render(&ShaderM);

		model31 = glm::scale(model31, glm::vec3(2.0f, 2.0f, 2.0f));
		model31 = glm::translate(model31, glm::vec3(18.0f, 0.0f, -12.5f));
		model31 = glm::rotate(model31, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model31));
		pawn.render(&ShaderM);


		ShaderM.turnOff();

		envCubeShader.turnOff();

		ShaderM.turnOn();

		view = inputManager.getCameraFPS()->GetViewMatrix();
		viewLoc = ShaderM.getUniformLocation("view");
		projLoc = ShaderM.getUniformLocation("projection");
		modelLoc = ShaderM.getUniformLocation("model");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model2, model4, model6, model8, model10, model12, model14, model16, model18,
			             model20, model22, model24, model26, model28, model30, model32, model34, model36;

		model10 = glm::scale(model10, glm::vec3(2.0f, 2.0f, 2.0f));
		model10 = glm::translate(model10, glm::vec3(-17.25f, 0.0f, 17.75f));
		model10 = glm::rotate(model10, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model10 = glm::rotate(model10, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model10));
		rook2.render(&ShaderM);

		model6 = glm::scale(model6, glm::vec3(2.0f, 2.0f, 2.0f));
		model6 = glm::translate(model6, glm::vec3(-12.25f, 0.0f, 17.75f));
		model6 = glm::rotate(model6, 55.0f, glm::vec3(10.0f, 0.0f, 0.0f));
		model6 = glm::rotate(model6, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));
		knight2.render(&ShaderM);

		model8 = glm::scale(model8, glm::vec3(2.0f, 2.0f, 2.0f));
		model8 = glm::translate(model8, glm::vec3(-7.25f, 0.0f, 17.75f));
		model8 = glm::rotate(model8, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model8 = glm::rotate(model8, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model8));
		bishop2.render(&ShaderM);

		model2 = glm::scale(model2, glm::vec3(2.0f, 2.0f, 2.0f));
		model2 = glm::translate(model2, glm::vec3(-2.0f, 0.0f, 17.75f));
		model2 = glm::rotate(model2, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model2 = glm::rotate(model2, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
		king2.render(&ShaderM);

		model4 = glm::scale(model4, glm::vec3(2.0f, 2.0f, 2.0f));
		model4 = glm::translate(model4, glm::vec3(3.0f, 0.0f, 17.75f));
		model4 = glm::rotate(model4, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model4 = glm::rotate(model4, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));
		queen2.render(&ShaderM);

        model16 = glm::scale(model16, glm::vec3(2.0f, 2.0f, 2.0f));
		model16 = glm::translate(model16, glm::vec3(8.0f, 0.0f, 17.75f));
		model16 = glm::rotate(model16, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model16 = glm::rotate(model16, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model16));
		bishop2.render(&ShaderM);

        model14 = glm::scale(model14, glm::vec3(2.0f, 2.0f, 2.0f));
		model14 = glm::translate(model14, glm::vec3(13.0f, 0.0f, 17.75f));
		model14 = glm::rotate(model14, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model14 = glm::rotate(model14, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model14));
		knight2.render(&ShaderM);

        model18 = glm::scale(model18, glm::vec3(2.0f, 2.0f, 2.0f));
		model18 = glm::translate(model18, glm::vec3(18.0f, 0.0f, 17.75f));
		model18 = glm::rotate(model18, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model18 = glm::rotate(model18, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model18));
		rook2.render(&ShaderM);

        model20 = glm::scale(model20, glm::vec3(2.0f, 2.0f, 2.0f));
		model20 = glm::translate(model20, glm::vec3(-17.25f, 0.0f, 12.75f));
		model20 = glm::rotate(model20, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model20 = glm::rotate(model20, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model20));
		pawn2.render(&ShaderM);

		model22 = glm::scale(model22, glm::vec3(2.0f, 2.0f, 2.0f));
		model22 = glm::translate(model22, glm::vec3(-12.25f, 0.0f, 12.75f));
		model22 = glm::rotate(model22, 55.0f, glm::vec3(10.0f, 0.0f, 0.0f));
		model22 = glm::rotate(model22, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model22));
		pawn2.render(&ShaderM);

		model24 = glm::scale(model24, glm::vec3(2.0f, 2.0f, 2.0f));
		model24 = glm::translate(model24, glm::vec3(-7.25f, 0.0f, 12.75f));
		model24 = glm::rotate(model24, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model24 = glm::rotate(model24, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model24));
		pawn2.render(&ShaderM);

		model26 = glm::scale(model26, glm::vec3(2.0f, 2.0f, 2.0f));
		model26 = glm::translate(model26, glm::vec3(-2.0f, 0.0f, 12.75f));
		model26 = glm::rotate(model26, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model26 = glm::rotate(model26, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model26));
		pawn2.render(&ShaderM);

		model28 = glm::scale(model28, glm::vec3(2.0f, 2.0f, 2.0f));
		model28 = glm::translate(model28, glm::vec3(3.0f, 0.0f, 12.75f));
		model28 = glm::rotate(model28, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model28 = glm::rotate(model28, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model28));
		pawn2.render(&ShaderM);

		model30 = glm::scale(model30, glm::vec3(2.0f, 2.0f, 2.0f));
		model30 = glm::translate(model30, glm::vec3(8.0f, 0.0f, 12.75f));
		model30 = glm::rotate(model30, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model30 = glm::rotate(model30, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model30));
		pawn2.render(&ShaderM);

        model32 = glm::scale(model32, glm::vec3(2.0f, 2.0f, 2.0f));
		model32 = glm::translate(model32, glm::vec3(13.0f, 0.0f, 12.75f));
		model32 = glm::rotate(model32, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model32 = glm::rotate(model32, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model32));
		pawn2.render(&ShaderM);

        model34 = glm::scale(model34, glm::vec3(2.0f, 2.0f, 2.0f));
		model34 = glm::translate(model34, glm::vec3(18.0f, 0.0f, 12.75f));
		model34 = glm::rotate(model34, 55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model34 = glm::rotate(model34, 3.15f, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model34));
		pawn2.render(&ShaderM);
		ShaderM.turnOff();

		shader.turnOn();

		view = inputManager.getCameraFPS()->GetViewMatrix();
		viewLoc = shader.getUniformLocation("view");
		projLoc = shader.getUniformLocation("projection");
		modelLoc = shader.getUniformLocation("model");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		texture1->bind(GL_TEXTURE0);
		glUniform1i(shader.getUniformLocation("textura1"), 0);
		texture2->bind(GL_TEXTURE1);
		glUniform1i(shader.getUniformLocation("textura2"), 1);

		glBindVertexArray(VAO);
		for (GLuint i = 0; i < 1; i++) {
			glm::mat4 model;

			model = glm::translate(model, glm::vec3(1.0f, -0.1f, 0.5f));
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			model = glm::scale(model, glm::vec3(100.0f, 0.05f, 100.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);

		shader.turnOff();


		// Render SBB
		lampShader.turnOn();
		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 model01;

		model01 = glm::translate(model1,
			glm::vec3(sbb1.center.x, sbb1.center.y, sbb1.center.z));
		model01 = glm::scale(model1,
			glm::vec3(sbb1.ratio, sbb1.ratio, sbb1.ratio));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model01));
		//sp.render();

		model2 = glm::translate(model2,
			glm::vec3(sbb2.center.x, sbb2.center.y, sbb2.center.z));
		model2 = glm::scale(model2,
			glm::vec3(sbb2.ratio, sbb2.ratio, sbb2.ratio));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
		//sp.render();
		lampShader.turnOff();

		// Render the light sphere model
		lampShader.turnOn();

		// For test collision sphere vs sphere
		SBB s1, s2;
		s1.center = glm::vec3(model01* glm::vec4(0, 0, 0, 1));
		s1.ratio = sbb1.ratio * 2.0f;
		s2.center = glm::vec3(model2* glm::vec4(0, 0, 0, 1));
		s2.ratio = sbb2.ratio * 2.0f;
		if (testSphereSphereIntersection(s1, s2))
			std::cout << "Model collision:" << std::endl;

		if (inputManager.isGenerateRay()) {
			std::cout << "Generate ray:" << std::endl;
			glm::vec4 viewport = glm::vec4(0.0f, 0.0f, screenWidth, screenHeight);
			glm::vec3 o =
				glm::unProject(
					glm::vec3(
						inputManager.getLastMousePos().x,
						screenHeight
						- inputManager.getLastMousePos().y,
						0.0f), view, projection,
					viewport);
			glm::vec3 t =
				glm::unProject(
					glm::vec3(
						inputManager.getLastMousePos().x,
						screenHeight
						- inputManager.getLastMousePos().y,
						1.0f), view, projection,
					viewport);

			glm::vec3 c1 = glm::vec3(model1 * glm::vec4(0, 0, 0, 1));
			glm::vec3 c2 = glm::vec3(model2 * glm::vec4(0, 0, 0, 1));
			glm::vec3 dir = glm::normalize(t - o);
			float t1;
			glm::vec3 q1;
			Plane p;

			glm::vec3 d = glm::vec3(-0.5f, 0.5f, -0.5f)*100.0f;
			glm::vec3 e = glm::vec3(0.5f, 0.5f, -0.5f)*100.0f;
			glm::vec3 f = glm::vec3(0.5f, 0.5f, 0.5f)*100.0f;

			p.Nor = glm::cross(e - d, f - d);
			p.Dir = glm::dot(p.Nor, d);


			glm::vec3 ab = t - o;
			t1 = (p.Dir - glm::dot(p.Nor, o)) / glm::dot(p.Nor, ab);

			if (t1 >= 0.0f && t1 <= 1.0f)
			{
				q1 = o + (t * ab);
			}

			if (raySphereIntersect(o, t, dir, c1, sbb1.ratio * 2.0f, t1) &&
				testPlane(o, t, d, e, f, p, t1, q1));
					std::cout << "Model collision:" << std::endl;

			if (inputManager.getKeyState()[InputCodes::Z]) {
				mov1 = glm::vec3(inputManager.getLastMousePos().x,	0.0f , 0.0f);
			}

				inputManager.setGenerateRay(false);

				GLuint VAO;

				VertexColor vertex[2] = { { o, glm::vec3(0.0) },
				{ t, glm::vec3(0.0) } };
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex,
					GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
					(GLvoid*)0);
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);

				rays.push_back(VAO);
		}

		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glm::mat4 modelLine;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelLine));

		for (int i = 0; i < rays.size(); i++) {
			glBindVertexArray(rays[i]);
			glDrawArrays(GL_LINES, 0, 2);
		}

		lampShader.turnOff();

		lightingShader.turnOn();

		viewPosLoc = lightingShader.getUniformLocation("viewPos");
		glUniform3f(viewPosLoc, inputManager.getCameraFPS()->Position.x, inputManager.getCameraFPS()->Position.y,
			inputManager.getCameraFPS()->Position.z);

		// Set material properties
		GLint matDiffuseLoc = lightingShader.getUniformLocation(
			"material.diffuse");
		GLint matSpecularLoc = lightingShader.getUniformLocation(
			"material.specular");
		GLint matShineLoc = lightingShader.getUniformLocation(
			"material.shininess");
		glUniform1i(matDiffuseLoc, 0);
		glUniform1i(matSpecularLoc, 1);
		glUniform1f(matShineLoc, 32.0f);

		// Set lights properties
		GLint lightAmbientLoc = lightingShader.getUniformLocation(
			"light.ambient");
		GLint lightDiffuseLoc = lightingShader.getUniformLocation(
			"light.diffuse");
		GLint lightSpecularLoc = lightingShader.getUniformLocation(
			"light.specular");
		GLint lightPosLoc = lightingShader.getUniformLocation("light.position");
		glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glUniform3f(lightDiffuseLoc, 0.7f, 0.5f, 0.5f); // Let's darken the light a bit to fit the scene
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

		// Get the uniform locations
		modelLoc = lightingShader.getUniformLocation("model");
		viewLoc = lightingShader.getUniformLocation("view");
		projLoc = lightingShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw a sphere
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(3.0f, 20.0f, 10.0f));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		textureDifuse.bind(GL_TEXTURE0);

		//sp2.render();
		snitch.render(&lightingShader);
		lightingShader.turnOff();

		lampShader.turnOn();
		// Create transformations
		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::translate(glm::mat4(), lightPos);
		model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		sp.render();
		lampShader.turnOff();

		glfwSwapBuffers(window);

		source0Pos[0] = model1[3].x;
		source0Pos[1] = model1[3].y;
		source0Pos[2] = model1[3].z;
		alSourcefv(source[0], AL_POSITION, source0Pos);

		listenerPos[0] = inputManager.getCameraFPS()->Position.x;
		listenerPos[1] = inputManager.getCameraFPS()->Position.y;
		listenerPos[2] = inputManager.getCameraFPS()->Position.z;
		alListenerfv(AL_POSITION, listenerPos);

		listenerOri[0] = inputManager.getCameraFPS()->Front.x;
		listenerOri[1] = inputManager.getCameraFPS()->Front.y;
		listenerOri[2] = inputManager.getCameraFPS()->Front.z;
		listenerOri[3] = inputManager.getCameraFPS()->Up.x;
		listenerOri[4] = inputManager.getCameraFPS()->Up.y;
		listenerOri[5] = inputManager.getCameraFPS()->Up.z;
		alListenerfv(AL_ORIENTATION, listenerOri);

		if (inputManager.getKeyState()[InputCodes::u]) {
			alSourcePlay(source[0]);
		}

	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}

