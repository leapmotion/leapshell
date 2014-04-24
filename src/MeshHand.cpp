// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "StdAfx.h"
#include "MeshHand.h"
#include "Globals.h"

static const double RADIANS_TO_DEGREES =  180.0 / M_PI;
static const double DEGREES_TO_RADIANS = M_PI / 180.0;
MeshHand::FbxLoader MeshHand::g_Loader;
ci::DataSourceRef MeshHand::g_leftDataSource;
ci::DataSourceRef MeshHand::g_rightDataSource;
int MeshHand::g_colorCounter = 0;
int MeshHand::g_vertexAttribIdx = 0;
int MeshHand::g_normalAttribIdx = 0;
int MeshHand::g_colorAttribIdx = 0;

Vector3 toEulerAngles(const Eigen::Quaterniond& quat) {
  const double& q0 = quat.w();
  const double& q1 = quat.x();
  const double& q2 = quat.y();
  const double& q3 = quat.z();

  double rotX = std::atan2(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2));
  double rotY = std::asin(2*(q0*q2 - q3*q1));
  double rotZ = std::atan2(2*(q0*q3 + q1*q2), 1 - 2*(q2*q2 + q3*q3));
  Vector3 result(rotX, rotY, rotZ);
  return result;
}

MeshHand::MeshHand(const std::string& name, Side side) : m_Scene(0), m_time(0), m_side(side), m_lastUpdateTime(0.0),
  m_vertexBuffer(GL_ARRAY_BUFFER), m_normalBuffer(GL_ARRAY_BUFFER), m_colorBuffer(GL_ARRAY_BUFFER), m_indexBuffer(GL_ELEMENT_ARRAY_BUFFER)
{
  g_Loader.load();
  if (g_Loader.error) {
    std::cerr << "MeshHand: " << g_Loader.errorString << std::endl;
    return;
  }

  m_Scene = g_Loader.createScene(side);

  std::stringstream ss;
  m_sideString = side == LEFT ? "L" : "R";
  ss.str("");
  ss << "Bip01 " << m_sideString << " Hand";
  m_rootNode = m_Scene->GetRootNode();
  m_handScale = m_rootNode->LclScaling.Get();
  FbxDouble3 scale = m_handScale;
  scale[0] *= 1.3;
  scale[1] *= 1.3;
  scale[2] *= 1.3;

  ss.str("");
  ss << "Bip01 " << m_sideString << " Finger";
  std::string baseString = ss.str();
  for (int i=0; i<5; i++) {
    ss.str("");
    ss << baseString << i;
    m_fingerStrings[i] = ss.str();
  }
  m_clusterDeformation = nullptr;
  m_clusterWeight = nullptr;
}

void MeshHand::Update(const Leap::Hand& hand, double timeSeconds) {
  m_Hand = hand;
  m_lastUpdateTime = timeSeconds;
  g_colorCounter++;
}

void MeshHand::SetScale(double scale) {
  FbxDouble3 handScale = m_handScale;
  handScale[0] *= scale;
  handScale[1] *= scale;
  handScale[2] *= scale;
  m_rootNode->LclScaling.Set(handScale);
}

