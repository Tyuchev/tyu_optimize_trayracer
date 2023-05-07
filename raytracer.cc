#include "raytracer.h"
#include <random>

#include <chrono>
#include <iostream>
#include <memory>
#include <algorithm>
#include <thread>
#include <execution>
#include <vector>

#include "sphere.h"
#include "random.h"
#include "mat4.h"




//------------------------------------------------------------------------------
/**
*/
Raytracer::Raytracer(unsigned w, unsigned h, std::vector<Color>& frameBuffer, unsigned rpp, unsigned bounces,
	std::vector<Sphere> const& objects) :
	frameBuffer(frameBuffer),
	rpp(rpp),
	bounces(bounces),
	width(w),
	height(h),
	worldObjects(objects)
{
	frameBufferWidthIt.resize(w);
	frameBufferHeightIt.resize(h);
	frameBufferRppIt.resize(rpp);

	randomVec.resize(w * h * rpp);
	randomVecIt.resize(w * h * rpp);

	for (int x = 0; x < w; x++)
	{
		frameBufferWidthIt[x] = x;
	}
	for (int y = 0; y < h; y++)
	{
		frameBufferHeightIt[y] = y;
	}
	for (int r = 0; r < rpp; r++)
	{
		frameBufferRppIt[r] = r;
	}

	for (int rit = 0; rit < w * h * rpp; rit++)
	{
		randomVecIt[rit] = rit;
	}
}


//------------------------------------------------------------------------------
/**
*/
void
Raytracer::Raytrace()
{
	//std::thread::hardware_concurrency();

	//Calculate all random values
	//Maybe there is a way to alter the values each frame a bit, instead of regenerating new random values
	//If it was the same operation it could be SIMD'd


	std::for_each(std::execution::par, randomVecIt.begin(), randomVecIt.end(),
		[this](int i)
		{
			randomVec[i] = randomGen.RandomFloat();
		});

	float widthInv = 1.0f / this->width;
	float heightInv = 1.0f / this->height;

	std::for_each(std::execution::par, frameBufferWidthIt.begin(), frameBufferWidthIt.end(),
		[this, widthInv, heightInv](int xarray)
		{
			//float pixelRandom = randoms[xarray];
			std::for_each(frameBufferHeightIt.begin(), frameBufferHeightIt.end(),
			[this, xarray, widthInv, heightInv](int yarray)
				{

					Color color;
					std::for_each(frameBufferRppIt.begin(), frameBufferRppIt.end(),
						[this, xarray, yarray, widthInv, heightInv, &color](int i)
						{

							float u = ((float(xarray + randomVec[xarray + i]) * widthInv) * 2.0f) - 1.0f;
							float v = ((float(yarray + randomVec[yarray * this->width + xarray + i]) * heightInv) * 2.0f) - 1.0f;

							// this is where i set camera position and direction
							Ray ray{ vec3{0, 10, 0}, vec3(u, v, 1.0f) };
							color += this->TracePath(ray, 0, randomVec[xarray]);

						});

					// divide by number of samples per pixel, to get the average of the distribution
					color.r /= this->rpp;
					color.g /= this->rpp;
					color.b /= this->rpp;

					// Ideas for making the frame buffer parralel
					this->frameBuffer[yarray * this->width + xarray] += color;
				});

		}); 
}

//------------------------------------------------------------------------------
/**
 * @parameter n - the current bounce level
*/
Color Raytracer::TracePath(Ray& ray, unsigned n, float random)
{
	vec3 hitPoint;
	vec3 hitNormal;
	const Sphere* hitObject = nullptr;

	if (Raycast(ray, hitPoint, hitNormal, hitObject))
	{
		//BSDF overwrites ray's memory
		BSDF(hitObject->material, ray, hitNormal, random);
		if (n < this->bounces)
		{
			Color colorHandle;
			hitObject->GetColor(colorHandle);
			return colorHandle * this->TracePath(ray, n + 1, random);
		}

		if (n == this->bounces)
		{
			return { 0,0,0 };
		}
	}
	else // misses
	{
		float missRay = ray.rayDirection.y * ray.magnitude;
		return this->Skybox(missRay);
	}
}

//------------------------------------------------------------------------------
/**
*/
bool
Raytracer::Raycast(Ray& ray, vec3& hitPoint, vec3& hitNormal, const Sphere*& hitObject)
{
	bool isHit = false;
	HitResult closestHit;
	int numHits = 0;
	float maxDistance = 10000;

	// Set max range for initial hit testing;
	closestHit.t = maxDistance;

	// Only thing maybe needs doing is reducing num of spheres we check against? are some behind us maybe?
	// I think behind culling is already implemented

	for (auto const& object : worldObjects)
	{
		// does raycast hit an object?
		if (object.Intersect(closestHit, ray))
		{
			isHit = true;

			hitPoint = closestHit.p;
			hitNormal = closestHit.normal;
			hitObject = closestHit.sphere;
		}
	}

	//std::for_each(std::execution::par, frameBufferWidthIt.begin(), frameBufferWidthIt.end(),
	//	[this, widthInv, heightInv](int xarray)
	//	{

	return isHit;
}


