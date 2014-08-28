#include "StdAfx.h"
#include "GraphShell.h"
#include "Globals.h"

GraphShell::GraphShell() :
  m_useOculusRender(true),
  m_useOculusControl(true),
  m_allowUpdate(false)
{
#if defined(CINDER_MSW)
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
  m_leapController.addListener(m_leapListener);

  m_yaw = 0.0f;
  m_pitch = 0.0f;
  m_dist = 100.0f;

#if USE_LEAP_IMAGE_API
  m_numImages = 0;
  m_useDistortion = true;
  for (int i=0; i<8; i++) {
    m_distortionTexturesGL[i] = 0;
    m_haveDistortionTextures[i] = false;
  }
  //m_overlayFov = 104.0f;
  m_overlayFov = 104.0f;
  m_gamma = 1.4f;
  m_redX = 0.0;
#if 0
  m_redY = -0.25f;
  m_greenX = 0.75f;
  m_greenY = -0.25f;
  m_blueX = 0.75f;
  m_blueY = 0.0f;
#else
  m_redY = -0.5f;
  m_greenX = 0.5f;
  m_greenY = -0.5f;
  m_blueX = 0.5f;
#endif
  m_blueY = 0.0f;

#endif
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
  settings->disableFrameRate();
#if defined(CINDER_MSW)
  settings->enableConsoleWindow(true);
#endif
}

