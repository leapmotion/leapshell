// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "StdAfx.h"
#include "Shell.h"
#include "Globals.h"

LeapShell::LeapShell() :
  m_render(nullptr)
{
  m_leapController.addListener(m_leapListener);
  m_state = new NavigationState();
  m_view = new View(*m_state);
  m_render = new Render();
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
  settings->setAlwaysOnTop(true);
  settings->setFullScreen(false);
  settings->setFrameRate(60.0f);
  settings->disableFrameRate();
#if defined(CINDER_MSW)
  settings->enableConsoleWindow(true);
#endif
  ci::gl::enableVerticalSync(false);
}

void LeapShell::setup()
{
  Globals::fontRegular = ci::gl::TextureFont::create(ci::Font(loadResource(RES_FONT_FREIGHTSANS_TTF), 36.0f));
  Globals::fontBold = ci::gl::TextureFont::create(ci::Font(loadResource(RES_FONT_FREIGHTSANSBOLD_TTF), 36.0f));

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
    }
  }

  m_view->Update();
}

void LeapShell::draw()
{
  ci::gl::clear();

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
}

void LeapShell::updateGlobals() {
  Globals::curTimeSeconds = ci::app::getElapsedSeconds();
  Globals::windowHeight = getWindowHeight();
  Globals::windowWidth = getWindowWidth();
  Globals::aspectRatio = getWindowAspectRatio();
}

CINDER_APP_BASIC(LeapShell, ci::app::RendererGl)