void MeshHand::Draw() {
  if (g_Loader.error || !m_Hand.isValid()) {
    return;
  }

  std::stringstream ss;
  const std::string handStr = m_side == LEFT ? "L" : "R";
  FbxNode* root = m_Scene->GetRootNode();
  ss.str("");
  ss << "Bip01 " << handStr << " Hand";
  FbxNode* handNode = getNodeByName(root, ss.str());

  double distancePalmToWrist = 40;

  const Vector3 direction = m_Hand.direction().toVector3<Vector3>();
  const Vector3 position = m_Hand.palmPosition().toVector3<Vector3>();
  const Vector3 normal = m_Hand.palmNormal().toVector3<Vector3>();

  Matrix3x3 basis;
  Vector3 side = normal.cross(direction).normalized();
  basis << side, -normal, -direction;

  const Vector3 handPosition = position - distancePalmToWrist*direction + Globals::LEAP_OFFSET;

  double pitch = m_Hand.direction().pitch();
  double yaw = -m_Hand.direction().yaw();
  double roll = m_Hand.palmNormal().roll();

  // enforce X Y Z order
  FbxAMatrix xMat, yMat, zMat;
  xMat.SetR(FbxVector4(RADIANS_TO_DEGREES*pitch, 0, 0));
  yMat.SetR(FbxVector4(0, RADIANS_TO_DEGREES*yaw, 0));
  zMat.SetR(FbxVector4(0, 0, RADIANS_TO_DEGREES*roll));
  FbxAMatrix rotMat = xMat * yMat * zMat;

  FbxDouble3 handTrans(handPosition.x(), handPosition.y(), handPosition.z());

  FbxVector4 rot = rotMat.GetR();

  handNode->LclRotation.Set(rot);
  root->LclTranslation.Set(handTrans);

  Matrix3x3 handRot = basis;
  /*if (m_side == RIGHT) {
    handRot.col(0) *= -1;
  }*/
  handRot = handRot.inverse().eval();

  const Leap::FingerList fingers = m_Hand.fingers();
  for (int i=0; i<5; i++) {
    const Vector3 mcpPos = handRot * fingers[i].jointPosition(Leap::Finger::JOINT_MCP).toVector3<Vector3>();
    const Vector3 pipPos = handRot * fingers[i].jointPosition(Leap::Finger::JOINT_PIP).toVector3<Vector3>();
    const Vector3 dipPos = handRot * fingers[i].jointPosition(Leap::Finger::JOINT_DIP).toVector3<Vector3>();
    const Vector3 tipPos = handRot * fingers[i].jointPosition(Leap::Finger::JOINT_TIP).toVector3<Vector3>();

    ss.str("");
    ss << "Bip01 " << handStr << " Finger" << i;
    FbxNode* mcp = getNodeByName(root, ss.str());
    ss.str("");
    ss << "Bip01 " << handStr << " Finger" << i << "1";
    FbxNode* pip = getNodeByName(root, ss.str());
    ss.str("");
    ss << "Bip01 " << handStr << " Finger" << i << "2";
    FbxNode* dip = getNodeByName(root, ss.str());

    Eigen::Quaterniond quat1 = Eigen::Quaterniond::FromTwoVectors(handRot * direction, (pipPos - mcpPos).normalized());
    Vector3 temp1 = -RADIANS_TO_DEGREES * toEulerAngles(quat1);
    mcp->LclRotation.Set(FbxDouble3(temp1.z(), temp1.y(), temp1.x()));

    Eigen::Quaterniond quat2 = Eigen::Quaterniond::FromTwoVectors((pipPos - mcpPos).normalized(), (dipPos - pipPos).normalized());
    Vector3 temp2 = -RADIANS_TO_DEGREES * toEulerAngles(quat2);
    pip->LclRotation.Set(FbxDouble3(temp2.z(), temp2.y(), temp2.x()));

    Eigen::Quaterniond quat3 = Eigen::Quaterniond::FromTwoVectors((dipPos - pipPos).normalized(), (tipPos - dipPos).normalized());
    Vector3 temp3 = -RADIANS_TO_DEGREES * toEulerAngles(quat3);
    dip->LclRotation.Set(FbxDouble3(temp3.z(), temp3.y(), temp3.x()));
  }

  FbxAMatrix globalPosition;
  updateNodeRecursive(root, globalPosition);
  globalPosition.SetIdentity();
  drawNodeRecursive(root, globalPosition);
}

void MeshHand::SetMeshSources(const ci::DataSourceRef& leftSource, const ci::DataSourceRef& rightSource) {
  g_leftDataSource = leftSource;
  g_rightDataSource = rightSource;
}

void MeshHand::SetAttributeIndices(int vertexIdx, int normalIdx, int colorIdx) {
  g_vertexAttribIdx = vertexIdx;
  g_normalAttribIdx = normalIdx;
  g_colorAttribIdx = colorIdx;
}

FbxNode* MeshHand::getNodeByName(FbxNode* start, const std::string& name) {
  if (start->GetName() == name) {
    return start;
  }
  FbxNode* temp;
  for (int i=0; i<start->GetChildCount(); i++) {
    temp = getNodeByName(start->GetChild(i), name);
    if (temp) {
      return temp;
    }
  }
  return NULL;
}

void MeshHand::updateNodeRecursive(FbxNode* node, FbxAMatrix& parentGlobalPosition) {
  FbxAMatrix curGlobalPosition = getGlobalPosition(node, &parentGlobalPosition);
  if (node->GetNodeAttribute()) {
    FbxAMatrix geometryOffset = getGeometry(node);
    FbxAMatrix globalOffsetPosition = curGlobalPosition * geometryOffset;
    FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
    if (nodeAttribute) {
      if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
        updateMesh(node, globalOffsetPosition);
      }
    }
  }
  const int numChildren = node->GetChildCount();
  for (int i=0; i<numChildren; i++) {
    updateNodeRecursive(node->GetChild(i), curGlobalPosition);
  }
}

void MeshHand::drawNodeRecursive(FbxNode* node, FbxAMatrix& parentGlobalPosition) {
  FbxAMatrix curGlobalPosition = getGlobalPosition(node, &parentGlobalPosition);
  if (node->GetNodeAttribute()) {
    FbxAMatrix geometryOffset = getGeometry(node);
    FbxAMatrix globalOffsetPosition = curGlobalPosition * geometryOffset;
    FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
    if (nodeAttribute) {
      if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
        //drawSkeleton(node, parentGlobalPosition, globalOffsetPosition);
      } else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
        drawMesh(node, globalOffsetPosition);
      }
    }
  }
  const int numChildren = node->GetChildCount();
  for (int i=0; i<numChildren; i++) {
    drawNodeRecursive(node->GetChild(i), curGlobalPosition);
  }
}

