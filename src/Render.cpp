#include "StdAfx.h"
#include "Render.h"
#include "Utilities.h"
#include "Globals.h"
#include "MeshHand.h"
#include "GLBuffer.h"

Render::Render() {
}

void Render::update_background(const ci::Surface8u& surface) {
  m_background = ci::gl::Texture::create(surface);
}

void Render::draw(const View& view) const {
  if (m_background) {
    ci::gl::draw(m_background);
  }
  m_camera.lookAt(ToVec3f(view.Position()), ToVec3f(view.LookAt()), ToVec3f(view.Up()));
  m_camera.setPerspective(view.FOV(), static_cast<float>(Globals::aspectRatio), view.Near(), view.Far());
  ci::gl::setMatrices(m_camera);

  // draw tiles
  ci::gl::enableAlphaBlending();
  const TileVector& tiles = view.Tiles();
  const float transitionOpacity = view.TransitionOpacity();

  // draw backmost tiles before all others
  for (TileVector::const_iterator it = tiles.begin(); it != tiles.end(); ++it) {
    const Tile& tile = *it;
    if (tile.Position().z() < 0 && tile.Activation() > 0.01f) {
      drawTile(tile, view.Forces(), transitionOpacity);
    }
  }

  // draw most of the tiles
  for (TileVector::const_iterator it = tiles.begin(); it != tiles.end(); ++it) {
    const Tile& tile = *it;
    if (tile.Activation() < 0.01f) {
      drawTile(tile, view.Forces(), transitionOpacity);
    }
  }

  // draw frontmost tiles after all others
  for (TileVector::const_iterator it = tiles.begin(); it != tiles.end(); ++it) {
    const Tile& tile = *it;
    if (tile.Position().z() >= 0 && tile.Activation() > 0.01f) {
      drawTile(tile, view.Forces(), transitionOpacity);
    }
  }

  drawHands(view);
}

void Render::drawTile(const Tile& tile, const ForceVector& forces, float transitionOpacity) const {
  if (!tile.Node()) {
    return;
  }
  glPushMatrix();

  // compute tile opacity
  const float opacity = tile.CreationWarmupFactor() * transitionOpacity;

  const Vector3 tileSize = tile.Size();
  float highlight = tile.Highlight();
  float activation = tile.Activation();
  
  // add forces from other tiles
  Vector3 tilePosition = tile.Position();
  Vector3 totalForce = Vector3::Zero();
  for (size_t i=0; i<forces.size(); i++) {
    totalForce += forces[i].ForceAt(tilePosition);
  }
  tilePosition += totalForce;
  glTranslated(tilePosition.x(), tilePosition.y(), tilePosition.z());

  // add some bonus scaling depending on highlight strength
  static const float HIGHLIGHT_BONUS_SCALE = 0.2f;
  const float scale = 1.0f + tile.Highlight()*HIGHLIGHT_BONUS_SCALE;
  glScalef(scale, scale, scale);

  const float halfWidth = static_cast<float>(tileSize.x()/2.0f);
  const float halfHeight = static_cast<float>(tileSize.y()/2.0f);
  const ci::Rectf rect(-halfWidth, -halfHeight, halfWidth, halfHeight);

  // draw border
  const ci::ColorA activeColor = ci::ColorA(0.9f, 0.9f, 0.9f, 0.65f * activation * opacity);
  const ci::ColorA highlightColor = ci::ColorA(0.7f, 0.7f, 0.7f, 0.4f * highlight * opacity);
  const ci::ColorA blended = blendColors(activeColor, highlightColor, activation);
  ci::gl::color(blended);
  ci::gl::drawSolidRoundedRect(rect, 2.0, 10);
  ci::gl::color(ci::ColorA(1.0f, 1.0f, 1.0f, 0.6f * opacity * std::max(activation, highlight)));
  ci::gl::drawStrokedRoundedRect(rect, 2.0, 10);

  if (!tile.Icon()) {
    ci::Surface8u icon = tile.Node()->icon();
    if (icon) {
      tile.Icon() = ci::gl::Texture::create(icon);
    }
  }
  if (tile.Icon()) {
    // draw the icon
    glPushMatrix();
    glScaled(1, -1, 1);
    ci::gl::color(ci::ColorA(1.0f, 1.0f, 1.0f, opacity));
    ci::gl::draw(tile.Icon(), rect);
    glPopMatrix();
  }

  // draw text
  const ci::ColorA nameColor = ci::ColorA(1.0f, 1.0f, 1.0f, opacity);
  const ci::ColorA shadowColor = ci::ColorA(0.1f, 0.1f, 0.1f, opacity);
  static const ci::Vec2f shadowOffset = ci::Vec2f(5.0, 7.0f);
  glPushMatrix();
  const std::string name = tile.Node()->get_metadata_as<std::string>("name");
  const ci::Vec2f nameSize = Globals::fontRegular->measureString(name);
  const ci::Rectf nameRect(-nameSize.x/2.0f, 0.0f, nameSize.x/2.0f + shadowOffset.x, 100.0f);

  const double TEXT_SCALE = std::min(0.03, tileSize.x() / nameSize.x);
  static const double NAME_OFFSET = 1.0;
  glTranslated(0.0, -tileSize.y()/2.0 - NAME_OFFSET, 0.0);
  glScaled(TEXT_SCALE, -TEXT_SCALE, TEXT_SCALE);
  glTranslated(0, -Globals::FONT_SIZE/2.0, 0.0);
  ci::gl::color(shadowColor);
  Globals::fontRegular->drawString(name, nameRect, shadowOffset);
  ci::gl::color(nameColor);
  Globals::fontRegular->drawString(name, nameRect);
  glPopMatrix();

  glPopMatrix();
}

