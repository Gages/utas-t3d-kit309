#include "Assignment2.h"
#include "Camera.h"
#include "Sphere.h"
#include "SphereData.h"
#include "RotateBehaviour.h"
#include "Assign2LogTask.h"
//#include "KeyboardController.h"
//#include "PerfLogTask.h"

using namespace T3D;

	Assignment2::Assignment2()
	{
	}


	Assignment2::~Assignment2()
	{
	}

	bool Assignment2::init() {
		if (!super::init()) return false;

		addTask(new Assign2LogTask(this));
		//addTask(new DiagMessageTask(this, "Press F9 for help",
		//	2, 32, true, 5.0));

		//Create a camera
		GameObject *camObj = new GameObject(this);
		renderer->camera = new Camera(Camera::PERSPECTIVE, 0.1, 500, 45.0, 1.6);
		camObj->getTransform()->setLocalPosition(Vector3(0, 0, 0));
		camObj->getTransform()->setLocalRotation(Vector3(0, 0, 0));
		camObj->setCamera(renderer->camera);
		camObj->getTransform()->setParent(root);

		//Add a rotate behaviour to the camera object
		//rotate the camera about the y axis, one revolution every five seconds
		camObj->addComponent(new RotateBehaviour(Vector3(0, Math::DEG2RAD * 360.0 / 5.0, 0)));
		
		//Add a controller to the camera object
		//camObj->addComponent(new KeyboardController());
		
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


		auto create_material = [this](uint32_t hexcolour) -> Material* {
			Material *mat = this->getRenderer()->createMaterial(Renderer::PR_OPAQUE);
			mat->setDiffuse(
				((0xFF0000 & hexcolour) >> 16) / 256.0,
				((0x00FF00 & hexcolour) >> 8) / 256.0,
				(0x0000FF & hexcolour) / 256.0,
				1.0f
				);
			return mat;
		};

		/*Materials*/
		Material* color[] = {
			create_material(0x27AE60), //green
			create_material(0xB03A2E), //red
			create_material(0xF4D03F), //yellow
			create_material(0x873600), //brown
			create_material(0x3498DB), //blue
			create_material(0x808B96), //grey
			create_material(0x8E44AD), //purple
			create_material(0xD35400), //orange
			create_material(0xD0D3D4), //white
		};
		
		Transform* node_at[NUMNODES] = {};
		node_at[0] = root;

		for (auto& sphere : SphereData_Array) {

			if (sphere.index == 0) continue;

			auto *o = new GameObject(this);
			
			node_at[sphere.index] = o->getTransform();
			o->getTransform()->setParent(node_at[sphere.parent]);
			o->getTransform()->setWorldPosition(Vector3(sphere.x, sphere.y, sphere.z) * 500);
			if (sphere.code == LEAF) {
				o->setMesh(new Sphere(0.5));
				o->setMaterial(color[8]);
				o->getTransform()->name = "Sphere";
			}
			else {
				o->getTransform()->name = "TreeNode";
				//o->addComponent(new RotateBehaviour(Vector3(1, 1, 1).normalised()  / sphere.depth));
			}
		    
			
			
			
		}
	
		return true;

	}
