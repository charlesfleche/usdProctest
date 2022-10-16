//
// Copyright © 2021 Weta Digital Limited
//
// SPDX-License-Identifier: Apache-2.0
//
#pragma once

#include <pxr/base/tf/staticTokens.h>
#include <pxr/pxr.h>
#include <pxr/usd/pcp/dynamicFileFormatInterface.h>
#include <pxr/usd/sdf/fileFormat.h>

PXR_NAMESPACE_OPEN_SCOPE

/* clang-format off */
#define USD_PROCTEST_FILE_FORMAT_TOKENS             \
    ((Id, "usdProctestFileFormat"))                 \
    ((Version, "1.0"))                              \
    ((Target, "usd"))                               \
    ((Extension, "proctest"))                       \
    ((SideLength, "Usd_Proctest_SideLength"))
/* clang-format on */

TF_DECLARE_PUBLIC_TOKENS(UsdProctestFileFormatTokens, USD_PROCTEST_FILE_FORMAT_TOKENS);

TF_DECLARE_WEAK_AND_REF_PTRS(UsdProctestFileFormat);

class UsdProctestFileFormat
  : public SdfFileFormat
  , public PcpDynamicFileFormatInterface {
public:
  bool CanRead(const std::string &filePath) const override;
  bool Read(SdfLayer *layer, const std::string &resolvedPath,
            bool metadataOnly) const override;

  bool WriteToString(const SdfLayer &layer, std::string *str,
                     const std::string &comment = std::string()) const override;
  bool WriteToStream(const SdfSpecHandle &spec, std::ostream &out,
                     size_t indent) const override;

  void ComposeFieldsForFileFormatArguments(const std::string& assetPath,
                                           const PcpDynamicFileFormatContext& context,
                                           FileFormatArguments* args,
                                           VtValue* contextDependencyData) const override;
  bool CanFieldChangeAffectFileFormatArguments(const TfToken& field,
                                               const VtValue& oldValue,
                                               const VtValue& newValue,
                                               const VtValue& contextDependencyData) const override;

protected:
  SDF_FILE_FORMAT_FACTORY_ACCESS;

  virtual ~UsdProctestFileFormat();
  UsdProctestFileFormat();
};

PXR_NAMESPACE_CLOSE_SCOPE
