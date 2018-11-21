#include"Map.h"
using namespace std;
void Map::ReadMap(const char* filename)
{
	ifstream modelFile;
	modelFile.open(filename);
	if (!modelFile.is_open())
	{
		printf("no such file!\n");
		return;
	}
	//int numLines = 0;
	modelFile >> MapSize[1]>> MapSize[0];//height width
	SavedMap = new char*[MapSize[0]];
	mapObj = new int*[MapSize[0]];
	for (int i = 0; i < MapSize[0]; ++i)
	{
		SavedMap[i] = new char[MapSize[1]];
		mapObj[i] = new int[MapSize[1]];
		for (int j = 0; j < MapSize[1]; ++j)
		{
			modelFile >> SavedMap[i][j];
			mapObj[i][j] = -1;
		}
	}
	printf("%d\n", MapSize[0]);
	printf("%d\n", MapSize[1]);
	modelFile.close();
}