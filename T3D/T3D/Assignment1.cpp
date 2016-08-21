#include "Assignment1.h"
#include "PerfLogTask.h"
#include "DiagMessageTask.h"
#include "Camera.h"
#include "Sphere.h"
#include "KeyboardController.h"
#include "Math.h"
#include "NWayHub.h"
#include "RotateBehaviour.h"

namespace T3D {

	Assignment1::Assignment1()
	{
	}


	Assignment1::~Assignment1()
	{
	}

	bool Assignment1::init() {
		if (!super::init()) return false;

		addTask(new PerfLogTask(this));
		addTask(new DiagMessageTask(this, "Press F9 for help",
			2, 32, true, 5.0));

		//Create a camera
		GameObject *camObj = new GameObject(this);
		renderer->camera = new Camera(Camera::PERSPECTIVE, 0.1, farclip, 45.0, 1.6);
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
		/*
		Create a "octtree" effect
		*/
		
		//Layer		#Objects
		//0			8^0 = 1
		//1			8^1 = 8
		//2			8^2 = 64
		//3			8^3 = 192

		const float sphere_radius = 0.5f;
		const float sphere_spread = sphere_radius * 3.0f;

		//map_index_to_quad
		auto mitq = [](int i) -> Vector3 {
			return Vector3(
				(i % 2) * 2.0 - 1.0,
				((i / 2) % 2) * 2.0 - 1.0,
				((i / 4) % 2) * 2.0 - 1.0
				);
		};
		auto map_index_to_position = [mitq](int i, int j, int k) -> Vector3 {
			return 4.0 * mitq(i) + 2.0 * mitq(j) + mitq(k);
		};

		auto layer0 = new GameObject(this);
		layer0->getTransform()->setParent(root);
		for (int i = 0; i < 8; ++i) {
			auto layer1 = new GameObject(this);
			layer1->getTransform()->setParent(layer0->getTransform());
			for (int j = 0; j < 8; ++j) {
				auto layer2 = new GameObject(this);
				layer2->getTransform()->setParent(layer1->getTransform());
				for (int k = 0; k < 8; ++k) {
					auto layer3 = new GameObject(this);
					layer3->getTransform()->setParent(layer2->getTransform());

					layer3->setMesh(new Sphere(sphere_radius, 8));
					layer3->setMaterial(color[i]);
					layer3->getTransform()->setLocalPosition(map_index_to_position(i,j,k) * sphere_spread);
				}
			}
		}
		
		//rotate the cog diagonally axis
		layer0->addComponent(new RotateBehaviour(Vector3(1, 1, 1).normalised() * 0.05));
			
		//A more complex shape; a "cog" with 8 spokes
		auto *nWayHub = new GameObject(this);
		nWayHub->setMesh(new NWayHub(8, 5, 2.5, 1, 1));
		nWayHub->setMaterial(color[8]);
		nWayHub->getTransform()->setLocalPosition(Vector3(0, 0, -20));
		nWayHub->getTransform()->setParent(root);
		nWayHub->getTransform()->name = "NWayHub";
				
		return true;
	
	}   

}