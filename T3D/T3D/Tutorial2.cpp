#include "Tutorial2.h"
#include "PerfLogTask.h"
#include "DiagMessageTask.h"
#include "Camera.h"
#include "Sphere.h"
#include "KeyboardController.h"
#include "Math.h"

namespace T3D {

	Tutorial2::Tutorial2()
	{
	}


	Tutorial2::~Tutorial2()
	{
	}

	bool Tutorial2::init() {
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


		const char* sphere_name = "Sphere";
		
		//the sphere is positioned at the origin;
		Vector3 point(0.0, 0.0, 0.0);

		//create sphere object
		GameObject *sphere = new GameObject(this);
		sphere->setMesh(new Sphere(0.5, 8));

		sphere->setMaterial(red); //thankfully the renderer owns the material. (Why isn't this the case for Meshes?)
		sphere->getTransform()->setLocalPosition(point);
		sphere->getTransform()->setParent(root);
		sphere->getTransform()->name = sphere_name;
	
		return true;
	}
}