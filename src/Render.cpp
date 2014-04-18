#include "StdAfx.h"
#include "Render.h"
#include "Utilities.h"
#include "Globals.h"

Render::Render() {
  
}

void Render::draw(const View& view) const {
  m_camera.lookAt(ToVec3f(view.Position()), ToVec3f(view.LookAt()), ToVec3f(view.Up()));
  m_camera.setPerspective(view.FOV(), static_cast<float>(Globals::aspectRatio), view.Near(), view.Far());
  ci::gl::setMatrices(m_camera);

  const TileVector& tiles = view.Tiles();
  for (TileVector::const_iterator it = tiles.begin(); it != tiles.end(); ++it) {
    drawTile(*it);
  }
}

void Render::drawTile(const Tile& tile) const {
  if (!tile.m_node) {
    return;
  }
  glPushMatrix();
  glTranslated(tile.m_position.x(), tile.m_position.y(), tile.m_position.z());

  // draw border
  const float halfWidth = static_cast<float>(tile.m_size.x()/2.0f);
  const float halfHeight = static_cast<float>(tile.m_size.y()/2.0f);
  const ci::Rectf rect(-halfWidth, -halfHeight, halfWidth, halfHeight);

  ci::gl::color(ci::ColorA(0.5f, 0.5f, 0.5f));
  ci::gl::drawSolidRoundedRect(rect, 2.0, 10);
  ci::gl::color(ci::ColorA::white());
  ci::gl::drawStrokedRoundedRect(rect, 2.0, 10);

  // draw text
  static const double TEXT_SCALE = 0.05;
  glPushMatrix();
  const std::string name = tile.m_node->get_metadata_as<std::string>("name");
  const ci::Vec2f nameSize = Globals::fontRegular->measureString(name);
  const ci::Rectf nameRect(-nameSize.x/2.0f, 0.0f, nameSize.x/2.0f, 100.0f);
  ci::gl::color(ci::ColorA::white());
  glScaled(TEXT_SCALE, -TEXT_SCALE, TEXT_SCALE);
  Globals::fontRegular->drawString(name, nameRect);
  glPopMatrix();

  glPopMatrix();
}
