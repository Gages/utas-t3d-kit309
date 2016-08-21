// =========================================================================================
// KXG363 - Advanced Games Programming, 2012
// =========================================================================================
//
// Author: Robert Ollington
//
// renderer.cpp
//
// Abstract base class for all rendering operations
// Recursively draws all objects in scene graph

#include "Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Cube.h"

namespace T3D
{
	/*! Constructor
	  Initialises members
	  */
	Renderer::Renderer(void)
	{
		camera = NULL;
		renderSkybox = false;

		showFog = false;
		fogDensity = 0;
		fogColour[0] = 0;
		fogColour[1] = 0;
		fogColour[2] = 0;
		fogColour[3] = 1;
		ambient[0] = 0.1f;
		ambient[1] = 0.1f;
		ambient[2] = 0.1f;
		ambient[3] = 1;

		showWireframe = false;
		showPoints = false;
		showGrid = false;
		showAxes = false;
	}

	/*! Destructor
	  The destructor is responsible for freeing all components
	  \todo		Free materials and check for other stuff that might need freeing
	  */
	Renderer::~Renderer(void)
	{
	}

	/*! Sets the fog colour and density
	  \param d	The new density
	  \param r	The new red compontent
	  \param g	The new green compontent
	  \param b	The new blue compontent
	  \param a	The new alpha compontent
	  */
	void Renderer::setFog(float d, float r, float g, float b, float a){ 
		fogDensity = d; 
		fogColour[0] =r; 
		fogColour[1] = g; 
		fogColour[2] = b; 
		fogColour[3] = a; 
	}

	/*! Creates a material and adds it to the priority list
	  GameObjects are sorted by material before being rendered.  The order of rendering is determined by the material priority
	  \param param	The priority for this material
	  */
	Material* Renderer::createMaterial(int priority){
		Material* m = new Material();
		materials[priority].push_back(m);
		return m;
	}

	struct GameObjectCameraDistanceCompare
	{
		bool operator()(const GameObject *t1, const GameObject *t2) const {
			return t1->getDistanceToCamera() < t2->getDistanceToCamera();
		}
	};

	/*! Renders the scenegraph
	  This method is responsible for sorting by material and rendering game objects in material priority order
	  \param root	The root of the scenegraph to be rendered
	  */
	void Renderer::render(Transform *root){

		if (camera == NULL) return;

		Vector3 cameraPos;
		float distance;
		GameObject *object;

		// Single common camera for all rendering
		cameraPos = camera->gameObject->getTransform()->getWorldPosition();
		camera->calculateWorldSpaceFrustum();

		HBVC_MaxDepth = 0;
		buildRenderQueue(root);

		for (int i=0; i<PRIORITY_LEVELS; i++) {

			// temp list of objects requiring sorted draw order (if any)
			std::priority_queue<GameObject*, std::vector<GameObject*>, GameObjectCameraDistanceCompare> sorted;

			for (auto mit : materials[i]) {
			//for (mit = materials[i].begin(); mit!=materials[i].end(); mit++){
				
				std::queue<GameObject*> &q = mit->getQueue();	// objects to be drawn

				if (!mit->getSortedDraw()) {
					// objects for normal unsorted materials are drawn immediately
					loadMaterial(mit);
					while (!q.empty()) {
						object = q.front();
						if (object->isVisible()) {
							draw(object);
						}
						q.pop();
					}
					unloadMaterial(mit);
				}
				else {
					// objects to be sorted and drawn later
					while (!q.empty()) {
						object = q.front();
						if (object->isVisible()) {
							// Note using squared distance as we only care about relative distance
							distance = cameraPos.squaredDistance(object->getTransform()->getWorldPosition());
							object->setDistanceToCamera(distance);
							sorted.push(object);
						}
						q.pop();
					}
				}
			}

			// Draw sorted objects (if any)
			Material *loaded = NULL;			// current loaded material
			while (!sorted.empty()) {
				object = sorted.top();
				if (loaded != object->getMaterial()) {
					// only load material if changed
					loaded = object->getMaterial();
					loadMaterial(loaded);
				}
				draw(object);
				sorted.pop();
			}
		}
	}

	//add each gameobject in depth first order
	static void buildRenderQueueDontCull(Transform* root) {
		GameObject* obj = root->gameObject;
		if (obj) {
			Material* m = obj->getMaterial();
			if (m) m->addToQueue(obj);
		}

		for (auto child : root->children)
		{
			buildRenderQueueDontCull(child);
		}

	}

	/*! Sorts game objects by material
	  This method traverses the scenegraph and adds game objects their respective material's render queues
	  \param root	The root of the scenegraph to be sorted
	  */
	void Renderer::buildRenderQueue(Transform *root){
			DefaultBoundingVolume rootBoundingSphere = root->getWorldMatrix() * root->getBoundingVolume();
			
			switch (camera->contains(rootBoundingSphere)) {
				//if the object is completely outside the view frustum,
				//do not add it to the render queue and do not recursively cull test its children
			case BoundingVolumeIntersects::Outside:
				return;
				//if the object is completely inside the view frustum,
				//add everything to the render queue and stop cull testing
			case BoundingVolumeIntersects::Inside:
					//start the fast path
					buildRenderQueueDontCull(root); return;
			case BoundingVolumeIntersects::Overlap:
				//if the object overlaps with the view frustum,
				//recursively cull test
					GameObject* obj = root->gameObject;
					if (obj) {
						//if (camera->contains(obj->getBoundingSphere()) != Camera::None) {
							Material* m = obj->getMaterial();
							if (m) m->addToQueue(obj);
						//}
					}

					//A ready crappy way to count the depth of a tree without passing an explicit variable.
					const int depth_start = HBVC_MaxDepth + 1; //save the current depth (counting one more)
					int max_depth_children = 0; //how deep is the deepest child
					for (auto child : root->children) {
						HBVC_MaxDepth = 0; //reset the variable to zero
						buildRenderQueue(child); //update the HBVC_depth variable (inductive)
						max_depth_children = std::max(max_depth_children, HBVC_MaxDepth); //update the depth iff it is deeper than before.
					}
					HBVC_MaxDepth = max_depth_children + depth_start;
				}

		}
		
}
