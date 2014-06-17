#ifndef __Geometry_h__
#define __Geometry_h__

#include "GLBuffer.h"

class Geometry {

public:

  void Initialize();
  void Draw(int posAttr, int normalAttr);

private:

  struct MapVertex {
    MapVertex(const Vector3f& pos, const Vector3f& normal) {
      p[0] = pos[0];
      p[1] = pos[1];
      p[2] = pos[2];
      n[0] = normal[0];
      n[1] = normal[1];
      n[2] = normal[2];
    }
    float p[3];
    float n[3];
    bool operator<(const MapVertex& other) const {
      return memcmp((const void*)this, (const void*)(&other), sizeof(MapVertex)) > 0;
    }
  };

  typedef std::map<MapVertex, unsigned short, std::less<MapVertex>, Eigen::aligned_allocator<std::pair<MapVertex, unsigned short> > > VertexIndexMap;

  static void makeIndexBuffer(const stdvectorV3f& vertices, const stdvectorV3f& normals, std::vector<float>& outVertices, std::vector<float>& outNormals, std::vector<unsigned short>& outIndices);

  int m_numSphereTriangles;
  GLBuffer m_sphereVertices;
  GLBuffer m_sphereIndices;
  GLBuffer m_sphereNormals;

};

#endif