void GraphShell::setup()
{
  const std::string supportedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890().?!,:;'\"&*=+-/\\@#_[]<>%^llflfiphridsfflffiff\303\251\303\241\303\250\303\240";
  Globals::fontRegular = ci::gl::TextureFont::create(ci::Font(loadResource(RES_FONT_PROXIMA_NOVA_OTF), Globals::FONT_SIZE), ci::gl::TextureFont::Format(), supportedChars);

  // setup hand meshes
  ci::DataSourceRef leftHand = loadResource(RES_LEFT_HAND_FBX);
  ci::DataSourceRef rightHand = loadResource(RES_RIGHT_HAND_FBX);
  MeshHand::SetMeshSources(leftHand, rightHand);

  // enable alpha
  ci::gl::enableAlphaBlending();

  //ci::gl::disableVerticalSync();

  // load shaders
  try {
    m_screenShader = ci::gl::GlslProg(loadResource(RES_PASSTHROUGH_VERT_GLSL), loadResource(RES_SCREEN_FRAG_GLSL));
    m_objectShader = ci::gl::GlslProg(loadResource(RES_MATERIAL_VERT_GLSL), loadResource(RES_MATERIAL_FRAG_GLSL));

    m_graph.m_initialPosProg = ci::gl::GlslProg(loadResource(RES_INITIAL_POS_VERT_GLSL), loadResource(RES_INITIAL_POS_FRAG_GLSL));
    m_graph.m_nbodyProg = ci::gl::GlslProg(loadResource(RES_NBODY_VERT_GLSL), loadResource(RES_NBODY_FRAG_GLSL));
    m_graph.m_edgeProg = ci::gl::GlslProg(loadResource(RES_EDGE_FORCE_VERT_GLSL), loadResource(RES_EDGE_FORCE_FRAG_GLSL));
    m_graph.m_drawEdgesProg = ci::gl::GlslProg(loadResource(RES_EDGES_VERT_GLSL), loadResource(RES_EDGES_FRAG_GLSL));
    m_graph.m_drawVerticesProg = ci::gl::GlslProg(loadResource(RES_VERTICES_VERT_GLSL), loadResource(RES_VERTICES_FRAG_GLSL));

    Globals::handsShader = ci::gl::GlslProg(loadResource(RES_HANDS_VERT_GLSL), loadResource(RES_HANDS_FRAG_GLSL));
    Globals::distortionShader = ci::gl::GlslProg(loadResource(RES_DISTORTION_VERT_GLSL), loadResource(RES_DISTORTION_FRAG_GLSL));
#if USE_LEAP_IMAGE_API
    Globals::undistortionShader = ci::gl::GlslProg(loadResource(RES_PASSTHROUGH_VERT_GLSL), loadResource(RES_UNDISTORTION_FRAG_GLSL));
#endif

  } catch (ci::gl::GlslProgCompileExc e) {
    std::cerr << e.what() << std::endl;
  }

  glEnable(GL_TEXTURE_2D);
  ci::gl::enableDepthRead();
  ci::gl::enableDepthWrite();

  m_params = ci::params::InterfaceGl::create(getWindow(), "App parameters", ci::app::toPixels(ci::Vec2i(200, 400)));
  m_params->minimize();

  m_params->addParam("Gravity", &m_graphAlt.m_gravity, "min=0 max=1 step=0.001");
  m_params->addParam("Gravity Radius", &m_graphAlt.m_gravityRadius, "min=0 max=1000 step=10");
  m_params->addParam("Charge", &m_graphAlt.m_charge, "min=-1000 max=1000 step=0.1");
  m_params->addParam("Friction", &m_graphAlt.m_friction, "min=0 max=1 step=0.01");
  m_params->addParam("Alpha", &m_graphAlt.m_alpha, "min=0 max=1 step=0.01");
  m_params->addParam("View Distance", &m_graphRadius, "min=0 max=500 step=1");
  m_params->addParam("Flattening", &m_graphAlt.m_flattening, "min=0 max=1 step=0.01");
#if USE_LEAP_IMAGE_API
  m_params->addParam("Overlay FOV", &m_overlayFov, "min=10 max=170 step=1");
  m_params->addParam("Gamma", &m_gamma, "min=0.1 max=2.0 step=0.1");
  m_params->addParam("Red Offset X", &m_redX, "min=-1.0 max=1.0 step=0.05");
  m_params->addParam("Red Offset Y", &m_redY, "min=-1.0 max=1.0 step=0.05");
  m_params->addParam("Green Offset X", &m_greenX, "min=-1.0 max=1.0 step=0.05");
  m_params->addParam("Green Offset Y", &m_greenY, "min=-1.0 max=1.0 step=0.05");
  m_params->addParam("Blue Offset X", &m_blueX, "min=-1.0 max=1.0 step=0.05");
  m_params->addParam("Blue Offset Y", &m_blueY, "min=-1.0 max=1.0 step=0.05");
  m_params->addParam("World Offset X", &HandInfo::oculusOffset.x(), "min=-100 max=100");
  m_params->addParam("World Offset Y", &HandInfo::oculusOffset.y(), "min=-100 max=100");
  m_params->addParam("World Offset Z", &HandInfo::oculusOffset.z(), "min=-100 max=100");
#endif

  m_params->addParam("DownsampleK", &m_graph.m_downsampleK, "min=0 max=2 step=0.01");
  m_params->addParam("Dt Vertices", &m_graph.m_dtVertices, "min=0 max=1 step=0.01");
  m_params->addParam("Dt Edges", &m_graph.m_dtEdges, "min=0 max=1 step=0.001");
  m_params->addParam("Point Size", &m_graph.m_pointSize, "min=0 max=10 step=0.1");
  m_params->addParam("Edge Forces", &m_graph.m_edgeForces);
  m_params->addParam("Vertex Forces", &m_graph.m_vertexForces);
  m_params->addParam("Draw Edges", &m_graph.m_renderEdges);
  m_params->addParam("Draw Vertices", &m_graph.m_renderVertices);

  m_geometry.Initialize();

  m_graphRadius = 300.0f;
  m_graphAlt.Start(GraphAlt::WEB);

  m_Oculus.SetHWND(getRenderer()->getHwnd());

  if (m_Oculus.Init()) {
    std::cout << "Oculus Initialized" << std::endl;
    m_useOculusControl = true;
    m_useOculusRender = true;
  } else {
    std::cout << "Oculus Failed" << std::endl;
    m_useOculusControl = false;
    m_useOculusRender = false;
  }

  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void GraphShell::shutdown()
{
}

void GraphShell::mouseDown(ci::app::MouseEvent e)
{
  m_mousePos = e.getPos();
}

void GraphShell::mouseUp(ci::app::MouseEvent e)
{
}

void GraphShell::mouseMove(ci::app::MouseEvent e)
{
  if (e.isLeftDown()) {
    const ci::Vec2i diff = e.getPos() - m_mousePos;
    m_yaw += diff.x * 0.001f;
    m_pitch += diff.y * 0.001f;
  }
  m_mousePos = e.getPos();
}

void GraphShell::mouseDrag(ci::app::MouseEvent e)
{
  if (e.isLeftDown()) {
    const ci::Vec2i diff = e.getPos() - m_mousePos;
    m_yaw += diff.x * 0.001f;
    m_pitch += diff.y * 0.001f;
  }
  m_mousePos = e.getPos();
}

void GraphShell::keyDown(ci::app::KeyEvent event)
{
  switch (event.getChar()) {
  case ci::app::KeyEvent::KEY_ESCAPE:
    quit();
    break;
  case '9':
    m_graphAlt.Start(GraphAlt::WEB);
    break;
  case '0':
    m_graphAlt.Start(GraphAlt::RANDOM);
    break;
  case 'o':
    m_useOculusRender = !m_useOculusRender;
    break;
#if USE_LEAP_IMAGE_API
  case ci::app::KeyEvent::KEY_BACKSLASH:
    m_useDistortion = !m_useDistortion;
    break;
#endif
  case 'u':
    m_allowUpdate = !m_allowUpdate;
    break;
  case ci::app::KeyEvent::KEY_PLUS:
    m_dist += 10.0f;
    break;
  case ci::app::KeyEvent::KEY_MINUS:
    m_dist -= 10.0f;
    m_dist = std::max(m_dist, 0.0001f);
    break;
  default:
    break;
  }
#if _WIN32
  if (event.isAltDown() && event.getCode() == ci::app::KeyEvent::KEY_F4) {
    quit();
  }
  if (event.isAltDown() && event.getCode() == ci::app::KeyEvent::KEY_RETURN) {
    setFullScreen(!isFullScreen());
  }
#endif
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
      HandInfo::oculusTransform = getOculusTransform();
      m_interaction.Update(frame);
#if USE_LEAP_IMAGE_API
      m_images = frame.images();
#endif
    }
  }

