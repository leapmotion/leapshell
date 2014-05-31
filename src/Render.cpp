#include "StdAfx.h"
#include "Render.h"
#include "Utilities.h"
#include "Globals.h"
#include "MeshHand.h"
#include "GLBuffer.h"

Render::Render() {
  m_parallaxSmoother.value = Vector2::Zero();
}

void Render::update_background(const ci::Surface8u& surface) {
  m_background = ci::gl::Texture::create(surface);
}

void Render::drawViewTiles(const View& view) const {
  static const float INACTIVE_VIEW_OPACITY = 0.2f;
  m_camera.lookAt(ToVec3f(view.Position()), ToVec3f(view.LookAt()), ToVec3f(view.Up()));
  m_camera.setPerspective(view.FOV(), static_cast<float>(Globals::aspectRatio), view.Near(), view.Far());
  ci::gl::setMatrices(m_camera);

  // draw tiles
  ci::gl::enableAlphaBlending();
  const TileVector& tiles = view.Tiles();
  const float opacity = (INACTIVE_VIEW_OPACITY + (1.0f-INACTIVE_VIEW_OPACITY)*view.Activation()) * view.TransitionOpacity();
  const Vector2 viewSize = view.ViewSizeAtPlane();

  // draw backmost tiles before all others
  for (TileVector::const_iterator it = tiles.begin(); it != tiles.end(); ++it) {
    const Tile& tile = *it;
    if (tile.IsVisible() && tile.Position().z() < 0 && tile.Activation() > 0.01f) {
      drawTile(tile, view.Forces(), opacity);
    }
  }

  // draw most of the tiles
  for (TileVector::const_iterator it = tiles.begin(); it != tiles.end(); ++it) {
    const Tile& tile = *it;
    if (tile.IsVisible() && tile.Activation() <= 0.01f && tileInView(viewSize, view.LookAt(), tile.Position())) {
      drawTile(tile, view.Forces(), opacity);
    }
  }

  // draw frontmost tiles after all others
  for (TileVector::const_iterator it = tiles.begin(); it != tiles.end(); ++it) {
    const Tile& tile = *it;
    if (tile.IsVisible() && tile.Position().z() >= 0 && tile.Activation() > 0.01f) {
      drawTile(tile, view.Forces(), opacity);
    }
  }
}

void Render::drawViewBackground(const View& view) const {
  if (m_background) {
    // compute ratios for where we are in the bounds
    static const double PADDING_SCALE = 1.5; // to compensate for rubber banding
    static const float PARALLAX_SMOOTH = 0.75f;
    Vector2 cameraMin = view.GetPositionLayout()->GetCameraMinBounds();
    Vector2 cameraMax = view.GetPositionLayout()->GetCameraMaxBounds();
    const Vector2 center = (cameraMax + cameraMin)/2.0;
    cameraMin = PADDING_SCALE * (cameraMin - center) + center;
    cameraMax = PADDING_SCALE * (cameraMax - center) + center;
    const Vector2 size = cameraMax - cameraMin;
    const double ratioX = size.x() > 0 ? (view.LookAt().x() - cameraMin.x()) / size.x() : 0.5;
    const double ratioY = size.y() > 0 ? (view.LookAt().y() - cameraMin.y()) / size.y() : 0.5;

    // compute the amount of parallax
    Vector2 parallax((ratioX-1.0)*(m_background->getWidth()-Globals::windowWidth), (ratioY-1.0)*(m_background->getHeight() - Globals::windowHeight));

    // apply smoothing to parallax based on transition/switch time to smooth harsh jumps
    const float multSinceTransition = static_cast<float>(1.0 - std::min(1.0, (Globals::curTimeSeconds - std::max(Globals::lastTileTransitionTime, Globals::lastTileSwitchTime))));
    const float smooth = PARALLAX_SMOOTH + (1.0f-PARALLAX_SMOOTH)*multSinceTransition;
    m_parallaxSmoother.Update(parallax, Globals::curTimeSeconds, smooth);

    // draw a rectangle translated by the current ratios
    ci::gl::color(ci::ColorA::white());
    m_background->bind();
    glPushMatrix();
    glTranslated(m_parallaxSmoother.value.x(), m_parallaxSmoother.value.y(), 0);
    ci::gl::drawSolidRect(ci::Rectf(0.0f, 0.0f, static_cast<float>(m_background->getWidth()), static_cast<float>(m_background->getHeight())));
    glPopMatrix();
    m_background->unbind();
  }
}

