#include "StdAfx.h"
#include "Globals.h"

double Globals::windowWidth = 0;
double Globals::windowHeight = 0;
double Globals::aspectRatio = 0;
const float Globals::TRANSITION_TIME = 0.5f;
const float Globals::SWITCH_TIME = 0.25f;
double Globals::curTimeSeconds = 0;
double Globals::prevTimeSeconds = 0;
double Globals::lastTileSwitchTime = 0;
double Globals::lastTileTransitionTime = 0;
const float Globals::FONT_SIZE = 144.0f;
ci::gl::TextureFontRef Globals::fontRegular;
//ci::gl::TextureFontRef Globals::fontBold;
ci::gl::GlslProg Globals::handsShader;
ci::gl::GlslProg Globals::distortionShader;
ci::gl::GlslProg Globals::graphShader;
ci::gl::GlslProg Globals::undistortionShader;
const Vector3 Globals::LEAP_OFFSET(0, 0, 0);
ci::gl::Fbo Globals::handsFbo;
bool Globals::haveSeenOpenHand = true;