#if USE_LEAP_IMAGE_API
  const double startTime = ci::app::getElapsedSeconds();
  m_numImages = 0;
  bool skipIR = m_images.count() == 8;
  for (int i=0; i<m_images.count(); i++) {
    if (!m_images[i].isValid() || (skipIR && (i == 0 || i == 1))) {
      continue;
    }
    const int id = m_images[i].id();
    //std::cout << "id: " << id << std::endl;
    //assert(id == 0 || id == 1);
    const int width = m_images[i].width();
    const int height = m_images[i].height();
    const int distortionWidth = m_images[i].distortionWidth();
    const int distortionHeight = m_images[i].distortionHeight();
    const unsigned char* data = m_images[i].data();
    const float* distortion = m_images[i].distortion();
    //ci::gl::Texture::Format format;
    //format.set
    m_textures[i] = ci::gl::Texture(data, GL_RED, width, height);

    glActiveTexture(GL_TEXTURE0);
    if (m_useDistortion && !m_haveDistortionTextures[id]) {
      glGenTextures(1, &m_distortionTexturesGL[id]);
      glBindTexture(GL_TEXTURE_2D, m_distortionTexturesGL[id]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, distortionWidth/2, distortionHeight, 0, GL_RG, GL_FLOAT, (void*)distortion);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
      m_haveDistortionTextures[id] = true;
    } /*else if (m_useDistortion && m_haveDistortionTextures[id]) {
      glBindTexture(GL_TEXTURE_2D, m_distortionTexturesGL[id]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, distortionWidth/2, distortionHeight, 0, GL_RG, GL_FLOAT, (void*)distortion);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
    }*/

    m_numImages++;
  }
  //std::cout << m_numImages << std::endl;
#endif
  glFlush();
  glFinish();
  const double endTime = ci::app::getElapsedSeconds();
  //std::cout << "upload time: " << endTime - startTime << std::endl;

  //m_graphAlt.Update();
  static bool first = true;
  if (first) {
    m_graph.reload();
    m_graph.runInitialPos();
    first = false;
  }
  if (m_allowUpdate) {
    m_graph.Update();
  }
}

Matrix4x4 GraphShell::getOculusTransform() {
  const Matrix4x4 rotation = getOculusRotation();
  Vector3 position = toVector3(m_Oculus.m_Position);
  Matrix4x4 translation = TranslationMatrix(-position);
  return translation * rotation;
}

Matrix4x4 GraphShell::getOculusRotation() {
  Matrix4x4 rotation(Matrix4x4::Identity());
  Vector3 up = toVector3(m_Oculus.m_FinalUp);
  const Vector3 forward = toVector3(m_Oculus.m_FinalForward);
  const Vector3 side = up.cross(forward).normalized();
  //up = side.cross(forward).normalized();
  up = forward.cross(side).normalized();
  //rotation.block<3, 3>(0, 0) << -side, -up, -forward;
  rotation.block<3, 3>(0, 0) << side, up, forward;
  return rotation;
}

