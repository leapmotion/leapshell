#include "StdAfx.h"
#include "Graph.h"

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
