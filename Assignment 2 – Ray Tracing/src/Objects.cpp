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
	if (text != NULL)
	{
		Vector Color;
		if(text->type==1)
			Color = lightSample.EL * text->value(0, 0, position);
		else if (text->type == 2)
		{
			float u, v;
			get_sphere_uv(normal, u, v);
			Color = lightSample.EL * text->value(u, v, normal);
		}
		return Vector(min(1, Color.x), min(1, Color.y), min(1, Color.z));
	}
	////Blinn-Phong
	//h = lightSample.L - ray.direction;
	//h = h.normalize();
	//nDotH = normal.dot(h);
	//specularTerm = specular * pow(max(nDotH, 0), shininess);

	//Phong
	Vector R = 2.0*(normal.dot(lightSample.L))*normal - lightSample.L;
	R = R.normalize();
	float vDotR = -1 * ray.direction.dot(R);
	specularTerm = specular * pow(vDotR, shininess);

	//Lambertian shading
	nDotL = normal.dot(lightSample.L);
	diffuseTerm = diffuse * max(nDotL, 0.0);
	diffuseTerm = (diffuseTerm + specularTerm)*lightSample.EL;
	return Vector(min(1, diffuseTerm.x), min(1, diffuseTerm.y), min(1, diffuseTerm.z));
}
//Vector TextureMaterial::sample(const Light& light, const Ray& ray, const Vector& position, const Vector& normal)const
//{
//	LightSample lightSample;
//	Vector Color;
//	lightSample = light.sample(position);//Position means hit position
//	if (lightSample.EL == Vector::zero())
//	{
//		//in the shadow
//		return Vector::zero();//
//	}
//	//texture
//	if (this->text != NULL)
//		Color = lightSample.EL * text->value(0, 0, position);
//	return Vector(min(1, Color.x), min(1, Color.y), min(1, Color.z));
//};
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

bool Sphere::bounding_box(float t0, float t1, aabb& box)const
{
	box = aabb(center - Vector(radius, radius, radius), center + Vector(radius, radius, radius));
	return true;
}
bool  Sphere:: intersect(const Ray& ray, IntersectResult & hit) const//, float& tmin, float& tmax
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
				hit = IntersectResult(this, d1, p, n);
				return true;
			}
			else
			{
				Vector p = ray.getPoint(d2);
				Vector n = (p - center).normalize();
				hit = IntersectResult(this, d2, p, n);
				return true;
			}
			
		}
	}
	return false;
}
bool moving_sphere::bounding_box(float t0, float t1, aabb& box)const
{
	Vector c0 = Center(t0);
	Vector c1 = Center(t1);
	Vector center0 = Vector(min(c0.x, c1.x), min(c0.y, c1.y), min(c0.z, c1.z));
	Vector center1 = Vector(max(c0.x, c1.x), max(c0.y, c1.y), max(c0.z, c1.z));
	box = aabb(center0 - Vector(radius, radius, radius), center1 + Vector(radius, radius, radius));
	return true;
}
bool  moving_sphere::intersect(const Ray& ray, IntersectResult&hit) const//, float& tmin, float& tmax
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
				hit= IntersectResult(this, d1, p, n);
				return true;
			}
			else
			{
				Vector p = ray.getPoint(d2);
				Vector n = (p - Center(ray.time)).normalize();
				hit=IntersectResult(this, d2, p, n);
				return true;
			}

		}
	}
	return false;
}

