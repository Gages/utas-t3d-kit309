// =========================================================================================
// KXG363 - Advanced Games Programming, 2012
// =========================================================================================
//
// Author: Robert Ollington
//
// t3dtest.cpp
//
// Example t3dapplication

#include "t3dtest.h"
#include "Cube.h"
#include "PlaneMesh.h"
#include "Sphere.h"
#include "KeyboardController.h"
#include "TerrainFollower.h"
#include "RotateBehaviour.h"
#include "LookAtBehaviour.h"
#include "Terrain.h"
#include "Camera.h"
#include "ParticleEmitter.h"
#include "ParticleBehaviour.h"
#include "PerfLogTask.h"
#include "DiagMessageTask.h"
#include "Material.h"
#include "Animation.h"
#include "Billboard.h"
#include "Math.h"
#include "Sweep.h"
#include "SweepPath.h"
#include "BoundingSphere.h"
#include <assert.h>

static const float TESTMIN = -10;
static const float TESTMAX = 10;

namespace T3D{

	T3DTest::T3DTest(void)
	{
	}


	T3DTest::~T3DTest(void)
	{
	}


	static float randFloat() {
		return Math::randRange(TESTMIN, TESTMAX);
	}

	static Vector3 randVector() {
		return Math::randRange(Vector3(TESTMIN, TESTMIN, TESTMIN), Vector3(TESTMAX, TESTMAX, TESTMAX));
	}

	static BoundingSphere randSphere() {
		return BoundingSphere::create(randVector(), randFloat());
	}

	static bool implies(bool a, bool b) {
		return a ? b : true;
		//return !a || b;
	}

