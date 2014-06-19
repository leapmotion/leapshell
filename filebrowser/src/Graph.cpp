#include "StdAfx.h"
#include "Utilities.h"
#include "Graph.h"
#include "Globals.h"

const float Graph::EDGE_DISTANCE_SMOOTH = 0.975f;
const float Graph::HIGHLIGHT_SMOOTH = 0.95f;
const float Graph::CONTRACTION_DISTANCE_RATIO = 0.5f;

void Graph::Start(GraphType type) {
  if (type == RANDOM) {
    createRandomGraph(100);
  } else if (type == WEB) {
    createWebGraph();
  }

  m_gravity = 0.005f;
  m_flattening = 0.03f;
  m_gravityRadius = 500.0f;
  m_alpha = 0.85f;
  m_friction = 0.85f;
  m_charge = -100.0f;
  m_leap = 1.0f;
}

void Graph::Update() {
  applyRelaxation();
  applyGravity();
  applyFlattening();
  applyForces();
  applyLeapForces();
  resolveConstraints();
  cleanupAttractors();
}

void Graph::Draw(const Matrix4x4& textTransform) const {
  ci::gl::color(ci::ColorA::white());
  ci::gl::enableDepthRead();
  ci::gl::enableDepthWrite();
  //ci::gl::enableAlphaBlending();
  ci::gl::enableAdditiveBlending();
  //glDisable(GL_DEPTH_TEST);
  //ci::gl::enableAdditiveBlending();
  //glEnable(GL_TEXTURE_2D);

#if 1
  for (size_t i=0; i<m_nodes.size(); i++) {
    const GraphNode& node = m_nodes[i];
    const Vector3& pos = node.m_position;
    const ci::Vec3f nodePos(pos.x(), pos.y(), pos.z());
    const float ratio = node.m_weight / m_maxWeight;
    const float radius = 0.25f + 2.5f*ratio;
    //ci::gl::drawSphere(nodePos, radius);

    ci::gl::color(node.m_color);
    glPointSize(15.0f);
    glBegin(GL_POINTS);
    ci::gl::vertex(nodePos);
    glEnd();

    //if (node.m_activation.value > 0.001f) {
      ci::gl::color(ci::ColorA::gray(node.m_highlight.value, 1.0f));
      glPointSize(30.0f);
      glBegin(GL_POINTS);
      ci::gl::vertex(nodePos);
      glEnd();
    //}
  }
#endif

#if 1
  for (size_t i=0; i<m_edges.size(); i++) {
    const GraphEdge& edge = m_edges[i];
    const GraphNode& node1 = m_nodes[edge.m_source];
    const GraphNode& node2 = m_nodes[edge.m_target];
    glLineWidth(2.0f);
    ci::ColorA c = 0.5f * (node1.m_color + node2.m_color);
    c = edge.m_activation.value * ci::ColorA::white() + (1.0f - edge.m_activation.value)*c;
    ci::gl::color(c);
    glBegin(GL_LINES);
    glVertex3dv(node1.m_position.data());
    glVertex3dv(node2.m_position.data());
    glEnd();
    //if (edge.m_activation.value > 0.001f) {
      glLineWidth(8.0f);
#if 0
      ci::gl::color(ci::ColorA::gray(edge.m_activation.value, 1.0f));
#else
      const float hue = 0.75f * edge.m_activation.value;
      ci::ColorA rgb = ci::hsvToRGB(ci::Vec3f(hue, 1.0f, edge.m_activation.value));
      //rgb.a = edge.m_activation.value;
      ci::gl::color(rgb);
#endif
      glBegin(GL_LINES);
      glVertex3dv(node1.m_position.data());
      glVertex3dv(node2.m_position.data());
      glEnd();
    //}
  }
#endif

  //glEnable(GL_DEPTH_TEST);
  //glDisable(GL_LIGHTING);
  //ci::gl::disableDepthRead();
  //ci::gl::disableDepthWrite();
  //ci::gl::enableAlphaBlending();

  //for (size_t i=0; i<m_attractors.size(); i++) {
  for (AttractorMap::const_iterator it = m_attractors.cbegin(); it != m_attractors.cend(); ++it) {
    const Attractor& attractor = it->second;
    const float radius = 25.0f - 10.0f*attractor.m_strength;
    ci::gl::color(ci::ColorA::gray(0.5f + 0.5f*attractor.m_strength, attractor.m_strength));
    ci::gl::drawSphere(ToVec3f(attractor.m_position), radius, 30);
  }


  // draw all text at the end
#if 1
  for (size_t i=0; i<m_nodes.size(); i++) {
    const GraphNode& node = m_nodes[i];
    const ci::Vec2f nameSize = Globals::fontRegular->measureString(node.m_name);
    const float start = -nameSize.x/2.0f;
    const ci::Rectf nameRect(start, 0.0f, start + nameSize.x, 200.0f);
    const Vector3& pos = node.m_position;
    const ci::Vec3f nodePos(pos.x(), pos.y(), pos.z());
    const float ratio = node.m_weight / m_maxWeight;

    glPushMatrix();
    ci::gl::translate(nodePos);
    glScalef(1, -1, 1);
    glMultMatrixd(textTransform.data());
    const float scale = 0.1f + 0.175f*ratio*ratio;
    glScalef(-scale, scale, scale);
    ci::gl::translate(ci::Vec3f(0, 0, -100));
    ci::gl::color(ci::ColorA::white());
    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);
    //ci::gl::disableDepthWrite();
    Globals::fontRegular->drawString(node.m_name, nameRect);
    //ci::gl::enableDepthWrite();
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glPopMatrix();
  }
