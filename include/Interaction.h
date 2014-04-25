#ifndef __Interaction_h__
#define __Interaction_h__

#include "View.h"
#include "Utilities.h"
#include "Globals.h"

class HandInfo;

class Interaction {

public:

  Interaction();
  void Update(const Leap::Frame& frame);
  void UpdateView(View &view);

private:

  void updateHandInfos();
  void cleanupHandInfos();
  void applyInfluenceToTiles(View& view);

  static Vector3 forceFromHand(const HandInfo& handInfo);

  typedef std::map<int, HandInfo> HandInfoMap;

  Leap::Frame m_frame;
  ExponentialFilter<Vector3> m_panForce;
  double m_lastViewUpdateTime;
  HandInfoMap m_handInfos;

};

class HandInfo {

public:

  HandInfo() : m_lastUpdateTime(0.0) {
    m_ratioSmoother.Update(1.0f, Globals::curTimeSeconds, 0.5f);
    m_grabSmoother.Update(0.0f, Globals::curTimeSeconds, 0.5f);
  }

  float GrabPinchStrength() const { return m_grabSmoother.value; }
  const Leap::Hand& Hand() const { return m_hand; }
  double LastUpdateTime() const { return m_lastUpdateTime; }

  const Vector3 ModifiedVelocity() const {
    const Vector3 velocity = m_hand.palmVelocity().toVector3<Vector3>();
    const float ratio = m_ratioSmoother.value;
    return Vector3(ratio*velocity.x(), ratio*velocity.y(), (1.0-ratio)*velocity.z());
  }

  void Update(const Leap::Hand& hand) {
    static const float TRANSLATION_RATIO_SMOOTH_STRENGTH = 0.95f;
    static const float GRAB_SMOOTH_STRENGTH = 0.8f;

    // calculate velocity and ratio of X-Y movement to Z movement
    const Vector3 normVelocity = hand.palmVelocity().toVector3<Vector3>().normalized();
    const double ratio = normVelocity.x()*normVelocity.x() + normVelocity.y()*normVelocity.y();
    m_ratioSmoother.Update(static_cast<float>(ratio), Globals::curTimeSeconds, TRANSLATION_RATIO_SMOOTH_STRENGTH);

    // calculate grab/pinch strength
    m_grabSmoother.Update(std::max(hand.grabStrength(), hand.pinchStrength()), Globals::curTimeSeconds, GRAB_SMOOTH_STRENGTH);

    m_lastUpdateTime = Globals::curTimeSeconds;
    m_hand = hand;
  }

private:

  Leap::Hand m_hand;
  ExponentialFilter<float> m_ratioSmoother;
  ExponentialFilter<float> m_grabSmoother;
  double m_lastUpdateTime;

};

#endif
