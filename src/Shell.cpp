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
  m_view(nullptr),
  m_state(new NavigationState()),
  m_render(nullptr),
  m_lastTextChangeTime(0.0)
{
#if defined(CINDER_MSW)
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
  m_leapController.addListener(m_leapListener);
}

LeapShell::~LeapShell()
{
  delete m_render;
  m_leapController.removeListener(m_leapListener);
#if defined(CINDER_MSW)
  CoUninitialize();
#endif
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

  // setup hand meshes
  ci::DataSourceRef leftHand = loadResource(RES_LEFT_HAND_FBX);
  ci::DataSourceRef rightHand = loadResource(RES_RIGHT_HAND_FBX);
  MeshHand::SetMeshSources(leftHand, rightHand);

  // enable alpha
  ci::gl::enableAlphaBlending();

  // load shaders
  try {
    Globals::handsShader = ci::gl::GlslProg(loadResource(RES_HANDS_VERT_GLSL), loadResource(RES_HANDS_FRAG_GLSL));
  } catch (ci::gl::GlslProgCompileExc e) {
    std::cerr << e.what() << std::endl;
  }

  glEnable(GL_TEXTURE_2D);

  m_params = ci::params::InterfaceGl::create(getWindow(), "App parameters", ci::app::toPixels(ci::Vec2i(200, 400)));
  m_params->minimize();

  m_render = new Render();
  m_interaction = new Interaction();

  m_root = std::shared_ptr<FileSystemNode>(new FileSystemNode("/"));
  m_state->setCurrentLocation(m_root);

  // this is done after m_state->setCurrentLocation so the metadata keys are accessible to View.
  // also, view must be created after meshes.
  m_view = std::shared_ptr<View>(new View(m_state));
  m_state->registerView(m_view);

  // initial sorting criteria keys TEMP HACK for 2014.04.21 demo
  std::vector<std::string> prioritizedKeys;
  prioritizedKeys.push_back("name");
  prioritizedKeys.push_back("ext");
  prioritizedKeys.push_back("time");
  prioritizedKeys.push_back("size");
  m_view->SetPrioritizedKeys(prioritizedKeys);

  unit_test_Value(); // TEMP until this is verified to work on all platforms
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
  case '1':
    m_view->SetSizeLayout(std::shared_ptr<SizeLayout>(new UniformSizeLayout()));
    m_view->SetPositionLayout(std::shared_ptr<PositionLayout>(new GridLayout()));
    setText("Grid Layout");
    break;
  case '2':
    m_view->SetSizeLayout(std::shared_ptr<SizeLayout>(new UniformSizeLayout()));
    m_view->SetPositionLayout(std::shared_ptr<PositionLayout>(new RingLayout()));
    setText("Ring Layout");
    break;
  case '3':
    m_view->SetSizeLayout(std::shared_ptr<SizeLayout>(new UniformSizeLayout()));
    m_view->SetPositionLayout(std::shared_ptr<PositionLayout>(new LinearSpiralLayout()));
    setText("Spiral Layout");
    break;
  case '4':
    m_view->SetSizeLayout(std::shared_ptr<SizeLayout>(new ExponentialSpiralLayout()));
    m_view->SetPositionLayout(std::shared_ptr<PositionLayout>(new ExponentialSpiralLayout())); 
    setText("Exponential Spiral Layout");
    break;
  case '5':
    { // for clustering layout only TEMP HACK for 2014.04.21 demo
      std::vector<std::string> prioritizedKeys;
      prioritizedKeys.push_back("ext");
      prioritizedKeys.push_back("name");
      prioritizedKeys.push_back("time");
      prioritizedKeys.push_back("size");
      m_view->SetPrioritizedKeys(prioritizedKeys);
    }
    m_view->SetSizeLayout(std::shared_ptr<SizeLayout>(new UniformSizeLayout()));
    m_view->SetPositionLayout(std::shared_ptr<PositionLayout>(new BlobClusterLayout()));
    setText("Cluster Layout");
    break;

  case '`':
    m_state->navigateUp();
    break;

  case '7':
    m_view->PrioritizeKey("name");
    setText("Sort by Name");
    break;
  case '8':
    m_view->PrioritizeKey("ext");
    setText("Sort by Extension");
    break;
  case '9':
    m_view->PrioritizeKey("time");
    setText("Sort by Time");
    break;
  case '0':
    m_view->PrioritizeKey("size");
    setText("Sort by Size");
    break;

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

  m_view->PerFrameUpdate();
}

void LeapShell::draw()
{
  ci::gl::clear(ci::ColorA::gray(0.4f, 1.0f));

  m_interaction->UpdateView(*m_view);
  m_render->draw(*m_view);

#if 0
  m_params->draw();
  ci::gl::setMatricesWindow(getWindowSize());
  ci::gl::drawString("FPS: " + ci::toString(getAverageFps()), ci::Vec2f(10.0f, 10.0f), ci::ColorA::white(), ci::Font("Arial", 18));
#endif

  // draw indicator text
  static const float TEXT_FADE_TIME = 2.0f;
  const float timeSinceTextChange = static_cast<float>(Globals::curTimeSeconds - m_lastTextChangeTime);
  const float textOpacity = SmootherStep(1.0f - std::min(1.0f, timeSinceTextChange/TEXT_FADE_TIME));
  if (textOpacity > 0.01f) {
    const ci::ColorA textColor = ci::ColorA(1.0f, 1.0f, 1.0f, textOpacity);
    const ci::Vec2f textSize = Globals::fontBold->measureString(m_textString);
    const ci::Rectf textRect(-textSize.x/2.0f, 0.0f, textSize.x/2.0f, 100.0f);
    ci::gl::color(textColor);
    glPushMatrix();
    glTranslated(Globals::windowWidth/2.0, 0.875 * Globals::windowHeight, 0.0);
    Globals::fontBold->drawString(m_textString, textRect);
    glPopMatrix();
  }
}

void LeapShell::resize()
{
  updateGlobals();
  Globals::handsFbo = ci::gl::Fbo(static_cast<int>(Globals::windowWidth), static_cast<int>(Globals::windowHeight));
  glViewport(0, 0, static_cast<int>(Globals::windowWidth), static_cast<int>(Globals::windowHeight));
#if defined(CINDER_COCOA)
  @autoreleasepool {
    NSScreen* screen = getDisplay()->getNsScreen();
    const CGFloat scale = [[NSScreen mainScreen] backingScaleFactor];
    NSSize size = NSMakeSize(Globals::windowWidth/scale, Globals::windowHeight/scale);
    NSImage* nsImage = [[[NSImage alloc] initWithContentsOfURL:[[NSWorkspace sharedWorkspace] desktopImageURLForScreen:screen]]  imageByScalingProportionallyToSize:size fill:YES];
    NSBitmapImageRep* nsBitmapImageRep = [NSBitmapImageRep imageRepWithData:[nsImage TIFFRepresentation]];
    NSBitmapFormat nsBitmapFormat = [nsBitmapImageRep bitmapFormat];
    unsigned char *srcBytes = [nsBitmapImageRep bitmapData];

    size = [nsImage pixelSize];
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

void LeapShell::setText(const std::string& text) {
  m_textString = text;
  m_lastTextChangeTime = Globals::curTimeSeconds;
}

CINDER_APP_BASIC(LeapShell, ci::app::RendererGl)
