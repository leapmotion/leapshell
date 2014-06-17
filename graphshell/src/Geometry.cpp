#include "StdAfx.h"
#include "Geometry.h"

void Geometry::Initialize() {
  static const int SPHERE_RESOLUTION = 40;
  m_sphereVertices.create(GL_ARRAY_BUFFER);
  m_sphereNormals.create(GL_ARRAY_BUFFER);
  m_sphereIndices.create(GL_ELEMENT_ARRAY_BUFFER);

  std::vector<float> vertices;
  std::vector<float> normals;
  std::vector<unsigned short> indices;

  const float resFloat = static_cast<float>(SPHERE_RESOLUTION);
  const float pi = static_cast<float>(M_PI);
  const float twoPi = static_cast<float>(2.0 * M_PI);

  stdvectorV3f v;
  stdvectorV3f n;

  for (int w=0; w<SPHERE_RESOLUTION; w++) {
    for (int h=-SPHERE_RESOLUTION/2; h<SPHERE_RESOLUTION/2; h++) {
      const float inc1 = (w/resFloat) * twoPi;
      const float inc2 = ((w+1)/resFloat) * twoPi;
      const float inc3 = (h/resFloat) * pi;
      const float inc4 = ((h+1)/resFloat) * pi;

      const float x1 = std::sin(inc1);
      const float y1 = std::cos(inc1);
      const float z1 = std::sin(inc3);

      const float x2 = std::sin(inc2);
      const float y2 = std::cos(inc2);
      const float z2 = std::sin(inc4);

      const float r1 = std::cos(inc3);
      const float r2 = std::cos(inc4);

      v.push_back(Vector3f(r1*x1, z1, r1*y1));
      v.push_back(Vector3f(r1*x2, z1, r1*y2));
      v.push_back(Vector3f(r2*x2, z2, r2*y2));
      v.push_back(Vector3f(r1*x1, z1, r1*y1));
      v.push_back(Vector3f(r2*x2, z2, r2*y2));
      v.push_back(Vector3f(r2*x1, z2, r2*y1));

      n.push_back(Vector3f(r1*x1, z1, r1*y1).normalized());
      n.push_back(Vector3f(r1*x2, z1, r1*y2).normalized());
      n.push_back(Vector3f(r2*x2, z2, r2*y2).normalized());
      n.push_back(Vector3f(r1*x1, z1, r1*y1).normalized());
      n.push_back(Vector3f(r2*x2, z2, r2*y2).normalized());
      n.push_back(Vector3f(r2*x1, z2, r2*y1).normalized());
    }
  }

  makeIndexBuffer(v, n, vertices, normals, indices);

  m_sphereVertices.create(GL_ARRAY_BUFFER);
  m_sphereVertices.bind();
  m_sphereVertices.allocate((void*)(vertices.data()), (int)vertices.size()*sizeof(float), GL_STATIC_DRAW);
  m_sphereVertices.release();

  m_sphereNormals.create(GL_ARRAY_BUFFER);
  m_sphereNormals.bind();
  m_sphereNormals.allocate((void*)(normals.data()), (int)normals.size()*sizeof(float), GL_STATIC_DRAW);
  m_sphereNormals.release();

  m_sphereIndices.create(GL_ELEMENT_ARRAY_BUFFER);
  m_sphereIndices.bind();
  m_sphereIndices.allocate((void*)(indices.data()), (int)indices.size()*sizeof(unsigned short), GL_STATIC_DRAW);
  m_sphereIndices.release();

  m_numSphereTriangles = (int)indices.size()/3;
}

void Geometry::Draw(int posAttr, int normalAttr) {
  m_sphereVertices.bind();
  glEnableVertexAttribArray(posAttr);
  glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  m_sphereNormals.bind();
  glEnableVertexAttribArray(normalAttr);
  glVertexAttribPointer(normalAttr, 3, GL_FLOAT, GL_TRUE, 0, 0);

  m_sphereIndices.bind();
  glDrawElements(GL_TRIANGLES, m_numSphereTriangles*3, GL_UNSIGNED_SHORT, 0);
  m_sphereIndices.release();

  glDisableVertexAttribArray(posAttr);
  glDisableVertexAttribArray(normalAttr);

  m_sphereNormals.release();
  m_sphereVertices.release();
}

void Geometry::makeIndexBuffer(const stdvectorV3f& vertices, const stdvectorV3f& normals, std::vector<float>& outVertices, std::vector<float>& outNormals, std::vector<unsigned short>& outIndices) {
  VertexIndexMap vmap;

  for (size_t i=0; i<vertices.size(); i++) {
    MapVertex cur(vertices[i], normals[i]);

    const VertexIndexMap::iterator result = vmap.find(cur);
    if (result == vmap.end()) {
      unsigned short newIndex = static_cast<unsigned short>(outVertices.size()/3);
      outVertices.push_back(cur.p[0]);
      outVertices.push_back(cur.p[1]);
      outVertices.push_back(cur.p[2]);
      outNormals.push_back(cur.n[0]);
      outNormals.push_back(cur.n[1]);
      outNormals.push_back(cur.n[2]);
      outIndices.push_back(newIndex);
      vmap[cur] = newIndex;
    } else {
      outIndices.push_back(result->second);
    }
  }
}
