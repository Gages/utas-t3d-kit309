// Author: Phillip Warren
//
// MeshExtra.h

// Some extra features on top of Mesh.h to assist building meshes.
#pragma once

#include "Mesh.h"
#include "Vector3.h"

namespace T3D {
	class MeshExtra : public Mesh {
	public:

		inline void setVertex(const int pos, Vector3 p) {
			((Mesh *)this)->setVertex(pos, p.x, p.y, p.z);
		}

		inline void copyVertex(int dest, int source) {
			setVertex(dest, getVertex(source));
		}

		typedef Vector3 Triangle[3];
		typedef Vector3 Quad[4];

		void SetupArrays(int numVerts, int numTris, int numQuads, bool includeColors = false, bool includeUVs = false) {
			this->numVerts = numVerts;
			this->numTris = numTris;
			this->numQuads = numQuads;

			vertices = new float[numVerts * 3];
			normals = new float[numVerts * 3];
			quadIndices = new unsigned int[numQuads * 4];
			triIndices = new unsigned int[numTris * 3];

			if (includeColors)
				colors = new float[numVerts * 4];

			if (includeUVs)
				uvs = new float[numVerts * 2];
		}

		template <unsigned int SIZEQ, unsigned int SIZET>
		void BuildMesh(const Quad(&quads)[SIZEQ], const Triangle(&tris)[SIZET]) {
			BuildMesh(quads, SIZEQ, tris, SIZET);
		}

	//	template <unsigned int SIZEQ, unsigned int SIZET>
		void BuildMesh(const Quad* quads, const int SIZEQ, const Triangle* tris, const int SIZET, bool includeColors = false, bool includeUVs = false) {
			
			SetupArrays((SIZET * 3) + (SIZEQ * 4), SIZET, SIZEQ, includeColors, includeUVs);

			int posv = 0;
			int triangle_faces = 0;
			int quad_faces = 0;
			int start;

			for (int i = 0; i < SIZET; i++) {
				const Vector3* points = tris[i];
			//for (const Triangle& points : tris) {
				start = posv;
				setVertex(posv++, points[0]);
				setVertex(posv++, points[1]);
				setVertex(posv++, points[2]);
				setFace(triangle_faces++, start + 2, start + 1, start);
			}

			for (int i = 0; i < SIZEQ; i++) {
				const Vector3* points = quads[i];
				start = posv;
				setVertex(posv++, points[0]);
				setVertex(posv++, points[1]);
				setVertex(posv++, points[2]);
				setVertex(posv++, points[3]);
				setFace(quad_faces++, start + 3, start + 2, start + 1, start);
			}

			calcNormals();
		}
	};
}