void MeshHand::drawSkeleton(FbxNode* node, FbxAMatrix& parentGlobalPosition, FbxAMatrix& globalPosition) {
  FbxSkeleton* skeleton = (FbxSkeleton*)node->GetNodeAttribute();
  if (skeleton->GetSkeletonType() == FbxSkeleton::eLimbNode &&
    node->GetParent() &&
    node->GetParent()->GetNodeAttribute() &&
    node->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
    glDisable(GL_DEPTH_TEST);
    glColor3f(1, 0, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex3dv((GLdouble*)parentGlobalPosition.GetT());
    glVertex3dv((GLdouble*)globalPosition.GetT());
    glEnd();
    glEnable(GL_DEPTH_TEST);
  }
}

void MeshHand::updateMesh(FbxNode* node, FbxAMatrix& globalPosition) {
  FbxMesh* mesh = node->GetMesh();
  const int numVertices = mesh->GetControlPointsCount();
  const FbxVector4* meshVertices = mesh->GetControlPoints();
  if (numVertices == 0) {
    return;
  }

  std::vector<FbxVector4>& vertexArray = m_VertexArrays[static_cast<int>(node->GetUniqueID())];
  std::vector<FbxVector4>& normalArray = m_NormalArrays[static_cast<int>(node->GetUniqueID())];
  std::vector<FbxVector4>& colorArray = m_ColorArrays[static_cast<int>(node->GetUniqueID())];

  //std::cout << m_VertexArrays.size() << " " << m_NormalArrays.size() << std::endl;

  vertexArray.resize(numVertices);
  normalArray.resize(numVertices);
  colorArray.resize(numVertices);

  memcpy(vertexArray.data(), mesh->GetControlPoints(), numVertices*sizeof(FbxVector4));
  memset(normalArray.data(), 0, numVertices*sizeof(FbxVector4));
  memset(colorArray.data(), 0, numVertices*sizeof(FbxVector4));

  /*if (mesh->GetShapeCount() > 0) {
    computeShapeDeformation(mesh, vertexArray.data());
  }*/

  const int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
  int clusterCount = 0;
  for (int i=0; i<skinCount; i++) {
    FbxSkin* skin = (FbxSkin*)(mesh->GetDeformer(i, FbxDeformer::eSkin));
    clusterCount += skin->GetClusterCount();
  }
  if (clusterCount > 0) {
    computeSkinDeformation(globalPosition, mesh, vertexArray.data(), colorArray.data());
  }

  // transform vertices by matrix
  for (int i=0; i<numVertices; i++) {
    vertexArray[i][3] = 1;
    vertexArray[i] = globalPosition.MultT(vertexArray[i]);
  }

  const bool flip = !globalPosition.IsRightHand();

  // recompute normals
  const int numPolygons = mesh->GetPolygonCount();
  FbxVector4 normal;
  for (int i=0; i<numPolygons; i++) {
    const int vertexCount = mesh->GetPolygonSize(i);
    if (vertexCount < 3) { continue; }
    FbxVector4 a = vertexArray[mesh->GetPolygonVertex(i, 0)];
    FbxVector4 b = vertexArray[mesh->GetPolygonVertex(i, 1)];
    FbxVector4 c = vertexArray[mesh->GetPolygonVertex(i, 2)];
    normal = (b-a).CrossProduct(c-a);
    if (flip) { normal *= -1; }
    for (int j=0; j<vertexCount; j++) {
      normalArray[mesh->GetPolygonVertex(i, j)] += normal;
    }
  }
  for (int i=0; i<numVertices; i++) {
    normalArray[i][3] = 0;
    normalArray[i].Normalize();
  }

  if (!m_vertexBuffer.isCreated()) {
    initBuffers(mesh);
  }

  // copy vertices, normals, and colors to GPU
  GLfloat* vertexArrayPtr = (GLfloat*)m_vertexBuffer.map(GL_WRITE_ONLY);
  GLfloat* normalArrayPtr = (GLfloat*)m_normalBuffer.map(GL_WRITE_ONLY);
  GLfloat* colorArrayPtr = (GLfloat*)m_colorBuffer.map(GL_WRITE_ONLY);
  for (int i=0; i<numVertices; i++) {
    const FbxVector4& pos = vertexArray[i];
    for (int j=0; j<3; j++) {
      vertexArrayPtr[3*i + j] = static_cast<GLfloat>(pos[j]);
    }

    const FbxVector4& normal = normalArray[i];
    for (int j=0; j<3; j++) {
      normalArrayPtr[3*i + j] = static_cast<GLfloat>(normal[j]);
    }

    const FbxVector4& color = colorArray[i];
    for (int j=0; j<4; j++) {
      colorArrayPtr[4*i + j] = static_cast<GLfloat>(color[j]);
    }
  }
  m_vertexBuffer.unmap();
  m_normalBuffer.unmap();
  m_colorBuffer.unmap();

  // copy indices to GPU
  GLuint* indexArrayPtr = (GLuint*)m_indexBuffer.map(GL_WRITE_ONLY);
  for (int i=0; i<numPolygons; i++) {
    for (int j=0; j<3; j++) {
      indexArrayPtr[3*i + j] = mesh->GetPolygonVertex(i, j);
    }
  }
  m_indexBuffer.unmap();
}

void MeshHand::drawMesh(FbxNode* node, FbxAMatrix& globalPosition) {
  FbxMesh* mesh = node->GetMesh();
  const int numPolygons = mesh->GetPolygonCount();
#if 0
  std::vector<FbxVector4>& vertexArray = m_VertexArrays[static_cast<int>(node->GetUniqueID())];
  std::vector<FbxVector4>& normalArray = m_NormalArrays[static_cast<int>(node->GetUniqueID())];
  std::vector<FbxVector4>& colorArray = m_ColorArrays[static_cast<int>(node->GetUniqueID())];

  glPushMatrix();
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glColor3f(1, 1, 1);
  for (int i=0; i<numPolygons; i++) {
    const int vertexCount = mesh->GetPolygonSize(i);
    //std::cout << vertexCount << " ";
    glBegin(GL_TRIANGLE_FAN);
    for (int j=0; j<vertexCount; j++) {
      int idx = mesh->GetPolygonVertex(i, j);
      const FbxVector4& color = colorArray[idx];
      glColor4d(color[0], color[1], color[2], color[3]);
      glNormal3dv((GLdouble*)normalArray[idx]);
      glVertex3dv((GLdouble*)vertexArray[idx]);
    }
    glEnd();
  }
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glPopMatrix();
#else
  m_vertexBuffer.bind();
  glEnableVertexAttribArray(g_vertexAttribIdx);
  glVertexAttribPointer(g_vertexAttribIdx, 3, GL_FLOAT, GL_TRUE, 0, 0);

  m_normalBuffer.bind();
  glEnableVertexAttribArray(g_normalAttribIdx);
  glVertexAttribPointer(g_normalAttribIdx, 3, GL_FLOAT, GL_TRUE, 0, 0);

  m_colorBuffer.bind();
  glEnableVertexAttribArray(g_colorAttribIdx);
  glVertexAttribPointer(g_colorAttribIdx, 4, GL_FLOAT, GL_TRUE, 0, 0);

  m_indexBuffer.bind();
  glDrawElements(GL_TRIANGLES, numPolygons*3, GL_UNSIGNED_INT, 0);
  m_indexBuffer.release();
  glDisableVertexAttribArray(g_vertexAttribIdx);
  glDisableVertexAttribArray(g_normalAttribIdx);
  glDisableVertexAttribArray(g_colorAttribIdx);

  m_colorBuffer.release();
  m_normalBuffer.release();
  m_vertexBuffer.release();
#endif
}

void MeshHand::computeShapeDeformation(FbxMesh* mesh, FbxVector4* vertexArray) {
  int lVertexCount = mesh->GetControlPointsCount();

  FbxTime time;
  time.SetSecondDouble(m_time);

  FbxVector4* lSrcVertexArray = vertexArray;
  FbxVector4* lDstVertexArray = new FbxVector4[lVertexCount];
  memcpy(lDstVertexArray, vertexArray, lVertexCount * sizeof(FbxVector4));

  int lBlendShapeDeformerCount = mesh->GetDeformerCount(FbxDeformer::eBlendShape);
  for (int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
  {
    FbxBlendShape* lBlendShape = (FbxBlendShape*)mesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);

    int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
    for (int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
    {
      FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
      if (lChannel)
      {
        // Get the percentage of influence on this channel.
        FbxAnimCurve* lFCurve = mesh->GetShapeChannel(lBlendShapeIndex, lChannelIndex, 0);
        if (!lFCurve) continue;
        double lWeight = lFCurve->Evaluate(time);

        int lShapeCount = lChannel->GetTargetShapeCount();
        double* lFullWeights = lChannel->GetTargetShapeFullWeights();

        // Find out which scope the lWeight falls in.
        int lStartIndex = -1;
        int lEndIndex = -1;
        for (int lShapeIndex = 0; lShapeIndex<lShapeCount; ++lShapeIndex)
        {
          if (lWeight > 0 && lWeight <= lFullWeights[0])
          {
            lEndIndex = 0;
            break;
          }
          if (lWeight > lFullWeights[lShapeIndex] && lWeight < lFullWeights[lShapeIndex+1])
          {
            lStartIndex = lShapeIndex;
            lEndIndex = lShapeIndex + 1;
            break;
          }
        }

        FbxShape* lStartShape = NULL;
        FbxShape* lEndShape = NULL;
        if (lStartIndex > -1)
        {
          lStartShape = lChannel->GetTargetShape(lStartIndex);
        }
        if (lEndIndex > -1)
        {
          lEndShape = lChannel->GetTargetShape(lEndIndex);
        }

        //The weight percentage falls between base geometry and the first target shape.
        if (lStartIndex == -1 && lEndShape)
        {
          double lEndWeight = lFullWeights[0];
          // Calculate the real weight.
          lWeight = (lWeight/lEndWeight) * 100;
          // Initialize the lDstVertexArray with vertex of base geometry.
          memcpy(lDstVertexArray, lSrcVertexArray, lVertexCount * sizeof(FbxVector4));
          for (int j = 0; j < lVertexCount; j++)
          {
            // Add the influence of the shape vertex to the mesh vertex.
            FbxVector4 lInfluence = (lEndShape->GetControlPoints()[j] - lSrcVertexArray[j]) * lWeight * 0.01;
            lDstVertexArray[j] += lInfluence;
          }
        }
        //The weight percentage falls between two target shapes.
        else if (lStartShape && lEndShape)
        {
          double lStartWeight = lFullWeights[lStartIndex];
          double lEndWeight = lFullWeights[lEndIndex];
          // Calculate the real weight.
          lWeight = ((lWeight-lStartWeight)/(lEndWeight-lStartWeight)) * 100;
          // Initialize the lDstVertexArray with vertex of the previous target shape geometry.
          memcpy(lDstVertexArray, lStartShape->GetControlPoints(), lVertexCount * sizeof(FbxVector4));
          for (int j = 0; j < lVertexCount; j++)
          {
            // Add the influence of the shape vertex to the previous shape vertex.
            FbxVector4 lInfluence = (lEndShape->GetControlPoints()[j] - lStartShape->GetControlPoints()[j]) * lWeight * 0.01;
            lDstVertexArray[j] += lInfluence;
          }
        }
      }//If lChannel is valid
    }//For each blend shape channel
  }//For each blend shape deformer

  memcpy(vertexArray, lDstVertexArray, lVertexCount * sizeof(FbxVector4));

  delete[] lDstVertexArray;
}

void MeshHand::computeSkinDeformation(FbxAMatrix& globalPosition, FbxMesh* mesh, FbxVector4* vertexArray, FbxVector4* colorArray) {
  FbxSkin * lSkinDeformer = (FbxSkin *)mesh->GetDeformer(0, FbxDeformer::eSkin);
  FbxSkin::EType lSkinningType = lSkinDeformer->GetSkinningType();

  if (lSkinningType == FbxSkin::eLinear || lSkinningType == FbxSkin::eRigid) {
    computeLinearDeformation(globalPosition, mesh, vertexArray, colorArray);
  } else if (lSkinningType == FbxSkin::eDualQuaternion) {
    computeDualQuaternionDeformation(globalPosition, mesh, vertexArray);
  } else if (lSkinningType == FbxSkin::eBlend) {
    int lVertexCount = mesh->GetControlPointsCount();

    FbxVector4* lVertexArrayLinear = new FbxVector4[lVertexCount];
    memcpy(lVertexArrayLinear, mesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

    FbxVector4* lVertexArrayDQ = new FbxVector4[lVertexCount];
    memcpy(lVertexArrayDQ, mesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));

    computeLinearDeformation(globalPosition, mesh, lVertexArrayLinear, colorArray);
    computeDualQuaternionDeformation(globalPosition, mesh, lVertexArrayDQ);

    // To blend the skinning according to the blend weights
    // Final vertex = DQSVertex * blend weight + LinearVertex * (1- blend weight)
    // DQSVertex: vertex that is deformed by dual quaternion skinning method;
    // LinearVertex: vertex that is deformed by classic linear skinning method;
    int lBlendWeightsCount = lSkinDeformer->GetControlPointIndicesCount();
    for (int lBWIndex = 0; lBWIndex<lBlendWeightsCount; ++lBWIndex) {
      double lBlendWeight = lSkinDeformer->GetControlPointBlendWeights()[lBWIndex];
      vertexArray[lBWIndex] = lVertexArrayDQ[lBWIndex] * lBlendWeight + lVertexArrayLinear[lBWIndex] * (1 - lBlendWeight);
    }
  }
}

void MeshHand::computeLinearDeformation(FbxAMatrix& globalPosition, FbxMesh* mesh, FbxVector4* vertexArray, FbxVector4* colorArray) {
  // All the links must have the same link mode.
  FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

  int lVertexCount = mesh->GetControlPointsCount();
  if (!m_clusterDeformation) {
    m_clusterDeformation = new FbxAMatrix[lVertexCount];
    m_clusterWeight = new double[lVertexCount];
  }
  memset(m_clusterDeformation, 0, lVertexCount * sizeof(FbxAMatrix));
  memset(m_clusterWeight, 0, lVertexCount * sizeof(double));

  if (lClusterMode == FbxCluster::eAdditive)
  {
    for (int i = 0; i < lVertexCount; ++i)
    {
      m_clusterDeformation[i].SetIdentity();
    }
  }

  // For all skins and all clusters, accumulate their deformation and weight
  // on each vertices and store them in lClusterDeformation and lClusterWeight.
  int lSkinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
  for (int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
  {
    FbxSkin * lSkinDeformer = (FbxSkin *)mesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);

    int lClusterCount = lSkinDeformer->GetClusterCount();
    for (int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
    {
      FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
      if (!lCluster->GetLink())
        continue;

      FbxAMatrix lVertexTransformMatrix;
      computeClusterDeformation(globalPosition, mesh, lCluster, lVertexTransformMatrix);

      int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
      for (int k = 0; k < lVertexIndexCount; ++k)
      {
        int lIndex = lCluster->GetControlPointIndices()[k];

        // Sometimes, the mesh can have less points than at the time of the skinning
        // because a smooth operator was active when skinning but has been deactivated during export.
        if (lIndex >= lVertexCount)
          continue;

        double lWeight = lCluster->GetControlPointWeights()[k];

        if (lWeight == 0.0)
        {
          continue;
        }

        // Compute the influence of the link on the vertex.
        FbxAMatrix lInfluence = lVertexTransformMatrix;
        matrixScale(lInfluence, lWeight);

        FbxVector4 color = getColorForNode(lCluster->GetLink()->GetName());
        color[0] *= lWeight;
        color[1] *= lWeight;
        color[2] *= lWeight;
        color[3] *= lWeight;

        if (lClusterMode == FbxCluster::eAdditive)
        {
          // Multiply with the product of the deformations on the vertex.
          matrixAddToDiagonal(lInfluence, 1.0 - lWeight);
          m_clusterDeformation[lIndex] = lInfluence * m_clusterWeight[lIndex];

          // Set the link to 1.0 just to know this vertex is influenced by a link.
          m_clusterWeight[lIndex] = 1.0;
        } else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
        {
          // Add to the sum of the deformations on the vertex.
          matrixAdd(m_clusterDeformation[lIndex], lInfluence);

          // Add to the sum of weights to either normalize or complete the vertex.
          m_clusterWeight[lIndex] += lWeight;
        }
        colorArray[lIndex] += color;
      }//For each vertex			
    }//lClusterCount
  }

  //Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
  for (int i = 0; i < lVertexCount; i++)
  {
    FbxVector4 lSrcVertex = vertexArray[i];
    FbxVector4& lDstVertex = vertexArray[i];
    double lWeight = m_clusterWeight[i];

    // Deform the vertex if there was at least a link with an influence on the vertex,
    if (lWeight != 0.0)
    {
      lDstVertex = m_clusterDeformation[i].MultT(lSrcVertex);
      if (lClusterMode == FbxCluster::eNormalize)
      {
        //***
        // In the normalized link mode, a vertex is always totally influenced by the links. 
        lDstVertex /= lWeight;
        colorArray[i] /= lWeight;
      } else if (lClusterMode == FbxCluster::eTotalOne)
      {
        // In the total 1 link mode, a vertex can be partially influenced by the links. 
        lSrcVertex *= (1.0 - lWeight);
        lDstVertex += lSrcVertex;
      }
    }
  }
}

void MeshHand::computeDualQuaternionDeformation(FbxAMatrix& globalPosition, FbxMesh* mesh, FbxVector4* vertexArray) {
  // All the links must have the same link mode.
  FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

  int lVertexCount = mesh->GetControlPointsCount();
  int lSkinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);

  FbxDualQuaternion* lDQClusterDeformation = new FbxDualQuaternion[lVertexCount];
  memset(lDQClusterDeformation, 0, lVertexCount * sizeof(FbxDualQuaternion));

  double* lClusterWeight = new double[lVertexCount];
  memset(lClusterWeight, 0, lVertexCount * sizeof(double));

  // For all skins and all clusters, accumulate their deformation and weight
  // on each vertices and store them in lClusterDeformation and lClusterWeight.
  for (int lSkinIndex=0; lSkinIndex<lSkinCount; ++lSkinIndex)
  {
    FbxSkin * lSkinDeformer = (FbxSkin *)mesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin);
    int lClusterCount = lSkinDeformer->GetClusterCount();
    for (int lClusterIndex=0; lClusterIndex<lClusterCount; ++lClusterIndex)
    {
      FbxCluster* lCluster = lSkinDeformer->GetCluster(lClusterIndex);
      if (!lCluster->GetLink())
        continue;

      FbxAMatrix lVertexTransformMatrix;
      computeClusterDeformation(globalPosition, mesh, lCluster, lVertexTransformMatrix);

      FbxQuaternion lQ = lVertexTransformMatrix.GetQ();
      FbxVector4 lT = lVertexTransformMatrix.GetT();
      FbxDualQuaternion lDualQuaternion(lQ, lT);

      int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
      for (int k = 0; k < lVertexIndexCount; ++k)
      {
        int lIndex = lCluster->GetControlPointIndices()[k];

        // Sometimes, the mesh can have less points than at the time of the skinning
        // because a smooth operator was active when skinning but has been deactivated during export.
        if (lIndex >= lVertexCount)
          continue;

        double lWeight = lCluster->GetControlPointWeights()[k];

        if (lWeight == 0.0)
          continue;

        // Compute the influence of the link on the vertex.
        FbxDualQuaternion lInfluence = lDualQuaternion * lWeight;
        if (lClusterMode == FbxCluster::eAdditive)
        {
          // Simply influenced by the dual quaternion.
          lDQClusterDeformation[lIndex] = lInfluence;

          // Set the link to 1.0 just to know this vertex is influenced by a link.
          lClusterWeight[lIndex] = 1.0;
        } else // lLinkMode == FbxCluster::eNormalize || lLinkMode == FbxCluster::eTotalOne
        {
          if (lClusterIndex == 0)
          {
            lDQClusterDeformation[lIndex] = lInfluence;
          } else
          {
            // Add to the sum of the deformations on the vertex.
            // Make sure the deformation is accumulated in the same rotation direction. 
            // Use dot product to judge the sign.
            double lSign = lDQClusterDeformation[lIndex].GetFirstQuaternion().DotProduct(lDualQuaternion.GetFirstQuaternion());
            if (lSign >= 0.0)
            {
              lDQClusterDeformation[lIndex] += lInfluence;
            } else
            {
              lDQClusterDeformation[lIndex] -= lInfluence;
            }
          }
          // Add to the sum of weights to either normalize or complete the vertex.
          lClusterWeight[lIndex] += lWeight;
        }
      }//For each vertex
    }//lClusterCount
  }

  //Actually deform each vertices here by information stored in lClusterDeformation and lClusterWeight
  for (int i = 0; i < lVertexCount; i++)
  {
    FbxVector4 lSrcVertex = vertexArray[i];
    FbxVector4& lDstVertex = vertexArray[i];
    double lWeightSum = lClusterWeight[i];

    // Deform the vertex if there was at least a link with an influence on the vertex,
    if (lWeightSum != 0.0)
    {
      lDQClusterDeformation[i].Normalize();
      lDstVertex = lDQClusterDeformation[i].Deform(lDstVertex);

      if (lClusterMode == FbxCluster::eNormalize)
      {
        // In the normalized link mode, a vertex is always totally influenced by the links. 
        lDstVertex /= lWeightSum;
      } else if (lClusterMode == FbxCluster::eTotalOne)
      {
        // In the total 1 link mode, a vertex can be partially influenced by the links. 
        lSrcVertex *= (1.0 - lWeightSum);
        lDstVertex += lSrcVertex;
      }
    }
  }

  delete[] lDQClusterDeformation;
  delete[] lClusterWeight;
}

void MeshHand::computeClusterDeformation(FbxAMatrix& globalPosition, FbxMesh* mesh, FbxCluster* cluster, FbxAMatrix& vertexTransformMatrix) {
  FbxCluster::ELinkMode lClusterMode = cluster->GetLinkMode();

  FbxAMatrix lReferenceGlobalInitPosition;
  FbxAMatrix lReferenceGlobalCurrentPosition;
  FbxAMatrix lAssociateGlobalInitPosition;
  FbxAMatrix lAssociateGlobalCurrentPosition;
  FbxAMatrix lClusterGlobalInitPosition;
  FbxAMatrix lClusterGlobalCurrentPosition;

  FbxAMatrix lReferenceGeometry;
  FbxAMatrix lAssociateGeometry;
  FbxAMatrix lClusterGeometry;

  FbxAMatrix lClusterRelativeInitPosition;
  FbxAMatrix lClusterRelativeCurrentPositionInverse;

  if (lClusterMode == FbxCluster::eAdditive && cluster->GetAssociateModel()) {
    cluster->GetTransformAssociateModelMatrix(lAssociateGlobalInitPosition);
    // Geometric transform of the model
    lAssociateGeometry = getGeometry(cluster->GetAssociateModel());
    lAssociateGlobalInitPosition *= lAssociateGeometry;
    lAssociateGlobalCurrentPosition = getGlobalPosition(cluster->GetAssociateModel());

    cluster->GetTransformMatrix(lReferenceGlobalInitPosition);
    // Multiply lReferenceGlobalInitPosition by Geometric Transformation
    lReferenceGeometry = getGeometry(mesh->GetNode());
    lReferenceGlobalInitPosition *= lReferenceGeometry;
    lReferenceGlobalCurrentPosition = globalPosition;

    // Get the link initial global position and the link current global position.
    cluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
    // Multiply lClusterGlobalInitPosition by Geometric Transformation
    lClusterGeometry = getGeometry(cluster->GetLink());
    lClusterGlobalInitPosition *= lClusterGeometry;
    lClusterGlobalCurrentPosition = getGlobalPosition(cluster->GetLink());

    // Compute the shift of the link relative to the reference.
    //ModelM-1 * AssoM * AssoGX-1 * LinkGX * LinkM-1*ModelM
    vertexTransformMatrix = lReferenceGlobalInitPosition.Inverse() * lAssociateGlobalInitPosition * lAssociateGlobalCurrentPosition.Inverse() *
      lClusterGlobalCurrentPosition * lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
  } else {
    cluster->GetTransformMatrix(lReferenceGlobalInitPosition);
    lReferenceGlobalCurrentPosition = globalPosition;
    // Multiply lReferenceGlobalInitPosition by Geometric Transformation
    lReferenceGeometry = getGeometry(mesh->GetNode());
    lReferenceGlobalInitPosition *= lReferenceGeometry;

    // Get the link initial global position and the link current global position.
    cluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
    lClusterGlobalCurrentPosition = getGlobalPosition(cluster->GetLink());

    // Compute the initial position of the link relative to the reference.
    lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

    // Compute the current position of the link relative to the reference.
    lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

    // Compute the shift of the link relative to the reference.
    vertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;
  }
}

void MeshHand::matrixScale(FbxAMatrix& matrix, double value) {
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
      matrix[i][j] *= value;
    }
  }
}

