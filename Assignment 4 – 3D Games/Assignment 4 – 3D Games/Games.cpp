const char* INSTRUCTIONS =
"***************\n"
"This demo shows multiple objects being draw at once along with user interaction.\n"
"\n"
"Up/down/left/right - Moves the knot.\n"
"c - Changes to teapot to a random color.\n"
"***************\n"
;

//Mac OS build: g++ multiObjectTest.cpp -x c glad/glad.c -g -F/Library/Frameworks -framework SDL2 -framework OpenGL -o MultiObjTest
//Linux build:  g++ multiObjectTest.cpp -x c glad/glad.c -g -lSDL2 -lSDL2main -lGL -ldl -I/usr/include/SDL2/ -o MultiObjTest

#include "glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif
#include <cstdio>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include"Texture.h"
#include"shader.h"
#include"Model.h"
#include"Camera.h"
#include"Map.h"
#include"Light.h"
#include"Skybox.h"
#include <stdarg.h>             // Required for TraceLog()
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include<math.h>
#define   PI 3.141592653
using namespace std;
const int screenWidth = 800;
const int screenHeight = 600;
float timePast = 0;

//SJG: Store the object coordinates
//You should have a representation for the state of each object
float objx = 0, objy = 0, objz = 0;
float colR = 1, colG = 1, colB = 1;
static const int SKYBOX_WIDTH = 6;
static const int SKYBOX_LENGTH = 6;
static const int SKYBOX_HEIGHT = 6;

bool DEBUG_ON = true;
//GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;

//srand(time(NULL));
float rand01() {
	return rand() / (float)RAND_MAX;
}
#ifndef __cplusplus
// Boolean type
typedef enum { false, true } bool;
#endif

// Color type, RGBA (32bit)
struct Color 
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

// Rectangle type
struct Rect {
	int x;
	int y;
	float width;
	float height;
	Rect() {};
	Rect(int Ax,int Ay, float Awidth, float Aheight) :x(Ax),y(Ay),width(Awidth),height(Aheight){}
};

// Camera type, defines a camera position/orientation in 3d space
typedef enum { LOG_INFO = 0, LOG_ERROR, LOG_WARNING, LOG_DEBUG, LOG_OTHER } TraceLogType;
// Projection matrix to draw our world
glm::mat4 matProjection;                // Projection matrix to draw our world
glm::mat4 matModelview;                 // Modelview matrix to draw our world
//-------------------------------------frame control------------------------------------------------
static double currentTime, previousTime;    // Used to track timmings
static double frameTime = 0.0;              // Time measure for one frame
static double targetTime = 0.0;             // Desired time for one frame, if 0 not applied
//callback functions to be registered: Error, Key, MouseButton, MouseCursor
//-----------------------------------------------------------------------------------------------------
void ErrorCallback(int error, const char* description);
void KeyCallback(SDL_Window* window, SDL_Event& windowEvent, bool& quit,Model& model,Camera& camera);
void MouseButtonCallback(SDL_Window *window, int button, int action, int mods);
void MouseCursorPosCallback(SDL_Window *window, double x, double y);
void TraceLog(int msgType, const char *text, ...);      // Show trace log messages (LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_DEBUG)
bool CheckCollisionCircleRec(glm::vec2 center, float radius, const Rect& rec);
//  Window and context creation, extensions loading
//----------------------------------------------------------------------------------
void InitGraphicsDevice(int width, int height);  // Initialize graphic device
void CloseWindow(SDL_GLContext& context);                          // Close window and free resources
void SetTargetFPS(int fps);                      // Set target FPS (maximum)
	void SyncFrame(void);                            // Synchronize to desired framerate
