// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#include "StdAfx.h"
#include "LeapListener.h"
#include "Globals.h"

LeapListener::LeapListener() : m_isConnected(false)
{
}

LeapListener::~LeapListener()
{
}

void LeapListener::onInit(const Leap::Controller& controller)
{
  m_devices = controller.devices();
}

void LeapListener::onConnect(const Leap::Controller& controller)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  controller.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);
#if USE_LEAP_IMAGE_API
  controller.setPolicyFlags(Leap::Controller::POLICY_IMAGES);
#endif

  m_devices = controller.devices();
  m_isConnected = true;
}

void LeapListener::onDisconnect(const Leap::Controller& controller)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  m_frames.clear();
  m_lastFrame = Leap::Frame();
  m_isConnected = false;
}

void LeapListener::onFocusGained(const Leap::Controller& controller)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  m_frames.clear();
  m_lastFrame = Leap::Frame();
}

void LeapListener::onFocusLost(const Leap::Controller& controller)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  m_frames.clear();
  m_lastFrame = Leap::Frame();
}

void LeapListener::onFrame(const Leap::Controller& controller)
{
  std::unique_lock<std::mutex> lock(m_mutex);

  if (m_lastFrame.isValid()) {
    int history = 0;

    // Find historical frames that we are missing
    while (controller.frame(++history).id() > m_lastFrame.id()) {
      ;;
    }
    // Add those historical frames to our deque
    while (--history > 0) {
      m_frames.push_back(controller.frame(history));
    }
  }
  m_lastFrame = controller.frame();
  m_frames.push_back(m_lastFrame);
}

std::deque<Leap::Frame> LeapListener::grabFrames()
{
  std::unique_lock<std::mutex> lock(m_mutex);

  std::deque<Leap::Frame> frames(std::move(m_frames));
  m_frames.clear();

  lock.unlock();

  return frames;
}

bool LeapListener::isConnected() const
{
  std::unique_lock<std::mutex> lock(m_mutex);

  return m_isConnected;
}

const Leap::DeviceList& LeapListener::getDevices() const {
  return m_devices;
}
