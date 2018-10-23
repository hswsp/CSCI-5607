#include"Objects.h"
#include<time.h>
Vector Ray::getPoint(float t) const
{
	return origin + t * direction;
}

Vector PhongMaterial::sample(const Light& light, const Ray& ray, const Vector& position, const Vector& normal)const
{
	
	float nDotL, nDotH;
	Vector h, diffuseTerm, specularTerm;
	LightSample lightSample;	
	lightSample = light.sample(position);//Position means hit position
	if (lightSample.EL == Vector::zero())
	{
		//in the shadow
		return Vector::zero();//
	}
	//Blinn-Phong
	/*h = lightSample.L - ray.direction;
	h = h.normalize();
	nDotH = normal.dot(h);*/
	//Phong
	Vector R = 2.0*(normal.dot(lightSample.L))*normal - lightSample.L;
	R = R.normalize();
	float vDotR = -1 * ray.direction.dot(R);
	specularTerm = specular * pow(vDotR, shininess);

	//Lambertian shading
	nDotL = normal.dot(lightSample.L);
	diffuseTerm = diffuse * max(nDotL, 0.0);

	////Blinn-Phong
	//specularTerm = specular * pow(max(nDotH, 0), shininess);

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
		result.EL = Vector::zero();//Vector(10E-3, 10E-3, 10E-3)
	else
		result.EL = irradiance / (distance*distance);//()
	result.L = direction;
	return result;
}
LightSample SpotLight::sample(const Vector& position)const
{
	//Position means hit position
	IntersectResult shadowResult;
	LightSample result;
	Vector direction(this->position - position);
	float distance = direction.length();
	direction = direction.normalize();
	double current_angle =180.f*acos(fabs(this->Litdirection.dot(direction)))/ PI;
	shadowResult = scene.intersect(Ray(position, direction));
	if (shadowResult.geometry != NULL && shadowResult.distance < distance)
		result.EL = Vector::zero();
	else
	{
		if (current_angle >= this->angle2)
			result.EL = Vector(0, 0, 0);
		else if (current_angle <= this->angle1)//like point_light
		{
				result.EL = irradiance / (distance*distance);
		}
		else//between angle: fall off smoothly
		{
			//linear
			result.EL = irradiance*(this->angle2 - current_angle) / ((this->angle2- this->angle1)*distance*distance);
			//result.EL = irradiance/distance;
		}
	}
	result.L = direction;
	return result;
}


IntersectResult Sphere:: intersect(const Ray& ray) const
{
	//make sure direction vector is normalized, the |a|=1;
	float epsino = 10E-3; //
	Vector v = ray.origin - center;//P_0-C
	float a0 = v.sqrLength() - (sqrRadius+ epsino);//c
	float DdotV = ray.direction.dot(v);//b/2
	if (DdotV <= 0.0) //ensure t>0
	{
		float discr = DdotV * DdotV - a0;
		if (discr >= 0.0) 
		{
			float d1 = -DdotV - sqrt(discr);//intersection distance
			float d2 = -DdotV + sqrt(discr);
			if (d1 > 0)//only use the smallest t which s.t t>0
			{
				Vector p = ray.getPoint(d1);
				Vector n = (p - center).normalize();
				return IntersectResult(this, d1, p, n);
			}
			else
			{
				Vector p = ray.getPoint(d2);
				Vector n = (p - center).normalize();
				return IntersectResult(this, d2, p, n);
			}
			
		}
	}
	return IntersectResult();
}
IntersectResult moving_sphere::intersect(const Ray& ray) const
{
	//repalce center with Center(time)
	float epsino = 10E-3; //
	Vector v = ray.origin - Center(ray.time);//P_0-C
	float a0 = v.sqrLength() - (sqrRadius + epsino);//c
	float DdotV = ray.direction.dot(v);//b/2
	if (DdotV <= 0.0) //ensure t>0
	{
		float discr = DdotV * DdotV - a0;
		if (discr >= 0.0)
		{
			float d1 = -DdotV - sqrt(discr);//intersection distance
			float d2 = -DdotV + sqrt(discr);
			if (d1 > 0)//only use the smallest t which s.t t>0
			{
				Vector p = ray.getPoint(d1);
				Vector n = (p - Center(ray.time)).normalize();
				return IntersectResult(this, d1, p, n);
			}
			else
			{
				Vector p = ray.getPoint(d2);
				Vector n = (p - Center(ray.time)).normalize();
				return IntersectResult(this, d2, p, n);
			}

		}
	}
	return IntersectResult();
}