void TriangleMesh::addVertex(Vector *vertx)
{
	vertices.push_back(vertx);
}
void TriangleMesh::addNormal(Vector *normal)
{
	normals.push_back(normal);
}
//bool Triangle::raytracingIntersect(const Ray& ray, float &u, float &v,float& t, bool& IsCounterclockwise)const
//{
//	float epsino = 1E-8;
//	// compute plane's normal
//	Vector v0v1 = V1 - V0;
//	Vector v0v2 = V2 - V0;
//	// no need to normalize
//	Vector N = v0v1.cross(v0v2); // N 
//	float NdotRayDirection = N.dot(ray.direction);
//	//ensure the normal is always directing to camera
//	if (NdotRayDirection > 0)
//	{
//		IsCounterclockwise = false;
//	}
//	else
//		IsCounterclockwise = true;
//	float totalArea2 = N.sqrLength();
//	// Step 1: finding P
//	// check if ray and plane are parallel ?
//	if (fabs(NdotRayDirection) < epsino) // almost 0 
//		return false; // they are parallel so they don't intersect ! 
//
//	// compute d parameter 
//	float d =  N.dot(V0);//-1 *
//
//	// compute t 
//	t = (N.dot(ray.origin) + d) / NdotRayDirection;//-1 * 
//	// check if the triangle is in behind the ray
//	if (t < 0) return false; // the triangle is behind 
//
//	// compute the intersection point using equation 1
//	Vector P = ray.getPoint(t);
//
//	// Step 2: inside-outside test
//	Vector C; // vector perpendicular to triangle's plane 
//
//	// edge 0
//	Vector edge0 = V1 - V0;
//	Vector vp0 = P - V0;
//	C = edge0.cross(vp0);
//	if (N.dot(C) < 0) return false; // P is on the right side 
//
//	// edge 1
//	Vector edge1 = V2 - V1;
//	Vector vp1 = P - V1;
//	C = edge1.cross(vp1);
//
//	if (u = N.dot(C) < 0)  return false; // P is on the right side 
//
//	// edge 2
//	Vector edge2 = V0 - V2;
//	Vector vp2 = P - V2;
//	C = edge2.cross(vp2);
//	if (v = N.dot(C) < 0) return false; // P is on the right side; 
//
//	u = u / totalArea2;
//	u = v / totalArea2;
//	return true; // this ray hits the triangle 
//}
bool Triangle::raytracingIntersect(const Ray& ray, float &u, float &v, float& t)const
{
	double kEpsilon = 1E-9;//0
	double Eplison = 1E-6;
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
bool Triangle::bounding_box(float t0, float t1, aabb& box)const
{
	Vector _min, _max;
	_min.x = min(min(V0.x, V1.x), V2.x);
	_max.x = max(max(V0.x, V1.x), V2.x);
	if (_max.x == _min.x)
		_max.x == _min.x + 1E-3;
	_min.y = min(min(V0.y, V1.y), V2.y);
	_max.y = max(max(V0.y, V1.y), V2.y);
	if (_max.y == _min.y)
		_max.y == _min.y + 1E-3;
	_min.z = min(min(V0.z, V1.z), V2.z);
	_max.z = max(max(V0.z, V1.z), V2.z);
	if (_max.x == _min.x)
		_max.x == _min.x + 1E-3;
	box = aabb(_min, _max);
	return true;
}
bool  Triangle::intersect(const Ray& ray, IntersectResult& hit)const  //, float& tmin, float& tmax
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
		hit = IntersectResult(this, t, ray.getPoint(t), hitNormal.normalize()); // this ray hits the triangle 
		return true;
	}
	else
	{
		return false;
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
	if (time1 - time0 != 0)
	{
		srand(time(0));
		float time = time0 + (rand() % 100 / (double)101) *(time1 - time0);
		return Ray(this->eye, r, time);
	}
	else
		return Ray(this->eye, r);
	
}
//Ray MotionCamera::generateRay(float x, float y, float ratio)const
//{
//	srand(time(0));
//	Vector r, u;
//	float tranScle = ratio * fovScale; 
//	r = right * (x - 0.5)*tranScle;
//	u = up * (y - 0.5)*fovScale;
//	r = front + r + u;
//	r = r.normalize();
//	//just add time to ray
//	float time = time0 + (rand() % 100 / (double)101) *(time1 - time0);
//	return Ray(this->eye, r, time);
//}

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
	bool hit_anything = false;
	if (isBVH)
	{
		hit_anything = BVH->intersect(ray, result);
	}
	else
	{
		//#pragma omp parallel for
		for (int i = 0; i < objects.size(); ++i)
		{
			//temp = objects[i]->intersect(ray);
		/*for (auto it =  objects.begin(); it != objects.end(); ++it)
		{

			temp = (*it)->intersect(ray);*/
			//#pragma omp critical//critical section
			{
				if (objects[i]->intersect(ray, temp) && (temp.distance < result.distance))//show the lastest object
					result = temp;
			}
		}
		
	}
	return result;
}

