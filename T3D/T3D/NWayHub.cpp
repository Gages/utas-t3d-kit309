#include "NWayHub.h"
#include "Quaternion.h"
#include <vector>
#include <assert.h>

namespace T3D {
	NWayHub::NWayHub(int density, int numspokes, float hubRadius, float filletRadius, float depth)
	{
		NWayHub::SetupNWayHub(this, density, numspokes, hubRadius, filletRadius, depth);
	}

	std::vector<Vector3> CircularLine(int density, float radius, int numSpokes) {
		//interior angle defines the sweep of the circular line.
		float interiorAngleRadians = ((numSpokes - 2) * Math::PI) / numSpokes;

		//The line is on the x-z plane, so each step is a rotation about the y axis.
		Quaternion rotationStepLeft = Quaternion(Vector3(0, interiorAngleRadians / density, 0));
		Quaternion rotationStepRight = Quaternion(rotationStepLeft);
		rotationStepRight.invert();

		//construct the vector to hold the points.
		//there will be _density_ points, plus one extra (central) point.
		auto points = std::vector<Vector3>(density + 1);
		
		const Vector3 central = Vector3(0, 0, radius);
		//Add the central point; radius units forward from the origin.
		points[density / 2] = central;

		Vector3 left = central, right = central;

		//constraint: density is divisible by 2.
		for (int i = 0; i < density / 2; i++) {
			left = left * rotationStepLeft;
			right = right * rotationStepRight;
			points[(density / 2) - (i + 1)] = left;
			points[(density / 2) + (i + 1)] = right;
		}

		return points;
	}

	void SetVertices(MeshExtra* mesh, int& offset, const std::vector<Vector3>& data,  Quaternion rotate, Vector3 translate) {
		for (auto v : data) {
			mesh->setVertex(offset++, (v * rotate) + translate);
		}
	}

