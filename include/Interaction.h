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

  static const float MAX_INFLUENCE_DISTANCE_SQ;
  static const float INFLUENCE_CHANGE_SPEED;

  void updateHandInfos(double frameTime);
  void cleanupHandInfos(double frameTime);
  void applyInfluenceToTiles(View& view);
 
  static void updateInactiveTiles(TileVector& tiles);
  static void computeForcesFromTiles(const TileVector& tiles, ForceVector& forces);
  static Tile* findClosestTile(const Leap::Hand& hand, const Vector3& lookat, Tile* prevClosest, TileVector& tiles);
  static Vector3 forceFromHand(const HandInfo& handInfo);

  typedef std::map<int, HandInfo, std::less<int>, Eigen::aligned_allocator<std::pair<int, HandInfo> > > HandInfoMap;

  Leap::Frame m_frame;
  ExponentialFilter<Vector3> m_panForce;
  double m_lastViewUpdateTime;
  HandInfoMap m_handInfos;

};

class HandInfo {

public:

  HandInfo() : m_lastUpdateTime(0.0), m_velocity(Vector3::Zero()), m_palmPosition(Vector3::Zero()), m_closestTile(nullptr) {
    m_ratioSmoother.Update(1.0f, Globals::curTimeSeconds, 0.5f);
    m_grabSmoother.Update(0.0f, Globals::curTimeSeconds, 0.5f);
  }

  float GrabPinchStrength() const { return m_grabSmoother.value; }
  const Leap::Hand& Hand() const { return m_hand; }
  double LastUpdateTime() const { return m_lastUpdateTime; }
  Tile* ClosestTile() const { return m_closestTile; }

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

  void SetClosestTile(Tile* tile) {
    m_closestTile = tile;
  }

private:

  Leap::Hand m_hand;
  ExponentialFilter<float> m_ratioSmoother;
  ExponentialFilter<float> m_grabSmoother;
  Vector3 m_velocity;
  Vector3 m_palmPosition;
  double m_lastUpdateTime;
  Tile* m_closestTile;

};

#endif
