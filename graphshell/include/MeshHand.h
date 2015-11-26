// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#ifndef __MeshHand_h__
#define __MeshHand_h__

#include "CinderFbxStream.h"
#include "GLBuffer.h"

class MeshHand {
public:

  //typedef enum Side { LEFT, RIGHT } Side;

  //MeshHand(const std::string& name, Side side);

  //void Update(const Leap::Hand& hand, double timeSeconds);

  //void SetScale(double scale);

  //void Draw();

  //double LastUpdateTime() const { return m_lastUpdateTime; }

  //Leap::Hand LeapHand() { return m_Hand; }

  //static void SetMeshSources(const ci::DataSourceRef& leftSource, const ci::DataSourceRef& rightSource);
  //static void SetAttributeIndices(int vertexIdx, int normalIdx, int colorIdx);

private:

  //struct FbxLoader {
  //  bool loaded;
  //  bool error;
  //  FbxManager* manager;
  //  FbxIOSettings* ioSettings;
  //  FbxImporter* leftImporter;
  //  FbxImporter* rightImporter;
  //  CinderFbxStream* leftHandStream;
  //  CinderFbxStream* rightHandStream;
  //  std::string errorString;
  //  FbxLoader() : loaded(false), error(false) { }
  //  void load() {
  //    if (!loaded) {
  //      manager = FbxManager::Create();
  //      ioSettings = FbxIOSettings::Create(manager, IOSROOT);
  //      manager->SetIOSettings(ioSettings);

  //      leftImporter = FbxImporter::Create(manager, "");
  //      leftHandStream = new CinderFbxStream(manager, g_leftDataSource);
  //      if (!leftImporter->Initialize(leftHandStream, NULL, -1, ioSettings)) {
  //        error = true;
  //        errorString = leftImporter->GetStatus().GetErrorString();
  //      }

  //      rightImporter = FbxImporter::Create(manager, "");
  //      rightHandStream = new CinderFbxStream(manager, g_rightDataSource);
  //      if (!rightImporter->Initialize(rightHandStream, NULL, -1, ioSettings)) {
  //        error = true;
  //        errorString = rightImporter->GetStatus().GetErrorString();
  //      }

  //      loaded = true;
  //    }
  //  }
  //  FbxScene* createScene(Side side) {
  //    FbxScene* scene = FbxScene::Create(manager, "simScene");
  //    if (side == LEFT) {
  //      leftImporter->Import(scene);
  //    } else if (side == RIGHT) {
  //      rightImporter->Import(scene);
  //    }
  //    FbxSystemUnit::mm.ConvertScene(scene);
  //    return scene;
  //  }
  //};

  //static const Vector3& colorForIndex(int idx) {
  //  static const int NUM_COLORS = 256;
  //  static Vector3 colors[NUM_COLORS];
  //  static bool loaded = false;
  //  idx = idx % NUM_COLORS;
  //  if (!loaded) {
  //    const float inc = 0.2f;
  //    float curAdd = 0;
  //    float curVal;
  //    for (int i=0; i<NUM_COLORS; i++) {
  //      curVal = inc*ci::randFloat() + curAdd;
  //      if (curVal > 1.0f) {
  //        curVal -= 1.0f;
  //      }
  //      ci::Vec3f hsv(curVal, 0.75f, 0.95f);
  //      ci::Colorf rgb = ci::hsvToRGB(hsv);
  //      colors[i] << rgb.r, rgb.g, rgb.b;
  //      curAdd += inc;
  //      if (curAdd > 1.0f) {
  //        curAdd -= 1.0f;
  //      }
  //    }
  //    loaded = true;
  //  }
  //  return colors[idx];
  //}

  //double m_time;

  //// update functions
  //void updateNodeRecursive(FbxNode* node, FbxAMatrix& parentGlobalPosition);
  //void updateMesh(FbxNode* node, FbxAMatrix& globalPosition);
  //void drawNodeRecursive(FbxNode* node, FbxAMatrix& parentGlobalPosition);
  //void drawSkeleton(FbxNode* node, FbxAMatrix& parentGlobalPosition, FbxAMatrix& globalPosition);
  //void drawMesh(FbxNode* node, FbxAMatrix& globalPosition);

  //// skin deformation functions
  //void computeShapeDeformation(FbxMesh* mesh, FbxVector4* vertexArray);
  //void computeSkinDeformation(FbxAMatrix& globalPosition, FbxMesh* mesh, FbxVector4* vertexArray, FbxVector4* colorArray);
  //void computeLinearDeformation(FbxAMatrix& globalPosition, FbxMesh* mesh, FbxVector4* vertexArray, FbxVector4* colorArray);
  //void computeDualQuaternionDeformation(FbxAMatrix& globalPosition, FbxMesh* mesh, FbxVector4* vertexArray);
  //void computeClusterDeformation(FbxAMatrix& globalPosition, FbxMesh* mesh, FbxCluster* cluster, FbxAMatrix& vertexTransformMatrix);

  //// convenience functions
  //FbxNode* getNodeByName(FbxNode* start, const std::string& name);
  //FbxAMatrix getGlobalPosition(FbxNode* node, FbxAMatrix* parentGlobalPosition = 0);
  //FbxAMatrix getGeometry(FbxNode* node);
  //static void matrixScale(FbxAMatrix& matrix, double value);
  //static void matrixAddToDiagonal(FbxAMatrix& matrix, double value);
  //static void matrixAdd(FbxAMatrix& dstMatrix, FbxAMatrix& srcMatrix);
  //FbxVector4 getColorForNode(const std::string& name) const;
  //void initBuffers(FbxMesh* mesh);

  //std::map< int, std::vector<FbxVector4> > m_VertexArrays;
  //std::map< int, std::vector<FbxVector4> > m_NormalArrays;
  //std::map< int, std::vector<FbxVector4> > m_ColorArrays;

  //FbxAMatrix* m_clusterDeformation;
  //double* m_clusterWeight;

  //static FbxLoader g_Loader;
  //FbxScene* m_Scene;

  //Leap::Hand m_Hand;
  //Side m_side;
  //std::string m_sideString;
  //std::string m_fingerStrings[5];
  //FbxDouble3 m_handScale;
  //FbxNode* m_rootNode;
  //double m_lastUpdateTime;
  //static int g_colorCounter;

  //// buffers for rendering
  //static int g_vertexAttribIdx;
  //static int g_normalAttribIdx;
  //static int g_colorAttribIdx;
  //GLBuffer m_vertexBuffer;
  //GLBuffer m_normalBuffer;
  //GLBuffer m_colorBuffer;
  //GLBuffer m_indexBuffer;

  //static ci::DataSourceRef g_leftDataSource;
  //static ci::DataSourceRef g_rightDataSource;

};

#endif
