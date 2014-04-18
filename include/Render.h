#ifndef __Render_h__
#define __Render_h__

#include "DataTypes.h"
#include "View.h"

class Render {
public:
  Render();
  void draw(const View& view) const;

private:

  void drawTile(const Tile& tile) const;

  mutable ci::CameraPersp m_camera;

};

#endif
