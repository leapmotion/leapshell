// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "StdAfx.h"
#include "Shell.h"
#include "Globals.h"
#include "Value.h"

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
  m_navView(nullptr),
  m_lastTextChangeTime(0.0),
  m_useOculusRender(true),
  m_useOculusControl(false)
{
#if defined(CINDER_MSW)
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);
#endif
  m_leapController.addListener(m_leapListener);
#if USE_LEAP_IMAGE_API
  m_numImages = 0;
  m_useDistortion = true;
  for (int i=0; i<2; i++) {
    m_distortionTexturesGL[i] = 0;
    m_haveDistortionTextures[i] = false;
  }
  //m_overlayFov = 104.0f;
  m_overlayFov = 104.0f;
  m_gamma = 1.4f;
#endif
}

LeapShell::~LeapShell()
{
  delete m_render;
  m_leapController.removeListener(m_leapListener);
#if defined(CINDER_MSW)
  CoUninitialize();
#endif
  delete m_handL;
  delete m_handR;
}

void LeapShell::prepareSettings(Settings* settings)
{
  settings->setWindowPos(100, 100);
  settings->setWindowSize(1024, 768);
  //settings->setBorderless(true);
  //settings->setAlwaysOnTop(true);
  //settings->setFullScreen(true);
  settings->disableFrameRate();
  ci::gl::disableVerticalSync();
#if defined(CINDER_MSW)
  settings->enableConsoleWindow(true);
#endif
}