void MeshHand::matrixAddToDiagonal(FbxAMatrix& matrix, double value) {
  matrix[0][0] += value;
  matrix[1][1] += value;
  matrix[2][2] += value;
  matrix[3][3] += value;
}

void MeshHand::matrixAdd(FbxAMatrix& dstMatrix, FbxAMatrix& srcMatrix) {
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
      dstMatrix[i][j] += srcMatrix[i][j];
    }
  }
}

FbxVector4 MeshHand::getColorForNode(const std::string& name) const {
#if 0
  // rave mode

  int val = g_colorCounter/10;
  for (int i=0; i<5; i++) {
    if (name.compare(0, m_fingerStrings[i].length(), m_fingerStrings[i]) == 0) {
      const Vector3& color = colorForIndex(i+val);
      return FbxVector4(color.x(), color.y(), color.z(), 1.0);
    }
  }
  return FbxVector4(0.0, 0.0, 0.0, 0.0);
#else
  for (int i=0; i<5; i++) {
    if (name.compare(0, m_fingerStrings[i].length(), m_fingerStrings[i]) == 0) {
      return FbxVector4(0.1, 0.2, 0.3, 1.0);
    }
  }

  return FbxVector4(0.0, 0.0, 0.0, 0.0);
#endif
}

FbxAMatrix MeshHand::getGlobalPosition(FbxNode* node, FbxAMatrix* parentGlobalPosition) {
  FbxAMatrix globalPosition;
  FbxTime time;
  time.SetSecondDouble(m_time);

  globalPosition = node->EvaluateGlobalTransform(time, FbxNode::eSourcePivot, false, true);

  return globalPosition;
}