//------------------------------------------------------------------------------
/**
*/
void
Raytracer::Clear()
{
	for (auto& color : this->frameBuffer)
	{
		color.r = 0.0f;
		color.g = 0.0f;
		color.b = 0.0f;
	}
}


//------------------------------------------------------------------------------
/**
*/
Color
Raytracer::Skybox(float rayY) const
{
	float t = 0.5 * (rayY + 1.0);

	float xColor = 1 - (0.5 * t);
	float yColor = 1 - (0.3 * t);

	return { xColor, yColor, 1.0f };
}




// returns a random point on the surface of a unit sphere
vec3 Raytracer::random_point_on_unit_sphere(float random)
{
	float x = random;
	float y = random;
	float z = random;
	vec3 v(x, y, z);
	return v;
};

//------------------------------------------------------------------------------
/**
*/

bool Raytracer::BSDF(Material const& material, Ray& ray, vec3& normal, float random)
{
	//these should be normalized... output will be between 1 & -1
	float cosTheta = -(ray.rayDirection.dot(normal));

	// If dotpr is 0 or less, then vectors go in opposites
	// cosTheta is *= -1, so, we if RAY and NORMAL are OPPOSITES, cosTheta INCREASES
	// 
	// IF RAY & NORMAL go in SAME direction, THETA will be NEGATIVE

	// Never seems to be hit, but recurring issue with material type comparison
	//assert(material.type != nullptr && "Sphere material MUST always be set!!!!");


	if (material.type != "Dielectric")
	{
		if (material.type == "Lambertian")
		{

			float F0 = 0.04f;
			bool isReflected = false;

			// Simplify Fresnel Schlick
			float F = F0 + ((1.0f - material.roughness) - F0) * pow(2, ((-5.55473f * cosTheta - 6.98316f) * cosTheta));

			if (F < random)
			{
				isReflected = false;
			}


			if (isReflected)
			{
				mat4 basis = basis.TBN(normal);
				// importance sample with brdf specular lobe
				vec3 H = ImportanceSampleGGX_VNDF(random, random, material.roughness, ray.rayDirection, basis);
				ray.rayDirection = ray.rayDirection.reflect(H);
				return true;
			}
			else
			{
				ray.rayDirection = normal + random_point_on_unit_sphere(random);
				return true;
			}
		}
		else
		{
			// F0 for Conductor material types
			//float F0 = 0.95f;
			// 
			// Fresnel Schlick removed, as 99.9% of the time it will just produce F0 with our specified F0

			// Probability that a ray will reflect on a microfacet is 95% in our sim

			// Approximation of a 95% chance of reflection
			if (material.roughness < 0.95f)
			{
				mat4 basis = basis.TBN(normal);
				// importance sample with brdf specular lobe
				vec3 H = ImportanceSampleGGX_VNDF(random, random, material.roughness, ray.rayDirection, basis);
				ray.rayDirection = ray.rayDirection.reflect(H);
				return true;
			}
			else
			{
				ray.rayDirection = normal + random_point_on_unit_sphere(random);
				return true;
			}
		}

	}
	else
	{
		vec3 outwardNormal;
		float niOverNt;
		vec3 refracted;
		float reflect_prob;
		float cosine;
		vec3 rayDir = ray.rayDirection;
		bool isReflected = true;

		if (cosTheta <= 0)
		{
			outwardNormal = -normal;
			niOverNt = material.refractionIndex;
			cosine = cosTheta * niOverNt * (1 / ray.magnitude);
		}
		else
		{
			outwardNormal = normal;
			niOverNt = 1.0 / material.refractionIndex;
			cosine = cosTheta * (1 / ray.magnitude);
		}

		if (Refract(rayDir, outwardNormal, niOverNt, refracted))
		{
			// fresnel reflectance at 0 deg incidence angle
			// 
			// float F0 = powf(material.refractionIndex - 1, 2) / powf(material.refractionIndex + 1, 2);
			// As we only have one refraction index, F0 can be pre-computed
			float F0 = 0.06016f;

			// Simplify Fresnel Schlick
			reflect_prob = F0 + (((1.0f - material.roughness) - F0) * pow(2, ((-5.55473f * cosine - 6.98316f) * cosine)));
			// roughness is already a random float, no need to test against another new randomFloat
			if (reflect_prob < material.roughness)
			{
				isReflected = false;
			}
		}

		if (isReflected)
		{
			ray.rayDirection = rayDir.reflect(normal);
			return true;
		}
		else
		{
			ray.rayDirection = refracted;
			return true;
		}
	}
}