void LeapShell::setup()
{
  const std::string supportedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890().?!,:;'\"&*=+-/\\@#_[]<>%^llflfiphridsfflffiff\303\251\303\241\303\250\303\240";
  Globals::fontRegular = ci::gl::TextureFont::create(ci::Font(loadResource(RES_FONT_PROXIMA_NOVA_OTF), Globals::FONT_SIZE), ci::gl::TextureFont::Format(), supportedChars);
  //Globals::fontBold = ci::gl::TextureFont::create(ci::Font(loadResource(RES_FONT_FREIGHTSANSBOLD_TTF), Globals::FONT_SIZE));

  // setup hand meshes
  ci::DataSourceRef leftHand = loadResource(RES_LEFT_HAND_FBX);
  ci::DataSourceRef rightHand = loadResource(RES_RIGHT_HAND_FBX);
  MeshHand::SetMeshSources(leftHand, rightHand);

  // enable alpha
  ci::gl::enableAlphaBlending();

  // load shaders
  try {
    Globals::handsShader = ci::gl::GlslProg(loadResource(RES_HANDS_VERT_GLSL), loadResource(RES_HANDS_FRAG_GLSL));
    m_blurShader = ci::gl::GlslProgRef(new ci::gl::GlslProg(loadResource(RES_BLUR_VERT_GLSL), loadResource(RES_BLUR_FRAG_GLSL)));
    Globals::distortionShader = ci::gl::GlslProg(loadResource(RES_DISTORTION_VERT_GLSL), loadResource(RES_DISTORTION_FRAG_GLSL));
    Globals::graphShader = ci::gl::GlslProg(loadResource(RES_LIGHTING_VERT_GLSL), loadResource(RES_LIGHTING_FRAG_GLSL));
#if USE_LEAP_IMAGE_API
    Globals::undistortionShader = ci::gl::GlslProg(loadResource(RES_PASSTHROUGH_VERT_GLSL), loadResource(RES_UNDISTORTION_FRAG_GLSL));
#endif
    //m_textureShader = ci::gl::GlslProgRef(new ci::gl::GlslProg(loadResource(RES_PASSTHROUGH_VERT_GLSL), loadResource(RES_TEXTURE_FRAG_GLSL)));
    //m_motionShader = ci::gl::GlslProgRef(new ci::gl::GlslProg(loadResource(RES_PASSTHROUGH_VERT_GLSL), loadResource(RES_MOTION_FRAG_GLSL)));
    //m_postprocessShader = ci::gl::GlslProgRef(new ci::gl::GlslProg(loadResource(RES_PASSTHROUGH_VERT_GLSL), loadResource(RES_POSTPROCESS_FRAG_GLSL)));
  } catch (ci::gl::GlslProgCompileExc e) {
    std::cerr << e.what() << std::endl;
  }

  glEnable(GL_TEXTURE_2D);
  //glEnable(GL_COLOR_MATERIAL);
  //glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT, GL_FILL);

  m_params = ci::params::InterfaceGl::create(getWindow(), "App parameters", ci::app::toPixels(ci::Vec2i(200, 400)));
  m_params->minimize();

  m_params->addParam("Gravity", &m_graph.m_gravity, "min=0 max=1 step=0.001");
  m_params->addParam("Gravity Radius", &m_graph.m_gravityRadius, "min=0 max=1000 step=10");
  m_params->addParam("Charge", &m_graph.m_charge, "min=-1000 max=1000 step=0.1");
  m_params->addParam("Friction", &m_graph.m_friction, "min=0 max=1 step=0.01");
  m_params->addParam("Alpha", &m_graph.m_alpha, "min=0 max=1 step=0.01");
  m_params->addParam("View Distance", &m_graphRadius, "min=0 max=500 step=1");
#if USE_LEAP_IMAGE_API
  m_params->addParam("Overlay FOV", &m_overlayFov, "min=10 max=170 step=1");
  m_params->addParam("Gamma", &m_gamma, "min=0.1 max=2.0 step=0.1");
#endif

  m_render = new Render();
  m_interaction = new Interaction();

  m_root = std::shared_ptr<FileSystemNode>(new FileSystemNode("/"));
  m_state->setCurrentLocation(m_root);

  m_handL = new MeshHand("Left Hand", MeshHand::LEFT);
  m_handR = new MeshHand("Right Hand", MeshHand::RIGHT);

  m_handL->SetScale(0.75f);
  m_handR->SetScale(0.75f);

  // this is done after m_state->setCurrentLocation so the metadata keys are accessible to View.
  // also, view must be created after meshes.
  m_view = std::shared_ptr<View>(new View(m_state));
  m_view->SetWorldView(m_view);
  m_view->SetIsNavView(false);
  m_view->SetInactiveOpacity(0.2f);
  m_state->registerView(m_view);

  m_navView = std::shared_ptr<View>(new View(m_state));
  m_navView->SetWorldView(m_view);
  m_navView->SetIsNavView(true);
  m_state->registerView(m_navView);
  m_navView->SetSizeLayout(std::shared_ptr<SizeLayout>(new UniformSizeLayout(12.0)));
  m_navView->SetPositionLayout(std::shared_ptr<PositionLayout>(new ListLayout()));
  m_navView->SetInactiveOpacity(0.0f);

  // initial sorting criteria keys TEMP HACK for 2014.04.21 demo
  std::vector<std::string> prioritizedKeys;
  prioritizedKeys.push_back("name");
  prioritizedKeys.push_back("ext");
  prioritizedKeys.push_back("time");
  prioritizedKeys.push_back("size");
  m_view->SetPrioritizedKeys(prioritizedKeys);

  unit_test_Value(); // TEMP until this is verified to work on all platforms

  m_graphRadius = 300.0f;
  m_graph.Start(Graph::RANDOM);

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
    //m_view->PrioritizeKey("time");
    //setText("Sort by Time");
    m_graph.Start(Graph::WEB);
    break;
  case '0':
    //m_view->PrioritizeKey("size");
    //setText("Sort by Size");
    m_graph.Start(Graph::RANDOM);
    break;
  case ci::app::KeyEvent::KEY_ESCAPE:
    quit();
    break;
  case ci::app::KeyEvent::KEY_BACKSPACE:
  case ci::app::KeyEvent::KEY_DELETE:
    if (!m_view->SearchFilter().empty()) {
      m_view->SetSearchFilter(m_view->SearchFilter().substr(0, m_view->SearchFilter().size()-1));
    }
    break;
  case ci::app::KeyEvent::KEY_UNKNOWN:
    break;
  case ci::app::KeyEvent::KEY_RETURN:
    m_useOculusRender = !m_useOculusRender;
    break;
#if USE_LEAP_IMAGE_API
  case ci::app::KeyEvent::KEY_BACKSLASH:
    m_useDistortion = !m_useDistortion;
    break;
#endif
  default:
    m_view->SetSearchFilter(m_view->SearchFilter() + event.getChar());
    break;
  }
}

