#include "StdAfx.h"
#include "GraphShell.h"
#include "Globals.h"

GraphShell::GraphShell()
{
#if defined(CINDER_MSW)
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
  m_leapController.addListener(m_leapListener);
  m_lastDrawTime = 0;
  m_translation = Vector3::Zero();
  m_rotation = Matrix4x4::Identity();
  m_scale = 1.0f;
  m_transform = Matrix4x4::Identity();
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

    m_graph.m_initialPosProg = ci::gl::GlslProg(loadResource(RES_INITIAL_POS_VERT_GLSL), loadResource(RES_INITIAL_POS_FRAG_GLSL));
    m_graph.m_nbodyProg = ci::gl::GlslProg(loadResource(RES_NBODY_VERT_GLSL), loadResource(RES_NBODY_FRAG_GLSL));
    m_graph.m_edgeProg = ci::gl::GlslProg(loadResource(RES_EDGE_FORCE_VERT_GLSL), loadResource(RES_EDGE_FORCE_FRAG_GLSL));
    m_graph.m_drawEdgesProg = ci::gl::GlslProg(loadResource(RES_EDGES_VERT_GLSL), loadResource(RES_EDGES_FRAG_GLSL));
    m_graph.m_drawVerticesProg = ci::gl::GlslProg(loadResource(RES_VERTICES_VERT_GLSL), loadResource(RES_VERTICES_FRAG_GLSL));

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
      m_interaction.Update(frame);
      // update something with Leap data here
    }
  }
}

void GraphShell::draw()
{
  const double deltaTime = Globals::curTimeSeconds - m_lastDrawTime;
  const Vector3 translation = -m_interaction.TranslationVelocity() * deltaTime;
  const Matrix4x4 rotationInv = m_rotation;// .inverse();
  const Vector3& transformCenter = m_interaction.TransformCenter();

  //m_translation += (rotationInv * Vector4(translation.x(), translation.y(), translation.z(), 0.0)).head<3>();
  m_translation += translation;

  m_transform = TranslationMatrix(translation) * m_transform;

  m_scale += (m_interaction.ScaleVelocity() * deltaTime);

  const Vector3 rotation = deltaTime * m_interaction.RotationVelocity();
  const double rotationAngle = rotation.norm();
  const Vector3 rotationAxis = rotation / rotationAngle;
  if (abs(rotationAngle) > 0.001) {
    const Matrix4x4 rotationMatrix = RotationMatrix(rotationAxis, -rotationAngle);
    //m_rotation = m_rotation * rotationMatrix;
    m_rotation = rotationMatrix * m_rotation;
    //m_transform = TranslationMatrix(-m_translation) * m_transform;
    m_transform = TranslationMatrix(transformCenter) * m_transform;
    m_transform = rotationMatrix * m_transform;
    m_transform = TranslationMatrix(-transformCenter) * m_transform;
    //m_transform = TranslationMatrix(m_translation) * m_transform;
  }

  m_transform = TranslationMatrix(transformCenter) * m_transform;
  m_transform = ScaleMatrix(Vector3::Constant(1.0 + m_interaction.ScaleVelocity() * deltaTime)) * m_transform;
  m_transform = TranslationMatrix(-transformCenter) * m_transform;

  const Vector3 forwardOrig = Vector3::UnitZ();
  const Vector3 upOrig = Vector3::UnitY();

  const Vector3 forward = (m_rotation * Vector4(forwardOrig.x(), forwardOrig.y(), forwardOrig.z(), 0.0)).head<3>();
  const Vector3 up = (m_rotation * Vector4(upOrig.x(), upOrig.y(), upOrig.z(), 0.0)).head<3>();

  ci::gl::clear();
  ci::gl::color(ci::ColorA::white());

  // setup perspective camera
  m_camera.setPerspective(60.0f, Globals::aspectRatio, 0.1f, 1000.0f);
  m_camera.lookAt(ci::Vec3f(0.0f, 0.0f, -3.0f), ci::Vec3f(0.0f, 0.0f, 0.0f), ci::Vec3f(0.0f, 1.0f, 0.0f));
  //const ci::Vec3f lookat = ToVec3f(m_translation);
  //const ci::Vec3f pos = lookat + ci::Vec3f::zAxis();
  //const ci::Vec3f up = ci::Vec3f::yAxis();
  //m_camera.lookAt(ToVec3f(m_translation), ToVec3f(m_translation + forward), ToVec3f(up));
  ci::gl::setMatrices(m_camera);

  // animate light position
  //ci::Vec3f lightPosition = ci::Vec3f(2.0f * static_cast<float>(std::sin(Globals::curTimeSeconds)), 0.0f, 2.0f * static_cast<float>(std::cos(Globals::curTimeSeconds)));
  
  const Vector3 side = forward.cross(up).normalized();

  Matrix4x4 rotMat = Matrix4x4::Identity();
  rotMat.block<3, 3>(0, 0) << side, up, forward;


  // draw sphere to FBO
  m_screenFbo.bindFramebuffer();
  ci::gl::clear();
  m_interaction.Draw();
  m_objectShader.bind();
  GLint vertexAttr = m_objectShader.getAttribLocation("vertex");
  GLint normalAttr = m_objectShader.getAttribLocation("normal");
  m_objectShader.uniform("surfaceColor", ci::Vec3f(0.5f, 0.7f, 1.0f));
  m_objectShader.uniform("ambient", 0.1f);
  glPushMatrix();
#if 0
  ci::gl::translate(ToVec3f(m_translation));
  glMultMatrixd(rotMat.data());
  glScalef(m_scale, m_scale, m_scale);
#else
  glMultMatrixd(m_transform.data());
#endif
  for (double x = -50; x <= 50; x += 10) {
    for (double y = -50; y <= 50; y += 10) {
      for (double z = -50; z <= 50; z += 10) {
        glPushMatrix();
        glTranslated(x, y, z);
        ci::Vec3f lightPosition = ToVec3f(m_interaction.TransformCenter() + Globals::LEAP_OFFSET);
        lightPosition.y *= -1;
        lightPosition.x -= x;
        lightPosition.y -= y;
        lightPosition.z -= z;
        m_objectShader.uniform("lightPosition", lightPosition);
        m_geometry.Draw(vertexAttr, normalAttr);
        glPopMatrix();
      }
    }
  }
  glPopMatrix();
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

  m_lastDrawTime = Globals::curTimeSeconds;
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
