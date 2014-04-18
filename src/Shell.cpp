// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "StdAfx.h"
#include "Shell.h"
#include "Globals.h"
#include "Value.h"
#if defined(CINDER_COCOA)
#include "NSImageExt.h"
#endif

// this doesn't test all possible types, but tests that different types are
// distinguishable via Value::compare, and that values of the same type are
// distinguishable via Value::compare.
void unit_test_Value () {
  Value v_int_6(int(6));
  Value v_int_7(int(7));
  Value v_uint32_t_6(uint32_t(6));
  Value v_uint32_t_7(uint32_t(7));
  Value v_double_6(double(6));
  Value v_double_7(double(7));
  Value v_string_6(std::string("6"));
  Value v_string_7(std::string("7"));
  Value v_bool_false(false);
  Value v_bool_true(true);

  // value comparisons
  assert(v_int_6.compare(v_int_6) == 0);
  assert(v_int_6.compare(v_int_7) < 0);
  assert(v_int_7.compare(v_int_6) > 0);
  assert(v_int_7.compare(v_int_7) == 0);

  assert(v_uint32_t_6.compare(v_uint32_t_6) == 0);
  assert(v_uint32_t_6.compare(v_uint32_t_7) < 0);
  assert(v_uint32_t_7.compare(v_uint32_t_6) > 0);
  assert(v_uint32_t_7.compare(v_uint32_t_7) == 0);

  assert(v_double_6.compare(v_double_6) == 0);
  assert(v_double_6.compare(v_double_7) < 0);
  assert(v_double_7.compare(v_double_6) > 0);
  assert(v_double_7.compare(v_double_7) == 0);

  assert(v_string_6.compare(v_string_6) == 0);
  assert(v_string_6.compare(v_string_7) < 0);
  assert(v_string_7.compare(v_string_6) > 0);
  assert(v_string_7.compare(v_string_7) == 0);

  assert(v_bool_false.compare(v_bool_false) == 0);
  assert(v_bool_false.compare(v_bool_true) < 0);
  assert(v_bool_true.compare(v_bool_false) > 0);
  assert(v_bool_true.compare(v_bool_true) == 0);

  // type comparisons
  assert(v_int_6.compare(v_uint32_t_6) != 0);
  assert(v_int_6.compare(v_uint32_t_7) != 0);
  assert(v_int_6.compare(v_double_6) != 0);
  assert(v_int_6.compare(v_double_7) != 0);
  assert(v_int_6.compare(v_string_6) != 0);
  assert(v_int_6.compare(v_string_7) != 0);
  assert(v_int_6.compare(v_bool_false) != 0);
  assert(v_int_6.compare(v_bool_true) != 0);

  assert(v_int_7.compare(v_uint32_t_6) != 0);
  assert(v_int_7.compare(v_uint32_t_7) != 0);
  assert(v_int_7.compare(v_double_6) != 0);
  assert(v_int_7.compare(v_double_7) != 0);
  assert(v_int_7.compare(v_string_6) != 0);
  assert(v_int_7.compare(v_string_7) != 0);
  assert(v_int_7.compare(v_bool_false) != 0);
  assert(v_int_7.compare(v_bool_true) != 0);

  assert(v_uint32_t_6.compare(v_int_6) != 0);
  assert(v_uint32_t_6.compare(v_int_7) != 0);
  assert(v_uint32_t_6.compare(v_double_6) != 0);
  assert(v_uint32_t_6.compare(v_double_7) != 0);
  assert(v_uint32_t_6.compare(v_string_6) != 0);
  assert(v_uint32_t_6.compare(v_string_7) != 0);
  assert(v_uint32_t_6.compare(v_bool_false) != 0);
  assert(v_uint32_t_6.compare(v_bool_true) != 0);

  assert(v_uint32_t_7.compare(v_int_6) != 0);
  assert(v_uint32_t_7.compare(v_int_7) != 0);
  assert(v_uint32_t_7.compare(v_double_6) != 0);
  assert(v_uint32_t_7.compare(v_double_7) != 0);
  assert(v_uint32_t_7.compare(v_string_6) != 0);
  assert(v_uint32_t_7.compare(v_string_7) != 0);
  assert(v_uint32_t_7.compare(v_bool_false) != 0);
  assert(v_uint32_t_7.compare(v_bool_true) != 0);

  assert(v_double_6.compare(v_int_6) != 0);
  assert(v_double_6.compare(v_int_7) != 0);
  assert(v_double_6.compare(v_uint32_t_6) != 0);
  assert(v_double_6.compare(v_uint32_t_7) != 0);
  assert(v_double_6.compare(v_string_6) != 0);
  assert(v_double_6.compare(v_string_7) != 0);
  assert(v_double_6.compare(v_bool_false) != 0);
  assert(v_double_6.compare(v_bool_true) != 0);

  assert(v_double_7.compare(v_int_6) != 0);
  assert(v_double_7.compare(v_int_7) != 0);
  assert(v_double_7.compare(v_uint32_t_6) != 0);
  assert(v_double_7.compare(v_uint32_t_7) != 0);
  assert(v_double_7.compare(v_string_6) != 0);
  assert(v_double_7.compare(v_string_7) != 0);
  assert(v_double_7.compare(v_bool_false) != 0);
  assert(v_double_7.compare(v_bool_true) != 0);

  assert(v_string_6.compare(v_int_6) != 0);
  assert(v_string_6.compare(v_int_7) != 0);
  assert(v_string_6.compare(v_uint32_t_6) != 0);
  assert(v_string_6.compare(v_uint32_t_7) != 0);
  assert(v_string_6.compare(v_double_6) != 0);
  assert(v_string_6.compare(v_double_7) != 0);
  assert(v_string_6.compare(v_bool_false) != 0);
  assert(v_string_6.compare(v_bool_true) != 0);

  assert(v_string_7.compare(v_int_6) != 0);
  assert(v_string_7.compare(v_int_7) != 0);
  assert(v_string_7.compare(v_uint32_t_6) != 0);
  assert(v_string_7.compare(v_uint32_t_7) != 0);
  assert(v_string_7.compare(v_double_6) != 0);
  assert(v_string_7.compare(v_double_7) != 0);
  assert(v_string_7.compare(v_bool_false) != 0);
  assert(v_string_7.compare(v_bool_true) != 0);

  assert(v_bool_false.compare(v_int_6) != 0);
  assert(v_bool_false.compare(v_int_7) != 0);
  assert(v_bool_false.compare(v_uint32_t_6) != 0);
  assert(v_bool_false.compare(v_uint32_t_7) != 0);
  assert(v_bool_false.compare(v_double_6) != 0);
  assert(v_bool_false.compare(v_double_7) != 0);
  assert(v_bool_false.compare(v_string_6) != 0);
  assert(v_bool_false.compare(v_string_7) != 0);

  assert(v_bool_true.compare(v_int_6) != 0);
  assert(v_bool_true.compare(v_int_7) != 0);
  assert(v_bool_true.compare(v_uint32_t_6) != 0);
  assert(v_bool_true.compare(v_uint32_t_7) != 0);
  assert(v_bool_true.compare(v_double_6) != 0);
  assert(v_bool_true.compare(v_double_7) != 0);
  assert(v_bool_true.compare(v_string_6) != 0);
  assert(v_bool_true.compare(v_string_7) != 0);
}