void Render::drawHands(const View& view, MeshHand& handL, MeshHand& handR) const {
#if 0
  const Vector3 lPos = handL.LeapHand().palmPosition().toVector3<Vector3>() + Globals::LEAP_OFFSET + view.LookAt();
  ci::gl::drawSphere(ToVec3f(lPos), 5, 30);

  const Vector3 rPos = handR.LeapHand().palmPosition().toVector3<Vector3>() + Globals::LEAP_OFFSET + view.LookAt();
  ci::gl::drawSphere(ToVec3f(rPos), 5, 30);
#endif

  const ci::Area origViewport = ci::gl::getViewport();
  ci::gl::setViewport(Globals::handsFbo.getBounds());
  Globals::handsFbo.bindFramebuffer();
  ci::gl::clear(ci::ColorA(0.0f, 0.0f, 0.0f, 0.0f));

  ci::gl::GlslProg& shader = Globals::handsShader;
  shader.bind();
  GLint vertex = shader.getAttribLocation("vertex");
  GLint normal = shader.getAttribLocation("normal");
  GLint color = shader.getAttribLocation("color");
  MeshHand::SetAttributeIndices(vertex, normal, color);
  ci::Matrix44f transformMatrix = ci::gl::getModelView();
  transformMatrix.translate(ToVec3f(view.LookAt()));
  ci::Matrix44f normalMatrix = transformMatrix.inverted().transposed();
  ci::Matrix44f projectionMatrix = ci::gl::getProjection();

  // set uniforms
  shader.uniform("transformMatrix", transformMatrix);
  shader.uniform("normalMatrix", normalMatrix);
  shader.uniform("projectionMatrix", projectionMatrix);
  shader.uniform("lightPosition1", ci::Vec3f(1000, 500, 0));
  shader.uniform("lightPosition2", ci::Vec3f(-1000, 500, 0));
  shader.uniform("cameraPosition", ci::Vec3f(0, 0, 0));
  shader.uniform("ambientFactor", 1.0f);
  shader.uniform("specularPower", 10.0f);
  shader.uniform("specularFactor", 1.0f);
  shader.uniform("diffuseFactor", 0.0f);
  shader.uniform("rimColor", ci::Color::white());
  shader.uniform("rimStart", 0.5f);
  shader.uniform("innerTransparency", 0.9f);

  handL.SetScale(0.6f);
  handR.SetScale(0.6f);

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

void Render::drawViewBounds(const View& view, const ci::ColorA& color) const {
  const Vector2 min = view.GetPositionLayout()->GetContentsMinBounds() - view.LookAt().head<2>();
  const Vector2 max = view.GetPositionLayout()->GetContentsMaxBounds() - view.LookAt().head<2>();

  ci::Rectf rect(min.x(), min.y(), max.x(), max.y());
  ci::gl::color(color);
  ci::gl::drawStrokedRect(rect);
}

void Render::drawTile(const Tile& tile, const ForceVector& forces, float tileOpacity) const {
  if (!tile.Node()) {
    return;
  }
  glPushMatrix();

  const std::string name = tile.Node()->get_metadata_as<std::string>("name");

  // compute tile opacity
  const float opacity = tile.TransitionWarmupFactor() * tileOpacity;

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
  const ci::ColorA highlightColor = ci::ColorA(0.7f, 0.7f, 0.7f, 0.05f * highlight * opacity);
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

ci::ColorA Render::blendColors(const ci::ColorA& c1, const ci::ColorA& c2, float blend) {
  const float r = blend*c1.r + (1.0f-blend)*c2.r;
  const float g = blend*c1.g + (1.0f-blend)*c2.g;
  const float b = blend*c1.b + (1.0f-blend)*c2.b;
  const float a = blend*c1.a + (1.0f-blend)*c2.a;
  return ci::ColorA(r, g, b, a);
}

bool Render::tileInView(const Vector2& viewSize, const Vector3& lookat, const Vector3& tilePosition) {
  static const double VIEW_SCALE = 1.25;
  const double maxX = VIEW_SCALE * viewSize.x() * 0.5;
  const double maxY = VIEW_SCALE * viewSize.y() * 0.5;
  const Vector3 pos = tilePosition - lookat;
  return ((pos.x() > -maxX) && (pos.x() < maxX) && (pos.y() > -maxY) && (pos.y() < maxY));
}
