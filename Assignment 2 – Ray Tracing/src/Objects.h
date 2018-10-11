#pragma once
#include<math.h>
#include<iostream>
#include<vector>
#include"Vector.h"
using namespace std;
typedef unsigned int Color;
#define PI 3.141592653589793238462643383279502884197169399375

class Geometry;
class Scene;
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
struct LightSample
{
	Vector L;
	float EL;
};
 
class Light
{
public:
	const Scene& scene;
	Light(const Scene& aScene) :scene(aScene) {};
	virtual LightSample sample(const Vector& position)const = 0;
};
class Ray 
{
public:
	Vector origin, direction;
	Ray() :origin(Vector::VNULL()),direction(Vector::VNULL()){};
	Ray(const Vector& o, const Vector& d) : origin(o), direction(d) {}

	Vector getPoint(float t) const;
};

class Material
{
public:
	float reflectiveness;
	Material(float ref) :reflectiveness(ref) {};
	virtual Color sample(const Light& light, const Ray& ray, const Vector& position, const Vector& normal)const = 0;
};
//All Objects in the scene
class Geometry
{
public:
	Material* material;
	Geometry(Material* aMaterial):material(aMaterial){}
	Geometry():material(NULL) {}
	virtual ~Geometry()
	{
		delete material;
	}
	virtual IntersectResult intersect(const Ray& ray)const = 0;
};

class Sphere : public Geometry 
{
public:
	Vector center;
	float radius, sqrRadius;
	Sphere() :center(Vector::VNULL()), radius(0), sqrRadius(0) {};
	Sphere(const Vector& c, float r, Material*m = NULL):Geometry(m), center(c), radius(r), sqrRadius(r*r){}
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
	Vector ambient_light;
	Scene():ambient_light(Vector::zero()){}
	Scene(Vector ambient_light=Vector(0,0,0)):ambient_light(ambient_light){}
	~Scene()
	{
		for (auto it = objects.begin(); it != objects.end(); ++it)
			delete *it;
	}
	void addObject(Geometry* obj);
	IntersectResult intersect(const Ray& ray)const;
};