LeapShell::LeapShell()
  :
  m_state(new NavigationState()),
  m_view(new View(m_state)),
  m_render(nullptr)
{
  m_leapController.addListener(m_leapListener);
  m_render = new Render();
  m_interaction = new Interaction();

  m_state->registerView(m_view);

  m_root = std::shared_ptr<FileSystemNode>(new FileSystemNode("/"));
  m_state->setCurrentLocation(m_root);

  unit_test_Value(); // TEMP until this is verified to work on all platforms
}

LeapShell::~LeapShell()
{
  delete m_render;
  m_leapController.removeListener(m_leapListener);
}

void LeapShell::prepareSettings(Settings* settings)
{
  settings->setWindowPos(100, 100);
  settings->setWindowSize(1024, 768);
  //settings->setBorderless(true);
  //settings->setAlwaysOnTop(true);
  settings->setFullScreen(true);
  settings->setFrameRate(60.0f);
  settings->disableFrameRate();
#if defined(CINDER_MSW)
  settings->enableConsoleWindow(true);
#endif
  ci::gl::enableVerticalSync(false);
}

void LeapShell::setup()
{
  Globals::fontRegular = ci::gl::TextureFont::create(ci::Font(loadResource(RES_FONT_FREIGHTSANS_TTF), Globals::FONT_SIZE));
  Globals::fontBold = ci::gl::TextureFont::create(ci::Font(loadResource(RES_FONT_FREIGHTSANSBOLD_TTF), Globals::FONT_SIZE));

  // disable depth test and enable alpha
  ci::gl::disableDepthRead();
  ci::gl::disableDepthWrite();
  ci::gl::enableAlphaBlending();

  m_params = ci::params::InterfaceGl::create(getWindow(), "App parameters", ci::app::toPixels(ci::Vec2i(200, 400)));
  m_params->minimize();
}