void TriangleMesh::addVertex(Vector *vertx)
{
	vertices.push_back(vertx);
}
void TriangleMesh::addNormal(Vector *normal)
{
	normals.push_back(normal);
}
bool Triangle::raytracingIntersect(const Ray& ray, float &u, float &v,float& t, bool& IsCounterclockwise)const
{
	float epsino = 1E-8;
	// compute plane's normal
	Vector v0v1 = V1 - V0;
	Vector v0v2 = V2 - V0;
	// no need to normalize
	Vector N = v0v1.cross(v0v2); // N 
	float NdotRayDirection = N.dot(ray.direction);
	//ensure the normal is always directing to camera
	if (NdotRayDirection > 0)
	{
		IsCounterclockwise = false;
	}
	else
		IsCounterclockwise = true;
	float totalArea2 = N.sqrLength();
	// Step 1: finding P
	// check if ray and plane are parallel ?
	if (fabs(NdotRayDirection) < epsino) // almost 0 
		return false; // they are parallel so they don't intersect ! 

	// compute d parameter 
	float d =  N.dot(V0);//-1 *

	// compute t 
	t = (N.dot(ray.origin) + d) / NdotRayDirection;//-1 * 
	// check if the triangle is in behind the ray
	if (t < 0) return false; // the triangle is behind 

	// compute the intersection point using equation 1
	Vector P = ray.getPoint(t);

	// Step 2: inside-outside test
	Vector C; // vector perpendicular to triangle's plane 

	// edge 0
	Vector edge0 = V1 - V0;
	Vector vp0 = P - V0;
	C = edge0.cross(vp0);
	if (N.dot(C) < 0) return false; // P is on the right side 

	// edge 1
	Vector edge1 = V2 - V1;
	Vector vp1 = P - V1;
	C = edge1.cross(vp1);

	if (u = N.dot(C) < 0)  return false; // P is on the right side 

	// edge 2
	Vector edge2 = V0 - V2;
	Vector vp2 = P - V2;
	C = edge2.cross(vp2);
	if (v = N.dot(C) < 0) return false; // P is on the right side; 

	u = u / totalArea2;
	u = v / totalArea2;
	return true; // this ray hits the triangle 
}
bool Triangle::raytracingIntersect(const Ray& ray, float &u, float &v, float& t)const
{
	double kEpsilon = 1E-9;
	double Eplison = 1E-1;
	Vector v0v1 = V1 - V0;
	Vector v0v2 = V2 - V0;
	//Vector N = v0v1.cross(v0v2).normalize();
	////make sure the counter clockwise
	//if (V0.dot(N) > 0)
	//{
	//	//swich E1 and E2
	//	Vector temp = v0v1;
	//	v0v1 = v0v2;
	//	v0v2 = temp;
	//}
	Vector dir = ray.direction;
	Vector pvec = dir.cross(v0v2);
	double det = v0v1.dot(pvec);
	// ray and triangle are parallel if det is close to 0
	
	if (fabs(det) < kEpsilon) return false; // if use fabs(), that is for two-side triangle

	float invDet = 1 / det;

	Vector tvec = ray.origin - V0;
	u = tvec.dot(pvec) * invDet;
	if (u < 0 || u > 1.0) return false;

	Vector qvec = tvec.cross(v0v1);
	v = dir.dot(qvec) * invDet;
	if (v < 0 || u + v > 1.0) return false;

	t = v0v2.dot(qvec) * invDet;

	return (t > Eplison) ? true : false;
}
IntersectResult Triangle::intersect(const Ray& ray)const  //,Vector & uv
{
	
	float u, v, t;
	Vector  uv;
	bool IsCounterclockwise;
	//float epsino = 10E-4;
	//// compute plane's normal
	//Vector v0v1 = V1 - V0;
	//Vector v0v2 = V2 - V0;
	//// no need to normalize
	//Vector N = v0v1.cross(v0v2); // N 
	//float totalArea2 = N.sqrLength();
	//// Step 1: finding P
	//// check if ray and plane are parallel ?
	//float NdotRayDirection = N.dot(ray.direction);
	//if (fabs(NdotRayDirection) < epsino) // almost 0 
	//	return IntersectResult(); // they are parallel so they don't intersect ! 
	//// compute d parameter 
	//float d = -1 * N.dot(V0);//
	//// compute t 
	//float t = -1 * (N.dot(ray.origin) + d) / NdotRayDirection;//
	//// check if the triangle is in behind the ray
	//if (t < 0) return IntersectResult(); // the triangle is behind 
	//// compute the intersection point using equation 1
	//Vector P = ray.getPoint(t);
	//// Step 2: inside-outside test
	//Vector C; // vector perpendicular to triangle's plane 
	//// edge 0
	//Vector edge0 = V1 - V0;
	//Vector vp0 = P - V0;
	//C = edge0.cross(vp0);
	//if (N.dot(C) < 0) return IntersectResult(); // P is on the right side 
	//// edge 1
	//Vector edge1 = V2 - V1;
	//Vector vp1 = P - V1;
	//C = edge1.cross(vp1);
	//if (u = N.dot(C) < 0)  return IntersectResult(); // P is on the right side 
	//// edge 2
	//Vector edge2 = V0 - V2;
	//Vector vp2 = P - V2;
	//C = edge2.cross(vp2);
	//if (v = N.dot(C) < 0) return IntersectResult(); // P is on the right side; 
	if (raytracingIntersect(ray, u, v, t))//, IsCounterclockwise
	{
		uv.x = u;
		uv.y = v;
		uv.z = 1 - uv.x - uv.y;
		Vector hitNormal;
		if (name == FACE_NORMAL)
		{   //face normal
			hitNormal = (V1 - V0).cross(V2 - V0);
			/*if (!IsCounterclockwise)
				hitNormal = -1 * hitNormal;*/
			if (ray.direction.dot(hitNormal) > 0)
				hitNormal = -1 * hitNormal;
		}
		else if (name == SMOOTH_NORMAL)
		{   //interpolate
			hitNormal = uv.z * N0 + uv.x * N1 + uv.y * N2;
		}
		return IntersectResult(this, t, ray.getPoint(t), hitNormal.normalize()); // this ray hits the triangle 
	}
	else
	{
		return IntersectResult();
	}
}

