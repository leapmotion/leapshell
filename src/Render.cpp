#include "StdAfx.h"
#include "Render.h"
#include "Utilities.h"
#include "Globals.h"

DummyNode::DummyNode() {
  m_position = 100 * Vector3::Random();
  m_position.z() = 0.0;
  m_size = Vector3::Constant(10);
  m_name = "Node";
}

void DummyNode::draw() const {
  ci::gl::disableDepthRead();
  ci::gl::disableDepthWrite();
  glPushMatrix();
  glTranslated(m_position.x(), m_position.y(), m_position.z());

  // draw border
  const ci::Rectf rect(-m_size.x()/2.0, -m_size.y()/2.0, m_size.x()/2.0, m_size.y()/2.0);
  ci::gl::drawStrokedRoundedRect(rect, 2.0, 10);

  // draw text
  static const double TEXT_SCALE = 0.1;
  glPushMatrix();
  const ci::Vec2f nameSize = Globals::fontRegular->measureString(m_name);
  const ci::Rectf nameRect(-nameSize.x/2.0f, 0.0f, nameSize.x/2.0f, 100.0f);
  ci::gl::color(ci::ColorA::white());
  glScaled(TEXT_SCALE, -TEXT_SCALE, TEXT_SCALE);
  Globals::fontRegular->drawString(m_name, nameRect);
  glPopMatrix();

  glPopMatrix();
}

Render::Render() {
  m_position = 100 * Vector3::UnitZ();
  m_lookat = Vector3::Zero();
  m_up = Vector3::UnitY();
  m_fov = 80.0f;
  m_near = 1.0f;
  m_far = 10000.0f;

  static const int NUM_GRID = 10;
  const double size = 100;
  for (int i=0; i<NUM_GRID; i++) {
    const double ratioX = static_cast<double>(i) / static_cast<double>(NUM_GRID-1);
    const double x = -(size/2.0) + ratioX*size;
    for (int j=0; j<NUM_GRID; j++) {
      const double ratioY = static_cast<double>(j) / static_cast<double>(NUM_GRID-1);
      const double y = -(size/2.0) + ratioY*size;
      m_nodes.push_back(DummyNode());
      m_nodes.back().m_position << x, y, 0.0;
    }
  }
}

void Render::draw() const {
#if 0
  _camera.lookAt(_campos_smoother.value, _lookat_smoother.value, _up_smoother.value.normalized());
  _camera.setPerspective(80.0f + _fov_modifier.value, getWindowAspectRatio(), 1.0f, 100000.f);
#endif
  
  m_camera.lookAt(ToVec3f(m_position), ToVec3f(m_lookat), ToVec3f(m_up));
  m_camera.setPerspective(m_fov, static_cast<float>(Globals::aspectRatio), m_near, m_far);
  ci::gl::setMatrices(m_camera);

  for (size_t i=0; i<m_nodes.size(); i++) {
    m_nodes[i].draw();
  }

  
}