void LeapShell::shutdown()
{
}

void LeapShell::mouseDown(ci::app::MouseEvent event)
{
}

void LeapShell::mouseUp(ci::app::MouseEvent event)
{
}

void LeapShell::mouseMove(ci::app::MouseEvent event)
{
}

void LeapShell::mouseDrag(ci::app::MouseEvent event)
{
}

void LeapShell::keyDown(ci::app::KeyEvent event)
{
  switch (event.getChar()) {
  case '1': m_view->SetLayout(std::shared_ptr<Layout>(new GridLayout())); break;
  case '2': m_view->SetLayout(std::shared_ptr<Layout>(new RingLayout())); break;
  case ci::app::KeyEvent::KEY_ESCAPE:
    quit();
    break;
  default:
    break;
  }
}

void LeapShell::keyUp(ci::app::KeyEvent event)
{
}

void LeapShell::update()
{
  updateGlobals();

  std::deque<Leap::Frame> frames = m_leapListener.grabFrames();

  if (!frames.empty()) {
    for (auto iter = frames.cbegin(); iter != frames.cend(); ++iter) {
      const Leap::Frame& frame = *iter;
      m_interaction->Update(frame);
    }
  }

  m_view->Update();
}

void LeapShell::draw()
{
  ci::gl::clear();

  m_interaction->UpdateView(*m_view);
  m_params->draw();
  m_render->draw(*m_view);

  ci::gl::setMatricesWindow(getWindowSize());
  ci::gl::drawString("FPS: " + ci::toString(getAverageFps()), ci::Vec2f(10.0f, 10.0f), ci::ColorA::white(), ci::Font("Arial", 18));
}

void LeapShell::resize()
{
  const GLsizei width = static_cast<GLsizei>(getWindowWidth());
  const GLsizei height = static_cast<GLsizei>(getWindowHeight());

  glViewport(0, 0, width, height);
  updateGlobals();
#if defined(CINDER_COCOA)
  @autoreleasepool {
    NSScreen* screen = getDisplay()->getNsScreen();
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
    m_render->update_background(surface);
  }
#endif
}

void LeapShell::updateGlobals() {
  Globals::curTimeSeconds = ci::app::getElapsedSeconds();
  Globals::windowHeight = getWindowHeight();
  Globals::windowWidth = getWindowWidth();
  Globals::aspectRatio = getWindowAspectRatio();
}

CINDER_APP_BASIC(LeapShell, ci::app::RendererGl)
