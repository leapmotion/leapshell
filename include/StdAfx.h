#pragma once

// cinder
#include <cinder/app/App.h>
#include <cinder/app/AppNative.h>
#include <cinder/params/Params.h>
#include <cinder/DataSource.h>
#include <cinder/Rand.h>
#include <cinder/Camera.h>
#include <cinder/gl/gl.h>
#include <cinder/gl/GlslProg.h>
#include <cinder/gl/Fbo.h>
#include <cinder/gl/TextureFont.h>
#include <cinder/Vector.h>
#include <cinder/Utilities.h>
#include <cinder/Url.h>
#include <cinder/Thread.h>
#include <cinder/ImageIo.h>
#include <cinder/Timeline.h>

// leap
#include "Leap.h"

// eigen
#include <Eigen/Dense>

// stl
#include <vector>
#include <iostream>
#include <string>
#include <deque>
#include <map>
#include <sstream>
#include <cmath>

// fbx
#include "fbxsdk.h"

// OS-specific
#if _WIN32
#include <Windows.h>
#include <Shellapi.h>
#include <direct.h>
#include <WinTrust.h>
#include <SoftPub.h>
#include <CommDlg.h>
#include <UserEnv.h>
#elif __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// boost
#include <boost/filesystem.hpp>
