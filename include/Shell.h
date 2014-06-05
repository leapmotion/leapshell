// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef __LeapShell_h__
#define __LeapShell_h__

#include "Resources.h"
#include "LeapListener.h"
#include "MeshHand.h"
#include "View.h"
#include "Render.h"
#include "NavigationState.h"
#include "Interaction.h"
#include "FileSystemNode.h"
#if defined(CINDER_COCOA)
#include <boost/uuid/sha1.hpp>
#include <mach-o/getsect.h>
#include <mach-o/dyld.h>
#endif

class LeapShell : public ci::app::AppBasic {
public:
  LeapShell();
  virtual ~LeapShell();

  // ci::app::AppBasic
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
  virtual void resize() override;

private:

  void updateGlobals();
  void setText(const std::string& text);
  void drawBlur();
  void drawShadow();
  void drawMotionBlur();
  void drawHands();
  void drawViewsToFBO();
  void drawWorldViewTexture();
  void drawNavViewTexture();
  void drawHUDStrings();
  void setOrthoCamera();

  void drawViewToFBO(const std::shared_ptr<View>& view, ci::gl::Fbo& fbo1, ci::gl::Fbo& fbo2);
  void drawDropShadow(ci::gl::Fbo& fbo);

  void drawViewBlurToFBO(View* view);
  void drawViewBlurTexture(View* view);

  static void drawString(const std::string& str, double x, double y, double lastChangeTime, float fadeTime, bool center);

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

  std::shared_ptr<NavigationState> m_state;
  std::shared_ptr<View> m_view;
  std::shared_ptr<View> m_navView;
  Render* m_render;
  Interaction* m_interaction;

  std::shared_ptr<HierarchyNode> m_root;

  // for text drawing
  std::string m_textString;
  double m_lastTextChangeTime;

  // hands
  MeshHand* m_handL;
  MeshHand* m_handR;

  // blur
  ci::gl::GlslProgRef m_blurShader;

  //ci::gl::Fbo m_navFboResult;
  ci::gl::Fbo m_navFbo1;
  ci::gl::Fbo m_navFbo2;

  //ci::gl::Fbo m_worldFboResult;
  ci::gl::Fbo m_worldFbo1;
  ci::gl::Fbo m_worldFbo2;

  ci::gl::GlslProgRef m_textureShader;

};

#endif
