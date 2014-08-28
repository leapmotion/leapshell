#include "StdAfx.h"
#include "Graph.h"
#include "Globals.h"

Graph::Graph() {
  m_downsampleK = 1.2f;
  m_dtVertices = 0.1f;
  m_dtEdges = 0.001f;
  m_pointSize = 1.0f;
  m_edgeForces = true;
  m_vertexForces = true;
  m_renderEdges = true;
  m_renderVertices = true;
  m_numVertices = 0;
  m_numEdges = 0;
  m_flipFlag = false;

  //createWebGraph();
  //createRandomGraph(3);
  createProductGraph();
}


int Graph::retrieveOrAddNode(const std::string& name) {
#if 0
  for (NodeMap::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
    if (it->second.name == name) {
      return it->second.id;
    }
  }
  const int newId = static_cast<int>(m_nodes.size());
  AddNode(newId);
  return newId;
#else
  std::map<std::string, int>::iterator it = m_nameCache.find(name);
  if (it == m_nameCache.end()) {
    const int newId = static_cast<int>(m_nodes.size());
    AddNode(newId);
    m_nameCache[name] = newId;
    return newId;
  } else {
    return m_nameCache[name];
  }
#endif
}

void Graph::createWebGraph() {
  m_nodes.clear();

  std::string graph = "youtube	web	media	video\n\
                      vimeo	web	media	video\n\
                      tumblr	web	media	pictures	social\n\
                      pinterest	web	media	pictures	social\n\
                      twitter	web	social\n\
                      instagram	web	media	pictures	social\n\
                      facebook	web	pictures	social\n\
                      soundcloud	web	media	music	social\n\
                      pandora	web	media	music\n\
                      spotify	web	media	music\n\
                      amazon	web	media	books	video	music	shopping	tv\n\
                      netflix	web	media	video	tv	movies\n\
                      hulu	web	media	video	tv\n\
                      hbo go	web	media	video	tv	movies\n\
                      wikipedia	web	search\n\
                      google	web	search\n\
                      vine	web	media	video\n\
                      npr	web	news	editorials	video	live radio	podcast\n\
                      new york times	web	news	editorials	politics\n\
                      economist	news	editorials	politics\n\
                      engadget	web	news	tech blog\n\
                      gizmodo	web	news	tech blog\n\
                      theverge	web	news	tech blog\n\
                      android central	web	news	tech blog\n\
                      wired	web	news	tech blog\n\
                      kexp	web	media	music	live radio	podcast\n\
                      somafm	web	media	music	live radio\n\
                      flickr	web	media	pictures\n\
                      foodlr	web	food	local\n\
                      eat24hrs	web	food	local\n\
                      yelp	web	food	local\n\
                      slashdot	web	news	tech blog\n\
                      reddit	web	news	social	media	pictures\n\
                      google calendar	web	utility	local\n\
                      google maps	web	utility	local\n\
                      TED	web	media	video\n\
                      google talk	web	social\n\
                      google music	web	media	music\n\
                      saved music	local	files	media	music\n\
                      saved video	local	files	media	video\n\
                      saved web	local	files	web\n\
                      saved documents	local	files	documents\n\
                      C Drive	local	files	drive\n\
                      Thumb Drive	local	files	drive\n\
                      Disc Drive	local	files	drive\n\
                      Google Drive	cloud	files	drive";

  std::vector<std::string> lines;
  split(graph, '\n', lines);
  for (size_t i=0; i<lines.size(); i++) {
    std::vector<std::string> elements;
    split(lines[i], '\t', elements);
    if (!elements.empty()) {
      const int sourceIdx = retrieveOrAddNode(elements[0]);
      for (size_t j=1; j<elements.size(); j++) {
        const int destIdx = retrieveOrAddNode(elements[j]);
      }
    }
  }
}

void Graph::createRandomGraph(int numNodes) {
  m_nodes.clear();

  static const int MIN_EDGES = 1;
  static const int MAX_EDGES = 100;
  for (int i=0; i<numNodes; i++) {
    AddNode(i);
  }

  for (int i=0; i<numNodes; i++) {
    float randFloat = ci::randFloat();
    randFloat = randFloat * randFloat * randFloat * randFloat;
    const int randNumEdges = static_cast<int>(randFloat * (MAX_EDGES - MIN_EDGES) + MIN_EDGES);
    int numEdgesAdded = 0;
    while (numEdgesAdded < randNumEdges) {
      const int idx = ci::randInt(0, numNodes);
      if (idx == i) {
        continue;
      }
      AddEdge(i, idx);
      numEdgesAdded++;
    }
  }
}

