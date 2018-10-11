#include"rendering.h"
#include "image.h"
Pixel rayTraceRecursive(const Image* img,const Scene& scene, Ray ray, int maxReflect)
{
	IntersectResult hit = scene.intersect(ray);
	if (hit.geometry != NULL)
	{
		return ApplyLightModel(scene, ray, maxReflect, &hit);
	}
	else
	{
		return *(img->backgroud);
	}
		
}

Pixel ApplyLightModel(const Scene& scene, Ray ray, int maxReflect, IntersectResult* hit)
{
	
}