#endif
}

//void Graph::ClearAttractors() {
//  m_attractors.clear();
//}
//
//void Graph::AddAttractor(const Vector3& position, float strength) {
//  Attractor temp;
//  temp.m_position = position;
//  temp.m_strength = strength;
//  m_attractors.push_back(temp);
//}

void Graph::UpdateAttractor(int id, const Vector3& position, float strength) {
  Attractor& attractor = m_attractors[id];
  attractor.m_strength = strength;
  attractor.m_position = position;
  attractor.m_lastUpdateTime = Globals::curTimeSeconds;
}

void Graph::createRandomGraph(int numNodes) {
  m_nodes.clear();
  m_edges.clear();

  m_nodes.resize(numNodes);

  static const int MIN_EDGES = 1;
  static const int MAX_EDGES = 4;
  for (int i=0; i<numNodes; i++) {
    GraphNode& node = m_nodes[i];
    node.m_weight = 0;
    node.m_position = getRandomPosition();
    node.m_desiredPosition = node.m_position;

    float randFloat = ci::randFloat();
    randFloat = randFloat * randFloat;
    const int randNumEdges = static_cast<int>(randFloat * (MAX_EDGES - MIN_EDGES) + MIN_EDGES);
    GraphEdge edge;
    edge.m_source = i;
    int numEdgesAdded = 0;
    while (numEdgesAdded < randNumEdges) {
      const int idx = ci::randInt(0, numNodes);
      if (idx == i) {
        continue;
      }
      edge.m_target = idx;
      edge.m_strength = 1.0f;
      edge.m_origDistance = ci::randFloat(150.0f, 170.0f);
      edge.m_contractedDistance = edge.m_origDistance * CONTRACTION_DISTANCE_RATIO;
      edge.m_distance.Update(edge.m_origDistance, Globals::curTimeSeconds, EDGE_DISTANCE_SMOOTH);
      edge.m_visited = false;
      edge.m_activation.Update(0.0f, Globals::curTimeSeconds, 0.5f);
      m_edges.push_back(edge);
      numEdgesAdded++;
    }
  }

  m_maxWeight = 0.0f;
  for (size_t i=0; i<m_edges.size(); i++) {
    const GraphEdge& edge = m_edges[i];
    GraphNode& source = m_nodes[edge.m_source];
    GraphNode& target = m_nodes[edge.m_target];
    source.m_weight += 1.0f;
    target.m_weight += 1.0f;
    m_maxWeight = std::max(m_maxWeight, source.m_weight);
    m_maxWeight = std::max(m_maxWeight, target.m_weight);
  }

  for (size_t i=0; i<m_nodes.size(); i++) {
    const float ratio = m_nodes[i].m_weight / m_maxWeight;
    const float hue = 0.75f * ratio;
    m_nodes[i].m_color = ci::hsvToRGB(ci::Vec3f(hue, 1.0f, 0.9f));
    std::stringstream ss;
    ss << static_cast<int>(m_nodes[i].m_weight);
    m_nodes[i].m_name = ss.str();
    //m_nodes[i].m_color.a = ratio;
  }
}