void Graph::createProductGraph() {
  m_nodes.clear();

  std::ifstream fileInput("facebook.txt");
  if (!fileInput) {
    return;
  }
  std::string graph((std::istreambuf_iterator<char>(fileInput)), std::istreambuf_iterator<char>());

#if 0
  std::vector<std::string> lines;
  std::vector<std::string> elements;
  split(graph, '\n', lines);
  std::stringstream ss;
  for (size_t i=0; i<lines.size(); i++) {
    elements.clear();
    split(lines[i], '\t', elements);
    if (!elements.empty()) {
      int sourceIdx, destIdx;
      ss.clear();
      ss.str(elements[0]);
      ss >> sourceIdx;
      AddNode(sourceIdx);
      for (size_t j=1; j<elements.size(); j++) {
        ss.clear();
        ss.str(elements[j]);
        ss >> destIdx;
        AddNode(destIdx);
        if (sourceIdx != destIdx) {
          AddEdge(sourceIdx, destIdx);
        }
      }
    }
  }
#else
  std::vector<std::string> lines;
  split(graph, '\n', lines);
  for (size_t i=0; i<lines.size(); i++) {
    std::vector<std::string> elements;
    split(lines[i], '\t', elements);
    if (!elements.empty()) {
      const int sourceIdx = retrieveOrAddNode(elements[0]);
      for (size_t j=1; j<elements.size(); j++) {
        const int destIdx = retrieveOrAddNode(elements[j]);
        if (sourceIdx == destIdx) {
          continue;
        }
        AddEdge(sourceIdx, destIdx);
      }
    }
  }
#endif
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

bool hasEntry(const std::vector<int>& data, int id) {
  for (size_t i=0; i<data.size(); i++) {
    if (data[i] == id) {
      return true;
    }
  }
  return false;
}

void Graph::AddEdge(int fromId, int toId) {
  if (fromId == toId) {
    return;
  }
  NodeMap::iterator fromIter = m_nodes.find(fromId);
  NodeMap::iterator toIter = m_nodes.find(toId);
  if (fromIter == m_nodes.end() || toIter == m_nodes.end()) {
    return;
  }

  Node& from = fromIter->second;
  Node& to = toIter->second;
  
  if (hasEntry(from.outEdges, toId)) {
    return;
  }
  if (hasEntry(from.inEdges, toId)) {
    return;
  }
  if (hasEntry(to.outEdges, fromId)) {
    return;
  }
  if (hasEntry(to.inEdges, fromId)) {
    return;
  }

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
  const double dw = 1.0f / width;
  const double dh = 1.0f / height;
  for (double i=dh*0.5; i<1.0; i+=dh) {
    for (double j=dw*0.5; j<1.0; j+=dw) {
      coords.push_back(static_cast<float>(j));
      coords.push_back(static_cast<float>(i));
      if (static_cast<int>(coords.size()) >= 2*limit) {
        break;
      }
    }
    if (static_cast<int>(coords.size()) >= 2*limit) {
      break;
    }
  }
}

void Graph::deleteVertexData() {
  m_vertCoordData.clear();
  if (m_vertCoords.isCreated()) {
    m_vertCoords.destroy();
  }
  if (m_vertColors.isCreated()) {
    m_vertColors.destroy();
  }
}

void Graph::createVertexData() {
  m_vertCoordData.clear();
  getTextureIndices(m_width, m_height, m_numVertices, true, m_vertCoordData);

  m_vertCoords.create(GL_ARRAY_BUFFER);
  m_vertCoords.bind();
  m_vertCoords.allocate((void*)(m_vertCoordData.data()), (int)(m_vertCoordData.size()*sizeof(float)), GL_STATIC_DRAW);
  m_vertCoords.release();

  std::vector<float> colors;
  const int numNodes = static_cast<int>(m_nodes.size());
  for (int i=0; i<numNodes; i++) {
    colors.push_back(1.0f);
    colors.push_back(1.0f);
    colors.push_back(1.0f);
    colors.push_back(1.0f);
  }

  m_vertColors.create(GL_ARRAY_BUFFER);
  m_vertColors.bind();
  m_vertColors.allocate((void*)(colors.data()), (int)(colors.size()*sizeof(float)), GL_STATIC_DRAW);
  m_vertColors.release();

  ci::gl::Fbo::Format format;
  format.setWrapS(GL_CLAMP_TO_EDGE);
  format.setWrapT(GL_CLAMP_TO_EDGE);
  format.setMinFilter(GL_NEAREST);
  format.setMagFilter(GL_NEAREST);
  format.setColorInternalFormat(GL_RGB32F);
  format.enableDepthBuffer(false, false);
  format.enableMipmapping(false);

  m_positionTarget1 = ci::gl::Fbo(m_width, m_height, format);
  m_positionTarget2 = ci::gl::Fbo(m_width, m_height, format);
  m_forceTarget = ci::gl::Fbo(m_width, m_height, format);

  m_positionTarget1.bindFramebuffer();
  ci::gl::clear();
  m_positionTarget1.unbindFramebuffer();

  m_positionTarget2.bindFramebuffer();
  ci::gl::clear();
  m_positionTarget2.unbindFramebuffer();
  
  m_forceTarget.bindFramebuffer();
  ci::gl::clear();
  m_forceTarget.unbindFramebuffer();

  updateVertexColors();
}

void Graph::updateVertexColors() {
  float* colors = (float*)m_vertColors.map(GL_WRITE_ONLY);
  const int numNodes = static_cast<int>(m_nodes.size());
  for (int i=0; i<numNodes; i++) {
    const float hue = static_cast<float>(i)/static_cast<float>(numNodes);
    ci::ColorA color = ci::hsvToRGB(ci::Vec3f(hue, 1.0f, 1.0f));

    colors[4*i] = color.r;
    colors[4*i + 1] = color.g;
    colors[4*i + 2] = color.b;
    colors[4*i + 3] = 1.0f;
  }
  m_vertColors.unmap();
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
  m_edgeCoordData.clear();
  for (NodeMap::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
    const Node& curNode = it->second;
    const int fromId = curNode.id;
    const std::vector<int>& outEdges = curNode.outEdges;
    for (size_t i=0; i<outEdges.size(); i++) {
      const int toId = outEdges[i];
      m_edgeCoordData.push_back(m_vertCoordData[2*fromId]);
      m_edgeCoordData.push_back(m_vertCoordData[2*fromId+1]);
      m_edgeCoordData.push_back(m_vertCoordData[2*toId]);
      m_edgeCoordData.push_back(m_vertCoordData[2*toId+1]);
    }
  }

  m_edgeCoords.create(GL_ARRAY_BUFFER);
  m_edgeCoords.bind();
  m_edgeCoords.allocate((void*)(m_edgeCoordData.data()), (int)(m_edgeCoordData.size()*sizeof(float)), GL_STATIC_DRAW);
  m_edgeCoords.release();

  std::vector<float> colors;
  for (NodeMap::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
    const std::vector<int>& outEdges = it->second.outEdges;
    for (size_t i=0; i<outEdges.size(); i++) {
      colors.push_back(1.0f);
      colors.push_back(1.0f);
      colors.push_back(1.0f);
      colors.push_back(0.1f);
      colors.push_back(1.0f);
      colors.push_back(1.0f);
      colors.push_back(1.0f);
      colors.push_back(0.1f);
    }
  }

  m_edgeColors.create(GL_ARRAY_BUFFER);
  m_edgeColors.bind();
  m_edgeColors.allocate((void*)(colors.data()), (int)(colors.size()*sizeof(float)), GL_STATIC_DRAW);
  m_edgeColors.release();

  updateEdgeColors();
}

void Graph::updateEdgeColors() {
  float* colors = (float*)m_edgeColors.map(GL_WRITE_ONLY);
  int idx = 0;
  for (NodeMap::iterator it = m_nodes.begin(); it != m_nodes.end(); ++it) {
    const std::vector<int>& outEdges = it->second.outEdges;
    for (size_t i=0; i<outEdges.size(); i++) {
      colors[idx++] = 1.0f;
      colors[idx++] = 1.0f;
      colors[idx++] = 1.0f;
      colors[idx++] = 0.1f;
      colors[idx++] = 1.0f;
      colors[idx++] = 1.0f;
      colors[idx++] = 1.0f;
      colors[idx++] = 0.1f;
    }
  }
  m_edgeColors.unmap();
}

void Graph::reload() {
  setNumVertices();
  deleteVertexData();
  createVertexData();

  setNumEdges();
  deleteEdgeData();
  createEdgeData();
}

void Graph::Update() {
  if (m_edgeForces) {
    runEdges();
  } else {
    m_forceTarget.bindFramebuffer();
    ci::gl::clear();
    m_forceTarget.unbindFramebuffer();
  }
  if (m_vertexForces) {
    runNBody();
  }
  swapTargets();
}

void Graph::Draw(const ci::Matrix44f& matrix) {
  if (m_renderVertices) {
    drawVertices(matrix);
  }
  if (m_renderEdges) {
    drawEdges(matrix);
  }
}

void Graph::runInitialPos() {
  tempTarget().bindFramebuffer();
  m_initialPosProg.bind();
  m_initialPosProg.uniform("time", 0.1f);
  const int coordsAttrib = m_initialPosProg.getAttribLocation("coords");

  m_vertCoords.bind();
  glEnableVertexAttribArray(coordsAttrib);
  glVertexAttribPointer(coordsAttrib, 2, GL_FLOAT, false, 0, 0);

  ci::gl::clear(ci::ColorA::zero());
  glDisable(GL_BLEND);
  //glEnable(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  ci::gl::setViewport(getArea());

  glDrawArrays(GL_POINTS, 0, m_numVertices);

  glDisableVertexAttribArray(coordsAttrib);
  m_vertCoords.release();

  m_initialPosProg.unbind();
  tempTarget().unbindFramebuffer();

  swapTargets();
}

void Graph::runNBody() {
  /*
  1) bind shader
  2) bind framebuffer
  3) clear
  4) blend settings
  5) depth settings
  6) cull face settings
  7) viewport
  8) attributes
  9) uniforms
  10) draw elements or arrays
  11) unload
  */

  m_nbodyProg.bind();
  const float width = static_cast<float>(m_width);
  const float height = static_cast<float>(m_height);
  positionTarget().bindTexture(0);
  m_forceTarget.bindTexture(1);

  tempTarget().bindFramebuffer();

  //ci::gl::clear();
  ci::gl::clear(ci::ColorA::zero());
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glDisable(GL_BLEND);
  //glEnable(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  const int coordsAttrib = m_nbodyProg.getAttribLocation("coords");
  m_vertCoords.bind();
  glEnableVertexAttribArray(coordsAttrib);
  glVertexAttribPointer(coordsAttrib, 2, GL_FLOAT, false, 0, 0);

  float ymax = std::ceil(m_numVertices / width) / height;

  m_nbodyProg.uniform("xsize", m_downsample / width);
  m_nbodyProg.uniform("ysize", m_downsample / height);
  m_nbodyProg.uniform("ymax", ymax);
  m_nbodyProg.uniform("xiter", 0.5f / width);
  m_nbodyProg.uniform("yiter", 0.5f / height);

  m_nbodyProg.uniform("positionTexture", 0);
  m_nbodyProg.uniform("forceTexture", 1);
  m_nbodyProg.uniform("dt", m_dtVertices * m_downsample);
  m_nbodyProg.uniform("xstart", m_downsampleIdx / width);
  m_nbodyProg.uniform("ystart", m_downsampleIdx / height);

  ci::gl::setViewport(getArea());

  glDrawArrays(GL_POINTS, 0, m_numVertices);
  glDisableVertexAttribArray(coordsAttrib);

  m_vertCoords.release();

  tempTarget().unbindFramebuffer();
  m_forceTarget.unbindTexture();
  positionTarget().unbindTexture();
  m_nbodyProg.unbind();

  m_downsampleIdx = (m_downsampleIdx + 1) % m_downsample;
}

void Graph::runEdges() {
  positionTarget().bindTexture(0);
  m_edgeProg.bind();
  m_edgeProg.uniform("positionTexture", 0);
  m_edgeProg.uniform("dt", m_dtEdges);
  //m_edgeProg.uniform("targetLength", 1.0f);
  m_forceTarget.bindFramebuffer();
  ci::gl::clear();

  glDisable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  //ci::gl::enableAdditiveBlending();

  glDisable(GL_CULL_FACE);

  const int coordsAttrib = m_edgeProg.getAttribLocation("coords");
  m_edgeCoords.bind();
  glEnableVertexAttribArray(coordsAttrib);
  glVertexAttribPointer(coordsAttrib, 4, GL_FLOAT, false, 0, 0);
  m_edgeCoords.release();

  ci::gl::setViewport(getArea());

  m_edgeProg.uniform("forceDir", 0.0f);
  glDrawArrays(GL_POINTS, 0, m_numEdges);

  m_edgeProg.uniform("forceDir", 1.0f);
  glDrawArrays(GL_POINTS, 0, m_numEdges);

  glDisableVertexAttribArray(coordsAttrib);

  m_edgeProg.unbind();
  m_forceTarget.unbindFramebuffer();
  positionTarget().unbindTexture();
}

/*
params = params || {};
this.attributes = {};
this.uniforms = {};
this.glProgram = null;
this.renderTarget = null;
this.framebuffer = null;
this.renderbuffer = null;
this.indexBuffer = null;
this.textureCount = 0;
this.viewport = params.viewport || {x: 0, y: 0, w: 1, h: 1};
this.blendEnabled = params.blendEnabled || false;
this.blendEquation = params.blendEquation || gl.FUNC_ADD;
this.blendFunc = params.blendFunc || [gl.SRC_ALPHA, gl.ONE];
this.drawMode = 'drawMode' in params ? params.drawMode : gl.TRIANGLES;
this.cullFace = 'cullFace' in params ? params.cullFace : null;
this.depthTest = 'depthTest' in params ? params.depthTest : true;
this.clear = 'clear' in params ? params.clear : (gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

*/

void Graph::drawVertices(const ci::Matrix44f& matrix) {
  m_drawVerticesProg.bind();

  positionTarget().bindTexture(0);
  m_drawVerticesProg.uniform("positionTexture", 0);
  m_drawVerticesProg.uniform("matrix", matrix);
  m_drawVerticesProg.uniform("pointSize", m_pointSize);

  GLint coordAttrib = m_drawVerticesProg.getAttribLocation("coords");
  GLint colorAttrib = m_drawVerticesProg.getAttribLocation("color");
  
  m_vertCoords.bind();
  glEnableVertexAttribArray(coordAttrib);
  glVertexAttribPointer(coordAttrib, 2, GL_FLOAT, false, 0, 0);
  m_vertCoords.release();

  m_vertColors.bind();
  glEnableVertexAttribArray(colorAttrib);
  glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, false, 0, 0);
  m_vertColors.release();

  //ci::gl::clear(ci::ColorA(0.0f, 0.0f, 0.0f, 0.0f));
  ci::gl::clear();
  //ci::gl::clear(ci::ColorA::zero());

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  ci::gl::enableAlphaBlending();

  //ci::gl::setViewport(m_origViewport);

  glDrawArrays(GL_POINTS, 0, m_numVertices);

  glDisableVertexAttribArray(coordAttrib);
  glDisableVertexAttribArray(colorAttrib);


  m_drawVerticesProg.unbind();
  positionTarget().unbindTexture();
}

void Graph::drawEdges(const ci::Matrix44f& matrix) {
  m_drawEdgesProg.bind();

  positionTarget().bindTexture(0);
  m_drawEdgesProg.uniform("positionTexture", 0);
  m_drawEdgesProg.uniform("matrix", matrix);


  GLint coordAttrib = m_drawEdgesProg.getAttribLocation("coords");
  GLint colorAttrib = m_drawEdgesProg.getAttribLocation("color");

  m_edgeCoords.bind();
  glEnableVertexAttribArray(coordAttrib);
  glVertexAttribPointer(coordAttrib, 2, GL_FLOAT, false, 0, 0);
  m_edgeCoords.release();

  m_edgeColors.bind();
  glEnableVertexAttribArray(colorAttrib);
  glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, false, 0, 0);
  m_edgeColors.release();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  ci::gl::enableAlphaBlending();

  glDrawArrays(GL_LINES, 0, 2*m_numEdges);

  glDisableVertexAttribArray(coordAttrib);
  glDisableVertexAttribArray(colorAttrib);


  m_drawEdgesProg.unbind();
  positionTarget().unbindTexture();
}

void Graph::swapTargets() {
  m_flipFlag = !m_flipFlag;
}