	void test_boundingsphere() {
		//test 1: forall a : BindingSphere, a.growToContain(BindingSphere::Identity) == a.
		const auto id = BoundingSphere::Identity();

		for (int i = 0; i < 1000; i++) {
			const auto a = randSphere();
			assert(a.growToContain(id) == a);
		}
		//test 2: forall a : BindingSphere, BindingSphere::Identity.growToContain(a) == a.
		for (int i = 0; i < 1000; i++) {
			const auto a = randSphere();
			assert(id.growToContain(a) == a);
		}
		
		//test 3: BoundingSphere::Identity().isIdentity() == true.
		assert(id.isIdentity() == true);

		//test 4: forall a b : BindingSphere, a.growToContain(b) == b.growToContain(a).
		for (int i = 0; i < 1000; i++) {
			const auto a = randSphere();
			const auto b = randSphere();
			assert(a.growToContain(b) == b.growToContain(a));
		}

		//test 5: forall p : Vector3, BoundingSphere::createFromPoint(p).contains(p) == true.
		//test 6: forall p : Vector3, BoundingSphere::Identity().contains(p) == false.
		//test 7: forall p : Vector3, BoundingSphere::createFromPoint(p).isIdentity() == false.
		//test 8: forall p : Vector3, BoundingSphere(p, 0.0) == BoundingSphere::Identity().
		//test 9: forall p : Vector3, BoundingSphere(p, 0.0).contains(p) == false.

		for (int i = 0; i < 1000; i++) {
			const auto p = randVector();
			assert(BoundingSphere::create(p).contains(p) == true);
			assert(id.contains(p) == false);
			assert(BoundingSphere::create(p).isIdentity() == false);
			//assert(BoundingSphere::create(p, 0).contains(p) == false);
			//assert(BoundingSphere::create(p, 0) == id);
		}

		//Test 10: Using BoundingSphere to appromimate pi.
		//What percent of a sphere is inside a cube with side length equal to the sphere's diameter?
		
		auto s = BoundingSphere::create(Vector3(0, 0, 0), TESTMAX);
		//Let VS be the volume of a sphere, Let VC be the volume of a cube.
		//Percent P = VS / CS = pi / 6 or approx 52.36%

		//Remember that the volume of a sphere A is
		//VS = (4/3) * pi * r * r * r
		//And of course the volume of a cube is
		//VC = size * size * size
		//suppose that the radius is one half the size
		//VC = 2r * 2r * 2r = 8 * r * r * r
		//VS / VC = (4/3) * pi / 8 = (4pi/3) / 8 = 4pi / 24 = pi / 6

		//Suppose we were to test 10,000 random points in this cubic volume
		//to see if they were in the bounding sphere.
		//We would expect that approximately 52% would be inside the sphere.
		//If we multiply this result by 6, it should be equal to pi!
		int count_inside = 0;
		for (int i = 0; i < 10000; i++) {
			Vector3 p = randVector();
			count_inside += s.contains(p) ? 1 : 0;
		}
		printf("Test 10: (Sphere test) approximate value for pi: %f. (Actual value is %f)\n", 6.0 * (float)count_inside / 10000.0, Math::PI);

		//test 11
		//if a sphere contains a point, adding another sphere includes all the original points
		//forall a : Vector3, b c : BoundingSphere, b.contains(a) || c.contains(a) -> b.growToContain(c).contains(a).
		for (int i = 0; i < 10000; i++) {
			auto a = randVector();
			auto b = randSphere(), c = randSphere();
			//auto x = b.growToContain(c);
			assert(implies(b.contains(a) || c.contains(a), b.growToContain(c).contains(a)));
			/*if (!implies(b.contains(a) == true, b.growToContain(c).contains(a) == true)) {
				std::cout << "Failed for \n"
					<< "\tfirst distance " << b.getPosition().distance(a) << std::endl
					<< "\tradius 1 " << b.getRadius() << std::endl
					<< "\tnext distance " << b.growToContain(c).getPosition().distance(a) << std::endl
					<< "\tradius 2 " << b.growToContain(c).getRadius() << std::endl
					<< std::endl;
			}*/
		}

		//test 12
		//the basic guarantee of a bounding volume is that it contains at least a given set of points.
		
		const Cube c = Cube(TESTMAX);
		const auto b = BoundingSphere::createFromMesh(&c);
		for (int i = 0; i < c.getNumVerts(); i++) {
			const auto v = c.getVertex(i);
			assert(b.contains(v));
		}

		//test 13
		//how much better is the two pass method than just doing a simple map / reduce?
		auto b2 = BoundingSphere::Identity();
		for (int i = 0; i < c.getNumVerts(); i++) {
			const auto v = c.getVertex(i);
			b2 = b2.growToContain(BoundingSphere::create(v));
		/*	if (!b2.contains(v)) {
				printf("Distance: %f\tRadius: %f\n", b2.getPosition().distance(v), b2.getRadius());
			}*/
			assert(b2.contains(v));
		}

		printf("Radius of sphere 1: %f.\nRadius of Sphere 2: %f.\n", b.getRadius(), b2.getRadius());
		//answer for this small example: radius is 17.3 vs 23.1.
		//the minimum radius here would be sqrt(2) * 10 = 14.1
	}

