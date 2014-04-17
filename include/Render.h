#ifndef __Render_h__
#define __Render_h__

#include "DataTypes.h"

class DummyNode {
public:
  DummyNode();
  void draw() const;
  ci::gl::TextureRef m_icon;
  Vector3 m_position;
  Vector3 m_size;
  std::string m_name;
  std::string m_type;
};

class Render {
public:
  Render();
  void draw() const;

private:
  std::vector<DummyNode> m_nodes;

  // render parameters
  mutable ci::CameraPersp m_camera;
  Vector3 m_position;
  Vector3 m_lookat;
  Vector3 m_up;
  float m_fov;
  float m_near;
  float m_far;

};

#endif