void LeapShell::keyUp(ci::app::KeyEvent event)
{
}

void LeapShell::update()
{
  updateGlobals();

  const double HIDDEN_OFFSET = -42;
  const double ACTIVE_OFFSET = -30;
  const double offset = m_navView->Activation() * (ACTIVE_OFFSET - HIDDEN_OFFSET) + HIDDEN_OFFSET;
  m_navView->GetPositionLayout()->SetOffset(Vector2(0, offset));

  std::deque<Leap::Frame> frames = m_leapListener.grabFrames();

  if (!frames.empty()) {
    Matrix4x4 viewTransform = Matrix4x4::Identity();
    if (m_useOculusControl) {
      viewTransform = getOculusBasis();// .inverse();
      //viewTransform.row(1) *= -1;
    }
    for (auto iter = frames.cbegin(); iter != frames.cend(); ++iter) {
      const Leap::Frame& frame = *iter;
      m_interaction->Update(frame);
      const Leap::HandList hands = frame.hands();
      m_hands = hands;
      for (int i=0; i<hands.count(); i++) {
        //const float strength = std::max(hands[i].pinchStrength(), hands[i].grabStrength());
        const float pinchStrength = hands[i].pinchStrength();
        const float grabStrength = hands[i].grabStrength();
        const float strength = std::max(pinchStrength, grabStrength);
        Vector3 pos(Vector3::Zero());
        if (pinchStrength > grabStrength) {
          // use index/thumb
          Vector3 thumb = Vector3::Zero();
          Vector3 index = Vector3::Zero();
          const Leap::FingerList fingers = hands[i].fingers();
          for (int j=0; j<fingers.count(); j++) {
            if (fingers[j].type() == Leap::Finger::TYPE_THUMB) {
              thumb = fingers[j].tipPosition().toVector3<Vector3>();
            } else if (fingers[j].type() == Leap::Finger::TYPE_INDEX) {
              index = fingers[j].tipPosition().toVector3<Vector3>();
            }
          }
          pos = 0.5 * (thumb + index);
        } else {
          // use palm
          pos = hands[i].palmPosition().toVector3<Vector3>() + Globals::LEAP_OFFSET;
        }

        pos = (viewTransform * Vector4(pos.x(), pos.z(), pos.y(), 1.0)).head<3>();
#if 0
        ci::Matrix33f rotMat = ci::Matrix33f::createRotation(ci::Vec3f::xAxis(), M_PI/2.0f);
        ci::Vec3f transformed = rotMat * ToVec3f(pos);
        transformed.x *= -1;
        transformed.z *= -1;
        transformed *= 2.0;
        pos << transformed.x, transformed.y, transformed.z;
#else
        pos.y() *= -1;
        pos *= 2.0;
#endif
        const float modifiedStrength = SmootherStep(strength*strength);
        if (modifiedStrength > 0.1f) {
          m_graph.UpdateAttractor(hands[i].id(), pos, modifiedStrength);
        }
      }
#if USE_LEAP_IMAGE_API
      m_images = frame.images();
#endif
    }
  }

#if USE_LEAP_IMAGE_API
  m_numImages = 0;
  for (int i=0; i<m_images.count(); i++) {
    if (!m_images[i].isValid()) {
      continue;
    }
    const int id = m_images[i].id();
    assert(id == 0 || id == 1);
    const int width = m_images[i].width();
    const int height = m_images[i].height();
    const int distortionWidth = m_images[i].distortionWidth();
    const int distortionHeight = m_images[i].distortionHeight();
    const unsigned char* data = m_images[i].data();
    const float* distortion = m_images[i].distortion();
    //ci::gl::Texture::Format format;
    //format.set
    m_textures[id] = ci::gl::Texture(data, GL_RED, width, height);

    if (m_useDistortion && !m_haveDistortionTextures[id]) {
      glGenTextures(1, &m_distortionTexturesGL[id]);
      glBindTexture(GL_TEXTURE_2D, m_distortionTexturesGL[id]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, distortionWidth/2, distortionHeight, 0, GL_RG, GL_FLOAT, (void*)distortion);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
      m_haveDistortionTextures[id] = true;
    } else if (m_useDistortion && m_haveDistortionTextures[id]) {
      glBindTexture(GL_TEXTURE_2D, m_distortionTexturesGL[id]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, distortionWidth/2, distortionHeight, 0, GL_RG, GL_FLOAT, (void*)distortion);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    m_numImages++;
  }
#endif

  //if (!m_state->updateChildren()) {
    //m_view->PerFrameUpdate();
    //m_navView->PerFrameUpdate();
  //}

  m_graph.Update();
  m_interaction->UpdateMeshHands(*m_handL, *m_handR);
}

Matrix4x4 LeapShell::getOculusBasis() {
  Matrix4x4 rotation(Matrix4x4::Identity());
  Vector3 up = Vector3(m_Oculus.m_FinalUp.x, m_Oculus.m_FinalUp.y, m_Oculus.m_FinalUp.z);
  const Vector3 forward = Vector3(m_Oculus.m_FinalForward.x, m_Oculus.m_FinalForward.y, m_Oculus.m_FinalForward.z).normalized();
  const Vector3 side = up.cross(forward).normalized();
  up = side.cross(forward).normalized();
  rotation.block<3, 3>(0, 0) << -side, -up, -forward;
  return rotation;
}

void LeapShell::drawGraphAndHands() {
  Matrix4x4 rotation = Matrix4x4::Identity();
  if (m_useOculusControl) {
    rotation = getOculusBasis();
  }

  glPushMatrix();
#if 0
  Matrix4x4 rotationTemp = rotation;
  rotationTemp.row(1) *= -1;
  glMultMatrixd(rotationTemp.data());
  glRotated(90, 1, 0, 0);
  glScaled(2, 2, -2);
#else
  ci::Matrix44f modelView = toCinder(m_Oculus.m_ModelView).inverted();
  //modelView.setRow(1, -1 * modelView.getRow(1));
  ci::gl::multModelView(modelView);
  //glRotated(90, 1, 0, 0);
  //glScaled(2, -2, -2);
  //glScaled(2, 2, -2);
  //glScaled(2, 2, 2);
#endif
  ci::gl::enableAlphaBlending();
  //glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  //m_render->drawHands(*m_view, *m_handL, *m_handR);
  for (int i=0; i<m_hands.count(); i++) {
    m_render->drawWireHand(m_hands[i]);
  }
  //glEnable(GL_TEXTURE_2D);

  glPopMatrix();

  m_graph.Draw(rotation);
  glDisable(GL_CULL_FACE);
}

void LeapShell::draw()
{
  ci::gl::clear(ci::ColorA(0.0f, 0.0f, 0.0f, 0.0f));
  ci::gl::color(ci::ColorA::white());

  const ci::Area origViewport = ci::gl::getViewport();
  //const ci::Rectf rect(0.0f, Globals::windowHeight, Globals::windowWidth, 0.0f);
  const ci::Rectf rect(0.0f, 0.0f, Globals::windowWidth, Globals::windowHeight);
  const ci::Vec2i size = getWindowSize();
  
  ci::gl::setMatricesWindow(size);

//#if USE_LEAP_IMAGE_API
//  if (!m_useOculusRender && m_numImages > 0) {
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_TEXTURE_2D);
//    m_textures[0].bind();
//    ci::gl::drawSolidRect(rect);
//    m_textures[0].unbind();
//    glDisable(GL_TEXTURE_2D);
//    glEnable(GL_DEPTH_TEST);
//  }
//#endif

  if (m_useOculusControl) {
    m_Oculus.BeginFrame();
  }
  if (m_useOculusRender) {
    m_Oculus.m_HMDFbo.bindFramebuffer();
    ci::gl::clear(ci::ColorA(0.0f, 0.0f, 0.0f, 1.0f));
  }
  int numEyes = m_useOculusRender ? 2 : 1;
  for (int i=0; i<numEyes; i++) {
    m_Oculus.BeginEye(i);
    const ci::Area area = m_useOculusRender ? toCinder(m_Oculus.m_EyeRenderViewport[m_Oculus.m_Eyes[i]]) : origViewport;
    ci::gl::setViewport(area);

#if USE_LEAP_IMAGE_API
    ci::CameraPersp tempCam;
    tempCam.setPerspective(m_overlayFov, static_cast<float>(area.getSize().x) / static_cast<float>(area.getSize().y), 0.1f, 10000.0f);
    tempCam.lookAt(ci::Vec3f(0, 0, 0), ci::Vec3f(0, 0, -1), ci::Vec3f(0, 1, 0));


    //ci::gl::setMatricesWindow(area.getSize());

    ci::gl::setMatrices(tempCam);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    ci::gl::enableAdditiveBlending();
    const int imgIdx = (area.x1 > 0) ? 1 : 0;
    m_textures[imgIdx].bind(0);
    ci::gl::color(ci::ColorA::white());
    Globals::undistortionShader.bind();
    Globals::undistortionShader.uniform("useDistortion", m_useDistortion);
    Globals::undistortionShader.uniform("gamma", 1.0f);
    Globals::undistortionShader.uniform("colorTex", 0);
    Globals::undistortionShader.uniform("distortionMap", 1);
    Globals::undistortionShader.uniform("gamma", m_gamma);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_distortionTexturesGL[imgIdx]);
    glPushMatrix();
    glTranslated(0, 0, -1);

    //ci::gl::drawSolidRect(ci::Rectf(0.0f, 0.0f, area.getSize().x, area.getSize().y));
    ci::gl::drawSolidRect(ci::Rectf(-4, -4, 4, 4));
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    Globals::undistortionShader.unbind();
    m_textures[imgIdx].unbind();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    ci::gl::enableAlphaBlending();
#endif

    ci::Matrix44f modelView = toCinder(m_Oculus.m_ModelView);
    ci::Matrix44f projection = toCinder(m_Oculus.m_Projection);
    if (!m_useOculusRender) {
      ci::CameraPersp cam;
      cam.setPerspective(m_overlayFov, static_cast<float>(Globals::aspectRatio), 0.1f, 1000.0f);
      projection = cam.getProjectionMatrix();
    }

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.m);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(modelView.m);

    glPushMatrix();
    glScaled(-1, 1, -1);
    Matrix4x4 rotation = getOculusBasis();
    m_graph.Draw(rotation);
    glPopMatrix();

