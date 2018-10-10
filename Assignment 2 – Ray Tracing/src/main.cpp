#include "image.h"
#include"Objects.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdlib>
#include<ctime>
#include "SDL.h"
#undef main


#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "stb_image_write.h"

#define HEIGHT 600
#define WIDTH 600

using namespace std;
//GLuint texture;
clock_t clocks;
int frames;
float rad;
static Color buffer[HEIGHT][WIDTH];

void CreateImage(string filepath, Image *img);

bool ShowImage(string outFile)
{
	SDL_Window* my_window = NULL;
	SDL_Surface* my_screen_surface = NULL;
	SDL_Surface *my_image = NULL;
	bool gameRunning = true;
	SDL_Event event;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
		return false;
	}
	my_window = SDL_CreateWindow(
		"HELLO WORLD",
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
		printf("Unable to load image %s! SDL Error: %s\n",outFile, SDL_GetError());
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

int main() {
	string line;
	int Width = 640;
	int Height = 480;
	Image *img = NULL;
	bool did_output = false;
	string fileName = "Image/spheres1.scn";
	string outFile = "test.bmp";
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
		else if (command == "sphere") 
		{ //If the command is a sphere command
			float x, y, z, r;
			input >> x >> y >> z >> r;
			printf("Sphere as position (%f,%f,%f) with radius %f\n", x, y, z, r);
		}
		else if (command == "background") 
		{ //If the command is a background command
			float r, g, b;
			input >> r >> g >> b;
			printf("Background color of (%f,%f,%f)\n", r, g, b);
		}
		else if (command == "output_image")
		{ //If the command is an output_image command
			
			input >> outFile;
			printf("Render to file named: %s\n", outFile.c_str());			
			did_output = true;
			img = new Image(Width, Height);
			CreateImage(outFile, img);
		}
		else if (command == "material")
		{

		}
		else if (command == "point_light")
		{

		}
		else if (command =="ambient_light")
		{

		}
		else if (command=="background")
		{

		}
		else if (command =="max_depth")
		{

		}
		else 
		{
			getline(input, line); //skip rest of line
			cout << "WARNING. Do not know command: " << command << endl;
		}
	}
	//if (!did_output)
	//{
	//	fprintf(stderr, "Warning, you didn't tell me to output anything.  I hope that's OK.\n");
	//}
	
	delete img;
	if (!ShowImage(outFile))
	{
		cerr << "Image error" << endl;
		system("pause>nul");
	}
	return 0;
}

void CreateImage(string filepath,Image *img)
{
	img->Write(const_cast<char*>(filepath.c_str()));
}