void Render::drawHands(const View& view) const {
  const ci::Area origViewport = ci::gl::getViewport();
  ci::gl::setViewport(Globals::handsFbo.getBounds());
  Globals::handsFbo.bindFramebuffer();
  ci::gl::clear(ci::ColorA(0.0f, 0.0f, 0.0f, 0.0f));

  MeshHand& handL = view.LeftHand();
  MeshHand& handR = view.RightHand();

  ci::gl::GlslProg& shader = Globals::handsShader;
  shader.bind();
  GLint vertex = shader.getAttribLocation("vertex");
  GLint normal = shader.getAttribLocation("normal");
  GLint color = shader.getAttribLocation("color");
  MeshHand::SetAttributeIndices(vertex, normal, color);
  ci::Matrix44f transformMatrix = ci::Matrix44f::identity();
  ci::Matrix44f normalMatrix = transformMatrix.inverted().transposed();
  ci::Matrix44f projectionMatrix = ci::gl::getProjection();

  // set uniforms
  shader.uniform("transformMatrix", transformMatrix);
  shader.uniform("normalMatrix", normalMatrix);
  shader.uniform("projectionMatrix", projectionMatrix);
  shader.uniform("lightPosition1", ci::Vec3f(1000, 500, 0));
  shader.uniform("lightPosition2", ci::Vec3f(-1000, 500, 0));
  shader.uniform("cameraPosition", ci::Vec3f(0, 0, 0));
  shader.uniform("ambientFactor", 0.0f);
  shader.uniform("specularPower", 1.0f);
  shader.uniform("specularFactor", 0.0f);
  shader.uniform("diffuseFactor", 0.0f);
  shader.uniform("rimColor", ci::Color::white());
  shader.uniform("rimStart", 0.5f);
  shader.uniform("innerTransparency", 1.0f);

  handL.SetScale(0.75f);
  handR.SetScale(0.75f);

  static const float FADE_TIME = 1.0f;

  ci::gl::enableDepthRead();
  ci::gl::enableDepthWrite();
  ci::gl::disableAlphaBlending();

  const float activeRatioL = 1.0f - SmootherStep(static_cast<float>(std::min(1.0, (Globals::curTimeSeconds - handL.LastUpdateTime())/FADE_TIME)));
  if (activeRatioL > 0.001f) {
    shader.uniform("opacity", activeRatioL);
    handL.Draw();
  }

  const float activeRatioR = 1.0f - SmootherStep(static_cast<float>(std::min(1.0, (Globals::curTimeSeconds - handR.LastUpdateTime())/FADE_TIME)));
  if (activeRatioR > 0.001f) {
    shader.uniform("opacity", activeRatioR);
    handR.Draw();
  }

  ci::gl::disableDepthRead();
  ci::gl::disableDepthWrite();
  ci::gl::enableAlphaBlending();

  ci::gl::color(ci::ColorA::white());
  Globals::handsFbo.unbindFramebuffer();

  shader.unbind();

  // draw hands FBO to screen
  ci::gl::setMatricesWindow(static_cast<int>(Globals::windowWidth), static_cast<int>(Globals::windowHeight));
  ci::gl::enableAdditiveBlending();
  ci::gl::setViewport(origViewport);
  Globals::handsFbo.bindTexture();
  const ci::Rectf rect(0.0f, static_cast<float>(Globals::windowHeight), static_cast<float>(Globals::windowWidth), 0.0f);
  ci::gl::drawSolidRect(rect);
  Globals::handsFbo.unbindTexture();
  ci::gl::enableAlphaBlending();
}

ci::ColorA Render::blendColors(const ci::ColorA& c1, const ci::ColorA& c2, float blend) {
  const float r = blend*c1.r + (1.0f-blend)*c2.r;
  const float g = blend*c1.g + (1.0f-blend)*c2.g;
  const float b = blend*c1.b + (1.0f-blend)*c2.b;
  const float a = blend*c1.a + (1.0f-blend)*c2.a;
  return ci::ColorA(r, g, b, a);  
}
