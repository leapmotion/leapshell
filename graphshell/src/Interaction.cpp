#include "StdAfx.h"
#include "Interaction.h"

Interaction::Interaction() {
  m_transformCenter = 1000*Vector3::UnitZ();
  m_transformVelocity = Vector3::Zero();
  m_translationVelocity = Vector3::Zero();
  m_scaleVelocity = 0.0f;
  m_lastDrawTime = 0;
  m_translation = Vector3::Zero();
  m_rotation = Matrix4x4::Identity();
  m_scale = 1.0f;
  m_transform = Matrix4x4::Identity();
}

void Interaction::Update(const Leap::Frame& frame) {
  const double timeSeconds = TIME_STAMP_TICKS_TO_SECS * frame.timestamp();
  m_frame = frame;
  updateHandInfos(timeSeconds);
  cleanupHandInfos(timeSeconds);
  calculateTransformCenter();
  calculateInteractions();
  calculateTransforms();
}

void Interaction::Draw() const {
  //m_objectShader.uniform("surfaceColor", ci::Vec3f(1.0f, 1.0f, 1.0f));
  //m_objectShader.uniform("ambient", 1.0f);
  //glPushMatrix();
  //glTranslated(-m_transformCenter.x(), -m_transformCenter.y(), -m_transformCenter.z());
  //m_geometry.Draw(vertexAttr, normalAttr);
  glPushMatrix();
  //glScalef(-1, -1, -1);
  ci::ColorA color = ci::ColorA::gray(1.0f);
  ci::gl::color(color);
  ci::gl::drawSphere(ToVec3f(m_transformCenter), 7.0f, 20);
  //glPopMatrix();
  for (HandInfoMap::const_iterator it = m_handInfos.cbegin(); it != m_handInfos.cend(); ++it) {
    const HandInfo& cur = it->second;
    cur.DrawHand();
    const Vector3 pos = cur.PalmPosition();
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

      //m_rotationVelocity += cur.RotationAngleVelocity() * cur.RotationAxis() * grabPinch;
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

void Interaction::calculateTransforms() {
  const double deltaTime = Globals::curTimeSeconds - m_lastDrawTime;
  const Vector3 translation = TranslationVelocity() * deltaTime;
  const Matrix4x4 rotationInv = m_rotation;// .inverse();
  const Vector3 transformCenter = -TransformCenter();

  //m_translation += (rotationInv * Vector4(translation.x(), translation.y(), translation.z(), 0.0)).head<3>();
  m_translation += translation;

  m_transform = TranslationMatrix(translation) * m_transform;

  m_scale += (ScaleVelocity() * deltaTime);

  const Vector3 rotation = deltaTime * RotationVelocity();
  const double rotationAngle = rotation.norm();
  const Vector3 rotationAxis = rotation / rotationAngle;
  if (abs(rotationAngle) > 0.001) {
    const Matrix4x4 rotationMatrix = RotationMatrix(rotationAxis, -rotationAngle);
    //m_rotation = m_rotation * rotationMatrix;
    m_rotation = rotationMatrix * m_rotation;
    //m_transform = TranslationMatrix(-m_translation) * m_transform;
    m_transform = TranslationMatrix(transformCenter) * m_transform;
    m_transform = rotationMatrix * m_transform;
    m_transform = TranslationMatrix(-transformCenter) * m_transform;
    //m_transform = TranslationMatrix(m_translation) * m_transform;
  }

  m_transform = TranslationMatrix(transformCenter) * m_transform;
  m_transform = ScaleMatrix(Vector3::Constant(1.0 + ScaleVelocity() * deltaTime)) * m_transform;
  m_transform = TranslationMatrix(-transformCenter) * m_transform;

  m_lastDrawTime = Globals::curTimeSeconds;
}

Matrix4x4 HandInfo::oculusTransform = Matrix4x4::Identity();
Vector3 HandInfo::oculusOffset = Vector3::Zero();

void HandInfo::DrawHand() const {
  const Leap::FingerList fingers = m_hand.fingers();
  for (int i=0; i<fingers.count(); i++) {
    for (int j=0; j<4; j++) {
      Leap::Bone curBone = fingers[i].bone(static_cast<Leap::Bone::Type>(j));
      const Vector3 prev = leapToWorld(curBone.prevJoint().toVector3<Vector3>(), false);
      const Vector3 next = leapToWorld(curBone.nextJoint().toVector3<Vector3>(), false);

      ci::gl::color(ci::ColorA(0.4f, 0.7f, 1.0f, 1.0f));
      glLineWidth(3.0f);
      glBegin(GL_LINES);
      glVertex3d(prev.x(), prev.y(), prev.z());
      glVertex3d(next.x(), next.y(), next.z());
      glEnd();

      ci::gl::color(ci::ColorA::gray(0.5f));
      glLineWidth(7.0f);
      glBegin(GL_LINES);
      glVertex3d(prev.x(), prev.y(), prev.z());
      glVertex3d(next.x(), next.y(), next.z());
      glEnd();

      ci::gl::color(ci::ColorA(0.4f, 1.0f, 0.7f, 1.0f));
      glPointSize(15.0f);
      glBegin(GL_POINTS);
      glVertex3d(prev.x(), prev.y(), prev.z());
      glVertex3d(next.x(), next.y(), next.z());
      glEnd();
    }
  }
}

Vector3 HandInfo::leapToWorld(const Vector3& pos, bool vel) {
  /*
  old
  x = right
  y = up
  z = back

  new
  x = right
  y = forward
  z = up
  */

  Vector3 result = pos;
  //result.x() *= -1.0;
  //result.y() *= -1.0;
  //result.z() *= -1.0;

  std::swap(result.y(), result.z());
  result.y() *= -1;
#if 1
  //std::cout << "before: " << result.transpose() << std::endl;

  Vector3 finalResult = (oculusTransform * Vector4(result.x(), result.y(), result.z(), vel ? 0.0 : 1.0)).head<3>();
  //std::cout << "after: " << finalResult.transpose() << std::endl;
  //finalResult *= 2.0;
#endif
  return finalResult + oculusOffset;
}
