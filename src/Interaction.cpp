#include "StdAfx.h"
#include "Interaction.h"
#include "Tile.h"
#include "Globals.h"

Interaction::Interaction() {
  m_panForce.Update(Vector3::Zero(), 0.0, 0.5f);
  m_lastViewUpdateTime = 0.0;
}

void Interaction::Update(const Leap::Frame& frame) {
  const double timeSeconds = TIME_STAMP_TICKS_TO_SECS * frame.timestamp();
  m_frame = frame;

  updateHandInfos();
  cleanupHandInfos();

  // accumulate force from all hands
  Vector3 force(Vector3::Zero());
  for (HandInfoMap::iterator it = m_handInfos.begin(); it != m_handInfos.end(); ++it) {
    const HandInfo& info = it->second;
    if (!Globals::haveSeenOpenHand) {
      const float grabPinchStrength = info.GrabPinchStrength();
      if (grabPinchStrength < 0.1f) {
        Globals::haveSeenOpenHand = true;
      }
    }
    force += forceFromHand(info);
  }

  // scale the force by a constant in each direction to make sure we're not flying around the screen
  static const Vector3 FORCE_POSITION_SCALE(-0.1, -0.1, -0.25);
  force = FORCE_POSITION_SCALE.cwiseProduct(force);

  // make speeding up have less lag than slowing down
  static const float SPEED_UP_SMOOTH = 0.8f;
  static const float SLOW_DOWN_SMOOTH = 0.975f;
  const Vector3 prevForce = m_panForce.value;
  const float curSmooth = force.squaredNorm() > prevForce.squaredNorm() ? SPEED_UP_SMOOTH : SLOW_DOWN_SMOOTH;
  m_panForce.Update(force, timeSeconds, curSmooth);
}

void Interaction::UpdateView(View &view) {
  const double deltaTime = Globals::curTimeSeconds - m_lastViewUpdateTime;
  assert(deltaTime > 0);

  // apply the force to the view camera
  view.ApplyVelocity(m_panForce.value, Globals::curTimeSeconds, deltaTime);

  const Leap::HandList hands = m_frame.hands();
  for (int i=0; i<hands.count(); i++) {
    if (hands[i].isLeft()) {
      view.LeftHand().Update(hands[i], Globals::curTimeSeconds);
    } else if (hands[i].isRight()) {
      view.RightHand().Update(hands[i], Globals::curTimeSeconds);
    }
  }

  applyInfluenceToTiles(view);

  m_lastViewUpdateTime = Globals::curTimeSeconds;
}

void Interaction::updateHandInfos() {
  const Leap::HandList hands = m_frame.hands();
  for (int i=0; i<hands.count(); i++) {
    const int id = hands[i].id();
    m_handInfos[id].Update(hands[i]);
  }
}

void Interaction::cleanupHandInfos() {
  static const float MAX_HAND_INFO_AGE = 0.1f; // seconds since last update until hand info gets cleaned up
  HandInfoMap::iterator it = m_handInfos.begin();
  while (it != m_handInfos.end()) {
    const HandInfo& cur = it->second;
    const float curAge = fabs(static_cast<float>(Globals::curTimeSeconds - cur.LastUpdateTime()));
    if (curAge > MAX_HAND_INFO_AGE) {
      m_handInfos.erase(it++);
    } else {
      ++it;
    }
  }
}

