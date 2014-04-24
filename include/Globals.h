#ifndef __Globals_h__
#define __Globals_h__

class Globals {
public:
  // viewport info
  static double windowWidth;
  static double windowHeight;
  static double aspectRatio;

  // timing info
  static const float TRANSITION_TIME; // from layout changes
  static const float SWITCH_TIME; // from sorting changes 
  static double curTimeSeconds;
  static double lastTileSwitchTime;
  static double lastTileTransitionTime;

  // fonts
  static const float FONT_SIZE;
  static ci::gl::TextureFontRef fontRegular;
  static ci::gl::TextureFontRef fontBold;

  // shaders
  static ci::gl::GlslProg handsShader;

  // FBOs
  static ci::gl::Fbo handsFbo;

  // leap
  static const Vector3 LEAP_OFFSET;
  static bool haveSeenOpenHand;

};

#endif