#if USE_LEAP_IMAGE_API
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
    m_Oculus.EndEye(i);

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

  ci::gl::setViewport(origViewport);

  ci::gl::enableAlphaBlending();
  drawHUDStrings();
}

void LeapShell::drawString(const std::string& str, double x, double y, double lastChangeTime, float fadeTime, bool center)
{
  if (str.empty()) {
    return;
  }
  //ci::gl::TextureFont::DrawOptions options;
  //options.ligate(false);
  const float timeSinceTextChange = static_cast<float>(Globals::curTimeSeconds - lastChangeTime);
  const float textOpacity = SmootherStep(1.0f - std::min(1.0f, timeSinceTextChange/fadeTime));
  static const ci::Vec2f shadowOffset = ci::Vec2f(3.0, 5.0f);
  if (textOpacity > 0.01f) {
    const ci::ColorA textColor = ci::ColorA(1.0f, 1.0f, 1.0f, textOpacity);
    const ci::ColorA shadowColor = ci::ColorA(0.1f, 0.1f, 0.1f, textOpacity);
    const ci::Vec2f textSize = Globals::fontRegular->measureString(str);
    const float start = center ? -textSize.x/2.0f : 0.0f;
    const ci::Rectf textRect(start, 0.0f, start + textSize.x, 100.0f);
    const ci::Rectf shadowRect(start, 0.0f, start + textSize.x + shadowOffset.x, 100.0f);
    glPushMatrix();
    glTranslated(x, y, 0.0);
    ci::gl::color(shadowColor);
    Globals::fontRegular->drawString(str, shadowRect, shadowOffset);
    ci::gl::color(textColor);
    Globals::fontRegular->drawString(str, textRect);
    glPopMatrix();
  }
}

