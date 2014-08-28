#ifndef __Graph_h__
#define __Graph_h__

#include "GLBuffer.h"

class Node {
public:
  Node(int _id = 0) : id(_id) { }
  int id;
  std::string name;
  std::vector<int> outEdges;
  std::vector<int> inEdges;
};

typedef std::map<int, Node> NodeMap;

class Graph {

public:

  Graph();
  void AddNode(int id);
  Node& GetNode(int id);
  void RemoveNode(int id);
  void AddEdge(int fromId, int toId);
  void RemoveEdge(int fromId, int toId);

  void reload();

  void runInitialPos();

  void Update();
  void Draw(const ci::Matrix44f& matrix);

  ci::gl::GlslProg m_nbodyProg;
  ci::gl::GlslProg m_initialPosProg;
  ci::gl::GlslProg m_edgeProg;
  ci::gl::GlslProg m_drawVerticesProg;
  ci::gl::GlslProg m_drawEdgesProg;

  float m_downsampleK;
  float m_dtVertices;
  float m_dtEdges;
  float m_pointSize;
  bool m_edgeForces;
  bool m_vertexForces;
  bool m_renderEdges;
  bool m_renderVertices;
  int m_numVertices;
  int m_numEdges;

private:

  void createWebGraph();
  void createRandomGraph(int numNodes);
  void createProductGraph();

  int retrieveOrAddNode(const std::string& name);

  void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
  }

  static void getPotSize(int num, int& w, int& h);
  static void getTextureIndices(int width, int height, int limit, bool keepData, std::vector<float>& coords);

  void setNumVertices();
  void deleteVertexData();
  void createVertexData();
  void updateVertexColors();
  void setNumEdges();
  void deleteEdgeData();
  void createEdgeData();
  void updateEdgeColors();

  void runNBody();
  void runEdges();
  void drawVertices(const ci::Matrix44f& matrix);
  void drawEdges(const ci::Matrix44f& matrix);
  void swapTargets();

  ci::gl::Fbo& positionTarget() {
    return m_flipFlag ? m_positionTarget1 : m_positionTarget2;
  }

  ci::gl::Fbo& tempTarget() {
    return m_flipFlag ? m_positionTarget2 : m_positionTarget1;
  }

  ci::Area getArea() {
    return ci::Area(0, 0, m_width, m_height);
  }

  NodeMap m_nodes;
  std::map<std::string, int> m_nameCache;

  bool m_flipFlag;


  int m_downsampleIdx;
  int m_downsample;

  int m_width;
  int m_height;

  std::vector<float> m_vertCoordData;
  std::vector<float> m_edgeCoordData;

  ci::Area m_origViewport;

  GLBuffer m_vertCoords;
  GLBuffer m_vertColors;
  GLBuffer m_edgeCoords;
  GLBuffer m_edgeColors;
  ci::gl::Fbo m_positionTarget1;
  ci::gl::Fbo m_positionTarget2;
  ci::gl::Fbo m_forceTarget;

};

#endif
