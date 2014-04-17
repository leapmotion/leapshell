#ifndef __Globals_h__
#define __Globals_h__

class Globals {
public:
  // viewport info
  static double windowWidth;
  static double windowHeight;
  static double aspectRatio;

  // timing info
  static double curTimeSeconds;

  // fonts
  static ci::gl::TextureFontRef fontRegular;
  static ci::gl::TextureFontRef fontBold;

};

#endif