void GraphShell::draw() {

  ci::gl::clear(ci::ColorA(0.0f, 0.0f, 0.0f, 0.0f));
  ci::gl::color(ci::ColorA::white());

  const ci::Area origViewport = ci::Area(0, 0, Globals::windowWidth, Globals::windowHeight);
  //const ci::Rectf rect(0.0f, Globals::windowHeight, Globals::windowWidth, 0.0f);
  const ci::Rectf rect(0.0f, 0.0f, Globals::windowWidth, Globals::windowHeight);
  const ci::Vec2i size = getWindowSize();

  ci::gl::setMatricesWindow(size);

  if (m_useOculusControl) {
    m_Oculus.BeginFrame();
  }
  if (m_useOculusRender) {
    m_Oculus.m_HMDFbo.bindFramebuffer();
    ci::gl::clear(ci::ColorA(0.0f, 0.0f, 0.0f, 1.0f));
  }
  int numEyes = m_useOculusRender ? 2 : 1;
  for (int i=0; i<numEyes; i++) {
    if (m_useOculusRender) {
      m_Oculus.BeginEye(i);
    }
    const ci::Area area = m_useOculusRender ? toCinder(m_Oculus.m_EyeRenderViewport[m_Oculus.m_Eyes[i]]) : origViewport;
    ci::gl::setViewport(area);

#if USE_LEAP_IMAGE_API
    const double startTime = ci::app::getElapsedSeconds();
    if (m_numImages >= 2) {
      ci::CameraPersp tempCam;
      tempCam.setPerspective(m_overlayFov, static_cast<float>(area.getSize().x) / static_cast<float>(area.getSize().y), 0.1f, 10000.0f);
      tempCam.lookAt(ci::Vec3f(0, 0, 0), ci::Vec3f(0, 0, -1), ci::Vec3f(0, 1, 0));


      //ci::gl::setMatricesWindow(area.getSize());

      ci::gl::setMatrices(tempCam);

      glDisable(GL_DEPTH_TEST);
      glEnable(GL_TEXTURE_2D);
      ci::gl::enableAdditiveBlending();
      const int imgIdx = (area.x1 > 0) ? 1 : 0;
      ci::gl::color(ci::ColorA::white());
      Globals::undistortionShader.bind();
      Globals::undistortionShader.uniform("useDistortion", m_useDistortion);
      Globals::undistortionShader.uniform("gamma", 1.0f);
      Globals::undistortionShader.uniform("distortionMap", 1);
      Globals::undistortionShader.uniform("gamma", m_gamma);

      const float width = 640;
      const float height = 360;
      Globals::undistortionShader.uniform("redOffset", ci::Vec2f(m_redX/width, m_redY/height));
      Globals::undistortionShader.uniform("greenOffset", ci::Vec2f(m_greenX/width, m_greenY/height));
      Globals::undistortionShader.uniform("blueOffset", ci::Vec2f(m_blueX/width, m_blueY/height));
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, m_distortionTexturesGL[imgIdx]);

      if (m_numImages == 2) {
        m_textures[imgIdx].bind(0);
        Globals::undistortionShader.uniform("tex0", 0);
        Globals::undistortionShader.uniform("useColor", false);
      } else if (m_numImages == 6) {
        m_textures[imgIdx + 2].bind(2);
        m_textures[imgIdx + 4].bind(3);
        m_textures[imgIdx + 6].bind(4);
        Globals::undistortionShader.uniform("tex1", 2);
        Globals::undistortionShader.uniform("tex2", 3);
        Globals::undistortionShader.uniform("tex3", 4);
        Globals::undistortionShader.uniform("useColor", true);
      }

      glPushMatrix();
      glTranslated(0, 0, -1);

      //ci::gl::drawSolidRect(ci::Rectf(0.0f, 0.0f, area.getSize().x, area.getSize().y));
      ci::gl::drawSolidRect(ci::Rectf(-4, -4, 4, 4));
      glPopMatrix();
      glBindTexture(GL_TEXTURE_2D, 0);
      glActiveTexture(GL_TEXTURE0);
      Globals::undistortionShader.unbind();
      if (m_numImages == 2) {
        m_textures[imgIdx].unbind();
      } else if (m_numImages == 6) {
        m_textures[imgIdx + 2].unbind();
        m_textures[imgIdx + 4].unbind();
        m_textures[imgIdx + 6].unbind();
      }
      glDisable(GL_TEXTURE_2D);
      glEnable(GL_DEPTH_TEST);
      ci::gl::enableAlphaBlending();
    }
    const double endTime = ci::app::getElapsedSeconds();
    //std::cout << "image time: " << endTime - startTime << std::endl;
#endif

    ci::Matrix44f modelView = toCinder(m_Oculus.m_ModelView);
    ci::Matrix44f projection = toCinder(m_Oculus.m_Projection);
    if (!m_useOculusRender) {
      ci::CameraPersp cam;
#if USE_LEAP_IMAGE_API
      cam.setPerspective(m_overlayFov, static_cast<float>(Globals::aspectRatio), 1.0f, 100000.0f);
#else
      cam.setPerspective(90.0f, static_cast<float>(Globals::aspectRatio), 1.0f, 100000.0f);
#endif
      cam.lookAt(ci::Vec3f(m_dist*std::sin(m_yaw)*std::cos(m_pitch), m_dist*std::sin(m_pitch), -m_dist*std::cos(m_yaw)*std::cos(m_pitch)), ci::Vec3f(0, 0, 0), ci::Vec3f(0, 1, 0));
      modelView = cam.getModelViewMatrix();
      projection = cam.getProjectionMatrix();
    }

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.m);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(modelView.m);

    glPushMatrix();
    //glScaled(-1, 1, -1);
    //glScaled(1, -1, 1);
    //m_interaction.Draw();
    const Matrix4x4 rotation = m_useOculusControl ? getOculusRotation() : Matrix4x4::Identity();
    //m_graph.Draw(rotation);
