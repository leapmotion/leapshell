#ifndef __UTILITIES_H__
#define __UTILITIES_H__

template <class T>
static inline T SmootherStep(const T& x) {
  // x is blending parameter between 0 and 1
  return x*x*x*(x*(x*6 - 15) + 10);
}

template <class T>
struct ExponentialFilter {
  ExponentialFilter() : first(true), lastTimeSeconds(0), targetFramerate(100) { }
  void Update(const T& data, double timeSeconds, float smoothStrength) {
    if (first || timeSeconds < lastTimeSeconds) {
      value = data;
      first = false;
    } else {
      const float dtExponent = static_cast<float>((timeSeconds - lastTimeSeconds) * targetFramerate);
      smoothStrength = std::pow(smoothStrength, dtExponent);
      assert(smoothStrength >= 0.0f && smoothStrength <= 1.0f);
      value = smoothStrength*value + (1.0f-smoothStrength)*data;
    }
    lastTimeSeconds = timeSeconds;
  }
  T value;
  bool first;
  double lastTimeSeconds;
  float targetFramerate;
};

inline ci::Vec3f ToVec3f(const Vector3& vec) {
  return ci::Vec3f(static_cast<float>(vec.x()), static_cast<float>(vec.y()), static_cast<float>(vec.z()));
}

inline bool RayPlaneIntersection(const Vector3& origin, const Vector3& direction, const Vector3& center, const Vector3& normal, Vector3& hitPoint) {
  double t = -(normal.dot(origin - center)) / normal.dot(direction);
  if (t <= 0) {
    return false;
  }
  hitPoint = origin + t*direction;
  return true;
}

inline float Clamp(float value) {
  return std::min(1.0f, std::max(0.0f, value));
}

static bool StringContains(const std::string& name, const std::string& searchFilter, bool anywhere) {
  if (searchFilter.empty()) {
    return true;
  }
  std::string lowerSearch = searchFilter, lowerName = name;
  std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
  std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
  if (anywhere) {
    // search filter can be anywhere in the name
    if (lowerName.find(lowerSearch) == std::string::npos) {
      return false;
    }
  } else {
    // search filter must be at the beginning of the name
    if (lowerName.compare(0, lowerSearch.length(), lowerSearch) != 0) {
      return false;
    }
  }
  return true;
}

static const int TIME_STAMP_TICKS_PER_SEC = 1000000;
static const double TIME_STAMP_SECS_TO_TICKS  = static_cast<double>(TIME_STAMP_TICKS_PER_SEC);
static const double TIME_STAMP_TICKS_TO_SECS  = 1.0/TIME_STAMP_SECS_TO_TICKS;

static const float RADIANS_TO_DEGREES = static_cast<float>(180.0 / M_PI);
static const float DEGREES_TO_RADIANS = static_cast<float>(M_PI / 180.0);

#endif
