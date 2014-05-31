#include "StdAfx.h"
#include "Interaction.h"
#include "Tile.h"
#include "Globals.h"

const float Interaction::MAX_INFLUENCE_DISTANCE_SQ = 10.0f * 10.0f;
const float Interaction::INFLUENCE_CHANGE_SPEED = 0.5f;

Interaction::Interaction() {
  m_panForce.Update(Vector3::Zero(), 0.0, 0.5f);
}

void Interaction::Update(const Leap::Frame& frame) {
  const double timeSeconds = TIME_STAMP_TICKS_TO_SECS * frame.timestamp();
  m_frame = frame;

  updateHandInfos(timeSeconds);
  cleanupHandInfos(timeSeconds);

  // accumulate force from all hands
  Vector3 force(Vector3::Zero());
  for (HandInfoMap::iterator it = m_handInfos.begin(); it != m_handInfos.end(); ++it) {
    const HandInfo& info = it->second;
    if (!Globals::haveSeenOpenHand) {
      const float grabPinchStrength = info.GrabPinchStrength();
      if (grabPinchStrength < 0.25f) {
        Globals::haveSeenOpenHand = true;
      }
    }
    force += forceFromHand(info);
  }

  // scale the force by a constant in each direction to make sure we're not flying around the screen
  static const Vector3 FORCE_POSITION_SCALE(-0.15, -0.15, -0.25);
  force = FORCE_POSITION_SCALE.cwiseProduct(force);

  // make speeding up have less lag than slowing down
  static const float SPEED_UP_SMOOTH = 0.75f;
  static const float SLOW_DOWN_SMOOTH = 0.95f;
  static const float DECAY_MULTIPLIER = 0.6f;
  const Vector3 prevForce = m_panForce.value;
  const float curSmooth = force.squaredNorm() > prevForce.squaredNorm() ? SPEED_UP_SMOOTH : SLOW_DOWN_SMOOTH;
  m_panForce.Update(force + DECAY_MULTIPLIER*prevForce, timeSeconds, curSmooth);
}

void Interaction::UpdateActiveView(View& primary, View& secondary) {
  static const float VIEW_ACTIVATION_SMOOTH = 0.95f;

  const Vector2 secondaryMin = secondary.GetPositionLayout()->GetContentsMinBounds();
  const Vector2 secondaryMax = secondary.GetPositionLayout()->GetContentsMaxBounds();

  bool closerToSecondary = false;

  double closestTileZ = 0;
  const TileVector& tiles = secondary.Tiles();
  for (TileVector::const_iterator it = tiles.begin(); it != tiles.end(); ++it) {
    const Tile& tile = *it;
    closestTileZ = std::max(closestTileZ, tile.Position().z());
  }
  
  for (HandInfoMap::iterator it = m_handInfos.begin(); it != m_handInfos.end(); ++it) {
    Leap::Hand hand = it->second.Hand();
    
    const Vector3 origin = secondary.Position();
    const Vector3 direction = (leapToView(hand.palmPosition(), secondary.LookAt()) - origin).normalized();
    Vector3 hitPoint;
    if (RayPlaneIntersection(origin, direction, closestTileZ*Vector3::UnitZ(), Vector3::UnitZ(), hitPoint)) {
      closerToSecondary = hitPoint.x() >= secondaryMin.x() && hitPoint.x() <= secondaryMax.x() && hitPoint.y() >= secondaryMin.y() && hitPoint.y() <= secondaryMax.y();
    }
  }

  float newPrimaryActivation = 1.0f;
  float newSecondaryActivation = 0.0f;
  if (closerToSecondary) {
    std::swap(newPrimaryActivation, newSecondaryActivation);
  }

  primary.UpdateActivation(newPrimaryActivation, VIEW_ACTIVATION_SMOOTH);
  secondary.UpdateActivation(newSecondaryActivation, VIEW_ACTIVATION_SMOOTH);
}

void Interaction::UpdateView(View &view) {
  const double deltaTime = Globals::curTimeSeconds - view.LastUpdateTime();

  // apply the force to the view camera
  view.ApplyVelocity(view.Activation() * m_panForce.value);
  
  if (view.Activation() > 0.9f) {
    updateActiveTile(view, deltaTime);
  }
  updateInactiveTiles(view.Tiles());
  computeForcesFromTiles(view.Tiles(), view.Forces());
}

void Interaction::UpdateMeshHands(MeshHand& handL, MeshHand& handR) {
  const Leap::HandList hands = m_frame.hands();
  for (int i=0; i<hands.count(); i++) {
    if (hands[i].isLeft()) {
      handL.Update(hands[i], Globals::curTimeSeconds);
    } else if (hands[i].isRight()) {
      handR.Update(hands[i], Globals::curTimeSeconds);
    }
  }
}

void Interaction::updateHandInfos(double frameTime) {
  const Leap::HandList hands = m_frame.hands();
  for (int i=0; i<hands.count(); i++) {
    const int id = hands[i].id();
    m_handInfos[id].Update(hands[i], frameTime);
  }
}

