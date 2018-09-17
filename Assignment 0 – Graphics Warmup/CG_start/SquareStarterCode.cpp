//HW 0 - Moving Square
//Starter code for the first homework assignment.
//This code assumes SDL2 and OpenGL are both properly installed on your system

#include "include/glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//Name of image texture
string textureName = "brick.ppm";//goldy.ppm

//Globals to store the state of the square (position, width, and angle)
float g_pos_x = 0.0f;
float g_pos_y = 0.0f;
float g_size = 0.6f;
float origin_g_size = g_size;
float g_angle = 0;

float vertices[] = {  //These values should be updated to match the square's state when it changes
//  X     Y     R     G     B     U    V
  0.3f,  0.3f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
  0.3f, -0.3f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
  -0.3f,  0.3f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left 
  -0.3f, -0.3f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom left
}; 

int screen_width = 600;
int screen_height = 600;

float g_mouse_down = false;
bool g_up_down = false;
bool g_down_down = false;
float g_clicked_x = -1;
float g_clicked_y = -1;
float g_lastCenter_x = 0;
float g_lastCenter_y = 0;
float g_clicked_angle = -1;
float g_clicked_size = -1;

void mouseClicked(float mx, float my);
void mouseDragged(float mx, float my);

bool g_bTranslate = false;
bool g_bRotate = false;
bool g_bScale = false;

//////////////////////////
///  Begin your code here
/////////////////////////

//TODO: Read from ASCII (P6) PPM files
//Inputs are output variables for returning the image width and heigth
unsigned char* loadImage(int& img_w, int& img_h){

   //Open the texture image file
   ifstream ppmFile;
   ppmFile.open(textureName.c_str());
   if (!ppmFile){
      printf("ERROR: Texture file '%s' not found.\n",textureName.c_str());
      exit(1);
   }

   //Check that this is an ASCII PPM (first line is P3)
   string PPM_style;
   ppmFile >> PPM_style; //Read the first line of the header    
   if (PPM_style != "P3") {
      printf("ERROR: PPM Type number is %s. Not an ASCII (P3) PPM file!\n",PPM_style.c_str());
      exit(1);
   }

   //Read in the texture width and height
   ppmFile >> img_w >> img_h;
   unsigned char* img_data = new unsigned char[4*img_w*img_h];

   //Check that the 3rd line is 255 (ie., this is an 8 bit/pixel PPM)
   int maximum;
   ppmFile >> maximum;
   if (maximum != 255) {
      printf("ERROR: Maximum size is (%d) not 255.\n",maximum);
      exit(1);
   }

   //TODO:
   int red, green, blue;
   int * r = new int[img_w*img_h];
   int * g = new int[img_w*img_h];
   int * b = new int[img_w*img_h];
   int texture_load_num = 0;
   while(ppmFile >> red >> green >> blue){
       //store the rgb pixel data from the file into an array
	   r[texture_load_num] = red;
	   g[texture_load_num] = green;
	   b[texture_load_num] = blue;
	   texture_load_num++;
   }

   //TODO: This loop puts in fake data, replace with the actual pixels read from the file
   for (int i = 0; i < img_h; i++){
      float fi = i/(float)img_h;
      for (int j = 0; j < img_w; j++){
         float fj = j/(float)img_w;
         img_data[i*img_w*4 + j*4] = r[j + (img_w -1- i)*img_w];  //Red (img_h - 1 - j)
         img_data[i*img_w*4 + j*4 + 1] = g[j + (img_w - 1 - i)* img_w];  //Green (img_h - 1 - j)
         img_data[i*img_w*4 + j*4 + 2] = b[j + (img_w - 1 - i)* img_w];  //Blue (img_h - 1 - j)
         img_data[i*img_w*4 + j*4 + 3] = 255;  //Alpha
      }
   }

   return img_data;
}

