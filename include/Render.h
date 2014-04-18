#ifndef __Render_h__
#define __Render_h__

#include "DataTypes.h"
#include "View.h"
#include "Tile.h"

class Render {
public:
  Render();
  void draw(const View& view) const;

  void update_background(int width, int height);

private:

  void drawTile(const Tile& tile) const;

  mutable ci::CameraPersp m_camera;
  mutable ci::gl::TextureRef m_background;

};

#endif
