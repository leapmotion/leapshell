#include "StdAfx.h"
#include "GLBuffer.h"

GLBuffer::GLBuffer() : m_type(0), m_buffer(0) { }

void GLBuffer::create(GLenum type) {
  m_type = type;
  glGenBuffers(1, &m_buffer);
  checkError("Create");
}

void GLBuffer::bind() {
  glBindBuffer(m_type, m_buffer);
  checkError("Bind");
}

void GLBuffer::allocate(const void* data, int count, GLenum pattern) {
  glBufferData(m_type, count, data, pattern);
  checkError("Allocate");
}

void GLBuffer::release() {
  glBindBuffer(m_type, 0);
  checkError("Release");
}

int GLBuffer::size() const {
  GLint value = -1;
  glGetBufferParameteriv(m_type, GL_BUFFER_SIZE, &value);
  checkError("Size");
  return value;
}

void* GLBuffer::map(GLuint access) {
  bind();
  void* ptr = glMapBufferARB(m_type, access);
  release();
  checkError("Map");
  return ptr;
}

bool GLBuffer::unmap() {
  bind();
  bool result = glUnmapBufferARB(m_type) == GL_TRUE;
  release();
  checkError("Unmap");
  return result;
}

bool GLBuffer::isCreated() const {
  return m_buffer != 0;
}

void GLBuffer::destroy() {
  glDeleteBuffers(1, &m_buffer);
  m_buffer = 0;
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
