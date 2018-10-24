#pragma once
#include<math.h>
#include<stdlib.h>
#include<iostream>
#include<algorithm>
#include<vector>
#include <omp.h>
#include"Vector.h"
#include"pixel.h"
#include"texture.h"
using namespace std;
typedef unsigned int Color;
#define PI 3.141592653589793238462643383279502884197169399375


class Geometry;
class Light;
class aabb;

inline Pixel mulColor(Pixel color, float n)
{
	return Pixel(color.r*n, color.g*n, color.b*n, color.a);
}

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
	IntersectResult& operator =(IntersectResult hit)
	{
		this->geometry = hit.geometry;
		this->distance = hit.distance;
		this->normal = hit.normal;
		this->position = hit.position;
		return *this;
	}
};
//class hitable
//{
//public:
//	virtual bool bounding_box(float t0, float t1, aabb&box)const = 0;
//};
//ray
class Ray 
{
public:
	Vector origin, direction;
	//motion blur
	float time;
	Ray() :origin(Vector::VNULL()),direction(Vector::VNULL()){};
	//Ray(const Vector& o, const Vector& d) : origin(o), direction(d) {}
	Ray(const Vector& o, const Vector& d, float ti = 0) :origin(o), direction(d), time(ti) {}
	Vector getPoint(float t) const;
};
//material
class Material
{
public:
	Vector ambient;
	Material() {}
	Material(const Vector& amb) :ambient(amb){};
	virtual Vector sample(const Light& light, const Ray& ray, const Vector& position, const Vector& normal)const = 0;
};
//material using Lectnote's strategy to shade.
class PhongMaterial :public Material
{
public:
	texture* text;
	Vector diffuse, specular;
	Vector transmissive;
	float ior;//index of refrection
	float shininess;//phone cosine
	PhongMaterial(PhongMaterial* aPhongMaterial):
		diffuse(aPhongMaterial->diffuse), specular(aPhongMaterial->specular),
		transmissive(aPhongMaterial ->transmissive),ior(aPhongMaterial->ior), 
		shininess(aPhongMaterial->shininess), text(aPhongMaterial->text),Material(aPhongMaterial->ambient){}
	PhongMaterial(const Vector& aDiffuse, const Vector& aSpecular, const Vector& aTransmissive,
		const Vector& aAmbient,
		float aShininess, 
		float aIor)
		:diffuse(aDiffuse), specular(aSpecular),transmissive(aTransmissive), shininess(aShininess), 
		ior(aIor),Material(aAmbient)
	{}
	void addTexture(texture* checktext)
	{
		text = checktext;
	}
	virtual Vector sample(const Light& light, const Ray& ray, const Vector& position, const Vector& normal)const;
	
};
//class TextureMaterial :public Material
//{
//public:
//	texture* text;
//	Vector diffuse, specular;
//	Vector transmissive;
//	float ior;//index of refrection
//	float shininess;//phone cosine
//	TextureMaterial() {}
//	TextureMaterial(TextureMaterial* aPhongMaterial) :text(aPhongMaterial->text),
//		diffuse(aPhongMaterial->diffuse), specular(aPhongMaterial->specular),
//		transmissive(aPhongMaterial ->transmissive), ior(aPhongMaterial->ior),
//		shininess(aPhongMaterial->shininess), Material(aPhongMaterial->ambient) {}
//	TextureMaterial(const Vector& aDiffuse, const Vector& aSpecular, const Vector& aTransmissive,
//		const Vector& aAmbient,
//		float aShininess,
//		float aIor, texture* text=NULL)
//		:text(text),diffuse(aDiffuse), specular(aSpecular), transmissive(aTransmissive), shininess(aShininess),
//		ior(aIor), Material(aAmbient) {}
//
//	void addTexture(texture* checktext)
//	{
//		text = checktext;
//	}
//	virtual Vector sample(const Light& light, const Ray& ray, const Vector& position, const Vector& normal)const;
//};
//All Objects in the scene
class Geometry
{
public:
	Material* material;
	bool IsPlane;
	Geometry() {}
	Geometry(Material* aMaterial,bool plane=false):material(aMaterial),IsPlane(plane)
	{
	}
	/*virtual ~Geometry()
	{
			delete material;
	}*/
	virtual bool intersect(const Ray& ray,IntersectResult& hit)const = 0;//, float& tmin, float& tmax
	virtual bool bounding_box(float t0, float t1, aabb& box )const = 0;//t0, t1 is for time
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
	virtual bool intersect(const Ray& ray, IntersectResult& hit) const;//, float& tmin, float& tmax
	virtual bool bounding_box(float t0, float t1, aabb& box)const;//
};
class moving_sphere :public Geometry
{
public:
	float time0, time1;
	Vector center0, center1;
	float radius, sqrRadius;
	moving_sphere():center0(Vector::VNULL()), center1(Vector::VNULL()), radius(0), sqrRadius(0) {};
	moving_sphere(const Vector& c0, const Vector& c1, float r,float t0,float t1,
		Material* m = new PhongMaterial(Vector(1, 1, 1), Vector(0, 0, 0), Vector(0, 0, 0), Vector(1, 1, 1), 5, 1))
		:Geometry(m), center0(c0), center1(c1), radius(r), sqrRadius(r*r),time0(t0),time1(t1)
	{

	}
	Vector Center(float time)const
	{
		return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
	}
	virtual bool intersect(const Ray& ray, IntersectResult& hit) const;//, float& tmin, float& tmax
	virtual bool bounding_box(float t0, float t1, aabb& box)const;
};
enum TriType //use face normal or smooth normal
{
	FACE_NORMAL,
	SMOOTH_NORMAL
};
class TriangleMesh
{
public:
	TriType name;
	int max_vertices;
	int max_normals;
	vector<Vector*> vertices; //Vertices pool
	vector<Vector*> normals; //Normals pool
	TriangleMesh(TriType type=FACE_NORMAL):name(type) {}
	~TriangleMesh()
	{

		for (auto it = vertices.begin(); it != vertices.end(); it++)
		{
			delete *it;
		}

		for (auto it = normals.begin(); it != normals.end(); it++)
		{
			delete *it;
		}
	}
	void IniVerticeN()
	{
		normals.reserve(max_normals);//reserve the storage
	}
	void IniVerticeV()
	{
		vertices.reserve(max_vertices);
	}
	void addVertex(Vector *vertx);
	void addNormal(Vector *normal);	
};