//TODO: Account for rotation by g_angle
void updateVertices(){ 
   float vx = g_size;
   float vy =  g_size;
   vertices[0] = g_pos_x + vx;  //Top right x
   vertices[1] = g_pos_y + vy;  //Top right y
   
   vx = g_size;
   vy = - g_size;
   vertices[7] = g_pos_x + vx;  //Bottom right x
   vertices[8] = g_pos_y + vy;  //Bottom right y
   
   vx = - g_size;
   vy = + g_size;
   vertices[14] =  g_pos_x + vx;  //Top left x
   vertices[15] =  g_pos_y + vy;  //Top left y
   
   vx = - g_size;
   vy = - g_size;
   vertices[21] =  g_pos_x + vx;  //Bottom left x
   vertices[22] =  g_pos_y + vy;  //Bottom left y
}

//TODO: Choose between translate, rotate, and scale based on where the user clicked
// I've implemented the logic for translate and scale, but you need to add rotate
void mouseClicked(float m_x, float m_y){   
   printf("Clicked at %f, %f\n",m_x,m_y);
   g_clicked_x = m_x;
   g_clicked_y = m_y;
   g_lastCenter_x = g_pos_x;
   g_lastCenter_y = g_pos_y;
   g_clicked_angle = g_angle;
   g_clicked_size = g_size;
   
   g_bTranslate = false;
   g_bRotate = false;
   g_bScale = false;
   
   // x and y is the click position normalized to size of the square, with (-1,-1) at one corner (1,1) the other
   float x = m_x - g_pos_x;  
   float y = m_y - g_pos_y;
   x = x / g_size;
   y = y / g_size;
   
   printf("Normalized click coord: %f, %f\n",x,y);
   
   if (x > 1.05 || y > 1.05 || x < -1.05 || y < -1.05) return; //TODO: Test your understanding: Why 1.05 and not 1?
   if (x < .9 && x > -.9 && y < .9 && y > -.9){ //TODO: Test your understanding: What happens if you change .9 to .8?
      g_bTranslate = true;
   }
   else if (abs(x) > .9 && abs(y) > .9)
   {
	   g_bRotate = true;
   }
   else
   {
      g_bScale = true; //clicked on the edges
	  origin_g_size = g_size;
   }
}