#if 0
    glMultMatrixd(m_interaction.Transform().data());
    m_graphAlt.Draw(m_interaction.Rotation().inverse() * rotation);
#else

    //m_graph.step(projection * modelView);
    m_graph.Draw(projection * modelView);
    //ci::gl::setViewport(origViewport);
#endif

    glPopMatrix();

#if 0
    //modelView = toCinder(m_Oculus.m_EyeTranslation);
    //modelView.setColumn(0, -1 * modelView.getColumn(0));
    //modelView = ci::Matrix44f::createTranslation(ci::Vec4f(imgIdx == 0 ? -20 : 20, 0, 0, 0)) * ci::Matrix44f::identity();
    //glLoadMatrixf(modelView.m);
    glPushMatrix();
    glLoadIdentity();
    //glTranslated(imgIdx > 0 ? -20 : 20, 0, 0);
    float baseline = 40;
    const Leap::DeviceList& devices = m_leapListener.getDevices();
    glTranslated(imgIdx > 0 ? -baseline/2 : baseline/2, 0, 0);
    glRotated(-90, 1, 0, 0);
    glScaled(-1, 1, -1);
    glScaled(2, 2, 2);
    for (int j=0; j<m_hands.count(); j++) {
      m_render->drawWireHand(m_hands[j]);
    }
    glPopMatrix();
#endif
    if (m_useOculusRender) {
      m_Oculus.EndEye(i);
    }
  }

  if (m_useOculusRender) {
    m_Oculus.m_HMDFbo.unbindFramebuffer();
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    ci::gl::disableAlphaBlending();
    ci::gl::setViewport(origViewport);
    ci::gl::color(ci::ColorA::white());
    ci::gl::setMatricesWindow(static_cast<int>(Globals::windowWidth), static_cast<int>(Globals::windowHeight));
    m_Oculus.DistortionMeshRender();
    glEnable(GL_DEPTH_TEST);
  }

  if (m_useOculusControl) {
    m_Oculus.EndFrame();
  }

#if 1
  ci::gl::enableAlphaBlending();
  ci::gl::setMatricesWindow(getWindowSize());
  m_params->draw();
  ci::gl::drawString("FPS: " + ci::toString(getAverageFps()), ci::Vec2f(10.0f, 10.0f), ci::ColorA::white(), ci::Font("Arial", 18));
  ci::gl::drawString("Nodes: " + ci::toString(m_graph.m_numVertices), ci::Vec2f(10.0f, 30.0f), ci::ColorA::white(), ci::Font("Arial", 18));
  ci::gl::drawString("Edges: " + ci::toString(m_graph.m_numEdges), ci::Vec2f(10.0f, 50.0f), ci::ColorA::white(), ci::Font("Arial", 18));
#endif
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
