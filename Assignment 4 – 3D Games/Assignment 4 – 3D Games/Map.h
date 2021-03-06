#pragma once
#include<fstream>
class Map
{
public:
	int MapSize[2];
	char** SavedMap;
	int ** mapObj;
	void ReadMap(const char* filename);
	~Map()
	{
		for (int i = 0; i < MapSize[0]; ++i)
		{
			delete[] SavedMap[i];
			delete[] mapObj[i];
		}
		delete[] SavedMap;
		delete[] mapObj;
	}
};