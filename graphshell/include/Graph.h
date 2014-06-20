#ifndef __Graph_h__
#define __Graph_h__

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

  void AddNode(int id);
  Node& GetNode(int id);
  void RemoveNode(int id);
  void AddEdge(int fromId, int toId);
  void RemoveEdge(int fromId, int toId);

private:

  NodeMap m_nodes;

};

#endif
