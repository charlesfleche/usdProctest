#include "fileFormat.h"

#include <pxr/pxr.h>

#include <pxr/base/tf/diagnostic.h>
#include <pxr/base/tf/stringUtils.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/usdaFileFormat.h>
#include <pxr/usd/usdGeom/mesh.h>

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

#include <iostream>

using namespace std;

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(UsdProctestFileFormatTokens, USD_PROCTEST_FILE_FORMAT_TOKENS);

TF_REGISTRY_FUNCTION(TfType) {
  SDF_DEFINE_FILE_FORMAT(UsdProctestFileFormat, SdfFileFormat);
}

enum ProctestCodes { PROCTEST_CANNOT_READ_PROCTEST_FILE, PROCTEST_CANNOT_CREATE_ATTRIBUTE };
TF_REGISTRY_FUNCTION(TfEnum) {
    TF_ADD_ENUM_NAME(PROCTEST_CANNOT_READ_PROCTEST_FILE, "Cannot read Proctest file.");
    TF_ADD_ENUM_NAME(PROCTEST_CANNOT_CREATE_ATTRIBUTE, "Cannot create attribute.");
};

auto _Convert(const vector<float> &vecs) {
  VtVec3fArray ret(vecs.size() / 3);
  auto pcoords = vecs.data();
  for (auto &v : ret) {
    v.Set(pcoords[0], pcoords[1], pcoords[2]);
    pcoords += 3;
  }
  return ret;
}

UsdProctestFileFormat::UsdProctestFileFormat()
    : SdfFileFormat(UsdProctestFileFormatTokens->Id, UsdProctestFileFormatTokens->Version,
                    UsdProctestFileFormatTokens->Target,
                    UsdProctestFileFormatTokens->Extension) {}

UsdProctestFileFormat::~UsdProctestFileFormat() {}

bool UsdProctestFileFormat::CanRead(const std::string &) const { return true; }

bool UsdProctestFileFormat::Read(SdfLayer *layer, const std::string &resolvedPath,
                            bool metadataOnly) const {
  if (!TF_VERIFY(layer)) {
    return false;
  }

  SdfLayerRefPtr newLayer = SdfLayer::CreateAnonymous(".usd");
  UsdStageRefPtr stage = UsdStage::Open(newLayer);

  // Define the loaded prim

  UsdGeomMesh mesh = UsdGeomMesh::Define(stage, SdfPath("/Root"));
  stage->SetDefaultPrim(mesh.GetPrim());

  // faceVertexCounts

  VtIntArray faceVertexCounts(6, 4);
  if (!mesh.CreateFaceVertexCountsAttr(VtValue(faceVertexCounts))) {
    TF_ERROR(PROCTEST_CANNOT_CREATE_ATTRIBUTE, "faceVertexCounts");
  }

  // faceVertexIndices

  VtIntArray faceVertexIndices = {0, 4, 6, 2, 3, 2, 6, 7, 7, 6, 4, 5, 5, 1, 3, 7, 1, 0, 2, 3, 5, 4, 0, 1};
  if (!mesh.CreateFaceVertexIndicesAttr(VtValue(faceVertexIndices))) {
    TF_ERROR(PROCTEST_CANNOT_CREATE_ATTRIBUTE, "faceVertexIndices");
  }

  // points

  const float sideLength = 1.0;
  const float halfLength = sideLength / 2.0f;

  VtVec3fArray points = {
    {halfLength, halfLength, halfLength},
    {halfLength, halfLength, -halfLength},
    {halfLength, -halfLength, halfLength},
    {halfLength, -halfLength, -halfLength},
    {-halfLength, halfLength, halfLength},
    {-halfLength, halfLength, -halfLength},
    {-halfLength, -halfLength, halfLength},
    {-halfLength, -halfLength, -halfLength}
  };
  if (!mesh.CreatePointsAttr(VtValue(points))) {
    TF_ERROR(PROCTEST_CANNOT_CREATE_ATTRIBUTE, "points");
  }

  // subdivisionScheme

  if (!mesh.CreateSubdivisionSchemeAttr(VtValue(UsdGeomTokens->none))) {
    TF_ERROR(PROCTEST_CANNOT_CREATE_ATTRIBUTE, "subdivisionScheme");
  }

  // transfer the layer

  layer->TransferContent(newLayer);
  return true;
}

bool UsdProctestFileFormat::WriteToString(const SdfLayer &layer, std::string *str,
                                     const std::string &comment) const {
  return SdfFileFormat::FindById(UsdUsdaFileFormatTokens->Id)
      ->WriteToString(layer, str, comment);
}

bool UsdProctestFileFormat::WriteToStream(const SdfSpecHandle &spec,
                                     std::ostream &out, size_t indent) const {
  return SdfFileFormat::FindById(UsdUsdaFileFormatTokens->Id)
      ->WriteToStream(spec, out, indent);
}

PXR_NAMESPACE_CLOSE_SCOPE