//--------------------------------------initial map----------------------------------------------------
void LoadMap(const Map& savedmap, Model& model,Camera& camera);
void DrawMap(Model& model,const Camera& camera,const Map& savedmap);
void showWin(SDL_Window * window, SDL_Event& event);

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
	//Ask SDL to get a recent version of OpenGL (3.2 or greater)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("MazeGame", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cout << "SDL_CreateWindow() Error: " << SDL_GetError() << std::endl;
		std::cin.get();
		return -1;
	}
	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);
	InitGraphicsDevice(screenWidth, screenHeight);  // Initialize graphic device (OpenGL)

	// Define our camera
	Camera camera(glm::vec3(3.f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),45.0f);
	/*camera.position = glm::vec3(3.f,0.0f,0.0f);
	camera.target = glm::vec3(0.0f,0.0f,0.0f);
	camera.up = glm::vec3(0.0f, 0.0f, 1.0f );
	camera.fovy = 45.0f;*/

	
	Map savedmap;
	savedmap.ReadMap("level1.txt");
	// 2D projection
	// matProjection = MatrixOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0);
	// matModelview = MatrixIdentity();
	Shader textshader;
	//load model
	Model model;
	LoadMap(savedmap,model,camera);
	GLuint texturedShader = textshader.InitShader("textured-Vertex.glsl", "textured-Fragment.glsl");
	model.texturedShader = texturedShader;
	GLuint vao = 0;
	GLuint vbo[3] = { 0 };
	model.UploadMeshData(vao, vbo);
	/*------------------------------------------------------*/
	const string f = "texture/front.bmp";
	const string b = "texture/back.bmp";
	const string r = "texture/right.bmp";
	const string l = "texture/left.bmp";
	const string u = "texture/up.bmp";
	const string d = "texture/down.bmp";
	SkyBox skybox;
	if (!skybox.Init(f, b, r, f, u, d))
	{
		printf("Load skybox error!\n");
	};
	GLuint vao1 = 0;
	GLuint vbo1[1] = { 0 };
	skybox.UpdateData("models/cube.txt", vao1, vbo1);
	SetTargetFPS(60);
	//--------------------------------------------------------------------------------------    
	/*glm::vec3 floorPosition(float(savedmap.MapSize[0]) / 2.0 + 1, float(savedmap.MapSize[1]) / 2.0 + 1, -0.1f);
	glm::vec3 floorScale(float(savedmap.MapSize[0]), float(savedmap.MapSize[1]), 0.1f);
	camera.position = glm::vec3(float(savedmap.MapSize[0]) / 2.0 + 1, float(savedmap.MapSize[1]) / 2.0 + 1, 0.2f);
	model.obj[1]->UploadPosition(glm::vec3(0.f,0.f,-3.1f));*/
	// Main game loop   
	SDL_Event windowEvent;
	SDL_Event gameExit;
	bool quit = false;
	int PersonBind = -1;
	while (!quit)
	{
		// Update
		glm::vec3 oldCamPos = camera.position;
		//----------------------------------------------------------------------------------
		KeyCallback(window, windowEvent, quit, model,camera);                   // Register input events (keyboard, mouse)
		// Clear the screen to default color
		glClearColor(.2f, 0.4f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(texturedShader);
		//map control
		glm::vec2 playerPos(camera.position.x/ SKYBOX_WIDTH, camera.position.y/ SKYBOX_HEIGHT);
		float playerRadius = 0.1f;  // Collision radius (player is modelled as a cilinder for collision)
		int playerCellX = (int)(playerPos.x);
		int playerCellY = (int)(playerPos.y);//+ 0.5f

		// Out-of-limits security check
		if (playerCellX < 0 || playerCellX >= savedmap.MapSize[0]|| playerCellY < 0|| playerCellY >= savedmap.MapSize[1])
			camera.position = oldCamPos;
		/*if (playerCellX < 0) playerCellX = 0;*/
		//else if (playerCellX >= savedmap.MapSize[0]) playerCellX = savedmap.MapSize[0] - 1;//
		//if (playerCellY < 0) playerCellY = 0;
		//else if (playerCellY >= savedmap.MapSize[1]) playerCellY = savedmap.MapSize[1] - 1;//
		// Check map collisions using image data and player position
		for (int y = 0; y < savedmap.MapSize[1]; y++)
		{
			for (int x = 0; x < savedmap.MapSize[0]; x++)
			{
				if ((savedmap.SavedMap[x][y]=='W'|| (savedmap.SavedMap[x][y] >='A'&& savedmap.SavedMap[x][y]<='E'))&&          // Collider (white pixel)
					(CheckCollisionCircleRec(playerPos, playerRadius,
					Rect(x, y, 1.0f, 1.0f))))//int(0.5f + x * 1.0f) int(0.5f + y * 1.0f)
				{
					if (camera.key - 32 != savedmap.SavedMap[x][y])//no key
						// Collision detected, reset camera position
						camera.position = oldCamPos;
					else//open door
					{
						savedmap.SavedMap[x][y] = '0';//clear door

						model.obj[PersonBind]->display = false;//not show that key
						model.obj[savedmap.mapObj[x][y]]->display = false; //not show that door
						PersonBind = -1;
					}
					break;
				}
			}
		}
		//hold the key
		if (PersonBind == -1)
		{
			glm::vec2 bias = glm::normalize(glm::vec2(camera.viewDirection.x, camera.viewDirection.y));
			glm::vec2 access = playerPos + bias * 0.25f;
			int touchX = (int)(access.x);
			int touchY = (int)(access.y);
			if (savedmap.SavedMap[touchX][touchY] >= 'a' && savedmap.SavedMap[touchX][touchY] <= 'e')
			{
				int objNum = savedmap.mapObj[touchX][touchY];
				if (!model.obj[objNum]->Isbind) 
				{
					PersonBind = objNum;
					model.obj[PersonBind]->Isbind = true;//bind the key to camera
					camera.key = savedmap.SavedMap[touchX][touchY];
					savedmap.SavedMap[touchX][touchY] = '0';//pick up the key
				}
			}
		}
		else
		{
			glm::vec2 bias = glm::normalize(glm::vec2(camera.viewDirection.x, camera.viewDirection.y));
			glm::vec2 access = playerPos + bias * 0.5f;
			model.obj[PersonBind]->objx = access.x * 2.0f;
			model.obj[PersonBind]->objy = access.y* 2.0f;
		}
		/*printf("playerCellX :%d ,playerCellY :%d\n", playerCellX, playerCellY);
		printf("playerPos.x :%f ,playerPos.y :%f\n", playerPos.x, playerPos.y);*/

		// Calculate projection matrix (from perspective) and view matrix from camera look at
		matProjection = glm::perspective(camera.fovy*PI / 180.0f, (double)screenWidth / (double)screenHeight, 0.1, 100.0);
		matModelview = glm::lookAt(camera.position, camera.target, camera.up);
		model.LoadModel(matModelview, matProjection);
		//----------------------------------------------------------------------------------
		// Draw
		DrawMap(model, camera, savedmap);
		//skybox.Render(camera, matProjection);
		//glDeleteProgram(texturedShader);
		if (savedmap.SavedMap[(int)playerPos.x][(int)playerPos.y] == 'G')
		{
			showWin(window, gameExit);
			quit = true;
		}
		//----------------------------------------------------------------------------------
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Clear used buffers: Color and Depth (Depth is used for 3D)
		// TODO: Draw on the screen
		SDL_GL_SwapWindow(window);          //Double buffering
		SyncFrame();                        // Wait required time to target framerate
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//Clean Up
	glDeleteProgram(texturedShader);
	glDeleteBuffers(1, vbo);
	glDeleteVertexArrays(1, &vao);
	CloseWindow(context);
	//--------------------------------------------------------------------------------------   
	
	//--------------------------------------------------------------------------------------
	return 0;
}

void LoadMap(const Map& savedmap, Model& model, Camera& camera)
{
	int text[7];//id
	GLuint shaderUnit[7]; //which textshader
	int modelNum = savedmap.MapSize[0] * savedmap.MapSize[1];
	float keyoffset= 0.25;//SKYBOX_HEIGHT
	float walloffset= 0.5;//SKYBOX_HEIGHT
	float gate0ffset= 0.36;//SKYBOX_HEIGHT
	float exitoffset = 0.5;
	for (int i = 0; i < 7; ++i)
	{
		text[i] = i;
		shaderUnit[i] = i;
	}
	//Texture2D* texture1 = new Texture2D(tex0, shaderUnit0);
	Texture2D texture1(text[0], shaderUnit[0]);
	texture1.LoadTexture("texture/wood.bmp");
	//Texture2D* texture2 = new Texture2D(tex0, shaderUnit1);
	Texture2D texture2(text[1], shaderUnit[1]);
	texture2.LoadTexture("texture/brick.bmp");
	Texture2D texture3(text[2], shaderUnit[2]);
	texture3.LoadTexture("texture/leaf.bmp");
	Texture2D texture4(text[3], shaderUnit[3]);
	texture4.LoadTexture("texture/drop.bmp");
	Texture2D texture5(text[4], shaderUnit[4]);
	texture5.LoadTexture("texture/1.bmp");
	Texture2D texture6(text[5], shaderUnit[5]);
	texture6.LoadTexture("texture/2.bmp");
	Texture2D texture7(text[6], shaderUnit[6]);
	texture7.LoadTexture("texture/3.bmp");
	model.InitModel(2*modelNum);
	int k = 0;
	//floor
	for (int i = 0; i < savedmap.MapSize[0]; ++i)
	{
		for (int j = 0; j < savedmap.MapSize[1]; ++j)
		{
			model.ImportModel("models/cube.txt", new Object(texture2, FLOOR));
			//vector<Object *>::iterator it = model.obj.end() - 1;
			//(*it)->UploadPosition(glm::vec3((float(i)+0.5), (float(j)+0.5), 0.0f));
			model.obj[model.obj.size()-1]->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), 0.0f));
			savedmap.mapObj[i][j] = k++;
		}
	}
	
	for (int i = 0; i < savedmap.MapSize[0]; ++i)
	{
		for (int j = 0; j < savedmap.MapSize[1]; ++j)
		{
			const char type = savedmap.SavedMap[i][j];
			vector<Object *>::iterator it = model.obj.end() - 1;
			switch (type)
			{
			case 'G':
				/*model.obj[i+j+1] = new Object(texture4);
				model.ImportModel("models/sphere.txt", i + j + 1);*/
				model.ImportModel("models/sphere.txt", new Object(texture4,EXIT));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), exitoffset));
				break;
			case 'W':
				model.ImportModel("models/cube.txt", new Object(texture2,WALL));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5),walloffset ));
				break;
			case 'A':
				model.ImportModel("models/knot.txt", new Object(texture1,DOOR));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), gate0ffset));
				break;
			case'a':
				model.ImportModel("models/teapot.txt", new Object(texture1,KEY));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), keyoffset));
				break;
			case 'B':
				model.ImportModel("models/knot.txt", new Object(texture3, DOOR));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), gate0ffset));
				break;
			case 'b':
				model.ImportModel("models/teapot.txt", new Object(texture3, KEY));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), keyoffset));
				break;
			case 'C':
				model.ImportModel("models/knot.txt", new Object(texture5, DOOR));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), gate0ffset));
				break;
			case'c':
				model.ImportModel("models/teapot.txt", new Object(texture5, KEY));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), keyoffset));
				break;
			case 'D':
				model.ImportModel("models/knot.txt", new Object(texture6, DOOR));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), gate0ffset));
				break;
			case 'd':
				model.ImportModel("models/teapot.txt", new Object(texture6, KEY));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), keyoffset));
				break;
			case 'E':
				model.ImportModel("models/knot.txt", new Object(texture7, DOOR));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), gate0ffset));
				break;
			case 'e':
				model.ImportModel("models/teapot.txt", new Object(texture7, KEY));
				savedmap.mapObj[i][j] = k++;
				(*(++it))->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), keyoffset));
				break;
			case 'S':
				camera.position = glm::vec3((float(i) + 0.5)*SKYBOX_WIDTH,(float(j) + 0.5)*SKYBOX_HEIGHT,2.0f );
				//(*it++)->UploadPosition(glm::vec3((float(i) + 0.5), (float(j) + 0.5), 0.1f));
				break;
			default:
				break;
			}
			
		}
		
	}
}
void DrawMap(Model& model, const Camera& camera , const Map& savedmap)
{
	//glm::vec3 floorScale(float(savedmap.MapSize[1])*SKYBOX_WIDTH, float(savedmap.MapSize[0])*SKYBOX_HEIGHT, 0.1f);
	glm::vec3 floorScale(SKYBOX_WIDTH, SKYBOX_HEIGHT, 0.1f);
	glm::vec3 scale(SKYBOX_WIDTH, SKYBOX_HEIGHT,SKYBOX_LENGTH);//
	//floor
	for (int i = 0; i < model.MaxmodelNum/2; ++i)
	{
		model.DrawModel(i, camera, floorScale);
		//model.DrawModel(model.MaxmodelNum / 2-1, camera, floorScale);
	}
	for (int i = model.MaxmodelNum / 2; i < model.obj.size(); ++i)// 
	{
		if (model.obj[i]->Isbind)
			model.DrawModel(i, camera, scale / 2.0f);
		else
			model.DrawModel(i, camera, scale);
		//model.DrawModel(model.obj.size()-1, camera, scale);
	}
}
// GLFW3: Error callback function
void ErrorCallback(int error, const char* description)
{
	TraceLog(LOG_ERROR, description);
}

