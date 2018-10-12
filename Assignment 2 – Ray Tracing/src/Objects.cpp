#include"Objects.h"

Vector Ray::getPoint(float t) const
{
	return origin + t * direction;
}

Vector PhongMaterial::sample(const Light& light, const Ray& ray, const Vector& position, const Vector& normal)const
{
	
	float nDotL, nDotH;
	Vector h, diffuseTerm, specularTerm;
	LightSample lightSample;	
	//Blinn-Phong
	lightSample = light.sample(position);//Position means hit position
	nDotL = normal.dot(lightSample.L);
	if (lightSample.EL == Vector::zero())
	{
		//in the shadow
		return Vector::zero();//
	}
	h = lightSample.L - ray.direction;
	h = h.normalize();
	nDotH = normal.dot(h);
	//Lambertian shading
	diffuseTerm = diffuse * max(nDotL, 0.0);
	specularTerm = specular * pow(max(nDotH, 0), shininess);//Blinn-Phong
	diffuseTerm = (diffuseTerm + specularTerm)*lightSample.EL;//  
	return Vector(min(1, diffuseTerm.x), min(1, diffuseTerm.y), min(1, diffuseTerm.z));
}
//Different Light
LightSample DirectionLight::sample(const Vector& position)const
{
	IntersectResult shadowResult;
	LightSample result;
	shadowResult = scene.intersect(Ray(position, direction));
	//add shadow terms,if ray is blocked s=0
	if (shadowResult.geometry == NULL)
		result.EL = irradiance;
	else
		result.EL = Vector(0, 0, 0);//10E-3,10E-3,10E-3
	result.L = direction;
	return result;
}
LightSample PointLight::sample(const Vector& position)const
{
	//Position means hit position
	IntersectResult shadowResult;
	LightSample result;
	Vector direction(this->position - position);
	float distance = direction.length();
	direction = direction.normalize();
	shadowResult = scene.intersect(Ray(position, direction));
	//add shadow terms,if ray is blocked s=0
	if (shadowResult.geometry != NULL && shadowResult.distance < distance)//
		result.EL = Vector(0.0, 0.0, 0.0);//Vector(10E-3, 10E-3, 10E-3)
	else
		result.EL = irradiance / (distance);//(distance*)
	result.L = direction;
	return result;
}
//LightSample AmbientLight::sample(const Vector& position)const
//{
//	IntersectResult shadowResult;
//	LightSample result;
//	result.EL = irradiance;
//	result.L = Vector::zero();
//	return result;
//}

IntersectResult Sphere:: intersect(const Ray& ray) const
{
	//make sure direction vector is normalized, the |a|=1;
	float epsino = 10E-4;
	Vector v = ray.origin - center;//P_0-C
	float a0 = v.sqrLength() - (sqrRadius+ epsino);//c
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
void Scene::addLights(Light* lit)
{
	lights.push_back(lit);
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