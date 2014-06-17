// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef __CinderFbxStream_h__
#define __CinderFbxStream_h__

#include "Resources.h"

class CinderFbxStream : public FbxStream {
public:
  CinderFbxStream(FbxManager* pSdkManager, const ci::DataSourceRef& resource);
  ~CinderFbxStream();

  virtual EState GetState();
  virtual bool Open(void*);
  virtual bool Close();
  virtual bool Flush();
  virtual int Write(const void* pData, int pSize);
  virtual int Read (void* pData, int pSize) const;
  virtual int GetReaderID() const;
  virtual int GetWriterID() const;
  virtual void Seek( const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos );
  virtual long GetPosition() const;
  virtual void SetPosition( long pPosition );
  virtual int GetError() const;
  virtual void ClearError();

private:
  // Data
  mutable int m_CurIdx;
  std::string m_Data;
  bool m_Opened;

  // Autodesk registry ID's
  int         m_ReaderId;
  int         m_WriterId;
};

#endif
