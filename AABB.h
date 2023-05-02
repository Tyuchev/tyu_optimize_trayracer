
#include <vector>

#include "sphere.h"
#include "vec3.h"




class AABB
{
public:

	//AABB() = delete;

	AABB(std::vector<Sphere> spheres)
		:containedObjects(spheres)
	{
		updateBounds();
	}


	void updateBounds()
	{
		vec3 lower{FLT_MAX, FLT_MAX, FLT_MAX };
		vec3 upper{ -FLT_MAX, -FLT_MAX, -FLT_MAX };;
		for (auto& const object : containedObjects)
		{
			float upperX = object.center.x + object.radius;
			float lowerX = object.center.x - object.radius;
			float upperY = object.center.y + object.radius;
			float lowerY = object.center.y - object.radius;
			float upperZ = object.center.z + object.radius;
			float lowerZ = object.center.z - object.radius;

			if (upperX > upper.x)
			{
				upper.x = upperX;
			}
			if (lowerX > lower.x)
			{
				lower.x = lowerX;
			}
			if (upperY > upper.y)
			{
				upper.y = upperY;
			}
			if (lowerY > lower.y)
			{
				lower.y = lowerY;
			}
			if (upperZ > upper.z)
			{
				upper.z = upperZ;
			}
			if (lowerZ > lower.z)
			{
				lower.z = lowerZ;
			}
		}

		this->upperLimit = upper;
		this->lowerLimit = lower;
	}


private:

	std::vector<Sphere> containedObjects;
	vec3 upperLimit;
	vec3 lowerLimit;


};