bool aabb::hit(const Ray& ray)const//, float& tmin, float& tmax
{
	float tmin, tmax;
	Vector invD(1.0f / ray.direction.x, 1.0f / ray.direction.y,1.0f / ray.direction.z);
	Vector t0 = (_min - ray.origin)*invD;
	Vector t1 = (_max - ray.origin)*invD;
	if (invD.x < 0.0f)
		std::swap(t0.x, t1.x);
	tmin = t0.x; tmax = t1.x;
	if (invD.y < 0.0f)
		std::swap(t0.y, t1.y);
	tmin = t0.y > tmin ? t0.y : tmin;
	tmax = t1.y < tmax ? t1.y : tmax;
	if (tmax < tmin)//no intersection
		return false;
	if (invD.z < 0.0f)
		std::swap(t0.z, t1.z);
	tmin = t0.z > tmin ? t0.z : tmin;
	tmax = t1.z < tmax ? t1.z : tmax;
	/*tmin = max(max(t0.x, t0.y), t0.z);
	tmax = min(min(t1.x, t1.y), t1.z);*/
	if (tmax < tmin)
		return false;
	return true;	
}


//constructor
int box_x_compare(const void*a, const void* b)
{
	aabb box_left, box_right;
	Geometry* ah = *(Geometry**)a;
	Geometry* bh = *(Geometry**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		cerr << "no bounding box in the BVH_node constructor!" << endl;
	if (box_left._min.x < box_right._min.x)
		return -1;
	else
		return 1;
}
int box_y_compare(const void*a, const void* b)
{
	aabb box_left, box_right;
	Geometry* ah = *(Geometry**)a;
	Geometry* bh = *(Geometry**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		cerr << "no bounding box in the BVH_node constructor!" << endl;
	if (box_left._min.y < box_right._min.y)
		return -1;
	else
		return 1;
}
int box_z_compare(const void*a, const void* b)
{
	aabb box_left, box_right;
	Geometry* ah = *(Geometry**)a;
	Geometry* bh = *(Geometry**)b;
	if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
		cerr << "no bounding box in the BVH_node constructor!" << endl;
	if (box_left._min.z < box_right._min.z)
		return -1;
	else
		return 1;
}
aabb BVH_node::surrounding_box(aabb box0, aabb box1)
{
	//merge box
	Vector small(min(box0._min.x, box1._min.x), min(box0._min.y, box1._min.y), min(box0._min.z, box1._min.z));
	Vector big(max(box0._max.x, box1._max.x), max(box0._max.y, box1._max.y), max(box0._max.z, box1._max.z));
	return aabb(small, big);
}
BVH_node::BVH_node(Geometry** l, int n, float time0, float time1)
{
	Vector interval = box._max - box._min;
	//split the large one
	if (interval.x>=interval.y&&interval.x>=interval.z)
		qsort(l, n, sizeof(Geometry*), box_x_compare);
	else if(interval.y >= interval.x&&interval.y >= interval.z)
		qsort(l, n, sizeof(Geometry*), box_y_compare);
	else
		qsort(l, n, sizeof(Geometry*), box_z_compare);
	if (n == 1)
		left = right = l[0];
	else if (n == 2)
	{
		left = l[0];
		right = l[1];
	}
	else
	{
		left = new BVH_node(l, n / 2, time0,time1);
		right = new BVH_node(l + n / 2, n - n / 2, time0, time1);
	}
	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
		cerr << "no bounding box in BVH_node contructor" << endl;
	box = surrounding_box(box_left, box_right);
	//for debugging
	/*cout << box_left._min.x <<" "<< box_left._min.y << " " << box_left._min.z << endl;
	cout << box_left._max.x << " " << box_left._max.y << " " << box_left._max.z << endl;
	cout << box_right._min.x << " " << box_right._min.y << " " << box_right._min.z << endl;
	cout << box_right._max.x << " " << box_right._max.y << " " << box_right._max.z << endl;*/
}

bool BVH_node::bounding_box(float t0, float t1, aabb& b)const
{
	b = box;
	return true;
}

bool BVH_node::intersect(const Ray& ray, IntersectResult& hit)const//, float& tmin, float& tmax
{
	if (box.hit(ray))
	{
		IntersectResult left_Res, right_Res;
		bool hit_left = left->intersect(ray, left_Res);
		bool hit_right = right->intersect(ray, right_Res);
		if (hit_left&&hit_right)
		{
			if (left_Res.distance < right_Res.distance)
				hit = left_Res;
			else
				hit = right_Res;
			return true;
		}
		else if (hit_left)
		{
			hit = left_Res;
			return true;
		}
		else if (hit_right)
		{
			hit = right_Res;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}