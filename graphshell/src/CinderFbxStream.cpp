#include "StdAfx.h"
#include "CinderFbxStream.h"

CinderFbxStream::CinderFbxStream(FbxManager* pSdkManager, const ci::DataSourceRef& resource) {
  static const char* format = "FBX (*.fbx)";
  m_ReaderId = pSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( format );
  m_WriterId = -1;

  ci::Buffer& buf = resource->getBuffer();
  m_Data = std::string((char*)buf.getData(), buf.getDataSize());
  m_Opened = false;
  m_CurIdx = 0;
}

CinderFbxStream::~CinderFbxStream() {
  Close();
}

FbxStream::EState CinderFbxStream::GetState() {
  return m_Opened ? FbxStream::eOpen : FbxStream::eClosed;
}

bool CinderFbxStream::Open(void* /*pStreamData*/) {
  m_CurIdx = 0;
  m_Opened = true;
  return true;
}

bool CinderFbxStream::Close() {
  m_Opened = false;
  return true;
}

bool CinderFbxStream::Flush() {
  m_CurIdx = 0;
  return true;
}

int CinderFbxStream::Write(const void* pData, int pSize) {
  return 0;
}

int CinderFbxStream::Read(void* pData, int pSize) const {
  const int numBytesAvailable = m_Data.size() - m_CurIdx;
  const int numBytesRead = std::min(numBytesAvailable, pSize);
  if (numBytesRead > 0) {
    char const* sourcePtr = m_Data.data() + m_CurIdx;
    memcpy(pData, (void*)sourcePtr, numBytesRead);
    m_CurIdx += numBytesRead;
  }
  return numBytesRead;
}

int CinderFbxStream::GetReaderID() const {
  return m_ReaderId;
}

int CinderFbxStream::GetWriterID() const {
  return m_WriterId;
}

void CinderFbxStream::Seek(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos) {
  switch (pSeekPos) {
  case FbxFile::eBegin:
    m_CurIdx = static_cast<int>(pOffset);
    break;
  case FbxFile::eCurrent:
    m_CurIdx += static_cast<int>(pOffset);
    break;
  case FbxFile::eEnd:
    m_CurIdx = static_cast<int>(m_Data.size()) - static_cast<int>(pOffset);
    break;
  }
}

long CinderFbxStream::GetPosition() const {
  return m_CurIdx;
}

void CinderFbxStream::SetPosition(long pPosition) {
  m_CurIdx = pPosition;
}

int CinderFbxStream::GetError() const {
  return 0;
}

void CinderFbxStream::ClearError() {
}
