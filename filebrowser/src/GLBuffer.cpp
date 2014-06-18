// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "StdAfx.h"
#include "GLBuffer.h"

GLBuffer::GLBuffer() : type_(0), buffer_(0) { }

void GLBuffer::create(GLenum type) {
  type_ = type;
  glGenBuffers(1, &buffer_);
  checkError("Create");
}

void GLBuffer::bind() {
  glBindBuffer(type_, buffer_);
  checkError("Bind");
}

void GLBuffer::allocate(const void* data, int count, GLenum pattern) {
  glBufferData(type_, count, data, pattern);
  checkError("Allocate");
}

void GLBuffer::release() {
  glBindBuffer(type_, 0);
  checkError("Release");
}

int GLBuffer::size() const {
  GLint value = -1;
  glGetBufferParameteriv(type_, GL_BUFFER_SIZE, &value);
  checkError("Size");
  return value;
}

void* GLBuffer::map(GLuint access) {
  bind();
  void* ptr = glMapBufferARB(type_, access);
  release();
  checkError("Map");
  return ptr;
}

bool GLBuffer::unmap() {
  bind();
  bool result = glUnmapBufferARB(type_) == GL_TRUE;
  release();
  checkError("Unmap");
  return result;
}

bool GLBuffer::isCreated() const {
  return buffer_ != 0;
}

void GLBuffer::destroy() {
  glDeleteBuffers(1, &buffer_);
  buffer_ = 0;
}

void GLBuffer::checkError(const std::string& loc) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    std::cout << "GL error ";
    if (!loc.empty()) {
      std::cout << "at " << loc << ": ";
    }
    std::cout << "code: " << std::hex << err << std::endl;
  }
}

void GLBuffer::checkFrameBufferStatus(const std::string& loc) {
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer error ";
    if (!loc.empty()) {
      std::cout << "at " << loc << ": ";
    }
    std::cout << "code: " << std::hex << status << std::endl;
  }
}