	bool T3DTest::init(){
		test_boundingsphere();

		// Call init of superclass (sets up sdl and opengl)
		//Bug: not checking return value?
		WinGLApplication::init();

		//Create a skybox and add some fog
		renderer->loadSkybox("Resources/Sunny1");
		renderer->setFog(0.007f,0.8f,0.8f,0.8f,1.0f);
		renderer->toggleFog();

		//Add a light
		GameObject *lightObj = new GameObject(this);
		Light *light = new Light(Light::DIRECTIONAL); 
		light->setAmbient(1,1,1);
		light->setDiffuse(1,1,1);
		light->setSpecular(1,1,1);
		lightObj->setLight(light);
		lightObj->getTransform()->setLocalRotation(Vector3(-45*Math::DEG2RAD,70*Math::DEG2RAD,0));
		lightObj->getTransform()->setParent(root);

		//Create some basic materials
		Material *red = renderer->createMaterial(Renderer::PR_OPAQUE);
		red->setDiffuse(1,0,0,1);
		Material *green = renderer->createMaterial(Renderer::PR_OPAQUE);
		green->setDiffuse(0,1,0,1);
		Material *blue = renderer->createMaterial(Renderer::PR_OPAQUE);
		blue->setDiffuse(0,0,1,1);

		//Create some textured materials (using texture files)
		Texture *smileytex = new Texture("Resources/Smiley.bmp", true, true);
		renderer->loadTexture(smileytex);
		Material *smiley = renderer->createMaterial(Renderer::PR_OPAQUE);
		smiley->setTexture(smileytex);
		
		Texture *sparkletex = new Texture("Resources/sparkle.png", true, true);
		renderer->loadTexture(sparkletex);
		Material *sparklemat = renderer->createMaterial(Renderer::PR_TRANSPARENT);
		sparklemat->setEmissive(1, 1, 1, 1);
		sparklemat->setBlending(Material::BLEND_DEFAULT);
		sparklemat->setSortedDraw(true, false);
		sparklemat->setTexture(sparkletex);

		//Create a textured material by adding text
		Texture *texttex = new Texture(128,32);
		texttex->clear(Colour(255,255,255,255));
		font *f = getFont("resources/FreeSans.ttf", 24);
		if (f != NULL) {
			texttex->writeText(2, 0, "Hello", Colour(0,255,0,255), f->getFont());
			texttex->writeText(64, 0, "World", Colour(255,0,0,0), f->getFont());
		}
		renderer->loadTexture(texttex, true);		
		Material *textmat = renderer->createMaterial(Renderer::PR_OPAQUE);
		textmat->setTexture(texttex,1);
		textmat->setEmissive(1,1,1,1);
				
		//Create a material with a procedurally generated texture
		Texture *proctex = new Texture(512,512);
		proctex->createFractal(Colour(40,150,50,255),Colour(120,220,100,255),25.0f,false);
		renderer->loadTexture(proctex, true);		
		Material *procmat = renderer->createMaterial(Renderer::PR_TERRAIN);
		procmat->setTexture(proctex,50);
		procmat->setSpecular(0,0,0,0);

		//Create a camera
		GameObject *camObj = new GameObject(this);
		renderer->camera = new Camera(Camera::PERSPECTIVE, 0.1, 500.0, 45.0, 1.6);
		camObj->getTransform()->setLocalPosition(Vector3(0,0,20));
		camObj->getTransform()->setLocalRotation(Vector3(0,0,0));
		camObj->setCamera(renderer->camera);
		camObj->getTransform()->setParent(root);

		//Add a controller to the camera object
		camObj->addComponent(new KeyboardController());

		//Add some terrain
		GameObject *terrain = new GameObject(this);
		Terrain *terrainComponent = new Terrain();
		terrain->addComponent(terrainComponent);
		terrainComponent->createFractalTerrain(256,500,15,2.0); // procedurally generate terrain
		terrain->setMaterial(procmat);
		terrain->getTransform()->setLocalPosition(Vector3(0,-20,0));
		terrain->getTransform()->setParent(root);
		terrain->getTransform()->name = "Terrain";

		//Add a welcome message
		GameObject *billboard = new GameObject(this);
		Billboard *billboardComponent = new Billboard(renderer->camera->gameObject->getTransform(),true);
		billboard->addComponent(billboardComponent);
		billboard->setMaterial(textmat);			// hello world
		billboard->getTransform()->setLocalPosition(Vector3(0,3,0));
		billboard->getTransform()->setLocalScale(Vector3(6,1.5,1));
		billboard->getTransform()->setParent(root);
		billboard->getTransform()->name = "Billboard";

		//Add a cube mesh
		GameObject *cube = new GameObject(this);
		cube->setMesh(new Cube(1));
		cube->setMaterial(smiley);
		cube->getTransform()->setLocalPosition(Vector3(4,-3,0));
		cube->getTransform()->setParent(root);
		cube->getTransform()->name = "Cube";

		//Create an empty node to use as a rotation point
		GameObject *rotateOrigin = new GameObject(this);
		rotateOrigin->getTransform()->setParent(root);
		rotateOrigin->addComponent(new RotateBehaviour(Vector3(0,1,0)));
		
		//Create a torus using the Sweep class as a child of rotateOrigin
		SweepPath sp;
		sp.makeCirclePath(2,32);
		GameObject *torus = new GameObject(this);
		vector<Vector3> points;
		points.push_back(Vector3(0.2f,0.0f,0.0f));
		points.push_back(Vector3(0.14f,0.14f,0.0f));
		points.push_back(Vector3(0.0f,0.2f,0.0f));
		points.push_back(Vector3(-0.14f,0.14f,0.0f));
		points.push_back(Vector3(-0.2f,0.0f,0.0f));
		points.push_back(Vector3(-0.14f,-0.14f,0.0f));
		points.push_back(Vector3(0.0f,-0.2f,0.0f));
		points.push_back(Vector3(0.14f,-0.14f,0.0f));
		torus->setMesh(new Sweep(points,sp,true));
		torus->setMaterial(red);
		torus->getTransform()->setLocalPosition(Vector3(10,0,0));
		torus->getTransform()->setParent(rotateOrigin->getTransform());
		torus->getTransform()->name = "Torus";

		//Add a sphere mesh as a child of the torus
		GameObject *sphere = new GameObject(this);
		sphere->setMesh(new Sphere(0.5,32));
		sphere->setMaterial(blue);
		sphere->getTransform()->setLocalPosition(Vector3(0,5,0));	
		sphere->getTransform()->setParent(torus->getTransform());
		sphere->getTransform()->name = "Sphere";
		
		//Create some animation for the sphere and torus
		Animation *anim = new Animation(10.0);
		torus->addComponent(anim);
		anim->addKey("Sphere",10.0,Quaternion(),Vector3(0,5,0));
		anim->addKey("Sphere",0,Quaternion(),Vector3(0,5,0));
		anim->addKey("Sphere",7.0,Quaternion(),Vector3(0,0,0));
		anim->addKey("Sphere",5.0,Quaternion(),Vector3(0,-5,0));
		anim->addKey("Torus",10.0,Quaternion(),Vector3(10,0,0));
		anim->addKey("Torus",0,Quaternion(),Vector3(10,0,0));
		anim->addKey("Torus",7.0,Quaternion(),Vector3(15,0,0));
		anim->addKey("Torus",5.0,Quaternion(Vector3(0,0,Math::HALF_PI)),Vector3(5,0,0));
		anim->loop(true);	
		
		//Add a particle system to the sphere
		ParticleEmitter *particleSys = new ParticleEmitter(0.0f, 20.0f, 20.0f, 20.0f, 5.0f, 5.0f, 0.2f);
		sphere->addComponent(particleSys);
		particleSys->createBillboardParticles(100, 1.0, 1.0, sparklemat, 1.0, root);
		particleSys->setPositionRange(0.01f, 0.01f, 0.01f);
		particleSys->setDirection(0*Math::DEG2RAD, 0.0*Math::DEG2RAD, 180*Math::DEG2RAD);
		particleSys->setStartVelocity(20.0f, 20.0f);
		particleSys->setAcceleration(-30.0f, 1.0f);
		particleSys->setAlphaFade(1.0f, 0.0f);
		particleSys->emit(40);
		
		//Add a behaviour to the Cube to make it "look at" the sphere
		cube->addComponent(new LookAtBehaviour(sphere->getTransform()));

		//Add a help overlay
		addTask(new DiagMessageTask(this, "Press F9 for help", 2, 32, true, 5.0));

		return true;
	}
}