int Graph::retrieveOrAddNode(const std::string& name) {
  for (size_t i=0; i<m_nodes.size(); i++) {
    if (m_nodes[i].m_name == name) {
      return static_cast<int>(i);
    }
  }
  GraphNode temp;
  temp.m_name = name;
  temp.m_weight = 0;
  temp.m_position = getRandomPosition();
  temp.m_desiredPosition = temp.m_position;
  temp.m_visited = false;
  temp.m_activation.Update(0.0f, Globals::curTimeSeconds, 0.5f);
  temp.m_highlight.Update(0.0f, Globals::curTimeSeconds, 0.5f);
  m_nodes.push_back(temp);
  return static_cast<int>(m_nodes.size()-1);
}

void Graph::createWebGraph() {
  m_nodes.clear();
  m_edges.clear();

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
        GraphEdge temp;
        temp.m_origDistance = 170.0f;
        temp.m_contractedDistance = temp.m_origDistance * CONTRACTION_DISTANCE_RATIO;
        temp.m_distance.Update(temp.m_origDistance, Globals::curTimeSeconds, EDGE_DISTANCE_SMOOTH);
        temp.m_source = sourceIdx;
        temp.m_target = destIdx;
        temp.m_strength = 1.0f;
        temp.m_visited = false;
        temp.m_activation.Update(0.0f, Globals::curTimeSeconds, 0.5f);
        m_edges.push_back(temp);
      }
    }
  }

  m_maxWeight = 0.0f;
  for (size_t i=0; i<m_edges.size(); i++) {
    const GraphEdge& edge = m_edges[i];
    GraphNode& source = m_nodes[edge.m_source];
    GraphNode& target = m_nodes[edge.m_target];
    source.m_weight += 1.0f;
    target.m_weight += 1.0f;
    m_maxWeight = std::max(m_maxWeight, source.m_weight);
    m_maxWeight = std::max(m_maxWeight, target.m_weight);
  }

  for (size_t i=0; i<m_nodes.size(); i++) {
    const float ratio = m_nodes[i].m_weight / m_maxWeight;
    const float hue = 0.75f * ratio;
    m_nodes[i].m_color = ci::hsvToRGB(ci::Vec3f(hue, 1.0f, 0.9f));
    std::stringstream ss;
    ss << static_cast<int>(m_nodes[i].m_weight);
    //m_nodes[i].m_name = ss.str();
    //m_nodes[i].m_color.a = ratio;
  }
}

Vector3 Graph::getRandomPosition() const {
  Vector3 result;
  result << ci::randFloat(-50, 50), ci::randFloat(0, -100), ci::randFloat(-50, 50);
  return result;
}

void Graph::applyRelaxation() {
  for (size_t i=0; i<m_edges.size(); i++) {
    const GraphEdge& edge = m_edges[i];
    assert(edge.m_source != edge.m_target);
    GraphNode& source = m_nodes[edge.m_source];
    GraphNode& target = m_nodes[edge.m_target];
    const Vector3 diff = target.Position() - source.Position();
    const float length = static_cast<float>(diff.norm());
    assert(length>0.00001f);

    const float targetRatio = source.Weight() / (source.Weight() + target.Weight());
    const float sourceRatio = 1.0f - targetRatio;

    const Vector3 movement = m_alpha * edge.m_strength * (length - edge.m_distance.value) * diff / length;
    target.m_position -= targetRatio * movement;
    source.m_position += sourceRatio * movement;
  }
}

void Graph::applyGravity() {
  const float strength = m_alpha * m_gravity;
  const Vector3 gravitySource = -m_gravityRadius * Vector3::UnitZ();
  for (size_t i=0; i<m_nodes.size(); i++) {
    GraphNode& node = m_nodes[i];
    node.m_position += (gravitySource - node.m_position) * strength;
  }
}

void Graph::applyFlattening() {
  const float strength = m_alpha * m_flattening;
  const Vector3 flatteningSource = Vector3::Zero();
  for (size_t i=0; i<m_nodes.size(); i++) {
    GraphNode& node = m_nodes[i];
    const Vector3 diff = flatteningSource - node.m_position;
    const Vector3 diffNorm = diff.normalized();
    const Vector3 desiredPos = flatteningSource - m_gravityRadius * diffNorm;
    node.m_position += (desiredPos - node.m_position) * strength;
  }
}

