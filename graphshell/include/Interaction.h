#ifndef __Interaction_h__
#define __Interaction_h__

#include "Globals.h"
#include "Utilities.h"

class HandInfo;

class Interaction {
public:
  Interaction();
  void Update(const Leap::Frame& frame);
  const Vector3& TranslationVelocity() const { return m_translationVelocity; }
  const Vector3& RotationVelocity() const { return m_rotationVelocity; }
  const Vector3& TransformCenter() const { return m_transformCenter; }
  float ScaleVelocity() const { return m_scaleVelocity; }
  void Draw() const;

private:

  void updateHandInfos(double frameTime);
  void cleanupHandInfos(double frameTime);
  void calculateTransformCenter();
  void calculateInteractions();

  typedef std::map<int, HandInfo, std::less<int>, Eigen::aligned_allocator<std::pair<int, HandInfo> > > HandInfoMap;

  Leap::Frame m_frame;
  ExponentialFilter<Vector3> m_panForce;
  HandInfoMap m_handInfos;

  //Vector3 m_rotationCenter;

  float m_transformStrength;
  Vector3 m_transformCenter;
  Vector3 m_transformVelocity;
  Vector3 m_rotationVelocity;

  Vector3 m_translationVelocity;
  float m_scaleVelocity;
};

#if 1
class HandInfo {

public:

  HandInfo() : m_lastUpdateTime(0.0), m_velocity(Vector3::Zero()), m_palmPosition(Vector3::Zero()) {
    m_ratioSmoother.Update(1.0f, Globals::curTimeSeconds, 0.5f);
    m_grabSmoother.Update(0.0f, Globals::curTimeSeconds, 0.5f);
  }

  float GrabPinchStrength() const { return m_grabSmoother.value; }
  const Leap::Hand& Hand() const { return m_hand; }
  double LastUpdateTime() const { return m_lastUpdateTime; }
  const Vector3& PalmPosition() const { return m_palmPosition; }
  const Vector3& RotationAxis() const { return m_rotationAxis; }
  float RotationAngleVelocity() const { return m_rotationAngleVelocity; }
  float ScaleVelocity() const { return m_scaleVelocity; }
  //Tile* ClosestTile() const { return m_closestTile; }

  const Vector3 ModifiedVelocity() const {
    const Vector3 velocity = m_hand.palmVelocity().toVector3<Vector3>();
    const float ratio = m_ratioSmoother.value;
    return Vector3(ratio*velocity.x(), ratio*velocity.y(), (1.0-ratio)*velocity.z());
  }

  void Update(const Leap::Hand& hand, double frameTime) {
    static const float TRANSLATION_RATIO_SMOOTH_STRENGTH = 0.95f;
    static const float GRAB_SMOOTH_STRENGTH = 0.8f;
    static const float MIN_STRENGTH_TO_GRAB = 0.95f;

    // calculate velocity and ratio of X-Y movement to Z movement
    m_velocity = hand.palmVelocity().toVector3<Vector3>();
    const Vector3 normVelocity = m_velocity.normalized();
    const double ratio = normVelocity.x()*normVelocity.x() + normVelocity.y()*normVelocity.y();
    m_ratioSmoother.Update(static_cast<float>(ratio), frameTime, TRANSLATION_RATIO_SMOOTH_STRENGTH);

    // calculate grab/pinch strength
    const float maxGrabPinch = std::max(hand.grabStrength(), hand.pinchStrength());
    const float clampedScaledStrength = std::min(1.0f, (maxGrabPinch - (1.0f-MIN_STRENGTH_TO_GRAB))/MIN_STRENGTH_TO_GRAB);
    const float grabPinchStrength = SmootherStep(clampedScaledStrength * clampedScaledStrength * clampedScaledStrength);
    m_grabSmoother.Update(grabPinchStrength, frameTime, GRAB_SMOOTH_STRENGTH);

    // update palm position
    m_palmPosition = hand.palmPosition().toVector3<Vector3>() + Globals::LEAP_OFFSET;
    m_rotationAxis = hand.rotationAxis(m_hand.frame()).toVector3<Vector3>();
    m_rotationAngleVelocity = hand.rotationAngle(m_hand.frame()) / (frameTime - m_lastUpdateTime);
    m_scaleVelocity = std::log(hand.scaleFactor(m_hand.frame())) / (frameTime - m_lastUpdateTime);
    m_lastUpdateTime = frameTime;
    m_hand = hand;
  }

  const Vector3& Velocity() const {
    return m_velocity;
  }

