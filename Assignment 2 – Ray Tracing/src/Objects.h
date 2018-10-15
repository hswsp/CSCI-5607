#pragma once
#include<math.h>
#include<iostream>
#include<algorithm>
#include<vector>
#include"Vector.h"
#include"pixel.h"
using namespace std;
typedef unsigned int Color;
#define PI 3.141592653589793238462643383279502884197169399375


class Geometry;
class Light;
inline Pixel mulColor(Pixel color, float n)
{
	return Pixel(color.r*n, color.g*n, color.b*n, color.a);
}
//if alpha works
//float crCalculateBlend(float a1, float a2, float c1, float c2)
//{
//	return (c1 * a1 * (1.0 - a2) + c2 * a2) / (a1 + a2 - a1 * a2);
//}
//no alpha
inline Pixel addColor(Pixel color1, Pixel color2)
{
	Pixel newp;
	newp.SetClamp(color1.r+ color2.r, color1.g + color2.g, color1.b + color2.b, color1.a);
	return newp;
}
struct IntersectResult
{
	const Geometry* geometry;
	float distance;
	Vector position;
	Vector normal;
	IntersectResult() :geometry(NULL), distance(1.0E30),
		position(Vector::VNULL()), normal(Vector::VNULL()) {}
	IntersectResult(const Geometry* geometry, float distance, Vector position, Vector normal) :
		geometry(geometry), distance(distance),position(position),normal(normal) {}
};

//ray
class Ray 
{
public:
	Vector origin, direction;
	Ray() :origin(Vector::VNULL()),direction(Vector::VNULL()){};
	Ray(const Vector& o, const Vector& d) : origin(o), direction(d) {}

	Vector getPoint(float t) const;
};
//material
class Material
{
public:
	Vector ambient;
	Material() {}
	Material(const Vector& amb) :ambient(amb) {};
	virtual Vector sample(const Light& light, const Ray& ray, const Vector& position, const Vector& normal)const = 0;
};
//material using Lectnote's strategy to shade.
class PhongMaterial :public Material
{
public:
	Vector diffuse, specular;
	Vector transmissive;
	float ior;//index of refrection
	float shininess;//phone cosine
	PhongMaterial(PhongMaterial* aPhongMaterial):
		diffuse(aPhongMaterial->diffuse), specular(aPhongMaterial->specular),
		transmissive(aPhongMaterial ->transmissive),ior(aPhongMaterial->ior), 
		shininess(aPhongMaterial->shininess),Material(aPhongMaterial->ambient){}
	PhongMaterial(const Vector& aDiffuse, const Vector& aSpecular, const Vector& aTransmissive,
		const Vector& aAmbient,
		float aShininess, 
		float aIor)
		:diffuse(aDiffuse), specular(aSpecular),transmissive(aTransmissive), shininess(aShininess), 
		ior(aIor),Material(aAmbient)
	{}
	virtual Vector sample(const Light& light, const Ray& ray, const Vector& position, const Vector& normal)const;
};
//All Objects in the scene
class Geometry
{
public:
	Material* material;
	Geometry() {}
	Geometry(Material* aMaterial):material(aMaterial)
	{
	}
	/*virtual ~Geometry()
	{
			delete material;
	}*/
	virtual IntersectResult intersect(const Ray& ray)const = 0;
};

class Sphere : public Geometry 
{
public:
	Vector center;
	float radius, sqrRadius;
	Sphere() :center(Vector::VNULL()), radius(0), sqrRadius(0) {};
	Sphere(const Vector& c, float r, 
		Material* m = new PhongMaterial(Vector(1, 1, 1), Vector(0, 0, 0), Vector(0, 0, 0), Vector(1, 1, 1),5,1))
		:Geometry(m), center(c), radius(r), sqrRadius(r*r)
	{

	}
	virtual IntersectResult intersect(const Ray& ray) const;
};

//viwer
class Camera
{
public:
	Vector eye, front, refup, right, up;
	float  fov, fovScale;
	Camera(const Vector& aEye, const Vector& aFront, const Vector& aUp, float aFov) :eye(aEye), front(aFront),
		refup(aUp), fov(aFov)
	{
		//aEye is the POSITION;aFront is the DIRECTION; aUp is UP vector; aFov is the one-half of the
		//¡°height¡± angle of the viewing frustum
		right = front.cross(refup);
		up = right.cross(front);
		fovScale = 2 * tan(fov*1.0*PI / 180);//compute 2ta(aFov)
	}
	Ray generateRay(float x, float y,float ratio)const;
};

//Scene
class Scene
{
public:
	vector<Geometry*> objects;
	vector<Light*> lights;
	Vector ambient_light;
	//Scene():ambient_light(Vector::zero()){}
	Scene(Vector ambient_light=Vector(0,0,0)):ambient_light(ambient_light){}
	~Scene()
	{
		int i = 0;
		for (auto it = objects.begin(); it != objects.end(); it++,i++)
		{
			delete *it;
		}
	}
	void addObject(Geometry* obj);
	void addLights(Light* lit);
	IntersectResult intersect(const Ray& ray)const;
};

//light
struct LightSample
{
	Vector L;//direction
	Vector EL;//lights intensity
};
class Light
{
public:
	const Scene& scene;
	Vector  irradiance;
	Light(const Scene& aScene,Vector aColor) :scene(aScene), irradiance(aColor) {};
	virtual LightSample sample(const Vector& position)const = 0;
};
class DirectionLight :public Light
{
public:
	//float irradiance; //intensity
	Vector direction;//light direction vector I
	DirectionLight(const Scene& aScene, const Vector& aDirection,const Vector aColor)
		: direction(aDirection), Light(aScene, aColor)
	{
		direction = direction.normalize();
		//the vector direction is the opposite of the transmission direction
		direction = -1 * direction;
	}
	virtual LightSample sample(const Vector& position)const;
};
//class AmbientLight :public Light
//{
//public:
//	//float irradiance;
//	AmbientLight(const Scene& aScene, const Vector aColor=Vector::zero()) :
//		Light(aScene, aColor) {}
//	virtual LightSample sample(const Vector& position)const;
//};
class PointLight :public Light
{
public:
	Vector position;
	PointLight(const Scene& aScene, const Vector& aPosition, const Vector aColor)
		:position(aPosition), Light(aScene, aColor)
	{
	}
	virtual LightSample sample(const Vector& position)const;
};