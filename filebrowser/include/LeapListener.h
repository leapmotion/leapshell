// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#if !defined(__LeapListener_h__)
#define __LeapListener_h__

class LeapListener : public Leap::Listener {
public:
  LeapListener();
  ~LeapListener();
  virtual void onInit(const Leap::Controller&);
  virtual void onConnect(const Leap::Controller&);
  virtual void onDisconnect(const Leap::Controller&);
  virtual void onFrame(const Leap::Controller&);
  virtual void onFocusGained(const Leap::Controller&);
  virtual void onFocusLost(const Leap::Controller&);
  std::deque<Leap::Frame> grabFrames();
  bool isConnected() const;
  const Leap::DeviceList& getDevices() const;

private:
  mutable std::mutex m_mutex;
  std::deque<Leap::Frame> m_frames;
  Leap::Frame m_lastFrame;
  bool m_isConnected;
  Leap::DeviceList m_devices;
};

#endif // __LeapListener_h__