//TODO: Update the position, rotation, or scale based on the mouse movement
//  I've implemented the logic for position, you need to do scaling and angle
//TODO: Notice how smooth draging the square is (e.g., there are no "jumps" when you click), 
//      try to make your implementation of rotate and scale as smooth
float LengthSide(float x1, float y1, float x2, float y2)
{
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}
void Rotate(float Theta)
{
	double xr = g_lastCenter_x;
	double yr = g_lastCenter_y;
	double xrr = g_lastCenter_x;
	double yrr = g_lastCenter_y;
	int x, y, px, py;
	double tx, ty, p1, p2, p3, p4, p1_2, p3_4;
	double ConstX = -xrr * cos(Theta) - yrr * sin(Theta) + xr;
	double ConstY = -yrr * cos(Theta) + xrr * sin(Theta) + yr;  
	vertices[0] = vertices[0] * cos(Theta) + vertices[1] * sin(Theta) + ConstX;
	vertices[1] = -vertices[0] * sin(Theta) + vertices[1] * cos(Theta) + ConstY;

	vertices[7] = vertices[7] * cos(Theta) + vertices[8] * sin(Theta) + ConstX;
	vertices[8] = -vertices[7] * sin(Theta) + vertices[8] * cos(Theta) + ConstY;

	vertices[14] = vertices[14] * cos(Theta) + vertices[15] * sin(Theta) + ConstX;
	vertices[15] = -vertices[14] * sin(Theta) + vertices[15] * cos(Theta) + ConstY;

	vertices[21] = vertices[21] * cos(Theta) + vertices[22] * sin(Theta) + ConstX;
	vertices[22] = -vertices[21] * sin(Theta) + vertices[22] * cos(Theta) + ConstY;


}
void mouseDragged(float m_x, float m_y)
{   
	float angle;
   if (g_bTranslate){
	   // so the g_pos_x can be more than 1
      g_pos_x = m_x-g_clicked_x+g_lastCenter_x;
      g_pos_y = m_y-g_clicked_y+g_lastCenter_y;
   }
   
   if (g_bScale){
	   
      //Compute the new size, g_size, based on the mouse positions
	   //printf("the g_lastCenter_x and  g_pos_x are : %f %f\n", g_lastCenter_x, g_pos_x);
	   //printf("the g_clicked_x is : %f\n", g_clicked_x);
	   float multx = m_x - g_clicked_x;
	   float multy = m_y - g_clicked_y;   
	   float dis1 = abs(g_clicked_x - g_pos_x) +abs(g_clicked_y - g_pos_y);
	   float dis2 = abs(m_x - g_pos_x) + abs(m_y - g_pos_y);
	   float add = (abs(multx) > abs(multy)) ? abs(multy) : abs(multx);
	   if (dis2 > dis1)
	   {
		   g_size = origin_g_size + add ;
		   printf("the add is : %f\n", add);
		   printf("the g_size is : %f\n", g_size);
	   }	  
	   else
	   {
		   g_size = origin_g_size - add;
		   printf("the add is : %f\n", -add);
		   printf("the g_size is : %f\n", g_size);
	   } 
	   if (g_size <= 0)
		   g_size = 0.01;
	   else if(abs(g_size) > 1)
		   g_size = 1;
   }
   
   if (g_bRotate){
       //Compute the new angle, g_angle, based on the mouse positions
	   float side_a, side_b, side_c;
	   side_a = LengthSide(m_x, m_y, g_lastCenter_x, g_lastCenter_y);
	   side_b = LengthSide(g_clicked_x, g_clicked_y, g_lastCenter_x, g_lastCenter_y);
	   side_c = LengthSide(m_x, m_y, g_clicked_x, g_clicked_y);
	   angle = acos((side_a*side_a + side_b * side_b - side_c * side_c) / (2.0*side_a*side_b));
	   /*if (g_clicked_x > g_lastCenter_x && m_y < g_clicked_y)
		   angle = -angle;
	   else if(g_clicked_x < g_lastCenter_x && m_y > g_clicked_y)
		   angle = -angle;*/
	   if((g_clicked_x - g_lastCenter_x)*(m_y - g_clicked_y) > 0)
		   angle = -angle;
   }
   
   updateVertices();
   if(g_bRotate)
	   Rotate(angle);
}
void Brighten(unsigned char* img_data, int& img_w, int& img_h)
{
	for (int i = 0; i < img_h; i++) {
		for (int j = 0; j < img_w; j++) {
			float step = 1.1;
			int shreshold = 255 / step;
			if (img_data[i*img_w * 4 + j * 4] <= shreshold && img_data[i*img_w * 4 + j * 4 +1] <= shreshold
				&& img_data[i*img_w * 4 + j * 4 + 2] <= shreshold)
			{
				img_data[i*img_w * 4 + j * 4] = int(img_data[i*img_w * 4 + j * 4] * step);  //Red
				img_data[i*img_w * 4 + j * 4 + 1] = int(img_data[i*img_w * 4 + j * 4 + 1] * step);  //Green
				img_data[i*img_w * 4 + j * 4 + 2] = int(img_data[i*img_w * 4 + j * 4 + 2] * step);  //Blue
			}
			
		}
	}
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Darken(unsigned char* img_data, int& img_w, int& img_h)
{
	for (int i = 0; i < img_h; i++) {
		for (int j = 0; j < img_w; j++) {
			float step = 1.1;
			int shreshold = step;
			if (img_data[i*img_w * 4 + j * 4] > shreshold && img_data[i*img_w * 4 + j * 4 + 1] > shreshold &&
				img_data[i*img_w * 4 + j * 4 + 2] > shreshold)
			{
				img_data[i*img_w * 4 + j * 4] = int(img_data[i*img_w * 4 + j * 4] / step);  //Red
				img_data[i*img_w * 4 + j * 4 + 1] = int(img_data[i*img_w * 4 + j * 4 + 1] / step);  //Green
				img_data[i*img_w * 4 + j * 4 + 2] = int(img_data[i*img_w * 4 + j * 4 + 2] / step);  //Blue
			}
		}
	}
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
	glGenerateMipmap(GL_TEXTURE_2D);
}

/////////////////////////////
/// ... below is OpenGL specifc code,
///     we will cover it in detail around Week 9,
///     but you should try to poke around a bit righ now.
///     I've annotated some parts with "TODO: Test ..." check those out.
////////////////////////////

// Shader sources
const GLchar* vertexSource =
   "#version 150 core\n"
   "in vec2 position;"
   "in vec3 inColor;"
   "in vec2 inTexcoord;"
   "out vec3 Color;"
   "out vec2 texcoord;"
   "void main() {"
   "   Color = inColor;"
   "   gl_Position = vec4(position, 0.0, 1.0);"
   "   texcoord = inTexcoord;"
   "}";
    
const GLchar* fragmentSource =
   "#version 150 core\n"
   "uniform sampler2D tex0;"
   "in vec2 texcoord;"
   "out vec3 outColor;"
   "void main() {"
   "   outColor = texture(tex0, texcoord).rgb;"
   "}";
    
bool fullscreen = false;

int main(int argc, char *argv[]){
   SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)
   
   //Ask SDL to get a fairly recent version of OpenGL (3.2 or greater)
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	
	//Create a window (offsetx, offsety, width, height, flags)
	SDL_Window* window = SDL_CreateWindow("Graphics	Warmup", 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
   //TODO: Test your understanding: Try changing the title of the window to something more personalized.
	
	//The above window cannot be resized which makes some code slightly easier.
	//Below show how to make a full screen window or allow resizing
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 0, 0, screen_width, screen_height, SDL_WINDOW_FULLSCREEN|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screen_width, screen_height, SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	//SDL_Window* window = SDL_CreateWindow("My OpenGL Program",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,0,0,SDL_WINDOW_FULLSCREEN_DESKTOP|SDL_WINDOW_OPENGL); //Boarderless window "fake" full screen

   float aspect = screen_width/(float)screen_height; //aspect ratio (needs to be updated if the window is resized)
	
	updateVertices(); //set initial position of the square to match it's state
	
	//Create a context to draw in
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	//OpenGL functions using glad library
   if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
      printf("OpenGL loaded\n");
      printf("Vendor:   %s\n", glGetString(GL_VENDOR));
      printf("Renderer: %s\n", glGetString(GL_RENDERER));
      printf("Version:  %s\n", glGetString(GL_VERSION));
   }
   else {
      printf("ERROR: Failed to initialize OpenGL context.\n");
      return -1;
   }

   //// Allocate Texture 0 (Created in Load Image) ///////
   GLuint tex0;
   glGenTextures(1, &tex0);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, tex0);

   //What to do outside 0-1 range
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_LINEAR
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //GL_LINEAR
   //TODO: Test your understanding: Try GL_LINEAR instead of GL_NEAREST on the 4x4 test image. What is happening?


   int img_w, img_h;
   unsigned char* img_data = loadImage(img_w,img_h);
   printf("Loaded Image of size (%d,%d)\n",img_w,img_h);
   //memset(img_data,0,4*img_w*img_h); //Load all zeros
   //Load the texture into memory
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
   glGenerateMipmap(GL_TEXTURE_2D);
   //// End Allocate Texture ///////


   //Build a Vertex Array Object. This stores the VBO and attribute mappings in one object
   GLuint vao;
   glGenVertexArrays(1, &vao); //Create a VAO
   glBindVertexArray(vao); //Bind the above created VAO to the current context


   //Allocate memory on the graphics card to store geometry (vertex buffer object)
   GLuint vbo;
   glGenBuffers(1, &vbo);  //Create 1 buffer called vbo
   glBindBuffer(GL_ARRAY_BUFFER, vbo); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo
   //GL_STATIC_DRAW means we won't change the geometry, GL_DYNAMIC_DRAW = geometry changes infrequently
   //GL_STREAM_DRAW = geom. changes frequently.  This effects which types of GPU memory is used


   //Load the vertex Shader
   GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); 
   glShaderSource(vertexShader, 1, &vertexSource, NULL);
   glCompileShader(vertexShader);

   //Let's double check the shader compiled 
   GLint status;
   glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
   if (!status){
      char buffer[512];
      glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
      printf("Vertex Shader Compile Failed. Info:\n\n%s\n",buffer);
   }

   GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
   glCompileShader(fragmentShader);

   //Double check the shader compiled 
   glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
   if (!status){
      char buffer[512];
      glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
      printf("Fragment Shader Compile Failed. Info:\n\n%s\n",buffer);
   }

   //Join the vertex and fragment shaders together into one program
   GLuint shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glBindFragDataLocation(shaderProgram, 0, "outColor"); // set output
   glLinkProgram(shaderProgram); //run the linker

   glUseProgram(shaderProgram); //Set the active shader (only one can be used at a time)


   //Tell OpenGL how to set fragment shader input 

   GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
   //               Attribute, vals/attrib., type, normalized?, stride, offset
   glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), 0);
   glEnableVertexAttribArray(posAttrib); //Binds the VBO current GL_ARRAY_BUFFER 

   GLint colAttrib = glGetAttribLocation(shaderProgram, "inColor");
   glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(2*sizeof(float)));
   glEnableVertexAttribArray(colAttrib);

   GLint texAttrib = glGetAttribLocation(shaderProgram, "inTexcoord");
   glEnableVertexAttribArray(texAttrib);
   glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(float), (void*)(5*sizeof(float)));
      

   //Event Loop (Loop forever processing each event as fast as possible)
   SDL_Event windowEvent;
   SDL_Event event;
   bool done = false;
   while (!done){
      while (SDL_PollEvent(&windowEvent)){  //Process input events (e.g., mouse & keyboard)
         if (windowEvent.type == SDL_QUIT) done = true;
         //List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
         //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
         done = true; //Exit event loop
         if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f) //If "f" is pressed
         fullscreen = !fullscreen;
         SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Set to full screen 
      }
      
      
      int mx, my;
      if (SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT)) 	  
	  { // Mouse Button 1 (left) is pressed
		  //Is the mouse down?
		 //printf("the g_mouse_down is %f\n", g_mouse_down);
         if (g_mouse_down == false)
		 {
         mouseClicked(2*mx/(float)screen_width - 1, 1-2*my/(float)screen_height);
         } 
         else{
         mouseDragged(2*mx/(float)screen_width-1, 1-2*my/(float)screen_height);
         }
         g_mouse_down = true;
		 //printf("the button is pressed\n");
      } 
      else
	  {
         g_mouse_down = false;
		// printf("the button is unpressed\n");
      }

	  // keyboard
	  /*while (SDL_PollEvent(&event))
	  {
		  switch (event.type)
		  {
		  case SDL_KEYDOWN:
			  switch (event.key.keysym.sym)
			  {
			  case SDLK_UP: 
			  {
				  Brighten(); 
				  printf("UP has been pressed\n");
				  break; 
			  }
			  case SDLK_DOWN:Darken(); break;
			  }
			  break;
		  default:
			  break;
		  }
	  }*/
	  const Uint8 *state = SDL_GetKeyboardState(NULL);
	  //Returns a pointer to an array of key states. 
	  //A value of 1 means that the key is pressed and a value of 0 means that it is not. 
	  if (state[SDL_SCANCODE_UP])
	  {
		  if (g_up_down == false)
		  {
			  Brighten(img_data, img_w, img_h);
			  printf("UP has been pressed\n");
		  } 
		  g_up_down = true;
	  }
	  else
	  {
		  g_up_down = false;
	  }
	  if (state[SDL_SCANCODE_DOWN])
	  {
		  if (g_down_down == false)
		  {
			  Darken(img_data, img_w, img_h);
			  printf("DOWN has been pressed\n");
		  }	  
		  g_down_down = true;
	  }
	  else
	  {
		  g_down_down = false;
	  }
	
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW); //upload vertices to vbo

      
      // Clear the screen to white
      glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
               
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); //Draw the two triangles (4 vertices) making up the square
      //TODO: Test your understanding: What shape do you expect to see if you change the above 4 to 3?  Guess, then try it!

      SDL_GL_SwapWindow(window); //Double buffering
   }

   delete [] img_data;
   glDeleteProgram(shaderProgram);
   glDeleteShader(fragmentShader);
   glDeleteShader(vertexShader);

   glDeleteBuffers(1, &vbo);

   glDeleteVertexArrays(1, &vao);


   //Clean Up
   SDL_GL_DeleteContext(context);
   SDL_Quit();
   return 0;
}
