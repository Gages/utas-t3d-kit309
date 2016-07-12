#include "Tutorial1_Baseline.h"
#include "PerfLogTask.h"
#include "DiagMessageTask.h"
#include "Camera.h"
#include "Sphere.h"
#include "KeyboardController.h"
#include "Math.h"

namespace T3D {

	Tutorial1_Baseline::Tutorial1_Baseline()
	{
	}


	Tutorial1_Baseline::~Tutorial1_Baseline()
	{
	}

	bool Tutorial1_Baseline::init() {
		//Why doesn't T3DTest.cpp check return values?
		if (!WinGLApplication::init()) return false;

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

		
		const double box_size = 5;
		const double sphere_density = 32;
		const char* sphere_name = "Sphere";
		//wasted memory:
		//the GameObject destructor deletes the mesh object,
		//i.e it assumes ownership.
		//this means, even though the sphere is identical and immutable
		//it can not be reused in multiple GameObjects.
		//solution:
		//remove Mesh ownership from the sphere GameObject,
		//mave an object with a greater lifetime manage the Mesh memory.
		//OR
		//use a c++ sharedpointer
		//OR
		//Use some other reference-counted design that involves a bigger refactor?
		
		for (int i = 0; i < 100; i++) {

			//generate a random Vector3 within a box
			Vector3 point(
				Math::randRange(-box_size, box_size),
				Math::randRange(-box_size, box_size),
				Math::randRange(-box_size, box_size));

			//create sphere object
			GameObject *sphere = new GameObject(this);
			sphere->setMesh(new Sphere(0.5, sphere_density));
			
			sphere->setMaterial(red); //thankfully the renderer owns the material. (Why isn't this the case for Meshes?)
			sphere->getTransform()->setLocalPosition(point);
			sphere->getTransform()->setParent(root);
			sphere->getTransform()->name = sphere_name;
		}


		return true;
		
	}
}