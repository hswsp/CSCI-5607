#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdlib>
#include<ctime>
#include "SDL.h"
#include "image.h"
#include"rendering.h"
#undef main


#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "stb_image_write.h"

//#define HEIGHT 600
//#define WIDTH 600

using namespace std;
//GLuint texture;
//clock_t clocks;
//int frames;
//float rad;
//static Color buffer[HEIGHT][WIDTH];
//store in file
void CreateImage(string filepath, Image *img, Camera* camera, Scene* scene, Shader* render);
//show in window
bool ShowImage(string outFile);

int main() 
{
	int Width = 640;
	int Height = 480;
	Image *img = NULL;
	PhongMaterial *material = NULL;//new PhongMaterial
	//bool isGetMaterial = false;
	Camera* camera = NULL;
	Scene* scene = new Scene;
	Pixel backgroud;
	bool did_background = false;
	Shader * render = new Shader;
	string fileName = "Image/bear.scn";//ambient_sphere  spheres2 
	string outFile = "./raytracer/raytraced.bmp";
	string line;
	// open the file containing the scene description
	ifstream input(fileName);
	// check for errors in opening the file
	if (input.fail()) {
		cout << "Can't open file '" << fileName << "'" << endl;
		return 0;
	}
	// determine the file size (this is optional -- feel free to delete the 6 lines below)
	streampos begin, end;
	begin = input.tellg();
	input.seekg(0, ios::end);
	end = input.tellg();
	cout << "File '" << fileName << "' is: " << (end - begin) << " bytes long.\n\n";
	input.seekg(0, ios::beg);
	//Loop through reading each line
	string command;

	while (input >> command) { //Read first word in the line (i.e., the command type)

		if (command[0] == '#') {
			getline(input, line); //skip rest of line
			cout << "Skipping comment: " << command << line << endl;
			continue;
		}
		if (command == "film_resolution")
		{
			input >> Width >> Height;
		}
		else if (command == "camera")
		{
			float px, py, pz, dx, dy, dz, ux, uy, uz, ha;
			input >> px >> py >> pz >> dx >> dy >> dz >> ux >> uy >> uz >> ha;
			Vector aEye(px, py, pz);
			Vector aFront(dx, dy, dz);
			Vector aUp(ux, uy, uz);
			camera = new Camera(aEye, aFront,aUp,ha);
		}
		else if (command == "sphere") 
		{ //If the command is a sphere command
			float x, y, z, r;
			input >> x >> y >> z >> r;
			Vector center(x, y, z);
			//use the current stored material
			PhongMaterial* local_material = new PhongMaterial(material);
			scene->addObject(new Sphere(center, r, local_material));//material&
			//else
			//	scene->addObject(new Sphere(center, r));
			//                                                                                                                                   delete material;
			//printf("Sphere as position (%f,%f,%f) with radius %f\n", x, y, z, r);
			//isGetMaterial = false;
		}
		else if (command == "background") 
		{ //If the command is a background command
			float r, g, b;
			input >> r >> g >> b;
			//printf("Background color of (%f,%f,%f)\n", r, g, b);
			backgroud.r = r;
			backgroud.b = b;
			backgroud.g = g;
			did_background = true;
		}
		else if (command == "output_image")
		{ //If the command is an output_image command
			
			input >> outFile;
			printf("Render to file named: %s\n", outFile.c_str());			
			
		}
		else if (command == "material")
		{
			float ar, ag, ab, dr, dg, db, sr, sg, sb, ns, tr, tg, tb, ior;
			input >> ar >> ag >> ab >> dr >> dg >> db >> sr >> sg >> sb >> ns >> tr >> tg >> tb >> ior;
			Vector ambient_color(ar, ag, ab);
			Vector diffuse(dr, dg, db);
			Vector specular(sr, sg, sb);
			Vector transmissive_color(tr,tg,tb);
			/*material->ambient = ambient_color;
			material->diffuse = diffuse;
			material->specular = specular;
			material->transmissive = transmissive_color;
			material->shininess = ns;
			material->ior = ior;*/
			//delete material;
			material = new PhongMaterial(diffuse,specular,transmissive_color,ambient_color,ns,ior);
			//isGetMaterial = true;
		}
		else if (command == "point_light")
		{
			float r, g, b,x, y, z;
			input >> r >> g >> b >> x >> y >> z;
			Vector color(r, g, b);
			color = color;//  /255.0
			Vector position(x, y, z);
			scene->addLights(new PointLight(*scene,position, color));
		}
		else if (command =="ambient_light")
		{
			float r, g, b;
			input >> r >> g >> b;
			//Vector color(r, g, b);
			/*scene->addLights(new AmbientLight(*scene,color));*/
			scene->ambient_light = Vector(r, g, b);
		}
		else if (command =="max_depth")
		{
			int maxdepth;
			input >> maxdepth;
			render->maxdepth = maxdepth;
		}
		else 
		{
			getline(input, line); //skip rest of line
			cout << "WARNING. Do not know command: " << command << endl;
		}
	}
	/*if (!did_output)
	{
		fprintf(stderr, "Warning, you didn't tell me to output anything.  I hope that's OK.\n");
	}*/
	/*else
	{*/
	if (camera == NULL)//default;
		camera = new Camera(Vector(0, 0, 0), Vector(0, 0, 1), Vector(0, 1, 0), 45);
	if(did_background)
		img = new Image(Width, Height,backgroud);
	else
	{
		img = new Image(Width, Height);
	}
	CreateImage(outFile, img, camera, scene, render);
	//}
	if (!ShowImage(outFile))
	{
		cerr << "Image error" << endl;
		system("pause>nul");
	}
	delete img;
	return 0;
}

bool ShowImage(string outFile)
{
	SDL_Window* my_window = NULL;
	SDL_Surface* my_screen_surface = NULL;
	SDL_Surface *my_image = NULL;
	bool gameRunning = true;
	SDL_Event event;
	int start = outFile.find_last_of('/', outFile.length() - 1);
	string WinName = outFile.substr(start + 1);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
		return false;
	}
	my_window = SDL_CreateWindow(
		WinName.c_str(),//"HELLO WORLD"
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_SHOWN
	);
	if (my_window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else {
		my_screen_surface = SDL_GetWindowSurface(my_window);
	}
	string filepath = outFile;
	my_image = SDL_LoadBMP(filepath.c_str());
	if (my_image == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", outFile, SDL_GetError());
		return false;
	}
	SDL_BlitSurface(my_image, NULL, my_screen_surface, NULL);
	SDL_UpdateWindowSurface(my_window);
	while (gameRunning)
	{

		if (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				gameRunning = false;
			}
		}
	}
	//	system("pause>nul");
	SDL_FreeSurface(my_image);
	SDL_DestroyWindow(my_window);
	SDL_Quit();
	return true;
}
void CreateImage(string filepath,Image *img, Camera* camera, Scene* scene,Shader* render)
{
	img->Raycast(render,*camera, *scene);
	img->Write(const_cast<char*>(filepath.c_str()));
}