void Interaction::applyInfluenceToTiles(View& view) {
  static const float MAX_INFLUENCE_DISTANCE_SQ = 30 * 30;
  static const float INFLUENCE_CHANGE_SPEED = 0.5f;

  TileVector& tiles = view.Tiles();
  ForceVector& forces = view.Forces();
  forces.clear();
  const Vector3& lookat = view.LookAt();
  Vector3 hitPoint;

  static const Vector3 origin = View::CAM_DISTANCE_FROM_PLANE * Vector3::UnitZ();
  static const Vector3 normal = Vector3::UnitZ();
  for (HandInfoMap::iterator it = m_handInfos.begin(); it != m_handInfos.end(); ++it) {
    Leap::Hand hand = it->second.Hand();
    const float handGrabPinch = it->second.GrabPinchStrength();
    const Vector3 position = hand.palmPosition().toVector3<Vector3>() + Globals::LEAP_OFFSET;
    const Vector3 palmPoint = position + lookat;
    const Vector3 direction = (position - origin).normalized();

    // calculate grab/pinch strength
    static const float MIN_STRENGTH_TO_GRAB = 0.95f;
    const float grabStrength = std::min(1.0f, (handGrabPinch - (1.0f-MIN_STRENGTH_TO_GRAB))/MIN_STRENGTH_TO_GRAB);
    const float grabMultiplier = SmootherStep(grabStrength * grabStrength * grabStrength * grabStrength);

    // find the closest tile to the projection point
    float closestDistSq = MAX_INFLUENCE_DISTANCE_SQ;
    Tile* closestTile = nullptr;
    for (TileVector::iterator it = tiles.begin(); it != tiles.end(); ++it) {
      Tile& tile = *it;

      // calculate projection point from camera to tile
      if (!RayPlaneIntersection(origin, direction, tile.Position(), normal, hitPoint)) {
        continue;
      }
      hitPoint += lookat;

      const float distSq = (1.0f - tile.Activation()) * static_cast<float>((hitPoint - tile.Position()).squaredNorm());
      if (distSq < closestDistSq) {
        closestTile = &tile;
        closestDistSq = distSq;
      }
    }

    // increase activation for closest tile to hand
    if (closestTile && Globals::haveSeenOpenHand) {
      // only allow activating the tile if it's already highlighted
      const float newActivation = closestTile->Highlight() * grabMultiplier;
      closestTile->UpdateActivation(newActivation, Tile::ACTIVATION_SMOOTH);
      closestTile->UpdateHighlight(std::min(1.0f, closestTile->Highlight() + INFLUENCE_CHANGE_SPEED), Tile::ACTIVATION_SMOOTH);
      
      // calculate pulling force from hand
      Vector3 grabDelta = closestTile->Activation()*(palmPoint - closestTile->OrigPosition());
      static const double PULL_SPEED = 0.5;
      grabDelta -= PULL_SPEED*closestTile->Activation()*hand.palmPosition().z*direction;
      closestTile->UpdateGrabDelta(grabDelta, Tile::GRABDELTA_SMOOTH);

      // add repulsive force from this tile to others
      forces.push_back(Force(closestTile->Position(), closestTile->Activation() + closestTile->Highlight()));
    }
  }

  // decrease activation for all other tiles
  for (TileVector::iterator it = tiles.begin(); it != tiles.end(); ++it) {
    Tile& tile = *it;
    if (tile.LastActivationUpdateTime() != Globals::curTimeSeconds) {
      tile.UpdateActivation(std::max(0.0f, tile.Activation() - INFLUENCE_CHANGE_SPEED), Tile::ACTIVATION_SMOOTH);
      tile.UpdateHighlight(std::max(0.0f, tile.Highlight() - INFLUENCE_CHANGE_SPEED), Tile::ACTIVATION_SMOOTH);
      tile.UpdateGrabDelta(INFLUENCE_CHANGE_SPEED * tile.GrabDelta(), Tile::GRABDELTA_SMOOTH);
      if (tile.Highlight() > 0.01f) {
        forces.push_back(Force(tile.Position(), tile.Activation() + tile.Highlight()));
      }
    }
  }
}

Vector3 Interaction::forceFromHand(const HandInfo& handInfo) {
  static float HAND_WARMUP_TIME = 2.0f;
  const Leap::Hand& hand = handInfo.Hand();
  const float warmupMultiplier = SmootherStep(std::min(1.0f, hand.timeVisible() / HAND_WARMUP_TIME));
  Vector3 totalForce = Vector3::Zero();
  const Vector3 handDirection = hand.direction().toVector3<Vector3>();
  const Leap::FingerList fingers = hand.fingers();
  const float grabMultiplier = 1.0f - SmootherStep(std::max(hand.grabStrength(), hand.pinchStrength()));
  const Vector3 modifiedVelocity = handInfo.ModifiedVelocity();
  for (int i=0; i<fingers.count(); i++) {
    if (!fingers[i].isExtended()) {
      continue;
    }
    const Vector3 direction = fingers[i].direction().toVector3<Vector3>();
    const Vector3 velocity = fingers[i].tipVelocity().toVector3<Vector3>();
    const Vector3 normVelocity = velocity.normalized();

    const double dot = std::abs(normVelocity.dot(hand.palmNormal().toVector3<Vector3>()));
    const double match = direction.dot(handDirection);

    totalForce += warmupMultiplier * warmupMultiplier * grabMultiplier * grabMultiplier * grabMultiplier * dot * dot * match * modifiedVelocity;
  }
  return totalForce;
}
