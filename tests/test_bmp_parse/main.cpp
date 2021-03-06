#include "test_util/assert.h"
#include "image/bmp/io.h"
#include "image/core/image.h"

using namespace image::bmp;

void verifyStructSizeCorrectness();
void testParsingOfSimplyImage();

int main()
{
    verifyStructSizeCorrectness();
    testParsingOfSimplyImage();
}

void verifyStructSizeCorrectness()
{
    ASSERT_EQ(sizeof(BmpFileHeader), 14);
    ASSERT_EQ(sizeof(DIBHeaderV0), (size_t)DIBVersion::BITMAPCOREHEADER);
    STATIC_ASSERT(sizeof(BmpFileHeader) == 14);
    STATIC_ASSERT(sizeof(DIBHeaderV0) == (size_t)DIBVersion::BITMAPCOREHEADER);
    STATIC_ASSERT(sizeof(DIBHeaderV1) == (size_t)DIBVersion::BITMAPINFOHEADER);
    STATIC_ASSERT(sizeof(DIBHeaderV2) == (size_t)DIBVersion::BITMAPV2INFOHEADER);
    STATIC_ASSERT(sizeof(DIBHeaderV3) == (size_t)DIBVersion::BITMAPV3INFOHEADER);
    STATIC_ASSERT(sizeof(DIBHeaderV4) == (size_t)DIBVersion::BITMAPV4INFOHEADER);
    STATIC_ASSERT(sizeof(DIBHeaderV5) == (size_t)DIBVersion::BITMAPV5INFOHEADER);
}

static const uint8_t bmp1[] =
{
    // BMP Header
    0x42, 0x4D,                 //  "BM"                ID field (42h, 4Dh)
    0x46, 0x00, 0x00, 0x00,     //  70 bytes (54+16)    Size of the BMP file
    0x00, 0x00,                 //  Unused              Application specific
    0x00, 0x00,                 //  Unused              Application specific
    0x36, 0x00, 0x00, 0x00,     //  54 bytes (14+40)    Offset where the pixel array (bitmap data) can be found

    // DIB Header
    0x28, 0x00, 0x00, 0x00,     //  40 bytes            Number of bytes in the DIB header (from this point)
    0x02, 0x00, 0x00, 0x00,     //  2 pixels            Width of the bitmap in pixels
    0x02, 0x00, 0x00, 0x00,     //  2 pixels            Height of the bitmap in pixels. Positive for bottom to top pixel order.
    0x01, 0x00,                 //  1 plane             Number of color planes being used
    0x18, 0x00,                 //  24 bits             Number of bits per pixel
    0x00, 0x00, 0x00, 0x00,     //  0                   BI_RGB, no pixel array compression used
    0x10, 0x00, 0x00, 0x00,     //  16 bytes            Size of the raw bitmap data (including padding)
    0x13, 0x0B, 0x00, 0x00,     //  2835 pixels/meter   Horizontal print resolution of the image, 72 DPI
    0x13, 0x0B, 0x00, 0x00,     //  2835 pixels/meter   Vertical print resolution of the image, 72 DPI
    0x00, 0x00, 0x00, 0x00,     //  0 colors            Number of colors in the palette
    0x00, 0x00, 0x00, 0x00,     //  0 important colors  0 means all colors are important

    // Start of pixel array (bitmap data)
    0x00, 0x00, 0xFF,           // 0 0 255              Red, Pixel (0,1)
    0xFF, 0xFF, 0xFF,           // 255 255 255          White, Pixel (1,1)
    0x00, 0x00,                 // 0 0                  Padding for 4 byte alignment (could be a value other than zero)
    0xFF, 0x00, 0x00,           // 255 0 0              Blue, Pixel (0,0)
    0x00, 0xFF, 0x00,           // 0 255 0              Green, Pixel (1,0)
    0x00, 0x00,                 // 0 0                  Padding for 4 byte alignment (could be a value other than zero)
};

struct Rgb
{
    uint8_t b, g, r;

    Rgb(uint32_t x) : b(x & 0xFF), g((x >> 8) & 0xFF), r((x >> 16) & 0xFF)
    { }

    friend bool operator==(const Rgb& a, const Rgb& b)
    {
        return a.r == b.r && a.g == b.g && a.b == b.b;
    }

    friend bool operator!=(const Rgb& a, const Rgb& b)
    {
        return !(a == b);
    }
};

void testParsingOfSimplyImage()
{
    using namespace image::core;

    Slice<const uint8_t> data = slice(bmp1);

    ASSERT_EQ(data[0], 0x42);

    BmpV5File header;
    Slice<const uint8_t> pixelData = loadBitmapImage(data, header);

    ASSERT_EQ(pixelData.bytes(), 16);
    ASSERT_EQ(pixelData[0], 0);

    ASSERT_EQ(header.signature, 0x4D42);
    ASSERT_EQ(header.fileSize, 70);
    ASSERT_EQ(header._reserved1, 0);
    ASSERT_EQ(header._reserved2, 0);
    ASSERT_EQ(header.offsetToPixelArray, 54);

    ASSERT_EQ(header.version, DIBVersion::BITMAPINFOHEADER);
    ASSERT_EQ(header.width, 2);
    ASSERT_EQ(header.height, 2);
    ASSERT_EQ(header.colorPlanesCount, 1);
    ASSERT_EQ(header.bpp, 24);
    ASSERT_EQ(header.compression, Compression::BI_RGB);
    ASSERT_EQ(header.sizeOfPixelArray, 16);
    ASSERT_EQ(header.pixelsPerMeterX, 2835);
    ASSERT_EQ(header.pixelsPerMeterY, 2835);
    ASSERT_EQ(header.colorsUsed, 0);
    ASSERT_EQ(header.colorsImportant, 0);

    Rgb red(0x00FF0000);
    ASSERT_EQ(red.r, 0xFF);
    ASSERT_EQ(red.g, 0);
    ASSERT_EQ(red.b, 0);

    Rgb green(0x0000FF00);
    ASSERT_EQ(green.r, 0);
    ASSERT_EQ(green.g, 0xFF);
    ASSERT_EQ(green.b, 0);

    Rgb blue(0x000000FF);
    ASSERT_EQ(blue.r, 0);
    ASSERT_EQ(blue.g, 0);
    ASSERT_EQ(blue.b, 0xFF);

    Rgb white(0x00FFFFFF);
    ASSERT_EQ(white.r, 0xFF);
    ASSERT_EQ(white.g, 0xFF);
    ASSERT_EQ(white.b, 0xFF);

    Image<const Rgb> pixels(pixelData, header.width, header.height, -8);
    ASSERT_EQ(pixels(0, 0), blue);
    ASSERT_EQ(pixels(1, 0), green);
    ASSERT_EQ(pixels(0, 1), red);
    ASSERT_EQ(pixels(1, 1), white);
}
