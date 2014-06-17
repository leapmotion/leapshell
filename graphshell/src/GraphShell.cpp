#include "StdAfx.h"
#include "GraphShell.h"
#include "Globals.h"

GraphShell::GraphShell()
{
#if defined(CINDER_MSW)
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
  m_leapController.addListener(m_leapListener);
}

GraphShell::~GraphShell()
{
  m_leapController.removeListener(m_leapListener);
#if defined(CINDER_MSW)
  CoUninitialize();
#endif
}

void GraphShell::prepareSettings(Settings* settings)
{
  settings->setWindowPos(100, 100);
  settings->setWindowSize(1024, 768);
  //settings->setBorderless(true);
  //settings->setAlwaysOnTop(true);
  //settings->setFullScreen(true);
  settings->setFrameRate(60.0f);
#if defined(CINDER_MSW)
  settings->enableConsoleWindow(true);
#endif
}

void GraphShell::setup()
{
  // enable alpha
  ci::gl::enableAlphaBlending();

  // load shaders
  try {
    m_screenShader = ci::gl::GlslProg(loadResource(RES_PASSTHROUGH_VERT_GLSL), loadResource(RES_SCREEN_FRAG_GLSL));
    m_objectShader = ci::gl::GlslProg(loadResource(RES_MATERIAL_VERT_GLSL), loadResource(RES_MATERIAL_FRAG_GLSL));
  } catch (ci::gl::GlslProgCompileExc e) {
    std::cerr << e.what() << std::endl;
  }

  glEnable(GL_TEXTURE_2D);
  ci::gl::enableDepthRead();
  ci::gl::enableDepthWrite();

  m_params = ci::params::InterfaceGl::create(getWindow(), "App parameters", ci::app::toPixels(ci::Vec2i(200, 400)));
  m_params->minimize();

  m_geometry.Initialize();
}

void GraphShell::shutdown()
{
}

void GraphShell::mouseDown(ci::app::MouseEvent event)
{
}

void GraphShell::mouseUp(ci::app::MouseEvent event)
{
}

void GraphShell::mouseMove(ci::app::MouseEvent event)
{
}

void GraphShell::mouseDrag(ci::app::MouseEvent event)
{
}

void GraphShell::keyDown(ci::app::KeyEvent event)
{
  switch (event.getChar()) {
  case ci::app::KeyEvent::KEY_ESCAPE:
    quit();
    break;
  default:
    break;
  }
}

void GraphShell::keyUp(ci::app::KeyEvent event)
{
}

void GraphShell::update()
{
  updateGlobals();

  std::deque<Leap::Frame> frames = m_leapListener.grabFrames();

  if (!frames.empty()) {
    for (auto iter = frames.cbegin(); iter != frames.cend(); ++iter) {
      const Leap::Frame& frame = *iter;
      // update something with Leap data here
    }
  }
}

void GraphShell::draw()
{
  ci::gl::clear();
  ci::gl::color(ci::ColorA::white());

  // setup perspective camera
  m_camera.setPerspective(60.0f, Globals::aspectRatio, 0.1f, 1000.0f);
  m_camera.lookAt(ci::Vec3f(0.0f, 0.0f, -3.0f), ci::Vec3f(0.0f, 0.0f, 0.0f), ci::Vec3f(0.0f, 1.0f, 0.0f));
  ci::gl::setMatrices(m_camera);

  // animate light position
  ci::Vec3f lightPosition = ci::Vec3f(2.0f * static_cast<float>(std::sin(Globals::curTimeSeconds)), 0.0f, 2.0f * static_cast<float>(std::cos(Globals::curTimeSeconds)));

  // draw sphere to FBO
  m_screenFbo.bindFramebuffer();
  ci::gl::clear();
  m_objectShader.bind();
  GLint vertexAttr = m_objectShader.getAttribLocation("vertex");
  GLint normalAttr = m_objectShader.getAttribLocation("normal");
  m_objectShader.uniform("surfaceColor", ci::Vec3f(0.5f, 0.7f, 1.0f));
  m_objectShader.uniform("lightPosition", lightPosition);
  m_objectShader.uniform("ambient", 0.1f);
  m_geometry.Draw(vertexAttr, normalAttr);
  m_objectShader.unbind();
  m_screenFbo.unbindFramebuffer();

  // setup orthographic camera
  ci::gl::setMatricesWindow(getWindowSize());
  ci::Rectf rect(0.0f, 0.0f, static_cast<float>(Globals::windowWidth), static_cast<float>(Globals::windowHeight));

  // draw FBO to screen with post-processing
  m_screenFbo.bindTexture(0);
  m_screenShader.bind();
  m_screenShader.uniform("tex0", 0);
  ci::gl::drawSolidRect(rect);
  m_screenShader.unbind();
  m_screenFbo.unbindTexture();

  // draw UI
  ci::gl::disableDepthRead();
  ci::gl::disableDepthWrite();
  m_params->draw();
  ci::gl::drawString("FPS: " + ci::toString(getAverageFps()), ci::Vec2f(10.0f, 10.0f), ci::ColorA::white(), ci::Font("Arial", 18));
  ci::gl::enableDepthRead();
  ci::gl::enableDepthWrite();
}

void GraphShell::resize()
{
  updateGlobals();

  const int width = static_cast<int>(Globals::windowWidth);
  const int height = static_cast<int>(Globals::windowHeight);
  m_screenFbo = ci::gl::Fbo(width, height);
  glViewport(0, 0, width, height);
}

void GraphShell::updateGlobals() {
  Globals::curTimeSeconds = ci::app::getElapsedSeconds();
  Globals::windowHeight = getWindowHeight();
  Globals::windowWidth = getWindowWidth();
  Globals::aspectRatio = getWindowAspectRatio();
}

CINDER_APP_BASIC(GraphShell, ci::app::RendererGl)
//CINDER_APP_BASIC(GraphShell, ci::app::RendererGl(ci::app::RendererGl::AA_NONE))