  const Vector3 VelocityAt(const Vector3& origin, const Vector3& target) {
    double depth = (m_palmPosition - origin).norm();
    double targetDepth = (target - origin).norm();
    return m_velocity * (targetDepth / depth);
  }

private:

  Leap::Hand m_hand;
  ExponentialFilter<float> m_ratioSmoother;
  ExponentialFilter<float> m_grabSmoother;
  Vector3 m_velocity;
  Vector3 m_palmPosition;
  double m_lastUpdateTime;
  Leap::Frame m_sinceFrame;
  Vector3 m_rotationAxis;
  float m_scaleVelocity;
  float m_rotationAngleVelocity;
  //Tile* m_closestTile;

};
#else

class HandInfo {
public:

  HandInfo() : m_lastUpdateTime(0.0), m_lastHandOpenChangeTime(0.0), m_handOpen(false), m_firstUpdate(true), m_lastPalmPos(Vector3::Zero()) {
    m_translation.value = Vector3::Zero();
    m_transRatio.value = 0.5f;
    m_normalY.value = 0.5f;
  }

  int getNumFingers() const { return m_numFingers.FilteredCategory(); }
  Vector3 getTranslation() const { return m_translation.value; }
  float getTranslationRatio() const { return m_transRatio.value; }
  float getNormalY() const { return m_normalY.value; }
  double getLastUpdateTime() const { return m_lastUpdateTime; }
  double getLastHandOpenChangeTime() const { return m_lastHandOpenChangeTime; }
  bool handOpen() const { return m_handOpen; }
  double LastUpdateTime() const { return m_lastUpdateTime; }

  Vector3 getModifiedTranslation() const {
    const float ratio = getTranslationRatio();
    const Vector3 origTrans = getTranslation();
    return Vector3(ratio*origTrans.x(), ratio*origTrans.y(), (1.0f-ratio)*origTrans.z());
  }

  void Update(const Leap::Hand& hand, double frameTime) {
    static const float NUM_FINGERS_SMOOTH_STRENGTH = 0.9f;
    static const float TRANSLATION_SMOOTH_STRENGTH = 0.5f;
    static const float TRANSLATION_RATIO_SMOOTH_STRENGTH = 0.985f;
    static const float NORMAL_Y_SMOOTH_STRENGTH = 0.9f;
    static const float MIN_POINTABLE_AGE = 0.1f;
    m_lastUpdateTime = frameTime;

    // update number of fingers
    const Leap::PointableList pointables = hand.pointables();
    int numFingers = 0;
    for (int i=0; i<pointables.count(); i++) {
      if (pointables[i].isExtended() && pointables[i].timeVisible() >= MIN_POINTABLE_AGE) {
        numFingers++;
      }
    }
    m_numFingers.Update(numFingers, frameTime, NUM_FINGERS_SMOOTH_STRENGTH);
    const int curNumFingers = m_numFingers.FilteredCategory();
    if (curNumFingers > 2 && !m_handOpen) {
      m_handOpen = true;
      m_lastHandOpenChangeTime = frameTime;
    } else if (m_handOpen && curNumFingers <= 2) {
      m_handOpen = false;
      m_lastHandOpenChangeTime = frameTime;
    }

    // update translation
    const Leap::Vector temp(hand.palmPosition());
    const Vector3 curPos(temp.x, temp.y, temp.z);
    Vector3 translation = curPos - m_lastPalmPos;
    if (m_firstUpdate) {
      translation = Vector3::Zero();
    }
    m_lastPalmPos = curPos;
    m_firstUpdate = false;
    m_translation.Update(translation, frameTime, TRANSLATION_SMOOTH_STRENGTH);

    // update translation ratio
    if (translation.squaredNorm() > 0.001f) {
      const Vector3 unitTranslation = translation.normalized();
      const float curRatio = unitTranslation.x()*unitTranslation.x() + unitTranslation.y()*unitTranslation.y();
      m_transRatio.Update(curRatio, frameTime, TRANSLATION_RATIO_SMOOTH_STRENGTH);
    }

    // update palm normal Y value
    m_normalY.Update(fabs(hand.palmNormal().y), frameTime, NORMAL_Y_SMOOTH_STRENGTH);
  }
private:
  bool m_firstUpdate;
  Vector3 m_lastPalmPos;
  CategoricalFilter<10> m_numFingers;
  ExponentialFilter<Vector3> m_translation;
  double m_lastUpdateTime;
  bool m_handOpen;
  double m_lastHandOpenChangeTime;
};
#endif


#endif
