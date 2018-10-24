#include"texture.h"
Vector checker_texture::value(float u, float v, const Vector&p)const
{
	float sines = sin(5.0*p.x)*sin(5.0*p.y)*sin(5.0*p.z);
	if (sines < 0)
		return odd->value(u, v, p);
	else
		return even->value(u, v, p);
}
Vector image_texture::value(float u, float v, const Vector&p)const
{
	int i =( u) * nx;
	int j = (1 - v)*ny - 1E-3;
	//clamp
	if (i < 0) i = 0;
	if (i > nx - 1) i = nx - 1;
	if (j < 0)j = 0;
	if (j > ny - 1)j = ny - 1;
	float r = int(data[3 * i + 3 * ny*j]) / 255.0;
	float g = int(data[3 * i + 3 * ny*j + 1]) / 255.0;
	float b = int(data[3 * i + 3 * ny*j + 2]) / 255.0;
	return Vector(r, g, b);

}
void get_sphere_uv(const Vector&p, float& u, float&v)
{
	float phi = atan2(p.z, p.x);
	float theta = asin(p.y);
	u = 1 - (phi + PI) / (2 * PI);
	v = (theta + PI / 2)/PI;
}