#ifndef __GraphShell_h__
#define __GraphShell_h__

#include "Resources.h"
#include "LeapListener.h"
#include "Geometry.h"
#include "Graph.h"
#include "Interaction.h"
#include "GraphAlt.h"
//#include "MeshHand.h"
//#include "OculusVR.h"
#if defined(CINDER_COCOA)
#include <boost/uuid/sha1.hpp>
#include <mach-o/getsect.h>
#include <mach-o/dyld.h>
#endif

class GraphShell : public ci::app::AppBasic {
public:
  GraphShell();
  virtual ~GraphShell();

  virtual void prepareSettings(Settings* settings) override;
  virtual void setup() override;
  virtual void shutdown() override;
  virtual void update() override;
  virtual void draw() override;
  virtual void mouseDown(ci::app::MouseEvent event) override;
  virtual void mouseUp(ci::app::MouseEvent event) override;
  virtual void mouseMove(ci::app::MouseEvent event) override;
  virtual void mouseDrag(ci::app::MouseEvent event) override;
  virtual void keyDown(ci::app::KeyEvent) override;
  virtual void keyUp(ci::app::KeyEvent) override;
  virtual void mouseWheel(ci::app::MouseEvent event) override;
  
  virtual void resize() override;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  Matrix4x4 getOculusTransform();
  Matrix4x4 getOculusRotation();
  void updateGlobals();

#if defined(CINDER_COCOA)
  ci::DataSourceRef loadResource(const std::string& macPath) {
    try {
      return ci::app::loadResource(macPath);
    } catch (...) {
      boost::uuids::detail::sha1 sha1;
      uint32_t hash[5];
      sha1.process_bytes(macPath.data(), macPath.size());
      sha1.get_digest(hash);
      std::ostringstream oss;
      oss << std::setfill('0') << std::setw(sizeof(hash[0])*2) << std::hex;
      for (size_t i = 0; i < sizeof(hash)/sizeof(hash[0]); i++) {
        oss << hash[i];
      }
      const std::string symbol = "__" + oss.str().substr(0, 14); // Max 16 characters long
      const auto* sect = getsectbyname("__DATA", symbol.c_str());

      if (sect != nullptr) {
        const auto size = sect->size;
        const auto addr = _dyld_get_image_vmaddr_slide(0) + sect->addr;
        if (size > 0 && addr != 0) {
          return ci::DataSourceBuffer::create(ci::Buffer(reinterpret_cast<void*>(addr), size));
        }
      }
      throw;
    }
  }
#elif defined(CINDER_MSW)
  HWND m_hWnd;
#endif

  LeapListener m_leapListener;
  Leap::Controller m_leapController;
  std::deque<Leap::Frame> m_frames;

  ci::params::InterfaceGlRef m_params;

  ci::gl::GlslProg m_screenShader;
  ci::gl::GlslProg m_objectShader;

  ci::CameraPersp m_camera;

  ci::gl::Fbo m_screenFbo;

  Geometry m_geometry;
  Graph m_graph;
  bool m_allowUpdate;
  Interaction m_interaction;

  // hands
  //MeshHand* m_handL;
  //MeshHand* m_handR;

  GraphAlt m_graphAlt;
  float m_graphRadius;

  //OculusVR m_Oculus;
  bool m_useOculusRender;
  bool m_useOculusControl;

  Leap::HandList m_hands;

  float m_yaw;
  float m_pitch;
  float m_dist;
  ci::Vec2i m_mousePos;
  ci::Vec2f m_viewOffset;

#if USE_LEAP_IMAGE_API
  Leap::ImageList m_images;
  int m_numImages;
  ci::gl::Texture m_textures[8];
  ci::gl::Texture m_distortionTextures[8];
  GLuint m_distortionTexturesGL[8];
  bool m_useDistortion;
  bool m_haveDistortionTextures[8];
  float m_overlayFov;
  float m_gamma;

  float m_redX;
  float m_redY;
  float m_greenX;
  float m_greenY;
  float m_blueX;
  float m_blueY;
#endif
};

#endif
