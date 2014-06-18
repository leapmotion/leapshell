#ifndef __Render_h__
#define __Render_h__

#include "View.h"
#include "Tile.h"
#include "MeshHand.h"

class Render {
public:
  Render();
  void setViewMatrices(const View& view) const;
  void drawViewTiles(const View& view, bool ignoreActivationOpacity) const;
  void drawViewBackground(const View& view) const;
  void drawHands(const View& view, MeshHand& handL, MeshHand& handR) const;
  void drawHandsToFBO(const View& view, MeshHand& handL, MeshHand& handR) const;
  void drawViewBounds(const View& view, const ci::ColorA& color) const;
  void drawWireHand(const Leap::Hand& hand) const;

  void drawHandsTexture() const;

  void update_background(const ci::Surface8u& surface);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  
  mutable ci::CameraPersp m_camera;

private:

  void drawTile(const Tile& tile, const ForceVector& forces, float tileOpacity) const;
  static ci::ColorA blendColors(const ci::ColorA& c1, const ci::ColorA& c2, float blend);
  static bool tileInView(const Vector2& viewSize, const Vector3& lookat, const Vector3& tilePosition);

  mutable ExponentialFilter<Vector2> m_parallaxSmoother;
  mutable ci::gl::TextureRef m_background;

};

#endif