	void NWayHub::SetupNWayHub(MeshExtra* mesh, int density, int numspokes, float hubRadius, float filletRadius, float depth) {
		
		const auto line = CircularLine(density, filletRadius, numspokes);
		
		/*BASIC VERTEX LAYOUT (incomplete)
		let n = density + 1;
		for offset = {0, 4n, 8n, ... 4 *(numspokes - 1)n};
		0	to 1(n - 1)):				top    (side)
		n	to 2(n - 1):				bottom (side)
		2n	to 3(n - 1):				top    (face)
		3n	to 4(n - 1):				bottom (face)
		*/
		const int numVerts = numspokes * 4 * (density + 2) + 2;
		const int numTris  = numspokes * 2;
		const int numQuads = numspokes * (2 * density + 1);

		mesh->SetupArrays(numVerts, numTris, numQuads);

		int vertexCount = 0;
		int quadCount = 0;
		int triCount = 0;

		for (int i = 0; i < numspokes; i++) {

			//Set it up so the 0th line does not need any extra rotation.
			//The line sweeps through the x-z axis towards the +z direction
			//Therefore the first fillet line must be translated to a focus point in the -z direction.

			//Translate to the correct fillet focus point. 
			float theta = i * (Math::TWO_PI / numspokes);
			Vector3 translate = Vector3(0, 0, -hubRadius) * Quaternion(0, theta, 0);

			//Rotate about the y axis so the circular strip to points towards the origin.
			Quaternion rotate = Quaternion(0, theta, 0);
			
			//Four sets of strips: top & bottom for side quads, top & bottom for face quads.
			SetVertices(mesh, vertexCount, line, rotate, translate);
			SetVertices(mesh, vertexCount, line, rotate, translate + Vector3(0, -depth, 0));
			SetVertices(mesh, vertexCount, line, rotate, translate);
			SetVertices(mesh, vertexCount, line, rotate, translate + Vector3(0, -depth, 0));
		}

		/*
				spoke	   line vertex
		vertices[numspokes][4][density + 1]
		*/
		auto vertex_offset = [density](int spoke, int line, int vertex) -> int {
			return 
				(density + 1) * 4 * spoke +
				(density + 1)     * line +
			                        vertex;
		};

		//Smooth shaded side quads.
		for (int spoke = 0; spoke < numspokes; spoke++) {
			for (int vertex = 0; vertex < density; vertex++) {
					mesh->setFace(quadCount++,
						vertex_offset(spoke, 0, vertex),
						vertex_offset(spoke, 0, vertex + 1),
						vertex_offset(spoke, 1, vertex + 1),
						vertex_offset(spoke, 1, vertex)
					);
			}
		}

		//central points for the top and bottom face triangles
		const int center_top = vertexCount++;
		const int center_bottom = vertexCount++;
		((Mesh *)mesh)->setVertex(center_top,    0,      0, 0);
		((Mesh *)mesh)->setVertex(center_bottom, 0, -depth, 0);

		//top and bottom faces
		//for each spoke, do (density / 2) quads connecting lines, and a single triangle filling in the central area.
		for (int i = 0; i < numspokes; i++) {
			//quads
			for (int v = 0; v < density / 2; v++) {
				mesh->setFace(quadCount++,
					vertex_offset(                  i, 2, density     - v),  //(last - ith) vertex in current spoke, top line (face)
					vertex_offset(                  i, 2, density - 1 - v),  //(next last - ith) vertex in current spoke, top line (face)
					vertex_offset((i + 1) % numspokes, 2,           1 + v),  //next vertex in next spoke, top line (face)
					vertex_offset((i + 1) % numspokes, 2,               v)  //current vertex in next spoke, top line (face)					
					);
				mesh->setFace(quadCount++,
					vertex_offset(                  i, 3, density     - v),  //(last - ith) vertex in current spoke, bottom line (face)
					vertex_offset((i + 1) % numspokes, 3,               v),  //current vertex in next spoke, bottom line (face)					
					vertex_offset((i + 1) % numspokes, 3,           1 + v),  //next vertex in next spoke, bottom line (face)
					vertex_offset(                  i, 3, density - 1 - v)  //(next last - ith) vertex in current spoke, bottom line (face)	
					);
			}
			//triangles filling in center
			//top
			mesh->setFace(triCount++,
				vertex_offset(                  i, 2, density / 2),   //middle vertex in first spoke, top line (face)
				center_top,									          //top center point
				vertex_offset((i + 1) % numspokes, 2, density / 2));  //middle vertex in next line (to the right), top line (face)
			//bottom
			mesh->setFace(triCount++,
				vertex_offset(                  i, 3, density / 2),  //middle vertex in first spoke, bottom line (face)
				vertex_offset((i + 1) % numspokes, 3, density / 2),  //middle vertex in next line (to the right), bottom line (face)
				center_bottom);                                      //bottoom center point
				
		}

		//The spoke face quads need their own vertices; they are discontinously shaded with the surrounding vertices.
		for (int i = 0; i < numspokes; i++) {
			int a = vertexCount++;
			int b = vertexCount++;
			int c = vertexCount++;
			int d = vertexCount++;
			mesh->copyVertex(a, vertex_offset( i,                  0, density)); //last vertex in first spoke, top line
			mesh->copyVertex(b, vertex_offset((i + 1) % numspokes, 0,       0)); //first vertex in next spoke, top line
			mesh->copyVertex(c, vertex_offset( i,                  1, density)); //last vertex in first spoke, bottom line
			mesh->copyVertex(d, vertex_offset((i + 1) % numspokes, 1,       0)); //first vertex in next spoke, bottom line
			mesh->setFace(quadCount++, a, b, d, c);
		}

		//check that the # of vertices, triangles and quads is correct.
 		assert(vertexCount == numVerts);
		assert(triCount == numTris);
		assert(quadCount == numQuads);

		mesh->calcNormals();
	}


	NWayHub::~NWayHub()
	{
	}
}