ci::Vec2f getScaledSizeWithAspect(const ci::Vec2f& windowSize, const ci::Vec2f& imageSize, float scale)
{
  const float windowAspect = windowSize.y / windowSize.x;
  const float imageAspect = imageSize.y / imageSize.x;
  float width, height;
  if (windowAspect > imageAspect) {
    // window is relatively taller than image, need to enlarge y more
    height = scale * windowSize.y;
    width = height / imageAspect;
  } else {
    // image is relatively taller than window, need to enlarge x more
    width = scale * windowSize.x;
    height = width * imageAspect;
  }
  return ci::Vec2f(width, height);
}

void LeapShell::setOrthoCamera() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, Globals::windowWidth, Globals::windowHeight, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void LeapShell::drawViewsToFBO() {
  ci::gl::enableAlphaBlending();
  ci::gl::color(ci::ColorA::white());
  drawViewToFBO(m_view, m_worldFbo1, m_worldFbo2);
  drawViewToFBO(m_navView, m_navFbo1, m_navFbo2);
}

void LeapShell::drawViewToFBO(const std::shared_ptr<View>& view, ci::gl::Fbo& fbo1, ci::gl::Fbo& fbo2) {
  //const float radius = 200.0f;
  const ci::Area origViewport = ci::gl::getViewport();
  const ci::Rectf rect(0.0f, Globals::windowHeight, Globals::windowWidth, 0.0f);
  //const ci::Rectf rect(0.0f, m_worldFboResult.getWidth(), m_worldFboResult.getHeight(), 0.0f);
  const ci::ColorA shadowColor(0.2f, 0.2f, 0.2f, 1.0f);
  //const ci::ColorA shadowColor = ci::ColorA::white();

  //const float worldMult = (1.0f - m_view->Activation());
  const float worldRadius = 300.0f; // *worldMult * worldMult;
  //ci::gl::color(ci::ColorA::black());
  fbo1.bindFramebuffer();
  ci::gl::setViewport(fbo1.getBounds());
  ci::gl::clear(ci::ColorA(0, 0, 0, 0));
  ci::gl::color(ci::ColorA::white());
  //m_render->drawViewBackground(*m_view);
  ci::gl::color(ci::ColorA::white());
  m_render->drawViewTiles(*view, false);
  fbo1.unbindFramebuffer();


  ci::gl::color(ci::ColorA::white());
  setOrthoCamera();

  const float horizSize = worldRadius / fbo2.getWidth();
  fbo1.bindTexture();
  fbo2.bindFramebuffer();
  ci::gl::setViewport(fbo2.getBounds());
  ci::gl::clear(ci::ColorA(0, 0, 0, 0));
  m_blurShader->bind();
  m_blurShader->uniform("color_tex", 0);
  m_blurShader->uniform("sample_offset", ci::Vec2f(horizSize, 0.0f));
  m_blurShader->uniform("blur_color", shadowColor);
  m_blurShader->uniform("override_alpha", false);
  m_blurShader->uniform("global_offset", ci::Vec2f(-0.0025f, 0.0065f));
  ci::gl::drawSolidRect(rect);
  m_blurShader->unbind();
  fbo1.unbindTexture();
  fbo2.unbindFramebuffer();


  const float vertSize = worldRadius / fbo1.getHeight();
  fbo2.bindTexture();
  fbo1.bindFramebuffer();
  ci::gl::setViewport(fbo1.getBounds());
  ci::gl::clear(ci::ColorA(0, 0, 0, 0));
  m_blurShader->bind();
  m_blurShader->uniform("color_tex", 0);
  m_blurShader->uniform("sample_offset", ci::Vec2f(0.0f, vertSize));
  m_blurShader->uniform("blur_color", shadowColor);
  m_blurShader->uniform("override_alpha", false);
  m_blurShader->uniform("global_offset", ci::Vec2f(-0.0025f, 0.0065f));
  ci::gl::drawSolidRect(rect);
  m_blurShader->unbind();
  fbo2.unbindTexture();
  fbo1.unbindFramebuffer();

  ci::gl::setViewport(origViewport);
}

