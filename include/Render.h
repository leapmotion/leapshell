#ifndef __Render_h__
#define __Render_h__

#include "View.h"
#include "Tile.h"

class Render {
public:
  Render();
  void draw(const View& view) const;

  void update_background(const ci::Surface8u& surface);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  void drawTile(const Tile& tile, const ForceVector& forces, float transitionOpacity, const std::string& searchFilter) const;
  void drawHands(const View& view) const;
  static ci::ColorA blendColors(const ci::ColorA& c1, const ci::ColorA& c2, float blend);
  static float getSearchFilterMult(const std::string& name, const std::string& searchFilter, bool anywhere);

  mutable ExponentialFilter<Vector2> m_parallaxSmoother;
  mutable ci::CameraPersp m_camera;
  mutable ci::gl::TextureRef m_background;

};

#endif