void Graph::applyForces() {
  const float forceChargeDistanceSq = 9999999.0f;

  for (size_t i=0; i<m_nodes.size(); i++) {
    GraphNode& node1 = m_nodes[i];
    for (size_t j=i+1; j<m_nodes.size(); j++) {
      GraphNode& node2 = m_nodes[j];
      const Vector3 diff = node2.m_position - node1.m_position;
      const float diffLengthSq = static_cast<float>(diff.squaredNorm());
      if (diffLengthSq < forceChargeDistanceSq) {
        const float diffLength = std::sqrt(diffLengthSq);
        const float bonus1 = node2.m_weight / m_maxWeight;
        const float bonus2 = node1.m_weight / m_maxWeight;
        const Vector3 movement = m_alpha * diff * (m_charge / diffLengthSq);
        node1.m_desiredPosition -= (1.0f + bonus1) * movement;
        node2.m_desiredPosition += (1.0f + bonus2) * movement;
      }
    }
  }
}

inline float Decay(const float input, const float maxValue, const float halfLife) {
  return (maxValue*halfLife)/(halfLife + input);
}

void Graph::applyLeapForces() {
  unmarkAllNodesVisited();
  unmarkAllEdgesVisited();
  const float MAX_DIST_SQ = 5000.0f;
  const float strength = m_leap * m_alpha;

  //for (size_t j=0; j<m_attractors.size(); j++) {
  for (AttractorMap::iterator it = m_attractors.begin(); it != m_attractors.end(); ++it) {
    Attractor& attractor = it->second;
    int closestNode = -1;
    float closestDistSq = MAX_DIST_SQ;
    for (size_t i=0; i<m_nodes.size(); i++) {
      GraphNode& node = m_nodes[i];
      const Vector3 diff = node.m_position - attractor.m_position;
      const float diffLengthSq = (1.0f - node.m_activation.value) * static_cast<float>(diff.squaredNorm());
      if (diffLengthSq < closestDistSq) {
        closestDistSq = diffLengthSq;
        closestNode = static_cast<int>(i);
      }
    }

    if (closestNode == attractor.m_prevClosestNode) {
      attractor.m_searchDepth = std::min(attractor.m_maxSearchDepth, static_cast<int>(Globals::curTimeSeconds - attractor.m_lastClosestSwitchTime) + 1);
    } else {
      attractor.m_lastClosestSwitchTime = Globals::curTimeSeconds;
    }

    if (closestNode >= 0) {
      bfs(closestNode, attractor.m_searchDepth);
      GraphNode& node = m_nodes[closestNode];
      const Vector3 diff = node.m_position - attractor.m_position;
      const float diffLengthSq = static_cast<float>(diff.squaredNorm());
      //const float falloff = Decay(diffLengthSq, 1.0f, 30.0f);
      const float falloff = 1.0f;// SmootherStep(1.0f - Clamp(diffLengthSq/MAX_DIST_SQ));
      node.m_desiredPosition += strength * falloff * attractor.m_strength * diff;
    }

    for (size_t i=0; i<m_nodes.size(); i++) {
      m_nodes[i].m_highlight.Update(hopsToHighlight(m_nodes[i].m_hops, attractor.m_searchDepth), Globals::curTimeSeconds, HIGHLIGHT_SMOOTH);
      if (i == closestNode) {
        m_nodes[i].m_activation.Update(attractor.m_strength, Globals::curTimeSeconds, HIGHLIGHT_SMOOTH);
      } else {
        m_nodes[i].m_activation.Update(0.0f, Globals::curTimeSeconds, HIGHLIGHT_SMOOTH);
      }
    }

    for (size_t i=0; i<m_edges.size(); i++) {
      m_edges[i].m_activation.Update(hopsToHighlight(m_edges[i].m_hops, attractor.m_searchDepth), Globals::curTimeSeconds, HIGHLIGHT_SMOOTH);
      const float ratio = 1.0f - m_edges[i].m_activation.value * attractor.m_strength;
      const float dist = m_edges[i].m_contractedDistance + ratio*(m_edges[i].m_origDistance - m_edges[i].m_contractedDistance);
      m_edges[i].m_distance.Update(dist, Globals::curTimeSeconds, EDGE_DISTANCE_SMOOTH);
    }

    attractor.m_prevClosestNode = closestNode;
  }

  for (size_t i=0; i<m_nodes.size(); i++) {
    if (m_nodes[i].m_highlight.lastTimeSeconds < Globals::curTimeSeconds) {
      m_nodes[i].m_highlight.Update(0.0f, Globals::curTimeSeconds, HIGHLIGHT_SMOOTH);
    }
    if (m_nodes[i].m_activation.lastTimeSeconds < Globals::curTimeSeconds) {
      m_nodes[i].m_activation.Update(0.0f, Globals::curTimeSeconds, HIGHLIGHT_SMOOTH);
    }
  }
  for (size_t i=0; i<m_edges.size(); i++) {
    if (m_edges[i].m_activation.lastTimeSeconds < Globals::curTimeSeconds) {
      m_edges[i].m_activation.Update(0.0f, Globals::curTimeSeconds, HIGHLIGHT_SMOOTH);
    }
    if (m_edges[i].m_distance.lastTimeSeconds < Globals::curTimeSeconds) {
      m_edges[i].m_distance.Update(m_edges[i].m_origDistance, Globals::curTimeSeconds, HIGHLIGHT_SMOOTH);
    }
  }
}