void LeapShell::drawDropShadow(ci::gl::Fbo& fbo) {
  //ci::gl::disableDepthWrite();
  //ci::gl::enableAlphaBlending();
  setOrthoCamera();
  const ci::Rectf rect(0.0f, Globals::windowHeight, Globals::windowWidth, 0.0f);
  ci::ColorA color = ci::ColorA::white();
  ci::gl::color(color);
  fbo.bindTexture();
  //glPushMatrix();
  //glTranslated(6, 11, 0);
  ci::gl::drawSolidRect(rect);
  //glPopMatrix();
  fbo.unbindTexture();
  //ci::gl::enableDepthWrite();
}

void LeapShell::drawWorldViewTexture() {
  ci::gl::disableDepthWrite();
  ci::gl::enableAlphaBlending();
  setOrthoCamera();
  const ci::Rectf rect(0.0f, Globals::windowHeight, Globals::windowWidth, 0.0f);
  ci::ColorA color = ci::ColorA::white();
  ci::gl::color(color);
  m_worldFbo1.bindTexture();
  glPushMatrix();
  glTranslated(6, 11, 0);
  ci::gl::drawSolidRect(rect);
  glPopMatrix();
  m_worldFbo1.unbindTexture();
  ci::gl::enableDepthWrite();
}

