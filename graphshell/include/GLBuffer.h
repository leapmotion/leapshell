#ifndef __GLBuffer_h__
#define __GLBuffer_h__

typedef unsigned int GLuint;
typedef unsigned int GLenum;

class GLBuffer {

public:

  GLBuffer();
  void create(GLenum type);
  void bind();
  void allocate(const void* data, int count, GLenum pattern);
  void release();
  int size() const;
  void* map(GLuint access);
  bool unmap();
  bool isCreated() const;
  void destroy();
  static void checkError(const std::string& loc = "");
  static void checkFrameBufferStatus(const std::string& loc = "");

private:

  GLuint m_buffer;
  GLenum m_type;

};

#endif
