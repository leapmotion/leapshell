#ifndef __Globals_h__
#define __Globals_h__

// flags
#define USE_LEAP_IMAGE_API 0

class Globals {
public:
  // viewport info
  static double windowWidth;
  static double windowHeight;
  static float aspectRatio;

  // timing info
  static double curTimeSeconds;

  // leap
  static const Vector3 LEAP_OFFSET;

  // fonts
  static const float FONT_SIZE;
  static ci::gl::TextureFontRef fontRegular;

  // shaders
  static ci::gl::GlslProg handsShader;
  static ci::gl::GlslProg distortionShader;
  static ci::gl::GlslProg undistortionShader;

};

#endif