void Interaction::cleanupHandInfos(double frameTime) {
  static const float MAX_HAND_INFO_AGE = 0.1f; // seconds since last update until hand info gets cleaned up
  HandInfoMap::iterator it = m_handInfos.begin();
  while (it != m_handInfos.end()) {
    const HandInfo& cur = it->second;
    const float curAge = fabs(static_cast<float>(frameTime - cur.LastUpdateTime()));
    if (curAge > MAX_HAND_INFO_AGE) {
      m_handInfos.erase(it++);
    } else {
      ++it;
    }
  }
}

void Interaction::updateActiveTile(View& view, double deltaTime) {
  const float viewActivation = view.Activation();
  for (HandInfoMap::iterator it = m_handInfos.begin(); it != m_handInfos.end(); ++it) {
    Leap::Hand hand = it->second.Hand();

    // find the closest tile to the hand based on a ray cast
    Tile* closestTile = findClosestTile(hand, view.Position(), view.LookAt(), it->second.ClosestTile(), view.Tiles());
    it->second.SetClosestTile(closestTile);

    // increase activation for closest tile to hand
    if (closestTile && Globals::haveSeenOpenHand) {
      // only allow activating the tile if it's already highlighted
      const float newActivation = viewActivation * closestTile->Highlight() * it->second.GrabPinchStrength();
      closestTile->UpdateActivation(newActivation, Tile::ACTIVATION_SMOOTH);
      closestTile->UpdateHighlight(std::min(viewActivation, closestTile->Highlight() + INFLUENCE_CHANGE_SPEED), Tile::ACTIVATION_SMOOTH);

      // calculate pulling force from hand
      const Vector3 grabDelta = it->second.GrabPinchStrength() * (closestTile->TargetGrabDelta() + deltaTime*it->second.VelocityAt((view.Position() - view.LookAt()), closestTile->Position() - view.LookAt()));
      closestTile->UpdateTargetGrabDelta(grabDelta);
      closestTile->UpdateGrabDelta(Tile::GRABDELTA_SMOOTH);
    }
  }
}

Tile* Interaction::findClosestTile(const Leap::Hand& hand, const Vector3& position, const Vector3& lookat, Tile* prevClosest, TileVector& tiles) {
  const Vector3 palmPoint = leapToView(hand.palmPosition(), lookat);
  const Vector3 direction = (palmPoint - position).normalized();

  Vector3 hitPoint;
  float closestDistSq = MAX_INFLUENCE_DISTANCE_SQ;
  Tile* closestTile = nullptr;
  for (TileVector::iterator it = tiles.begin(); it != tiles.end(); ++it) {
    Tile& tile = *it;

    // calculate projection point from camera to tile
    if (!tile.IsVisible() || !RayPlaneIntersection(position, direction, tile.Position(), Vector3::UnitZ(), hitPoint)) {
      continue;
    }

    // make it easier for this hand to hit its previous closest tile
    float distMult = 1.0f;
    if (&tile == prevClosest) {
      distMult = (1.0f - tile.Activation());
    }

    const float distSq = distMult * static_cast<float>((hitPoint - tile.Position()).squaredNorm());
    if (distSq < closestDistSq) {
      closestTile = &tile;
      closestDistSq = distSq;
    }
  }
  return closestTile;
}

void Interaction::updateInactiveTiles(TileVector& tiles) {
  // decrease activation for all tiles that weren't updated this time
  for (TileVector::iterator it = tiles.begin(); it != tiles.end(); ++it) {
    Tile& tile = *it;
    if (tile.LastActivationUpdateTime() != Globals::curTimeSeconds) {
      tile.UpdateActivation(std::max(0.0f, tile.Activation() - INFLUENCE_CHANGE_SPEED), Tile::ACTIVATION_SMOOTH);
      tile.UpdateHighlight(std::max(0.0f, tile.Highlight() - INFLUENCE_CHANGE_SPEED), Tile::ACTIVATION_SMOOTH);
      tile.UpdateTargetGrabDelta(INFLUENCE_CHANGE_SPEED * tile.TargetGrabDelta());
      tile.UpdateGrabDelta(Tile::GRABDELTA_SMOOTH);
    }
  }
}

void Interaction::computeForcesFromTiles(const TileVector& tiles, ForceVector& forces) {
  forces.clear();
  for (TileVector::const_iterator it = tiles.cbegin(); it != tiles.cend(); ++it) {
    const Tile& tile = *it;
    if (tile.Highlight() > 0.01f) {
      // add repulsive force from this tile to others
      forces.push_back(Force(tile.Position(), tile.Activation() + tile.Highlight()));
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

Vector3 Interaction::leapToView(const Leap::Vector& position, const Vector3& lookat) {
  return position.toVector3<Vector3>() + Globals::LEAP_OFFSET + lookat;
}
