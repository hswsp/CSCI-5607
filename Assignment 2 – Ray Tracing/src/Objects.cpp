#include"Objects.h"
//Vector
inline Vector operator*(float l, const Vector& r) { return r * l; }
float Vector::sqrLength() const
{
	return x * x + y * y + z * z;
}

float Vector::length() const
{
	return sqrt(sqrLength());
}

Vector Vector::operator+(const Vector& r) const {
	return Vector(x + r.x, y + r.y, z + r.z);
}

Vector Vector::operator-(const Vector& r) const {
	return Vector(x - r.x, y - r.y, z - r.z);
}

Vector Vector::operator*(float v) const {
	return Vector(v * x, v * y, v * z);
}

Vector Vector::operator/(float v) const {
	float inv = 1 / v;
	return *this * inv;
}

Vector Vector::normalize() const {
	float invlen = 1 / length();
	return *this * invlen;
}

float Vector::dot(const Vector& r) const {
	return x * r.x + y * r.y + z * r.z;
}

Vector Vector::cross(const Vector& r) const {
	return Vector(-z * r.y + y * r.z,
		z * r.x - x * r.z,
		-y * r.x + x * r.y);
}

Vector Ray::getPoint(float t) const
{
	return origin + t * direction;
}
IntersectResult Sphere:: intersect(const Ray& ray) const
{
	Vector v = ray.origin - center;
	float a0 = v.sqrLength() - sqrRadius;
	float DdotV = ray.direction.dot(v);
	if (DdotV <= 0.0) 
	{
		float discr = DdotV * DdotV - a0;
		if (discr >= 0.0) {
			float d = -DdotV - sqrt(discr);
			Vector p = ray.getPoint(d);
			Vector n = (p - center).normalize();
			return IntersectResult(this, d, p, n);
		}
	}
	return IntersectResult();
}

Ray Camera::generateRay(float x, float y)const
{
	Vector r, u;
	r = right * (x - 0.5)*fovScale;
	u = up * (y - 0.5)*fovScale;
	r = front + r + u;
	r.normalize();
	return Ray(eye, r);
}