void LeapShell::drawNavViewTexture() {
  ci::gl::disableDepthWrite();
  ci::gl::enableAlphaBlending();
  setOrthoCamera();
  const ci::Rectf rect(0.0f, Globals::windowHeight, Globals::windowWidth, 0.0f);
  ci::ColorA color = ci::ColorA::white();
  ci::gl::color(color);
  m_navFbo1.bindTexture();
  glPushMatrix();
  glTranslated(6, 11, 0);
  ci::gl::drawSolidRect(rect);
  glPopMatrix();
  m_navFbo1.unbindTexture();
  ci::gl::enableDepthWrite();
}

void LeapShell::drawHUDStrings() {
  ci::gl::setMatricesWindow(getWindowSize());
#if 1
  m_params->draw();
  ci::gl::drawString("FPS: " + ci::toString(getAverageFps()), ci::Vec2f(10.0f, 10.0f), ci::ColorA::white(), ci::Font("Arial", 18));
#endif

  drawString(m_view->SearchFilter(), 0.05*Globals::windowWidth, 0.025*Globals::windowHeight, Globals::curTimeSeconds, 1.0f, false);
  drawString(m_textString, Globals::windowWidth/2.0, 0.875*Globals::windowHeight, m_lastTextChangeTime, 2.0f, true);
}

