#include"Objects.h"

Vector Ray::getPoint(float t) const
{
	return origin + t * direction;
}


IntersectResult Sphere:: intersect(const Ray& ray) const
{
	//make sure direction vector is normalized, the |a|=1;
	Vector v = ray.origin - center;//P_0-C
	float a0 = v.sqrLength() - sqrRadius;//c
	float DdotV = ray.direction.dot(v);//b/2
	if (DdotV <= 0.0) //ensure t>0
	{
		float discr = DdotV * DdotV - a0;
		if (discr >= 0.0) 
		{
			float d = -DdotV - sqrt(discr);//intersection distance
			Vector p = ray.getPoint(d);
			Vector n = (p - center).normalize();
			return IntersectResult(this, d, p, n);
		}
	}
	return IntersectResult();
}

Ray Camera::generateRay(float x, float y, float ratio)const
{
	Vector r, u;
	float tranScle = ratio * fovScale;
	//o = front - front.length()*tranScle/2*right.normalize();
	/*r = front.length()*right.normalize()*tranScle * (x-0.5);
	u = -(y-0.5)*front.length()*fovScale*up.normalize();*/
	r = right * (x - 0.5)*fovScale*ratio;
	u = up * (y - 0.5)*fovScale;//
	r = front + r + u;
	r = r.normalize();
	return Ray(this->eye, r);
}

void Scene::addObject(Geometry* obj)
{
	objects.push_back(obj);
}

IntersectResult Scene::intersect(const Ray& ray)const
{
	IntersectResult temp, result;
	result.geometry = NULL;
	result.distance = 1.0E30;
	//vector<Geometry*>::const_iterator  it;
	for (auto it =  objects.begin(); it != objects.end(); ++it)
	{
		temp = (*it)->intersect(ray);
		if ((temp.geometry != NULL) && (temp.distance < result.distance))//show the lastest object
			result = temp;
	}
	return result;
}