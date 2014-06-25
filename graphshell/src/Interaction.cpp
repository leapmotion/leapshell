#include "StdAfx.h"
#include "Interaction.h"

Interaction::Interaction() {
  m_transformCenter = Vector3::Zero();
  m_transformVelocity = Vector3::Zero();
  m_translationVelocity = Vector3::Zero();
  m_scaleVelocity = 0.0f;
}

void Interaction::Update(const Leap::Frame& frame) {
  const double timeSeconds = TIME_STAMP_TICKS_TO_SECS * frame.timestamp();
  m_frame = frame;
  updateHandInfos(timeSeconds);
  cleanupHandInfos(timeSeconds);
  calculateTransformCenter();
  calculateInteractions();
}

void Interaction::Draw() const {
  //m_objectShader.uniform("surfaceColor", ci::Vec3f(1.0f, 1.0f, 1.0f));
  //m_objectShader.uniform("ambient", 1.0f);
  //glPushMatrix();
  //glTranslated(-m_transformCenter.x(), -m_transformCenter.y(), -m_transformCenter.z());
  //m_geometry.Draw(vertexAttr, normalAttr);
  glPushMatrix();
  glScalef(-1, -1, -1);
  ci::ColorA color = ci::ColorA::gray(1.0f);
  ci::gl::color(color);
  ci::gl::drawSphere(ToVec3f(m_transformCenter), 7.0f, 20);
  //glPopMatrix();
  for (HandInfoMap::const_iterator it = m_handInfos.cbegin(); it != m_handInfos.cend(); ++it) {
    const HandInfo& cur = it->second;
    const Vector3& pos = cur.PalmPosition();
    const double grabPinch = cur.GrabPinchStrength();
    ci::ColorA handColor = ci::ColorA(grabPinch, 0.0f, 1.0-grabPinch);
    ci::gl::color(handColor);
    ci::gl::drawSphere(ToVec3f(pos), 5.0f, 20);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex3dv(m_transformCenter.data());
    glVertex3dv(pos.data());
    glEnd();
  }
  glPopMatrix();
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

void Interaction::calculateTransformCenter() {
  Vector3 sumVel = Vector3::Zero();
  double sumGrabPinch = 0;
  for (HandInfoMap::iterator it = m_handInfos.begin(); it != m_handInfos.end(); ++it) {
    HandInfo& cur = it->second;
    sumVel += cur.Velocity();
    sumGrabPinch += cur.GrabPinchStrength();
  }

  // compute total velocity
  const double sumVelNorm = sumVel.norm();

  // compute weighted average of hands depending on how fast they're moving
  Vector3 totalWeightedPos = Vector3::Zero();
  Vector3 totalWeightedVelocity = Vector3::Zero();
  double totalWeight = 0;
  double totalGrabPinch = 0;
  m_transformStrength = 0.0f;
  Vector3 totalPos = Vector3::Zero();
  double numPos = 0.0;
  for (HandInfoMap::iterator it = m_handInfos.begin(); it != m_handInfos.end(); ++it) {
    HandInfo& cur = it->second;
    const double curVelNorm = cur.Velocity().norm();
    const double curVelRatio = 1.0 - curVelNorm / sumVelNorm;
    const double curGrabPinchRatio = cur.GrabPinchStrength();
    //const double grabPinch = cur.GrabPinchStrength();

    const double weight = curGrabPinchRatio; // *curVelRatio;
    totalWeightedPos += weight * cur.PalmPosition();
    totalWeightedVelocity += weight * cur.Velocity();

    totalWeight += weight;
    //m_transformStrength += cur.GrabPinchStrength();

    totalPos += cur.PalmPosition();
    numPos++;
  }

  if (totalWeight > 0.5) {
    m_transformCenter = totalWeightedPos / totalWeight;
    m_transformStrength = totalWeight;
    //m_transformVelocity = totalWeightedVelocity / totalWeight;
  } else if (numPos > 0) {
    m_transformCenter = totalPos /= numPos;
    m_transformStrength = 0.0f;
  } else {
    m_transformStrength = 0.0f;
  }
}

void Interaction::calculateInteractions() {
  /*
  1) calculate vectors from hand to transform center
  2) decompose velocity into perpendicular and parallel components along that vector
  3) use parallel component for scale
  4) use perpendicular component for rotational torque
  5) use overall velocity for translation
  */

  m_translationVelocity.setZero();
  m_rotationVelocity.setZero();
  m_scaleVelocity = 0.0f;

  if (m_handInfos.size() <= 1) {
    for (HandInfoMap::iterator it = m_handInfos.begin(); it != m_handInfos.end(); ++it) {
      HandInfo& cur = it->second;
      const Vector3 position = cur.PalmPosition();
      const Vector3 diff = position - m_transformCenter;
      const Vector3 velocity = cur.Velocity();
      const double grabPinch = cur.GrabPinchStrength();
      m_translationVelocity += velocity * grabPinch;
      m_transformCenter = position;

      m_rotationVelocity += cur.RotationAngleVelocity() * cur.RotationAxis() * grabPinch;
      //m_scaleVelocity += cur.ScaleVelocity();
    }
  } else {
    double totalWeight = 0;
    for (HandInfoMap::iterator it = m_handInfos.begin(); it != m_handInfos.end(); ++it) {
      HandInfo& cur = it->second;
      const Vector3 position = cur.PalmPosition();
      const Vector3 diff = position - m_transformCenter;
      const Vector3 velocity = cur.Velocity();
      const double grabPinch = cur.GrabPinchStrength();
      if (diff.squaredNorm() > 0.1) {
        const Vector3 along = diff * (diff.dot(velocity)) / diff.squaredNorm();
        const Vector3 perp = velocity - along;


        const Vector3 rotationAxis = perp.cross(diff).normalized();
        m_rotationVelocity += std::atan2(perp.norm(), diff.norm()) * rotationAxis * grabPinch;

        const double beta2 = 5.0;
        m_scaleVelocity += grabPinch * std::log((beta2 + (diff + along).norm()) / (beta2 + diff.norm()));
      }

      m_translationVelocity += velocity * grabPinch;
      totalWeight += 1.0;
    }
    m_translationVelocity /= totalWeight;
  }
}
