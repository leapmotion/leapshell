#include "StdAfx.h"
#include "Graph.h"
#include "Globals.h"

Graph::Graph() {
  m_downsampleK = 1.2f;
  m_dtVertices = 0.01f;
  m_dtEdges = 0.01f;
  m_pointSize = 3.0f;
  m_edgeForces = true;
  m_vertexForces = true;
  m_renderEdges = true;
  m_renderVertices = true;
  m_numVertices = 0;
  m_numEdges = 0;
  m_flipFlag = false;
}

void Graph::AddNode(int id) {
  NodeMap::iterator it = m_nodes.find(id);
  if (it == m_nodes.end()) {
    m_nodes[id] = Node(id);
  }
}

Node& Graph::GetNode(int id) {
  return m_nodes[id];
}

void Graph::RemoveNode(int id) {
  NodeMap::iterator it = m_nodes.find(id);
  if (it != m_nodes.end()) {
    Node& node = it->second;

    for (size_t i=0; i<node.outEdges.size(); i++) {
      const int& outEdgeId = node.outEdges[i];
      RemoveEdge(id, outEdgeId);
    }

    for (size_t i=0; i<node.inEdges.size(); i++) {
      const int& inEdgeId = node.inEdges[i];
      RemoveEdge(inEdgeId, id);
    }

    m_nodes.erase(it);
  }
}

void Graph::AddEdge(int fromId, int toId) {
  NodeMap::iterator fromIter = m_nodes.find(fromId);
  NodeMap::iterator toIter = m_nodes.find(toId);
  if (fromIter == m_nodes.end() || toIter == m_nodes.end()) {
    return;
  }

  Node& from = fromIter->second;
  Node& to = toIter->second;
  from.outEdges.push_back(toId);
  to.inEdges.push_back(fromId);
}

void Graph::RemoveEdge(int fromId, int toId) {
  NodeMap::iterator fromIter = m_nodes.find(fromId);
  NodeMap::iterator toIter = m_nodes.find(toId);
  if (fromIter == m_nodes.end() || toIter == m_nodes.end()) {
    return;
  }

  Node& from = fromIter->second;
  Node& to = toIter->second;
  from.outEdges.erase(std::remove(from.outEdges.begin(), from.outEdges.end(), toId), from.outEdges.end());
  to.inEdges.erase(std::remove(to.inEdges.begin(), to.inEdges.end(), fromId), to.inEdges.end());
}

void Graph::getPotSize(int num, int& w, int& h) {
  w = 1;
  h = 1;
  while (w * h < num) {
    w *= 2;
    if (w * h >= num) {
      break;
    }
    h *= 2;
  }
}

void Graph::setNumVertices() {
  m_numVertices = static_cast<int>(m_nodes.size());
  getPotSize(m_numVertices, m_width, m_height);
  m_downsampleIdx = 0;
  m_downsample = static_cast<int>(m_numVertices * m_downsampleK * 0.00015 + 1);
}

void Graph::getTextureIndices(int width, int height, int limit, bool keepData, std::vector<float>& coords) {
  coords.clear();
  if (limit == 0) {
    limit = width * height;
  }
  const float dw = 1.0f / width;
  const float dh = 1.0f / height;
  for (float i=dh*0.5f; i<1.0f; i+=dh) {
    for (float j=dw*0.5f; j<1.0f; j+=dw) {
      coords.push_back(j);
      coords.push_back(i);
      if (static_cast<int>(coords.size()/2) >= limit) {
        break;
      }
    }
    if (static_cast<int>(coords.size()/2) >= limit) {
      break;
    }
  }
}

void Graph::deleteVertexData() {
  if (m_vertCoords.isCreated()) {
    m_vertCoords.destroy();
  }
  if (m_vertColors.isCreated()) {
    m_vertColors.destroy();
  }
}

void Graph::createVertexData() {
  std::vector<float> coords;
  getTextureIndices(m_width, m_height, m_numVertices, true, coords);

  m_vertCoords.create(GL_ARRAY_BUFFER);
  m_vertCoords.bind();
  m_vertCoords.allocate((void*)(coords.data()), (int)coords.size()*sizeof(float), GL_STATIC_DRAW);
  m_vertCoords.release();

#if 0
  m_vertColors.create(GL_ARRAY_BUFFER);
  m_vertColors.bind();
  m_vertColors.allocate();
  m_vertColors.release();
#endif

  ci::gl::Fbo::Format format;
  format.setColorInternalFormat(GL_RGB32F);
  format.enableDepthBuffer(false, false);

  m_positionTarget1 = ci::gl::Fbo(m_width, m_height, format);
  m_positionTarget2 = ci::gl::Fbo(m_width, m_height, format);
  m_forceTarget = ci::gl::Fbo(m_width, m_height, format);

  updateVertexColors();
}

void Graph::updateVertexColors() {

}

