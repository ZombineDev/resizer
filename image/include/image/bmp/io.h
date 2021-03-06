#ifndef IMAGE_BMP_IO_H
#define IMAGE_BMP_IO_H

#include "image/core/slice.h"
#include "image/bmp/defs.h"

namespace image
{
namespace bmp
{
    #pragma pack(1)
    struct BmpGenericHeader : BmpFileHeader
    {
        DIBVersion version;
    };

    #pragma pack(1)
    struct BmpV0File : BmpFileHeader, DIBHeaderV0 { };

    #pragma pack(1)
    struct BmpV1File : BmpFileHeader, DIBHeaderV1 { };

    #pragma pack(1)
    struct BmpV2File : BmpFileHeader, DIBHeaderV2 { };

    #pragma pack(1)
    struct BmpV3File : BmpFileHeader, DIBHeaderV3 { };

    #pragma pack(1)
    struct BmpV4File : BmpFileHeader, DIBHeaderV4 { };

    #pragma pack(1)
    struct BmpV5File : BmpFileHeader, DIBHeaderV5 { };

    core::Slice<const uint8_t> loadBitmapImage(
        core::Slice<const uint8_t> bytes, BmpV5File& resultInfo);


} /* namespace bmp */
} /* namespace image */

#endif
