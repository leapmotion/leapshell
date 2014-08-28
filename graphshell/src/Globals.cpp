#include "StdAfx.h"
#include "Globals.h"

double Globals::windowWidth = 0;
double Globals::windowHeight = 0;
float Globals::aspectRatio = 0;
double Globals::curTimeSeconds = 0;
//const Vector3 Globals::LEAP_OFFSET(0, -200, -100);
const Vector3 Globals::LEAP_OFFSET(0, 0, 0);
const float Globals::FONT_SIZE = 144.0f;
ci::gl::TextureFontRef Globals::fontRegular;
ci::gl::GlslProg Globals::handsShader;
ci::gl::GlslProg Globals::distortionShader;
ci::gl::GlslProg Globals::undistortionShader;