void Graph::setNumEdges() {
  int numEdges = 0;
  for (NodeMap::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
    numEdges += static_cast<int>(it->second.outEdges.size());
  }
  m_numEdges = numEdges;
}

void Graph::deleteEdgeData() {
  if (m_edgeCoords.isCreated()) {
    m_edgeCoords.destroy();
  }
  if (m_edgeColors.isCreated()) {
    m_edgeColors.destroy();
  }
}

void Graph::createEdgeData() {
  std::vector<float> edges;
  for (NodeMap::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
    const Node& curNode = it->second;
    const std::vector<int>& edges = curNode.outEdges;
    for (size_t i=0; i<edges.size(); i++) {
      //curNode.
      //edges.push_back(
    }
  }
}

void Graph::reload() {
  setNumVertices();
  deleteVertexData();
  createVertexData();

  setNumEdges();
  deleteEdgeData();
  createEdgeData();

  init();
}

void Graph::init() {
  if (m_nbodyProg) {

    return;
  }


}

void Graph::initInitialPos(ci::gl::GlslProg& shader) {
  //m_initialPosProg.uniform()
}

void Graph::initNBody(ci::gl::GlslProg& shader) {

}

void Graph::initEdges(ci::gl::GlslProg& shader) {

}

void Graph::initDrawVertices(ci::gl::GlslProg& shader) {

}

void Graph::initDrawEdges(ci::gl::GlslProg& shader) {

}

void Graph::step() {
  if (m_edgeForces) {
    runEdges();
  }
  if (m_vertexForces) {
    runNBody();
  }
  if (m_renderVertices) {
    drawVertices();
  }
  if (m_renderEdges) {
    drawEdges();
  }
}

void Graph::runInitialPos() {
  tempTarget().bindFramebuffer();
  ci::gl::setViewport(ci::Area(0, 0, m_width, m_height));
  m_initialPosProg.bind();
  m_initialPosProg.uniform("time", static_cast<float>(Globals::curTimeSeconds));
  const int coordsAttr = m_initialPosProg.getAttribLocation("coords");

  // bind coords buffer here

  // glDrawElements of coords

  m_initialPosProg.unbind();
  tempTarget().unbindFramebuffer();

  swapTargets();
}

void Graph::runNBody() {
  const float width = static_cast<float>(m_width);
  const float height = static_cast<float>(m_height);
  positionTarget().bindTexture(0);
  m_forceTarget.bindTexture(1);
  m_nbodyProg.bind();

  float ymax = std::ceil(m_numVertices / width) / height;

  m_nbodyProg.uniform("xsize", m_downsample / width);
  m_nbodyProg.uniform("ysize", m_downsample / height);
  m_nbodyProg.uniform("ymax", ymax);
  m_nbodyProg.uniform("xiter", 0.5f / m_width);
  m_nbodyProg.uniform("yiter", 0.5f / m_height);

  m_nbodyProg.uniform("positionTexture", 0);
  m_nbodyProg.uniform("forceTexture", 1);
  m_nbodyProg.uniform("dt", m_dtVertices * m_downsample);
  m_nbodyProg.uniform("xstart", m_downsampleIdx / width);
  m_nbodyProg.uniform("ystart", m_downsampleIdx / height);

  tempTarget().bindFramebuffer();

  // bind buffer

  // glDrawElements

  tempTarget().unbindFramebuffer();
  m_nbodyProg.unbind();
  m_forceTarget.unbindTexture();
  positionTarget().unbindTexture();

  swapTargets();
  m_downsampleIdx = (m_downsampleIdx + 1) % m_downsample;
}

void Graph::runEdges() {
  positionTarget().bindTexture(0);
  m_edgeProg.bind();
  m_edgeProg.uniform("positionTexture", 0);
  m_edgeProg.uniform("forceDir", 0);
  m_edgeProg.uniform("dt", m_dtEdges);
  
  // bind
  // draw

  // bind
  // draw

  m_edgeProg.unbind();
  positionTarget().unbindTexture();
}

void Graph::drawVertices() {
  ci::Matrix44f matrix = ci::Matrix44f::identity();
  positionTarget().bindTexture(0);
  m_drawVerticesProg.uniform("positionTexture", 0);
  m_drawVerticesProg.uniform("matrix", matrix);
  m_drawVerticesProg.uniform("pointSize", m_pointSize);

  // bind
  // draw

  m_drawVerticesProg.unbind();
}

void Graph::drawEdges() {
  ci::Matrix44f matrix = ci::Matrix44f::identity();
  positionTarget().bindTexture(0);
  m_drawEdgesProg.uniform("positionTexture", 0);
  m_drawEdgesProg.uniform("matrix", matrix);

  // bind
  // draw

  m_drawEdgesProg.unbind();
}

void Graph::swapTargets() {
  m_flipFlag = !m_flipFlag;
}
