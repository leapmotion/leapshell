#ifndef __Globals_h__
#define __Globals_h__

// flags
#define USE_LEAP_IMAGE_API 1

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
  static double prevTimeSeconds;
  static double lastTileSwitchTime;
  static double lastTileTransitionTime;

  // fonts
  static const float FONT_SIZE;
  static ci::gl::TextureFontRef fontRegular;
  //static ci::gl::TextureFontRef fontBold;

  // shaders
  static ci::gl::GlslProg handsShader;
  static ci::gl::GlslProg distortionShader;
  static ci::gl::GlslProg graphShader;
  static ci::gl::GlslProg undistortionShader;

  // FBOs
  static ci::gl::Fbo handsFbo;

  // leap
  static const Vector3 LEAP_OFFSET;
  static bool haveSeenOpenHand;

};

#endif
