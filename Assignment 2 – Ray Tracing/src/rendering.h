#pragma once
#include"Objects.h"
class Image;
//Ray Cast 
Pixel rayTraceRecursive(const Image* img,const Scene& scene, Ray ray, int maxReflect);
Pixel ApplyLightModel(const Scene& scene, Ray ray, int maxReflect, IntersectResult* hit);

