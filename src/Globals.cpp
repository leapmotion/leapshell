#include "StdAfx.h"
#include "Globals.h"

double Globals::windowWidth = 0;
double Globals::windowHeight = 0;
double Globals::aspectRatio = 0;
double Globals::curTimeSeconds = 0;
double Globals::lastTileSwitchTime = 0;
double Globals::lastTileTransitionTime = 0;
const float Globals::FONT_SIZE = 72.0f;
ci::gl::TextureFontRef Globals::fontRegular;
ci::gl::TextureFontRef Globals::fontBold;
ci::gl::GlslProg Globals::handsShader;
const Vector3 Globals::LEAP_OFFSET(0, -200, -100);
ci::gl::Fbo Globals::handsFbo;
bool Globals::haveSeenOpenHand = true;