void Graph::resolveConstraints() {
  for (size_t i=0; i<m_nodes.size(); i++) {
    GraphNode& node = m_nodes[i];
    const Vector3 diff = node.m_desiredPosition - node.m_position;
    node.m_desiredPosition = node.m_position;
    node.m_position -= m_friction * diff;
  }
}

void Graph::cleanupAttractors() {
  static const float MAX_AGE = 0.1f; // seconds since last update until hand info gets cleaned up
  AttractorMap::iterator it = m_attractors.begin();
  while (it != m_attractors.end()) {
    Attractor& cur = it->second;
    const float curAge = fabs(static_cast<float>(Globals::curTimeSeconds - cur.m_lastUpdateTime));
    if (curAge > MAX_AGE) {
      m_attractors.erase(it++);
    } else {
      ++it;
    }
  }
}

void Graph::unmarkAllNodesVisited() {
  for (size_t i=0; i<m_nodes.size(); i++) {
    m_nodes[i].m_visited = false;
    m_nodes[i].m_hops = 999999;
  }
}

void Graph::unmarkAllEdgesVisited() {
  for (size_t i=0; i<m_edges.size(); i++) {
    m_edges[i].m_visited = false;
    m_edges[i].m_hops = 999999;
  }
}

float Graph::hopsToHighlight(int hops, int searchDepth) {
  if (hops <= searchDepth) {
    return 1.0f - static_cast<float>(hops)/static_cast<float>(searchDepth);
    //return 1.0f;
  }
  return 0.0f;
}

void Graph::bfs(int startIdx, int maxDepth) {
  std::deque<int> nodeQueue;
  m_nodes[startIdx].m_hops = 0;
  nodeQueue.push_back(startIdx);

  while (!nodeQueue.empty()) {
    const int curIdx = nodeQueue.front();
    nodeQueue.pop_front();
    GraphNode& node = m_nodes[curIdx];
    node.m_visited = true;

    if (node.m_hops < maxDepth) {
      for (size_t i=0; i<m_edges.size(); i++) {
        GraphEdge& edge = m_edges[i];
        const int& idx1 = edge.m_source;
        const int& idx2 = edge.m_target;
        if (idx1 == curIdx) {
          GraphNode& neighbor = m_nodes[idx2];
          if (!neighbor.m_visited) {
            neighbor.m_hops = node.m_hops + 1;
            edge.m_hops = node.m_hops;
            edge.m_visited = true;
            nodeQueue.push_back(idx2);
          }
        } else if (idx2 == curIdx) {
          GraphNode& neighbor = m_nodes[idx1];
          if (!neighbor.m_visited) {
            edge.m_visited = true;
            edge.m_hops = node.m_hops;
            neighbor.m_hops = node.m_hops + 1;
            nodeQueue.push_back(idx1);
          }
        }
      }
    }
  }
}
