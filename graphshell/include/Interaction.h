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
  const Matrix4x4& Transform() const { return m_transform; }
  const Matrix4x4& Rotation() const { return m_rotation; }
  void Draw() const;

private:

  void updateHandInfos(double frameTime);
  void cleanupHandInfos(double frameTime);
  void calculateTransformCenter();
  void calculateInteractions();
  void calculateTransforms();
  static void drawWireHand(const Leap::Hand& hand);
  
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

  Vector3 m_translation;
  Matrix4x4 m_rotation;
  float m_scale;

  Matrix4x4 m_transform;
  double m_lastDrawTime;
};

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
  void DrawHand() const;
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
    m_velocity = leapToWorld(hand.palmVelocity().toVector3<Vector3>(), true);
    const Vector3 normVelocity = m_velocity.normalized();
    const double ratio = normVelocity.x()*normVelocity.x() + normVelocity.y()*normVelocity.y();
    m_ratioSmoother.Update(static_cast<float>(ratio), frameTime, TRANSLATION_RATIO_SMOOTH_STRENGTH);

    // calculate grab/pinch strength
    const float maxGrabPinch = std::max(hand.grabStrength(), hand.pinchStrength());
    const float clampedScaledStrength = std::min(1.0f, (maxGrabPinch - (1.0f-MIN_STRENGTH_TO_GRAB))/MIN_STRENGTH_TO_GRAB);
    const float grabPinchStrength = SmootherStep(clampedScaledStrength * clampedScaledStrength * clampedScaledStrength);
    m_grabSmoother.Update(grabPinchStrength, frameTime, GRAB_SMOOTH_STRENGTH);

    // update palm position
    m_palmPosition = leapToWorld(hand.palmPosition().toVector3<Vector3>() + Globals::LEAP_OFFSET, false);
    m_rotationAxis = leapToWorld(hand.rotationAxis(m_hand.frame()).toVector3<Vector3>(), true);
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

  static Matrix4x4 oculusTransform;
  static Vector3 oculusOffset;

  static Vector3 leapToWorld(const Vector3& pos, bool vel);

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

};

#endif
