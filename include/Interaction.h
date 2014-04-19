#ifndef __Interaction_h__
#define __Interaction_h__

#include "View.h"
#include "Utilities.h"

class Interaction {

public:

  Interaction();
  void Update(const Leap::Frame& frame);
  void UpdateView(View &view);

private:

  static Vector3 forceFromHand(const Leap::Hand& hand);

  Leap::Frame m_prevFrame;
  ExponentialFilter<Vector3> m_panForce;
  double m_lastViewUpdateTime;

};

#endif
