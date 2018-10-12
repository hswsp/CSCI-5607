#include"rendering.h"
#include "image.h"

Pixel Shader::rayTraceRecursive(const Scene& scene, Ray ray)
{
	Pixel p;
	Vector color = EvaluateRayTree(scene,ray, maxdepth);
	p.SetClamp(color.x * 255, color.y * 255, color.z * 255, 255);
	return p;
}
Vector Shader::EvaluateRayTree(const Scene& scene, Ray ray, int maxReflect)
{
	IntersectResult hit = scene.intersect(ray);
	if (hit.geometry != NULL)
	{
		return ApplyLightModel(scene, ray, maxdepth, &hit);
	}
	else
	{
		return Vector(img->backgroud.r, img->backgroud.g, img->backgroud.b)/255.0;
	}
}
Vector Shader::ApplyLightModel(const Scene& scene, Ray ray, int maxReflect, IntersectResult* hit)
{
	Vector contribution(0, 0, 0);
	//IntersectResult intersectResult,temp;
	Vector ambient;
	Vector reflectiveness, refraction, reflectedColor, refractionColor;
	Vector r;
	//intersectResult = scene.intersect(ray);
	ambient = hit->geometry->material->ambient;//origin color
	// ambient light
	contribution = contribution + ambient * scene.ambient_light;// 
	PhongMaterial *mat = dynamic_cast<PhongMaterial*>(hit->geometry->material);
	reflectiveness = mat->specular;
	refraction = mat->transmissive;
	for (auto it = scene.lights.begin(); it != scene.lights.end(); ++it)
	{
		Vector res = hit->geometry->material->sample(**it, ray, hit->position, hit->normal);
		contribution = contribution + (Vector(1, 1, 1) - reflectiveness)*res;//
	}
	if ((reflectiveness.x > 0|| reflectiveness.y > 0|| reflectiveness.z > 0) && maxReflect > 0)
	{
		Ray glass;
		glass.direction = Refract(scene, ray, hit);
		glass.origin = hit->position;
		r = hit->normal*(-2 * hit->normal.dot(ray.direction)) + ray.direction;
		ray.origin = hit->position;
		ray.direction = r;
		reflectedColor = EvaluateRayTree(scene, ray, maxReflect - 1);
		contribution = contribution + reflectiveness * reflectedColor;
		if (glass.direction != Vector::zero())
		{
			refractionColor = EvaluateRayTree(scene, glass, maxReflect - 1);
			contribution = contribution + refraction * refractionColor;
		}
		
	}
	return Vector(min(1, contribution.x), min(1, contribution.y), min(1, contribution.z));
}

Vector Shader::Refract(const Scene& scene, Ray ray, IntersectResult* hit)
{
	Vector Nrefr = hit->normal.normalize();
	Vector I = ray.direction.normalize();
	Vector refractionLit;
	PhongMaterial *mat = dynamic_cast<PhongMaterial*>(hit->geometry->material);
	float NdotI = Nrefr.dot(I);
	float etai = 1, etat = mat->ior; // etai is the index of refraction of the medium the ray is in before entering the second medium 
	if (NdotI < 0) 
	{
		// we are outside the surface, we want cos(theta) to be positive
		NdotI = -NdotI;
	}
	else 
	{
		// we are inside the surface, cos(theta) is already positive but reverse normal direction
		Nrefr = -1* Nrefr;
		// swap the refraction indices
		std::swap(etai, etat);
	}
	float eta = etai / etat; // n_1 / n_2 
	float c1 = NdotI;
	float k = 1 - eta * eta*(1 - NdotI * NdotI);
	return (k < 0) ? Vector::zero() : eta * I + (eta*c1 - sqrt(k))*Nrefr;
}