FbxAMatrix MeshHand::getGeometry(FbxNode* node) {
  const FbxVector4 T = node->GetGeometricTranslation(FbxNode::eSourcePivot);
  const FbxVector4 R = node->GetGeometricRotation(FbxNode::eSourcePivot);
  const FbxVector4 S = node->GetGeometricScaling(FbxNode::eSourcePivot);
  return FbxAMatrix(T, R, S);
}

void MeshHand::initBuffers(FbxMesh* mesh) {
  const int numVertices = mesh->GetControlPointsCount();
  const int numTriangles = mesh->GetPolygonCount();

  const int vertexBytes = numVertices*3*sizeof(GLfloat);
  const int colorBytes = numVertices*4*sizeof(GLfloat);
  const int indexBytes = numTriangles*3*sizeof(GLuint);

  m_vertexBuffer.create();
  m_vertexBuffer.bind();
  m_vertexBuffer.allocate(0, vertexBytes, GL_DYNAMIC_DRAW);
  m_vertexBuffer.release();

  m_normalBuffer.create();
  m_normalBuffer.bind();
  m_normalBuffer.allocate(0, vertexBytes, GL_DYNAMIC_DRAW);
  m_normalBuffer.release();

  m_colorBuffer.create();
  m_colorBuffer.bind();
  m_colorBuffer.allocate(0, colorBytes, GL_DYNAMIC_DRAW);
  m_colorBuffer.release();

  m_indexBuffer.create();
  m_indexBuffer.bind();
  m_indexBuffer.allocate(0, indexBytes, GL_DYNAMIC_DRAW);
  m_indexBuffer.release();
}
