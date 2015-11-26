#ifndef __GraphAlt_h__
#define __GraphAlt_h__

class GraphNode;
typedef std::shared_ptr<GraphNode> GraphNodePtr;

class GraphNode {
public:

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  //private:
  //std::vector<GraphNodePtr> m_neighbors;
  //ExponentialFilter<Vector3> m_position;
  //std::vector<int> m_neighbors;
  inline float Weight() const { return m_weight; }
  const Vector3& Position() const { return m_position; }
  void SetPosition(const Vector3& position) { m_position = position; }
  Vector3 m_position;
  Vector3 m_desiredPosition;
  //int m_id;
  float m_weight;
  ci::ColorA m_color;
  std::string m_name;
  bool m_visited;
  int m_hops;
  ExponentialFilter<float> m_activation;
  ExponentialFilter<float> m_highlight;
};

struct GraphEdge {
public:
  ExponentialFilter<float> m_activation;
  int m_source;
  int m_target;
  float m_strength;
  float m_contractedDistance;
  float m_origDistance;
  ExponentialFilter<float> m_distance;
  bool m_visited;
  int m_hops;
};

struct Attractor {
  Attractor() {
    m_maxSearchDepth = 4;
    m_searchDepth = 1;
    m_prevClosestNode = -1;
    m_lastClosestSwitchTime = 0;
  }
  Vector3 m_position;
  float m_strength;
  double m_lastUpdateTime;
  double m_lastClosestSwitchTime;
  int m_maxSearchDepth;
  int m_searchDepth;
  int m_prevClosestNode;
};

typedef std::vector<GraphNode, Eigen::aligned_allocator<GraphNode> > GraphNodeVector;
typedef std::vector<GraphEdge> GraphEdgeVector;

class GraphAlt {
public:

  GraphAlt();
  enum GraphType { RANDOM, WEB, COMPANY, MUSIC };

  void Start(GraphType type);
  void Update();
  void Draw(const Matrix4x4& textTransform = Matrix4x4::Identity()) const;
  //void ClearAttractors();
  //void AddAttractor(const Vector3& position, float strength);
  void UpdateAttractor(int id, const Vector3& position, float strength);

  double m_gravity;
  double m_gravityRadius;
  double m_flattening;
  double m_alpha;
  double m_friction;
  double m_charge;
  double m_leap;
  double m_minEdgeDistance;
  double m_extraEdgeDistance;
  double m_weightPower;

private:
  void createRandomGraph(int numNodes);
  void createWebGraph();
  void createCompanyGraph();
  void createCompanyGraph2();
  void createMusicGraph();
  Vector3 getRandomPosition() const;
  void applyRelaxation();
  void applyGravity();
  void applyFlattening();
  void applyForces();
  void applyLeapForces();
  void resolveConstraints();
  void cleanupAttractors();

  void unmarkAllNodesVisited();
  void unmarkAllEdgesVisited();

  static float hopsToHighlight(int hops, int searchDepth);

  void bfs(int startIdx, int maxDepth);

  int retrieveOrAddNode(const std::string& name);

  void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
      elems.push_back(item);
    }
  }

  static const float EDGE_DISTANCE_SMOOTH;
  static const float HIGHLIGHT_SMOOTH;
  static const float CONTRACTION_DISTANCE_RATIO;

  //std::vector<Attractor, Eigen::aligned_allocator<Attractor> > m_attractors;

  typedef std::map<int, Attractor, std::less<int>, Eigen::aligned_allocator<std::pair<int, Attractor> > > AttractorMap;

  AttractorMap m_attractors;

  float m_maxWeight;
  GraphNodeVector m_nodes;
  GraphEdgeVector m_edges;

};

#endif
