#include"rendering.h"
#include "image.h"

Pixel Shader::rayTraceRecursive(const Scene& scene, Ray ray)
{
	Pixel p;
	Vector color = EvaluateRayTree(scene,ray, maxdepth);//
	p.SetClamp(color.x * 255, color.y * 255, color.z * 255, 255);
	return p;
}
Vector Shader::EvaluateRayTree(const Scene& scene, Ray ray, int maxdepth)
{
	IntersectResult hit = scene.intersect(ray);
	if (hit.geometry != NULL)
	{
		return ApplyLightModel(scene, ray, maxdepth, &hit);
	}
	else
	{
		return Vector(img->backgroud.x, img->backgroud.y, img->backgroud.z);///255.0
	}
}
Vector Shader::ApplyLightModel(const Scene& scene, Ray ray, int maxdepth, IntersectResult* hit)
{
	if (maxdepth<=0)
		return Vector::zero();
	Vector contribution(0, 0, 0);
	Vector ambient;
	Vector reflectiveness, refraction, reflectedColor, refractionColor;
	Vector r;
	reflectedColor = Vector(0, 0, 0);
	// ambient light
	ambient = hit->geometry->material->ambient;//origin color
	contribution = contribution + ambient * scene.ambient_light;// 
	PhongMaterial *mat = dynamic_cast<PhongMaterial*>(hit->geometry->material);
	reflectiveness = mat->specular;
	refraction = mat->transmissive;
	for (auto it = scene.lights.begin(); it != scene.lights.end(); ++it)
	{
		Vector res = hit->geometry->material->sample(**it, ray, hit->position, hit->normal);
		contribution = contribution + (Vector(1, 1, 1) - reflectiveness)*res;//
	}

	//Ray glass, refl;
	//float kr;
	//float fbias = 1E-4;
	//Vector bias = fbias * hit->normal;
	////refraction
	//bool outside = ray.direction.dot(hit->normal) < 0;
	//glass.direction = Refract(ray, hit).normalize();//scene, 
	//glass.origin = outside?hit->position- bias: hit->position + bias;
	//fresnel(ray.direction, hit->normal, mat->ior, kr);
	//if (kr < 1)//&& glass.direction != Vector::zero()
	//{
	//	refractionColor = EvaluateRayTree(scene, glass, maxdepth - 1);
	//	contribution = contribution + refraction * refractionColor*(1 - kr);//
	//}
	////reflection
	//r = hit->normal*(-2 * hit->normal.dot(ray.direction)) + ray.direction;
	//refl.direction = r.normalize();
	//refl.origin = outside ? hit->position + bias : hit->position - bias;
	//reflectedColor = EvaluateRayTree(scene, refl, maxdepth - 1);
	//contribution = contribution + reflectiveness * reflectedColor ;//* kr

		//Ray refr, refl;
		//// compute refraction if it is not a case of total internal reflection
		//float kr;
		//PhongMaterial *mat = dynamic_cast<PhongMaterial*>(hit->geometry->material);
		//fresnel(ray.direction, hit->normal, mat->ior, kr);
		//if (kr < 1)
		//{
		//	refr.direction = Refract(scene, ray, hit).normalize();
		//	refr.origin = hit->position;
		//	refractionColor = EvaluateRayTree(scene, refr, maxReflect - 1);
		//}
		//refl.direction = hit->normal*(-2 * hit->normal.dot(ray.direction)) + ray.direction;
		//refl.direction = refl.direction.normalize();
		//refl.origin = hit->position;
		//reflectedColor = EvaluateRayTree(scene, refl, maxReflect - 1);
		//contribution = contribution + reflectiveness *reflectedColor * kr + refraction * refractionColor * (1 - kr) ;//
	return Vector(min(1, contribution.x), min(1, contribution.y), min(1, contribution.z));
}

void Shader::fresnel(const Vector &I, const Vector &N, const float &ior, float &kr)
{
	float cosi = N.dot(I);
	// etai is the index of refraction of the medium the ray is in before entering the second medium
	float etai = 1, etat = ior;
	//clamp 
	if (cosi < -1)cosi = -1;
	else if (cosi > 1)cosi = 1;
	if (cosi > 0) { std::swap(etai, etat); }
	// Compute sini using Snell's law
	float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection
	if (sint >= 1)
	{
		kr = 1;
	}
	else
	{
		float cost = sqrtf(std::max(0.f, 1 - sint * sint));
		// if we are outside of the surface, we actually computed the -cosi
		cosi = fabsf(cosi);
		float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
		float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	// As a consequence of the conservation of energy, transmittance is given by:
	// kt = 1 - kr;
}
Vector Shader::Refract(Ray ray, IntersectResult* hit)//const Scene& scene, 
{
	Vector Nrefr = hit->normal.normalize();
	Vector I = ray.direction.normalize();
	Vector refractionLit;
	PhongMaterial *mat = dynamic_cast<PhongMaterial*>(hit->geometry->material);
	float NdotI = Nrefr.dot(I);
	// etai is the index of refraction of the medium the ray is in before entering the second medium
	float etai = 1, etat = mat->ior;
	//clamp 
	if (NdotI < -1)NdotI = -1;
	else if (NdotI > 1)NdotI = 1;
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



