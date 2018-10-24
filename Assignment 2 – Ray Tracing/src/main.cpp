#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <cstdlib>
#include "time.h"
#include "SDL.h"
#include "image.h"
#include"rendering.h"
#undef main


#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "stb_image_write.h"


using namespace std;
//GLuint texture;
//clock_t clocks;
//int frames;
//float rad;
//static Color buffer[HEIGHT][WIDTH];
//store in file
void CreateImage(string filepath, Image *img, Camera* camera, Scene* scene, Shader* render, bool useBVH, float t0, float t1);
//show in window
bool ShowImage(string outFile, int Width, int Height);
/**
 * prototypes
 **/
static void ShowUsage(void);
static void CheckOption(char *option, int argc, int minargc);
int main(int argc, char* argv[])
{
	long start = clock();// start time
	int Width = 640;
	int Height = 480;
	Image *img = NULL;
	PhongMaterial *material = NULL;//new Material
	texture * text = NULL;
	Camera* camera = NULL;
	//MotionCamera * m
	float t0 = 0;
	float t1 = 0;
	bool isMotion = false;
	Scene* scene = new Scene;
	bool useBVH = true;//false
	Vector backgroud;
	bool did_background = false;
	Shader * render = new Shader;
	string fileName = "Image/bear.scn";//ambient_sphere  spheres2 
	string outFile = "./raytracer/raytraced.bmp";
	string line;
	// first argument is program name
	argv++, argc--;
	// no argument case
	if (argc == 0) 
	{
		ShowUsage();
	}
	// parse arguments
	while (argc > 0)
	{
		if (**argv == '-')
		{
			if (!strcmp(*argv, "-input"))
			{
				CheckOption(*argv, argc, 2);
				fileName = argv[1];
				argv += 2, argc -= 2;
			}
		}
		else
		{
			fprintf(stderr, "image: invalid option: %s\n", *argv);
			ShowUsage();
		}
	}
	// open the file containing the scene description
	//ifstream input(fileName);
	// check for errors in opening the file
	fstream input;
	input.open(fileName);
	if (!input) 
	{
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
	
	// read input file
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
		else if (command == "shutter")// make sure shutter come first
		{
			isMotion = true;
			input >> t0 >> t1;
		}
		else if (command == "camera")
		{
			float px, py, pz, dx, dy, dz, ux, uy, uz, ha;
			input >> px >> py >> pz >> dx >> dy >> dz >> ux >> uy >> uz >> ha;
			Vector aEye(px, py, pz);
			Vector aFront(dx, dy, dz);
			Vector aUp(ux, uy, uz);
			if (isMotion)
			{
				camera = new Camera(aEye, aFront, aUp, ha, t0, t1);
			}
			else
			{
				camera = new Camera(aEye, aFront, aUp, ha);
			}
		}
		else if (command == "sphere") 
		{ //If the command is a sphere command
			float x, y, z, r;
			input >> x >> y >> z >> r;
			Vector center(x, y, z);
			//use the current stored material
			if (isMotion)
			{
				PhongMaterial* local_material = new PhongMaterial(material);
				srand(time(0));
				float delta = 1.0*(rand() % 100 / (double)101);
				Vector center1 = center + Vector(0, delta, 0);
				scene->addObject(new moving_sphere(center, center1, r, t0, t1, local_material));
				
			}
			else
			{
				if (material == NULL)
					scene->addObject(new Sphere(center, r));
				/*else if (text != NULL)
				{
					TextureMaterial* textm = new TextureMaterial(static_cast<TextureMaterial*>(material));
					textm->addTexture(text);
					scene->addObject(new Sphere(center, r, textm));
				}*/
				else
				{
					PhongMaterial* local_material = new PhongMaterial(material);
					scene->addObject(new Sphere(center, r, local_material));//material&
				}
			}
		}
		else if (command=="max_normals")
		{
			input >> scene->Vertices.max_normals;
		}
		else if (command == "max_vertices")
		{
			input >> scene->Vertices.max_vertices;
		}
		else if (command == "normal")
		{
			float x, y, z;
			input >> x>> y>> z;
			scene->Vertices.addNormal(new Vector(x, y, z));
			scene->Vertices.IniVerticeN();
		}
		else if (command == "vertex")
		{
			float x, y, z;
			input >> x>> y>> z;
			scene->Vertices.addVertex(new Vector(x, y, z));
			scene->Vertices.IniVerticeV();
		}
		else if (command == "triangle")
		{
			scene->Vertices.name = FACE_NORMAL;
			int v0, v1, v2;
			input >> v0 >> v1 >> v2;
			assert(scene->Vertices.max_vertices > 0);
			Vector V0 = *scene->Vertices.vertices[v0];
			Vector V1 = *scene->Vertices.vertices[v1];
			Vector V2 = *scene->Vertices.vertices[v2];
			if (material == NULL)
			{
				scene->addObject(new Triangle(V0, V1, V2,scene->Vertices.name));
			}	
			else
			{
				PhongMaterial* local_material = new PhongMaterial(material);
				scene->addObject(new Triangle(V0, V1, V2, scene->Vertices.name, local_material));
			}
		}
		else if (command=="normal_triangle")
		{
			scene->Vertices.name = SMOOTH_NORMAL;
			int v0, v1, v2, n0, n1, n2;
			input >> v0 >> v1 >> v2 >> n0 >> n1 >> n2;
			assert(scene->Vertices.max_vertices > 0);
			assert(scene->Vertices.max_normals > 0);
			Vector V0 = *scene->Vertices.vertices[v0];
			Vector V1 = *scene->Vertices.vertices[v1];
			Vector V2 = *scene->Vertices.vertices[v2];
			if (material == NULL)
			{
				Vector N0 = *scene->Vertices.normals[n0];
				Vector N1 = *scene->Vertices.normals[n1];
				Vector N2 = *scene->Vertices.normals[n2];
				scene->addObject(new Triangle(V0, V1, V2, N0, N1, N2, scene->Vertices.name));
			}
			else
			{
				PhongMaterial* local_material = new PhongMaterial(material);
				Vector N0 = *scene->Vertices.normals[n0];
				Vector N1 = *scene->Vertices.normals[n1];
				Vector N2 = *scene->Vertices.normals[n2];
				scene->addObject(new Triangle(V0, V1, V2, N0, N1, N2, scene->Vertices.name, local_material));
			}
			

		}
		else if (command == "background") 
		{ //If the command is a background command
			float r, g, b;
			input >> r >> g >> b;
			//printf("Background color of (%f,%f,%f)\n", r, g, b);
			backgroud.x = r;
			backgroud.y = g;
			backgroud.z = b;
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
			//delete material;
			material = new PhongMaterial(diffuse,specular,transmissive_color,ambient_color,ns,ior);
		}
		else if (command == "checkertexture")
		{
			float ox, oy, oz, ex, ey, ez;
			//tree for even , three for odd
			input >> ox >> oy >> oz >> ex >> ey >> ez;
			Vector odd(ox, oy, oz);
			Vector even(ex, ey, ez);
			constant_texture oddtext(odd);
			constant_texture eventext(even);
			text = new checker_texture(&eventext, &oddtext);
			material->addTexture(text);
        }
		else if (command == "imagetexture")
		{
			string image_path;
			input >> image_path;
			int nx, ny;
			int nn;//chanel
			unsigned char* text_data = stbi_load(image_path.c_str(), &nx, &ny, &nn, 0);
			text = new image_texture(text_data, nx, ny);
			material->addTexture(text);
        }
		else if (command == "point_light")
		{
			float r, g, b,x, y, z;
			input >> r >> g >> b >> x >> y >> z;
			Vector color(r, g, b);
			color = color;
			Vector position(x, y, z);
			scene->addLights(new PointLight(*scene,position, color));
		}
		else if (command == "directional_light")
		{
			float r, g, b, x, y, z;
			input >> r >> g >> b >> x >> y >> z;
			Vector color(r, g, b);
			color = color;//  /255.0
			Vector direction(x, y, z);
			scene->addLights(new DirectionLight(*scene, direction, color));
		}
		else if (command =="ambient_light")
		{
			float r, g, b;
			input >> r >> g >> b;
			scene->ambient_light = Vector(r, g, b);
		}
		else if (command == "spot_light")
		{
			float r, g, b, px, py, pz, dx, dy, dz, angle1, angle2;
			input >> r >> g >> b >> px >> py >> pz >> dx >> dy >> dz >> angle1 >> angle2;
			Vector color(r, g, b);
			Vector position(px, py, pz);
			Vector direction(dx, dy, dz);
			scene->addLights(new SpotLight(*scene, position, direction, angle1, angle2, color));
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
	if (did_background)
	{
		img = new Image(Width, Height, backgroud);
	}
	else
	{
		img = new Image(Width, Height);
	}
	render->img = img;
	CreateImage(outFile, img, camera, scene, render, useBVH, t0, t1);
	//}
	long finish = clock();//end time
	long t = finish - start;
	cout << "The run time is:"<<t<<"ms!\n" << endl; //show processing time
	if (!ShowImage(outFile,Width,Height))
	{
		cerr << "Image error" << endl;
		system("pause>nul");
	}
	delete img;
	return 0;
}

bool ShowImage(string outFile, int Width = 640,int Height = 480)
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
		Width,//640
		Height,//480
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
void CreateImage(string filepath,Image *img, Camera* camera, Scene* scene,Shader* render, bool isBVH,float t0,float t1)
{
	scene->isBVH = isBVH;
	if (isBVH)
	{
		scene->obj = new Geometry*[scene->objects.size()];
		for (int i = 0; i < scene->objects.size(); ++i)
		{
			scene->obj[i] = scene->objects[i];
		}
		scene->BVH = new BVH_node(scene->obj, scene->objects.size(), t0, t1);
	}
	img->Raycast(render,camera, *scene);
	img->Write(const_cast<char*>(filepath.c_str()));
}

static char options[] =
"-help\n"
"-input <file>\n"
;

static void ShowUsage(void)
{
	fprintf(stderr, "Usage: image -input <filename> \n");
	fprintf(stderr, "%s", options);
	system("pause>nul");
	exit(EXIT_FAILURE);
}
static void CheckOption(char *option, int argc, int minargc)
{
	if (argc < minargc)
	{
		fprintf(stderr, "Too few arguments for %s\n", option);
		ShowUsage();
	}
}