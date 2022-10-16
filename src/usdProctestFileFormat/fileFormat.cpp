#include "fileFormat.h"

#include <pxr/pxr.h>

#include <pxr/base/tf/diagnostic.h>
#include <pxr/base/tf/stringUtils.h>
#include <pxr/usd/pcp/dynamicFileFormatContext.h>
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

static const float defaultSideLengthValue = 1.0f;

TF_DEFINE_PUBLIC_TOKENS(UsdProctestFileFormatTokens, USD_PROCTEST_FILE_FORMAT_TOKENS);

TF_REGISTRY_FUNCTION(TfType) {
  SDF_DEFINE_FILE_FORMAT(UsdProctestFileFormat, SdfFileFormat);
}

enum ProctestCodes { PROCTEST_CANNOT_READ_PROCTEST_FILE, PROCTEST_CANNOT_CREATE_ATTRIBUTE };
TF_REGISTRY_FUNCTION(TfEnum) {
    TF_ADD_ENUM_NAME(PROCTEST_CANNOT_READ_PROCTEST_FILE, "Cannot read Proctest file.");
    TF_ADD_ENUM_NAME(PROCTEST_CANNOT_CREATE_ATTRIBUTE, "Cannot create attribute.");
};

TF_DEBUG_CODES(
  PROCTEST_INFO
);

static float
_ExtractSideLengthFromContext(const PcpDynamicFileFormatContext& context)
{
    // Default sideLength.
    auto sideLength = defaultSideLengthValue;

    VtValue value;
    if (!context.ComposeValue(UsdProctestFileFormatTokens->SideLength,
                              &value) ||
        value.IsEmpty()) {
        return sideLength;
    }

    if (!value.IsHolding<float>()) {
        TF_CODING_ERROR("Expected '%s' value to hold a float, got '%s'",
                        UsdProctestFileFormatTokens->SideLength.GetText(),
                        TfStringify(value).c_str());
        return sideLength;
    }

    return value.UncheckedGet<float>();
}

static auto
_ExtractSideLengthFromArgs(const SdfFileFormat::FileFormatArguments& args)
{
    // Default sideLength.
    float sideLength = defaultSideLengthValue;

    // Find "sideLength" file format argument.
    auto it = args.find(UsdProctestFileFormatTokens->SideLength);
    if (it == args.end()) {
        return sideLength;
    }

    // Try to convert the string value to the actual output value type.
    float extractVal;
    bool success = true;
    extractVal = TfUnstringify<float>(it->second, &success);
    if (!success) {
        TF_CODING_ERROR(
            "Could not convert arg string '%s' to value of type float",
            UsdProctestFileFormatTokens->SideLength.GetText());
        return sideLength;
    }

    sideLength = extractVal;
    return sideLength;
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

  // Extract file format arguments.

  FileFormatArguments args;
  std::string layerPath;
  SdfLayer::SplitIdentifier(layer->GetIdentifier(), &layerPath, &args);
  const auto sideLength = _ExtractSideLengthFromArgs(args);

  // TF_ERROR(PROCTEST_CANNOT_CREATE_ATTRIBUTE, "HEY !");
  TF_WARN("SideLength: %f", sideLength);

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

void UsdProctestFileFormat::ComposeFieldsForFileFormatArguments(
  const std::string& assetPath,
  const PcpDynamicFileFormatContext& context,
  FileFormatArguments* args,
  VtValue* contextDependencyData) const
{
    auto sideLength = _ExtractSideLengthFromContext(context);
    (*args)[UsdProctestFileFormatTokens->SideLength] = TfStringify(sideLength);
}

bool UsdProctestFileFormat::CanFieldChangeAffectFileFormatArguments(
  const TfToken& field,
  const VtValue& oldValue,
  const VtValue& newValue,
  const VtValue& contextDependencyData) const
{
    // Check if the "sideLength" argument changed.
    // double oldLength = oldValue.IsHolding<double>()
    //                        ? oldValue.UncheckedGet<double>()
    //                        : defaultSideLengthValue;
    // double newLength = newValue.IsHolding<double>()
    //                        ? newValue.UncheckedGet<double>()
    //                        : defaultSideLengthValue;

    // return oldLength != newLength;
    return true;
}

PXR_NAMESPACE_CLOSE_SCOPE
