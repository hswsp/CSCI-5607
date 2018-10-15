#pragma once
#include"Objects.h"
class Image;

class Shader
{
public:
	int maxdepth;
	Image* img;
	Shader(int maxreflect = 5) :maxdepth(maxreflect) {}
	//Ray Cast 
	Pixel rayTraceRecursive(const Scene& scene, Ray ray);
	Vector EvaluateRayTree(const Scene& scene, Ray ray, int maxReflect);
	Vector ApplyLightModel(const Scene& scene, Ray ray, int maxReflect, IntersectResult* hit);
	Vector Refract(const Scene& scene, Ray ray, IntersectResult* hit);
	void fresnel(const Vector &I, const Vector &N, const float &ior, float &kr);
};