Ray Camera::generateRay(float x, float y, float ratio)const
{
	Vector r, u;
	float tranScle = ratio * fovScale; // 
	//o = front - front.length()*tranScle/2*right.normalize();
	/*r = front.length()*right.normalize()*tranScle * (x-0.5);
	u = -(y-0.5)*front.length()*fovScale*up.normalize();*/
	r = right * (x - 0.5)*tranScle;
	u = up * (y - 0.5)*fovScale;//
	r = front + r + u;
	r = r.normalize();
	return Ray(this->eye, r);
}
Ray MotionCamera::generateRay(float x, float y, float ratio)const
{
	srand(time(0));
	Vector r, u;
	float tranScle = ratio * fovScale; 
	r = right * (x - 0.5)*tranScle;
	u = up * (y - 0.5)*fovScale;
	r = front + r + u;
	r = r.normalize();
	//just add time to ray
	float time = time0 + (rand() % 100 / (double)101) *(time1 - time0);
	return Ray(this->eye, r, time);
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
//#pragma omp parallel for
	for (int i = 0; i < objects.size(); ++i)
	{
		temp = objects[i]->intersect(ray);
	/*for (auto it =  objects.begin(); it != objects.end(); ++it)
	{
		temp = (*it)->intersect(ray);*/
//#pragma omp critical//critical section
		{
			if ((temp.geometry != NULL) && (temp.distance < result.distance))//show the lastest object
				result = temp;
		}
	}
	return result;
}