// GLFW3: Keyboard callback function
void KeyCallback(SDL_Window* window, SDL_Event& windowEvent, bool& quit, Model& model, Camera& camera)
{
	bool fullscreen = false;
	glm::vec4 m_direction(camera.viewDirection.x, camera.viewDirection.y, camera.viewDirection.z, 1.0f);
	glm::vec4 m_up(camera.up.x, camera.up.y, camera.up.z, 1.0f);
	glm::mat4 rot;
	glm::vec3 pitchAxis = glm::normalize(glm::cross(camera.viewDirection,camera.up));
	float m_speed = 0.02*SKYBOX_WIDTH;
	while (SDL_PollEvent(&windowEvent)) 
	{  //inspect all events in the queue
		switch (windowEvent.type)
		{
		case SDL_QUIT:
			quit = true; break;
		case SDL_KEYUP:
		//List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
		//Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
			if (windowEvent.key.keysym.sym == SDLK_ESCAPE)
				quit = true; //Exit event loop
			else if (windowEvent.key.keysym.sym == SDLK_f) { //If "f" is pressed
				fullscreen = !fullscreen;
				SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen 
			}
			break;
		case SDL_KEYDOWN:
			switch (windowEvent.key.keysym.sym)
			{
			case  SDLK_UP:
				if (windowEvent.key.keysym.mod & KMOD_SHIFT)
					camera.position = camera.position+ camera.up*m_speed;
					//model.obj[0]->objx -= .1; //Is shift pressed?
				else 
					camera.position = camera.position + camera.viewDirection*m_speed;
					//model.obj[0]->objz += .1;
				break;
			case SDLK_DOWN:
				if (windowEvent.key.keysym.mod & KMOD_SHIFT)
					camera.position = camera.position - camera.up*m_speed;
					//model.obj[0]->objx += .1; //Is shift pressed?
				else 
					camera.position = camera.position - camera.viewDirection*m_speed;
					//model.obj[0]->objz -= .1;
				break;
			case SDLK_LEFT:
				//if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT)  //If "up key" is pressed
				camera.position = camera.position- pitchAxis * m_speed;
				//model.obj[0]->objy -= .1;
				break;
			case SDLK_RIGHT:
				//if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT)  //If "down key" is pressed
				camera.position = camera.position + pitchAxis * m_speed;;
				//model.obj[0]->objy += .1;
				break;
			case SDLK_a:
				rot = glm::rotate(rot, 1.f * m_speed, glm::normalize(camera.up));//glm::vec3(0.0f,0.0f,1.0f)
				m_direction = rot * m_direction;
				camera.viewDirection = glm::vec3(m_direction.x, m_direction.y, m_direction.z);
				camera.target = camera.position + camera.viewDirection;
					break;
			case SDLK_d:
				rot = glm::rotate(rot, -1.f * m_speed, glm::normalize(camera.up));//glm::vec3(0.0f, 0.0f, 1.0f)
				m_direction = rot * m_direction;
				camera.viewDirection = glm::vec3(m_direction.x, m_direction.y, m_direction.z);
				camera.target = camera.position + camera.viewDirection;
				break;
			case SDLK_w:
				rot = glm::rotate(rot, 1.f * m_speed, glm::normalize(pitchAxis));
				m_direction = rot * m_direction;
				m_up = rot * m_up;
				camera.viewDirection = glm::normalize(glm::vec3(m_direction.x, m_direction.y, m_direction.z));
				camera.up= glm::normalize(glm::vec3(m_up.x, m_up.y, m_up.z));
				camera.target = camera.position + camera.viewDirection;
				break;
			case SDLK_s:
				rot = glm::rotate(rot, -1.f * m_speed, glm::normalize(pitchAxis));
				m_direction = rot * m_direction;
				m_up = rot * m_up;
				camera.viewDirection = glm::normalize(glm::vec3(m_direction.x, m_direction.y, m_direction.z));
				camera.up = glm::normalize(glm::vec3(m_up.x, m_up.y, m_up.z));
				camera.target = camera.position + camera.viewDirection;
				break;
			default:
				break;
			}
			//if (windowEvent.key.keysym.sym == SDLK_UP)
			//{
			//	if (windowEvent.key.keysym.mod & KMOD_SHIFT) model.obj[0]->objx -= .1; //Is shift pressed?
			//	else model.obj[0]->objz += .1;
			//}
			//else if (windowEvent.key.keysym.sym == SDLK_DOWN)
			//{
			//	if (windowEvent.key.keysym.mod & KMOD_SHIFT) model.obj[0]->objx += .1; //Is shift pressed?
			//	else model.obj[0]->objz -= .1;
			//}
			//else if (windowEvent.key.keysym.sym == SDLK_LEFT)
			//{
			//	if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT)  //If "up key" is pressed
			//		model.obj[0]->objy -= .1;
			//}
			//else if (windowEvent.key.keysym.sym == SDLK_RIGHT)
			//{
			//	if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT)  //If "down key" is pressed
			//		model.obj[0]->objy += .1;
			//}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (SDL_BUTTON_LEFT == windowEvent.button.button)
			{
				

			}
			else if (SDL_BUTTON_RIGHT == windowEvent.button.button)
			{
				
			}
			break;
		default:break;
		}
		//if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_c) { //If "c" is pressed
		//	colR = rand01();
		//	colG = rand01();
		//	colB = rand01();
		//}
	}
}
// Show trace log messages (LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_DEBUG)
void TraceLog(int msgType, const char *text, ...)
{
	va_list args;
	va_start(args, text);

	switch (msgType)
	{
	case LOG_INFO: fprintf(stdout, "INFO: "); break;
	case LOG_ERROR: fprintf(stdout, "ERROR: "); break;
	case LOG_WARNING: fprintf(stdout, "WARNING: "); break;
	case LOG_DEBUG: fprintf(stdout, "DEBUG: "); break;
	default: break;
	}

	vfprintf(stdout, text, args);
	fprintf(stdout, "\n");

	va_end(args);

	if (msgType == LOG_ERROR) exit(1);
}

