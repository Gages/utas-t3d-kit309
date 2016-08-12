#include "Tutorial4.h"
#include "PerfLogTask.h"
#include "DiagMessageTask.h"
#include "Camera.h"
#include "Sphere.h"
#include "KeyboardController.h"
#include "Math.h"

namespace T3D {

	Tutorial4::Tutorial4()
	{
	}


	Tutorial4::~Tutorial4()
	{
	}

	bool Tutorial4::init() {
		if (!super::init()) return false;

		addTask(new PerfLogTask(this));
		addTask(new DiagMessageTask(this, "Press F9 for help",
			2, 32, true, 5.0));

		//Create a camera
		GameObject *camObj = new GameObject(this);
		renderer->camera = new Camera(Camera::PERSPECTIVE, 0.1, 500.0, 45.0, 1.6);
		camObj->getTransform()->setLocalPosition(Vector3(0, 0, 0));
		camObj->getTransform()->setLocalRotation(Vector3(0, 0, 0));
		camObj->setCamera(renderer->camera);
		camObj->getTransform()->setParent(root);

		//Add a controller to the camera object
		camObj->addComponent(new KeyboardController());

		//Add a light
		GameObject *lightObj = new GameObject(this);
		Light *light = new Light(Light::DIRECTIONAL);
		light->setAmbient(1, 1, 1);
		light->setDiffuse(1, 1, 1);
		light->setSpecular(1, 1, 1);
		lightObj->setLight(light);
		lightObj->getTransform()->setLocalRotation(
			Vector3(-45 * Math::DEG2RAD, 70 * Math::DEG2RAD, 0));
		lightObj->getTransform()->setParent(root);

		//Create some basic materials
		Material *red = renderer->createMaterial(Renderer::PR_OPAQUE);
		red->setDiffuse(1, 0, 0, 1);
		
		const char* sphere_name = "Sphere";

		struct spheredef {
			int parent;
			float pos[3];
			float rad;
		};


		//x,y,z, radius, parent_index
		struct spheredef spheres[] = {
				{
					-1, //parent
					{ 1, 0, 0 }, //pos
					0.5, //rad
				},
				{
					0, //parent
					{0,0,0}, //pos
					0.5, //rad
				},
				{
					1, //parent
					{ 0, 1, 0 }, //pos
					0.5, //rad
				},
		};

		const size_t numspheres = sizeof(spheres) / sizeof(spheredef);

		GameObject* sphere_obj[numspheres];

		for (size_t i = 0; i < numspheres; i++)
		{
			sphere_obj[i] = new GameObject(this);
			sphere_obj[i]->setMesh(new Sphere(spheres[i].rad, 8));
			sphere_obj[i]->setMaterial(red); //thankfully the renderer owns the material. (Why isn't this the case for Meshes?)
			sphere_obj[i]->getTransform()->setLocalPosition(Vector3(spheres[i].pos));
			sphere_obj[i]->getTransform()->name = sphere_name;

			int parent_index = spheres[i].parent;
			//Transform* parent = parent_index > 0 ? sphere_obj[parent_index]->getTransform() : root;
			//sphere_obj[i]->getTransform()->setParent(parent);
			sphere_obj[i]->getTransform()->setParent(root);
		}
		
		//	printf("Sphere %f\n", sphere->getTransform()->getBoundingSphere().getRadius());

		return true;
	
	}

}