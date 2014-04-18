#include "StdAfx.h"
#include "Interaction.h"

Interaction::Interaction() {
  m_panForce.Update(Vector3::Zero(), 0.0, 0.5f);
}

void Interaction::Update(const Leap::Frame& frame, View* view) {
  const Leap::HandList hands = frame.hands();
  const double deltaTime = m_prevFrame.isValid() ? TIME_STAMP_TICKS_TO_SECS * (frame.timestamp() - m_prevFrame.timestamp()) : 0.0f;
  const double timeSeconds = TIME_STAMP_TICKS_TO_SECS * frame.timestamp();

  // accumulate force from all hands
  Vector3 force(Vector3::Zero());
  for (int i=0; i<hands.count(); i++) {
    force += forceFromHand(hands[i]);
  }
  force.z() = 0; // disable zoom movement for now

  // scale the force by a constant to make sure we're not flying around the screen
  static const double FORCE_POSITION_SCALE = -0.1;
  force = FORCE_POSITION_SCALE * force;

  // make speeding up have less lag than slowing down
  static const float SPEED_UP_SMOOTH = 0.75f;
  static const float SLOW_DOWN_SMOOTH = 0.95f;
  const Vector3 prevForce = m_panForce.value;
  const float curSmooth = force.squaredNorm() > prevForce.squaredNorm() ? SPEED_UP_SMOOTH : SLOW_DOWN_SMOOTH;
  m_panForce.Update(force, timeSeconds, curSmooth);

  // apply the force to the view camera
  const Vector3 deltaPosition = deltaTime * m_panForce.value;
  view->SetPosition(view->Position() + deltaPosition);
  view->SetLookAt(view->LookAt() + deltaPosition);

  m_prevFrame = frame;
}

Vector3 Interaction::forceFromHand(const Leap::Hand& hand) {
  Vector3 totalForce = Vector3::Zero();
  const Vector3 handDirection = hand.direction().toVector3<Vector3>();
  const Leap::FingerList fingers = hand.fingers();
  for (int i=0; i<fingers.count(); i++) {
    if (!fingers[i].isExtended()) {
      continue;
    }
    const Vector3 direction = fingers[i].direction().toVector3<Vector3>();
    const Vector3 velocity = fingers[i].tipVelocity().toVector3<Vector3>();
    const Vector3 normVelocity = velocity.normalized();
    const double dot = std::abs(normVelocity.dot(hand.palmNormal().toVector3<Vector3>()));
    const double match = direction.dot(handDirection);
    totalForce += dot * dot * match * velocity;
  }
  return totalForce;
}
