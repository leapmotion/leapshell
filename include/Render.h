#ifndef __Render_h__
#define __Render_h__

#include "View.h"
#include "Tile.h"
#include "MeshHand.h"

class Render {
public:
  Render();
  void drawViewTiles(const View& view) const;
  void drawViewBackground(const View& view) const;
  void drawHands(const View& view, MeshHand& handL, MeshHand& handR) const;

  void update_background(const ci::Surface8u& surface);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  void drawTile(const Tile& tile, const ForceVector& forces, float transitionOpacity) const;
  static ci::ColorA blendColors(const ci::ColorA& c1, const ci::ColorA& c2, float blend);
  static bool tileInView(const Vector2& viewSize, const Vector3& lookat, const Vector3& tilePosition);

  mutable ExponentialFilter<Vector2> m_parallaxSmoother;
  mutable ci::CameraPersp m_camera;
  mutable ci::gl::TextureRef m_background;

};

#endif