void LeapShell::resize()
{
  const float SCALE_FACTOR = 1.2f; // must be a bit larger than 1.0 to ensure parallax is OK
  updateGlobals();
  Globals::handsFbo = ci::gl::Fbo(static_cast<int>(Globals::windowWidth), static_cast<int>(Globals::windowHeight));
  glViewport(0, 0, static_cast<int>(Globals::windowWidth), static_cast<int>(Globals::windowHeight));
#if defined(CINDER_COCOA)
  @autoreleasepool {
    int scaledWindowWidth = static_cast<int>(Globals::windowWidth*SCALE_FACTOR);
    int scaledWindowHeight = static_cast<int>(Globals::windowHeight*SCALE_FACTOR);

    ci::Surface8u surface = ci::Surface8u(scaledWindowWidth, scaledWindowHeight, true, ci::SurfaceChannelOrder::RGBA);
    surface.setPremultiplied(true);
    unsigned char* dstBytes = surface.getData();
    ::memset(dstBytes, 0, scaledWindowWidth*scaledWindowHeight*4);

    NSScreen* screen = getDisplay()->getNsScreen();
    NSImage* nsImage = [[NSImage alloc] initWithContentsOfURL:[[NSWorkspace sharedWorkspace] desktopImageURLForScreen:screen]];
    if (nsImage) {
      CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
      CGContextRef cgContextRef =
        CGBitmapContextCreate(dstBytes, scaledWindowWidth, scaledWindowHeight, 8, 4*scaledWindowWidth, rgb, kCGImageAlphaPremultipliedLast);
      NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:cgContextRef flipped:NO];
      [NSGraphicsContext saveGraphicsState];
      [NSGraphicsContext setCurrentContext:gc];

      const NSSize imageSize = [nsImage size];
      const CGFloat scaleX = scaledWindowWidth/imageSize.width;
      const CGFloat scaleY = scaledWindowHeight/imageSize.height;
      const CGFloat scale = (scaleX >= scaleY) ? scaleX : scaleY;
      const NSSize scaledImageSize = NSMakeSize(imageSize.width * scale, imageSize.height * scale);
      const CGFloat xoffset = (imageSize.width*scaleX - scaledImageSize.width)/2.0;
      const CGFloat yoffset = (imageSize.height*scaleY - scaledImageSize.height)/2.0;
      const NSRect rect = NSMakeRect(xoffset, yoffset, scaledImageSize.width, scaledImageSize.height);

      [nsImage drawInRect:rect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
      [NSGraphicsContext restoreGraphicsState];
      CGContextRelease(cgContextRef);
      CGColorSpaceRelease(rgb);
    }
    m_render->update_background(surface);
  }
#else
  ci::Surface8u surface;

  bool haveDesktopWallpaper = false;
  try {
    WCHAR pvParam[1024];
    if (SystemParametersInfo(SPI_GETDESKWALLPAPER, 1024, pvParam, 0)) {
      std::wstring pathString(pvParam);
      if (!pathString.empty()) {
        surface = ci::loadImage(boost::filesystem::path(pathString));
        haveDesktopWallpaper = true;
      }
    }
  } catch (...) {
    haveDesktopWallpaper = false;
  }

  if (!haveDesktopWallpaper) {
    surface = ci::loadImage(loadResource(RES_WINTER_JPG));
  }

  const ci::Vec2f windowSize(static_cast<float>(Globals::windowWidth), static_cast<float>(Globals::windowHeight));
  const ci::Vec2f imageSize(static_cast<float>(surface.getWidth()), static_cast<float>(surface.getHeight()));
  const ci::Vec2f scaledSize = getScaledSizeWithAspect(windowSize, imageSize, SCALE_FACTOR);

  const float croppedX = std::min(static_cast<float>(SCALE_FACTOR*Globals::windowWidth), scaledSize.x);
  const float croppedY = std::min(static_cast<float>(SCALE_FACTOR*Globals::windowHeight), scaledSize.y);
  const ci::Vec2i croppedSize(static_cast<int>(croppedX), static_cast<int>(croppedY));

  const float diffX = (scaledSize.x - croppedX)/SCALE_FACTOR;
  const float diffY = (scaledSize.y - croppedY)/SCALE_FACTOR;

  ci::Area origArea = surface.getBounds();
  origArea.x1 += static_cast<int>(diffX/2.0f);
  origArea.x2 -= static_cast<int>(diffX/2.0f);
  origArea.y1 += static_cast<int>(diffY/2.0f);
  origArea.y2 -= static_cast<int>(diffY/2.0f);

  m_render->update_background(ci::ip::resizeCopy(surface, origArea, croppedSize));
#endif

  static const int BLUR_DOWNSCALE_FACTOR = 4;
  const int blurWidth = static_cast<int>(Globals::windowWidth / BLUR_DOWNSCALE_FACTOR);
  const int blurHeight = static_cast<int>(Globals::windowHeight / BLUR_DOWNSCALE_FACTOR);

  ci::gl::Fbo::Format blurFormat;
  blurFormat.setMinFilter(GL_LINEAR);
  blurFormat.setMagFilter(GL_LINEAR);
  blurFormat.enableMipmapping(false);
  blurFormat.enableDepthBuffer(false);
  blurFormat.setSamples(16);

  //m_navFboResult = ci::gl::Fbo(blurWidth, blurHeight, blurFormat);
  m_navFbo1 = ci::gl::Fbo(blurWidth, blurHeight, blurFormat);
  m_navFbo2 = ci::gl::Fbo(blurWidth, blurHeight, blurFormat);

  //m_worldFboResult = ci::gl::Fbo(blurWidth, blurHeight, blurFormat);
  m_worldFbo1 = ci::gl::Fbo(blurWidth, blurHeight, blurFormat);
  m_worldFbo2 = ci::gl::Fbo(blurWidth, blurHeight, blurFormat);

}

void LeapShell::updateGlobals() {
  Globals::prevTimeSeconds = Globals::curTimeSeconds;
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
//CINDER_APP_BASIC(LeapShell, ci::app::RendererGl(ci::app::RendererGl::AA_NONE))