//----------------------------------------------------------------------------------
// Initialize graphic device (OpenGL 3.3)
void InitGraphicsDevice(int width, int height)
{
	//Load OpenGL extentions with GLAD
	if (gladLoadGLLoader(SDL_GL_GetProcAddress)) 
	{
		// Print current OpenGL and GLSL version
		TraceLog(LOG_INFO, "GPU: Vendor:   %s\n", glGetString(GL_VENDOR));
		TraceLog(LOG_INFO, "GPU: Renderer: %s\n", glGetString(GL_RENDERER));
		TraceLog(LOG_INFO, "GPU: Version:  %s\n", glGetString(GL_VERSION));
		TraceLog(LOG_INFO, "GPU: GLSL:     %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
/*
		printf("\nOpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n\n", glGetString(GL_VERSION));*/
	}
	else
	{
		printf("ERROR: Failed to initialize OpenGL context.\n");
		TraceLog(LOG_WARNING, "GLAD: Cannot load OpenGL extensions");
	}
	//// Initialize OpenGL context (states and resources)
	////----------------------------------------------------------
	//// Init state: Depth test
	//glDepthFunc(GL_LEQUAL);                                 // Type of depth testing to apply
	glEnable(GL_DEPTH_TEST);                                // Enable depth testing for 3D
	//// Init state: Blending mode

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Color blending function (how colors are mixed)
	//glEnable(GL_BLEND);                                     // Enable color blending (required to work with transparencies)

	//// Init state: Culling
	//// NOTE: All shapes/models triangles are drawn CCW
	//glCullFace(GL_BACK);                                    // Cull the back face (default)
	//glFrontFace(GL_CCW);                                    // Front face are defined counter clockwise (default)
	//glEnable(GL_CULL_FACE);                                 // Enable backface culling

	//// Init state: Color/Depth buffers clear
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear color (black)
	//glClearDepth(1.0f);                                     // Set clear depth value (default)
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear color and depth buffers (depth buffer required for 3D)

	TraceLog(LOG_INFO, "OpenGL default states initialized successfully");

	//// Initialize viewport
	//glViewport(0, 0, width, height);

	// Init internal matProjection and matModelview matrices
	matProjection = glm::mat4(1.0f);//MatrixIdentity()
	matModelview = glm::mat4(1.0f);
}

// Close window and free resources
void CloseWindow(SDL_GLContext& context)
{
	SDL_GL_DeleteContext(context);
	SDL_Quit();
	//glfwDestroyWindow(window);      // Close window
	//glfwTerminate();                // Free GLFW3 resources
}

// Set target FPS (maximum)
void SetTargetFPS(int fps)
{
	if (fps < 1) targetTime = 0.0;
	else targetTime = 1.0 / (double)fps;
}

// Synchronize to desired framerate
void SyncFrame(void)
{
	// Frame time control system
	currentTime = SDL_GetTicks() / 1000.f;
	frameTime = currentTime - previousTime;
	previousTime = currentTime;
	// Wait for some milliseconds...
	if (frameTime < targetTime)
	{
		double prevTime = SDL_GetTicks() / 1000.f;
		double nextTime = 0.0;

		// Busy wait loop
		while ((nextTime - prevTime) < (targetTime - frameTime)) nextTime = SDL_GetTicks() / 1000.f;

		currentTime = SDL_GetTicks() / 1000.f;
		double extraTime = currentTime - previousTime;
		previousTime = currentTime;

		frameTime += extraTime;
	}
}
// Check collision between circle and rectangle. true means collision
bool CheckCollisionCircleRec(glm::vec2 center, float radius, const Rect& rec)
{
	float recCenterX = rec.x + rec.width / 2;
	float recCenterY = rec.y + rec.height / 2;

	float dx = fabsf(center.x - recCenterX);
	float dy = fabsf(center.y - recCenterY);
	float cornerDistanceSq = (dx - (float)rec.width / 2.0f)*(dx - (float)rec.width / 2.0f) +
		(dy - (float)rec.height / 2.0f)*(dy - (float)rec.height / 2.0f);
	if (dx > ((float)rec.width / 2.0f + radius)) 
	{ 
		return false; 
	}
	if (dy > ((float)rec.height / 2.0f + radius)) 
	{ 
		return false; 
	}
	return true;
	/*if (dx <= ((float)rec.width / 2.0f)) { return true; }
	if (dy <= ((float)rec.height / 2.0f)) { return true; }*/

	//return (cornerDistanceSq <= (radius*radius));
}

void showWin(SDL_Window * window, SDL_Event& event)
{
	bool quit = false;
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Surface * image = SDL_LoadBMP("texture/win.bmp");
	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	while (!quit)
	{
		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_q)
				quit = true; //press q to exit event loop
			break;
		}
	}
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(image);
	SDL_DestroyRenderer(renderer);
}