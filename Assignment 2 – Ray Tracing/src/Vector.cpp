#include"Vector.h"
//Vector


float Vector::sqrLength() const
{
	return x * x + y * y + z * z;
}
float Vector::length() const
{
	return sqrt(sqrLength());
}
bool Vector::operator == (const Vector&r)const
{
	return((x == r.x) && (y == r.y) && (z == r.z));
}
bool Vector::operator != (const Vector&r)const
{
	return(!(*this==r));
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
Vector Vector::operator*(Vector v) const {
	return Vector(v.x * x, v.y * y, v.z * z);
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