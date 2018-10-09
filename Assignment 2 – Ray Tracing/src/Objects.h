#include<math.h>
#include<iostream>
typedef unsigned int Color;
#define PI 3.141592653589793238462643383279502884197169399375
struct Vector
{
	float x, y, z;
	Vector(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	Vector() {};
	Vector(const Vector& r) : x(r.x), y(r.y), z(r.z) {}
	
	float sqrLength() const;
	float length() const;
	Vector operator+(const Vector& r) const;
	Vector operator-(const Vector& r) const;
	Vector operator*(float v) const;
	Vector operator/(float v) const;
	Vector normalize() const;
	float dot(const Vector& r) const;
	Vector cross(const Vector& r) const;
	static Vector zero() 
	{
		return Vector(0, 0, 0);
	}
};
class Geometry;
struct IntersectResult
{
	const Geometry* geometry;
	float distance;
	Vector position;
	Vector normal;
	IntersectResult() :geometry(NULL), distance(NULL),
		position(Vector::zero()), normal(Vector::zero()) {}
	IntersectResult(const Geometry* geometry, float distance, Vector position, Vector normal) :
		geometry(geometry), distance(distance),position(position),normal(normal) {}
};
class Light
{
	
};
class Ray 
{
public:
	Vector origin, direction;
	Ray(const Vector& o, const Vector& d) : origin(o), direction(d) {}

	Vector getPoint(float t) const;
};

class Material
{
public:
	float reflectiveness;
	Material(float ref) :reflectiveness(ref){}
	
};
class Geometry
{
public:
	Material* material;
	Geometry(Material* aMaterial):material(aMaterial){}
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
	Sphere(const Vector& c, float r, Material*m = NULL):Geometry(m), center(c), radius(r), sqrRadius(r*r){}
	IntersectResult intersect(const Ray& ray) const;
};

class Camera
{
public:
	Vector eye, front, refup, right, up;
	float  fov, fovScale;
	Camera(const Vector& aEye, const Vector& aFront, const Vector& aUp, float aFov) :eye(aEye), front(aFront), refup(aUp), fov(aFov)
	{
		right = front.cross(refup);
		up = right.cross(front);
		fovScale = tan(fov*0.5*PI / 180) * 2;
	}
	Ray generateRay(float x, float y)const;
};