class Triangle : public Geometry
{
	TriType name;
	//bool raytracingIntersect(const Ray& ray,float &u,float &v,float& t, bool& IsCounterclockwise)const;
	bool raytracingIntersect(const Ray& ray, float &u, float &v, float& t)const;
public:
	TriType type;
	Vector V0, V1, V2;// tree Vertices
	Vector N0, N1, N2;//Tree Normal
	Triangle() {};
	Triangle(const Vector & v0, const Vector &v1, const Vector &v2, TriType name
	, Material* m = new PhongMaterial(Vector(1, 1, 1), Vector(0, 0, 0), Vector(0, 0, 0), Vector(1, 1, 1), 5, 1))
		:V0(v0),V1(v1),V2(v2),name(name), Geometry(m,true)
	{
	}
	Triangle(const Vector & v0, const Vector &v1, const Vector &v2, const Vector & n0, const Vector &n1, 
		const Vector &n2, const TriType name,
		Material* m = new PhongMaterial(Vector(1, 1, 1), Vector(0, 0, 0), Vector(0, 0, 0), Vector(1, 1, 1), 5, 1))
		:V0(v0), V1(v1), V2(v2), N0(n0), N1(n1), N2(n2), name(name),Geometry(m,true)
	{
	}
	virtual bool  intersect(const Ray& ray, IntersectResult& hit)const; //,  float& tmin, float& tmax
	virtual bool bounding_box(float t0, float t1, aabb& box)const;
};
//BVH
class aabb//axis-aligned bounding box
{
public:
	Vector _min, _max;//box bound
	aabb() {};
	aabb(const Vector& a, const Vector& b) :_min(a), _max(b) {}
	bool hit(const Ray& ray)const;//,float& tmin, float& tmax

};

class BVH_node :public Geometry
{
	aabb surrounding_box(aabb box0, aabb box1);//add two box
public:
	aabb box;
	Geometry* left;
	Geometry* right;
	BVH_node() {}
	BVH_node(Geometry** l, int n, float time0, float time1);
	virtual  bool intersect(const Ray& ray, IntersectResult& hit)const;//, float& tmin, float& tmax
	virtual bool bounding_box(float t0, float t1, aabb& b)const;
};
//viwer
class Camera
{
public:
	Vector eye, front, refup, right, up;
	float  fov, fovScale;
	float time0, time1;//variables for shutter open/close time
	Camera(const Vector& aEye, const Vector& aFront, const Vector& aUp, float aFov,float t0=0,float t1=0) :eye(aEye), front(aFront),
		refup(aUp), fov(aFov), time0(t0), time1(t1)
	{
		//aEye is the POSITION;aFront is the DIRECTION; aUp is UP vector; aFov is the one-half of the
		//¡°height¡± angle of the viewing frustum
		front = front.normalize();
		up = up.normalize();
		right = front.cross(refup);
		up = right.cross(front);
		fovScale = 2 * tan(fov*1.0*PI / 180);//compute 2ta(aFov)
	}
    Ray generateRay(float x, float y,float ratio)const;
};
//class MotionCamera:public Camera
//{
//public:
//	float time0, time1;//variables for shutter open/close time
//	MotionCamera(const Vector& aEye, const Vector& aFront, const Vector& aUp, float aFov, float t0, float t1) 
//		:Camera(aEye, aFront, aUp, aFov),time0(t0),time1(t1)
//	{
//		//aEye is the POSITION;aFront is the DIRECTION; aUp is UP vector; aFov is the one-half of the
//		//¡°height¡± angle of the viewing frustum
//		front = front.normalize();
//		up = up.normalize();
//		right = front.cross(refup);
//		up = right.cross(front);
//		fovScale = 2 * tan(fov*1.0*PI / 180);//compute 2ta(aFov)
//	}
//	Ray generateRay(float x, float y, float ratio)const;
//};
//Scene
class Scene
{
public:
	vector<Geometry*> objects;
	Geometry** obj;//for BVH
	vector<Light*> lights;
	Vector ambient_light;
	TriangleMesh Vertices;
	BVH_node* BVH;
	bool isBVH;
	Scene(Vector ambient_light=Vector(0,0,0), bool isbvh=false):
		ambient_light(ambient_light),isBVH(isbvh)
	{}
	~Scene()
	{
		for (auto it = objects.begin(); it != objects.end(); it++)
		{
			delete *it;
		}
		Vertices.~TriangleMesh();
		delete[] obj;
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
class SpotLight :public Light
{
public:
	Vector position, Litdirection;
	float angle1, angle2;
	SpotLight(const Scene& aScene, const Vector& aPosition, const Vector& aDirection,
		float ang1,float ang2, const Vector aColor)
		:position(aPosition),Litdirection(aDirection),angle1(ang1),angle2(ang2),Light(aScene, aColor)
	{
		Litdirection = Litdirection.normalize();
		//the vector direction is the opposite of the transmission direction
		Litdirection = -1 * Litdirection;
	}
	virtual LightSample sample(const Vector& position)const;
};

