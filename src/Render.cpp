#include "StdAfx.h"
#include "Render.h"
#include "Utilities.h"
#include "Globals.h"
#if defined(CINDER_COCOA)
#include "NSImageExt.h"
#endif

Render::Render() {
  
}

void Render::update_background(int width, int height)
{
#if defined(CINDER_COCOA)
  @autoreleasepool {
    NSScreen* screen = [NSScreen mainScreen];

    NSSize size = NSMakeSize(width, height);
    NSImage* nsImage = [[[NSImage alloc] initWithContentsOfURL:[[NSWorkspace sharedWorkspace] desktopImageURLForScreen:screen]] imageByScalingProportionallyToSize:size];
    NSBitmapImageRep* nsBitmapImageRep = [NSBitmapImageRep imageRepWithData:[nsImage TIFFRepresentation]];
    NSBitmapFormat nsBitmapFormat = [nsBitmapImageRep bitmapFormat];
    unsigned char *srcBytes = [nsBitmapImageRep bitmapData];

    size = [nsImage size];
    const int32_t width = static_cast<int32_t>(size.width);
    const int32_t height = static_cast<int32_t>(size.height);
    const int32_t srcRowBytes = [nsBitmapImageRep bytesPerRow];

    ci::Surface8u surface = ci::Surface8u(width, height, true,
        (nsBitmapFormat & NSAlphaFirstBitmapFormat) ? ci::SurfaceChannelOrder::ARGB :ci::SurfaceChannelOrder::RGBA);
    surface.setPremultiplied((nsBitmapFormat & NSAlphaNonpremultipliedBitmapFormat) == 0);
    unsigned char* dstBytes = surface.getData();
    int32_t dstRowBytes = width*4;

    for (int32_t i = 0; i < height; i++) {
      ::memcpy(dstBytes, srcBytes, dstRowBytes);
      dstBytes += dstRowBytes;
      srcBytes += srcRowBytes;
    }
    m_background = ci::gl::Texture::create(surface);
  }
#endif
}

void Render::draw(const View& view) const {
  if (m_background) {
    ci::gl::draw(m_background);
  }
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

  const float halfWidth = static_cast<float>(tile.m_size.x()/2.0f);
  const float halfHeight = static_cast<float>(tile.m_size.y()/2.0f);
  const ci::Rectf rect(-halfWidth, -halfHeight, halfWidth, halfHeight);

  if (!tile.m_icon) {
    ci::Surface8u icon = tile.m_node->icon();
    if (icon) {
      tile.m_icon = ci::gl::Texture::create(icon);
    }
  }
  if (tile.m_icon) {
    // draw the icon
    ci::gl::draw(tile.m_icon, rect);
  } else {
    // draw border
    ci::gl::color(ci::ColorA(0.5f, 0.5f, 0.5f));
    ci::gl::drawSolidRoundedRect(rect, 2.0, 10);
    ci::gl::color(ci::ColorA::white());
    ci::gl::drawStrokedRoundedRect(rect, 2.0, 10);
  }

  // draw text
  glPushMatrix();
  const std::string name = tile.m_node->get_metadata_as<std::string>("name");
  const ci::Vec2f nameSize = Globals::fontRegular->measureString(name);
  const ci::Rectf nameRect(-nameSize.x/2.0f, 0.0f, nameSize.x/2.0f, 100.0f);

  const double TEXT_SCALE = std::min(0.03, tile.m_size.x() / nameSize.x);
  static const double NAME_OFFSET = 1.0;
  ci::gl::color(ci::ColorA::white());
  glTranslated(0.0, -tile.m_size.y()/2.0 - NAME_OFFSET, 0.0);
  glScaled(TEXT_SCALE, -TEXT_SCALE, TEXT_SCALE);
  glTranslated(0, -Globals::FONT_SIZE/2.0, 0);
  Globals::fontRegular->drawString(name, nameRect);
  glPopMatrix();

  glPopMatrix();
}
