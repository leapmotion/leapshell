#ifndef __Graph_h__
#define __Graph_h__

#include "GLBuffer.h"

class Node {
public:
  Node(int _id = 0) : id(_id) { }
  int id;
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

  ci::gl::GlslProg m_nbodyProg;
  ci::gl::GlslProg m_initialPosProg;
  ci::gl::GlslProg m_edgeProg;
  ci::gl::GlslProg m_drawVerticesProg;
  ci::gl::GlslProg m_drawEdgesProg;

private:

  static void getPotSize(int num, int& w, int& h);
  static void getTextureIndices(int width, int height, int limit, bool keepData, std::vector<float>& coords);

  void setNumVertices();
  void deleteVertexData();
  void createVertexData();
  void updateVertexColors();
  void setNumEdges();
  void deleteEdgeData();
  void createEdgeData();
  
  void reload();

  void init();
  void initInitialPos(ci::gl::GlslProg& shader);
  void initNBody(ci::gl::GlslProg& shader);
  void initEdges(ci::gl::GlslProg& shader);
  void initDrawVertices(ci::gl::GlslProg& shader);
  void initDrawEdges(ci::gl::GlslProg& shader);

  void step();
  void runInitialPos();
  void runNBody();
  void runEdges();
  void drawVertices();
  void drawEdges();
  void swapTargets();

  ci::gl::Fbo& positionTarget() {
    return m_flipFlag ? m_positionTarget1 : m_positionTarget2;
  }

  ci::gl::Fbo& tempTarget() {
    return m_flipFlag ? m_positionTarget2 : m_positionTarget1;
  }

  NodeMap m_nodes;

  bool m_flipFlag;

  float m_downsampleK;
  float m_dtVertices;
  float m_dtEdges;
  float m_pointSize;
  bool m_edgeForces;
  bool m_vertexForces;
  bool m_renderEdges;
  bool m_renderVertices;

  int m_downsampleIdx;
  int m_downsample;

  int m_width;
  int m_height;
  int m_numVertices;
  int m_numEdges;

  GLBuffer m_vertCoords;
  GLBuffer m_vertColors;
  GLBuffer m_edgeCoords;
  GLBuffer m_edgeColors;
  ci::gl::Fbo m_positionTarget1;
  ci::gl::Fbo m_positionTarget2;
  ci::gl::Fbo m_forceTarget;

};

#endif
