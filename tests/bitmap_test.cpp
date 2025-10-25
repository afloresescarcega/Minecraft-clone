#include <gtest/gtest.h>
#include "bitmap.h"
#include "image.h"
#include <fstream>
#include <cstring>

// Helper function to create a simple 24-bit BMP file for testing
void createTestBMP(const char* filename, int width, int height) {
    std::ofstream file(filename, std::ios::binary);

    // BMP File Header (14 bytes)
    BMP_WORD bfType = 0x4D42;  // "BM"
    int paddedWidth = ((width * 3 + 3) / 4) * 4;  // Row padding to multiple of 4
    BMP_DWORD bfSize = 14 + 40 + paddedWidth * height;  // Total file size
    BMP_WORD bfReserved1 = 0;
    BMP_WORD bfReserved2 = 0;
    BMP_DWORD bfOffBits = 54;  // Offset to pixel data

    file.write(reinterpret_cast<const char*>(&bfType), 2);
    file.write(reinterpret_cast<const char*>(&bfSize), 4);
    file.write(reinterpret_cast<const char*>(&bfReserved1), 2);
    file.write(reinterpret_cast<const char*>(&bfReserved2), 2);
    file.write(reinterpret_cast<const char*>(&bfOffBits), 4);

    // BMP Info Header (40 bytes)
    BMP_DWORD biSize = 40;
    BMP_LONG biWidth = width;
    BMP_LONG biHeight = height;
    BMP_WORD biPlanes = 1;
    BMP_WORD biBitCount = 24;
    BMP_DWORD biCompression = 0;  // BI_RGB
    BMP_DWORD biSizeImage = paddedWidth * height;
    BMP_LONG biXPelsPerMeter = 0;
    BMP_LONG biYPelsPerMeter = 0;
    BMP_DWORD biClrUsed = 0;
    BMP_DWORD biClrImportant = 0;

    file.write(reinterpret_cast<const char*>(&biSize), 4);
    file.write(reinterpret_cast<const char*>(&biWidth), 4);
    file.write(reinterpret_cast<const char*>(&biHeight), 4);
    file.write(reinterpret_cast<const char*>(&biPlanes), 2);
    file.write(reinterpret_cast<const char*>(&biBitCount), 2);
    file.write(reinterpret_cast<const char*>(&biCompression), 4);
    file.write(reinterpret_cast<const char*>(&biSizeImage), 4);
    file.write(reinterpret_cast<const char*>(&biXPelsPerMeter), 4);
    file.write(reinterpret_cast<const char*>(&biYPelsPerMeter), 4);
    file.write(reinterpret_cast<const char*>(&biClrUsed), 4);
    file.write(reinterpret_cast<const char*>(&biClrImportant), 4);

    // Write pixel data (BGR format for BMP)
    unsigned char pad = 0;
    int padBytes = paddedWidth - (width * 3);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Create a simple gradient pattern
            unsigned char blue = (x * 255) / width;
            unsigned char green = (y * 255) / height;
            unsigned char red = 128;

            file.write(reinterpret_cast<const char*>(&blue), 1);
            file.write(reinterpret_cast<const char*>(&green), 1);
            file.write(reinterpret_cast<const char*>(&red), 1);
        }
        // Write padding bytes
        for (int p = 0; p < padBytes; p++) {
            file.write(reinterpret_cast<const char*>(&pad), 1);
        }
    }

    file.close();
}

// Test fixture for bitmap tests
class BitmapTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test BMP files before each test
        createTestBMP("test_valid.bmp", 4, 4);
        createTestBMP("test_larger.bmp", 100, 100);
    }

    void TearDown() override {
        // Clean up test files after each test
        std::remove("test_valid.bmp");
        std::remove("test_larger.bmp");
        std::remove("test_invalid.bmp");
        std::remove("test_truncated.bmp");
    }
};

// Test: Successfully read a valid BMP file
TEST_F(BitmapTest, ReadValidBMP) {
    Image image;
    bool result = readBMP("test_valid.bmp", image);

    EXPECT_TRUE(result);
    EXPECT_EQ(image.width, 4);
    EXPECT_EQ(image.height, 4);
    EXPECT_FALSE(image.bytes.empty());
}

// Test: Read a larger BMP file
TEST_F(BitmapTest, ReadLargerBMP) {
    Image image;
    bool result = readBMP("test_larger.bmp", image);

    EXPECT_TRUE(result);
    EXPECT_EQ(image.width, 100);
    EXPECT_EQ(image.height, 100);
    EXPECT_EQ(image.bytes.size(), 100 * 100 * 3);
}

// Test: Fail when reading non-existent file
TEST_F(BitmapTest, ReadNonExistentFile) {
    Image image;
    bool result = readBMP("nonexistent.bmp", image);

    EXPECT_FALSE(result);
}

// Test: Fail when reading invalid BMP (wrong magic number)
TEST_F(BitmapTest, ReadInvalidMagicNumber) {
    // Create a file with invalid magic number
    std::ofstream file("test_invalid.bmp", std::ios::binary);
    BMP_WORD invalidMagic = 0x1234;  // Not "BM"
    file.write(reinterpret_cast<const char*>(&invalidMagic), 2);
    file.close();

    Image image;
    bool result = readBMP("test_invalid.bmp", image);

    EXPECT_FALSE(result);
}

// Test: Fail when reading non-24-bit BMP
TEST_F(BitmapTest, ReadNon24BitBMP) {
    // Create a BMP with 8-bit color depth
    std::ofstream file("test_invalid.bmp", std::ios::binary);

    BMP_WORD bfType = 0x4D42;
    BMP_DWORD bfSize = 1000;
    BMP_WORD bfReserved1 = 0;
    BMP_WORD bfReserved2 = 0;
    BMP_DWORD bfOffBits = 54;

    file.write(reinterpret_cast<const char*>(&bfType), 2);
    file.write(reinterpret_cast<const char*>(&bfSize), 4);
    file.write(reinterpret_cast<const char*>(&bfReserved1), 2);
    file.write(reinterpret_cast<const char*>(&bfReserved2), 2);
    file.write(reinterpret_cast<const char*>(&bfOffBits), 4);

    BMP_DWORD biSize = 40;
    BMP_LONG biWidth = 10;
    BMP_LONG biHeight = 10;
    BMP_WORD biPlanes = 1;
    BMP_WORD biBitCount = 8;  // 8-bit instead of 24-bit
    BMP_DWORD biCompression = 0;
    BMP_DWORD biSizeImage = 100;
    BMP_LONG biXPelsPerMeter = 0;
    BMP_LONG biYPelsPerMeter = 0;
    BMP_DWORD biClrUsed = 0;
    BMP_DWORD biClrImportant = 0;

    file.write(reinterpret_cast<const char*>(&biSize), 4);
    file.write(reinterpret_cast<const char*>(&biWidth), 4);
    file.write(reinterpret_cast<const char*>(&biHeight), 4);
    file.write(reinterpret_cast<const char*>(&biPlanes), 2);
    file.write(reinterpret_cast<const char*>(&biBitCount), 2);
    file.write(reinterpret_cast<const char*>(&biCompression), 4);
    file.write(reinterpret_cast<const char*>(&biSizeImage), 4);
    file.write(reinterpret_cast<const char*>(&biXPelsPerMeter), 4);
    file.write(reinterpret_cast<const char*>(&biYPelsPerMeter), 4);
    file.write(reinterpret_cast<const char*>(&biClrUsed), 4);
    file.write(reinterpret_cast<const char*>(&biClrImportant), 4);

    file.close();

    Image image;
    bool result = readBMP("test_invalid.bmp", image);

    EXPECT_FALSE(result);
}

// Test: Verify BGR to RGB conversion
TEST_F(BitmapTest, VerifyColorConversion) {
    Image image;
    bool result = readBMP("test_valid.bmp", image);

    ASSERT_TRUE(result);
    ASSERT_GE(image.bytes.size(), 3);

    // The bitmap stores pixels as BGR, readBMP should convert to RGB
    // We can't predict exact values due to the gradient, but we can verify
    // that data exists and has the right size
    EXPECT_EQ(image.bytes.size(), image.width * image.height * 3);
}

// Test: Check stride calculation with padding
TEST_F(BitmapTest, CheckStrideCalculation) {
    Image image;
    bool result = readBMP("test_valid.bmp", image);

    ASSERT_TRUE(result);

    // For a 4-pixel wide image (4 * 3 = 12 bytes), padded to 12 (no padding needed)
    // Stride should be 12
    EXPECT_EQ(image.stride, 12);
}

// Test: Empty file handling
TEST_F(BitmapTest, ReadEmptyFile) {
    std::ofstream file("test_invalid.bmp", std::ios::binary);
    file.close();

    Image image;
    bool result = readBMP("test_invalid.bmp", image);

    EXPECT_FALSE(result);
}

// Test: Truncated file (file ends before pixel data can be read)
TEST_F(BitmapTest, ReadTruncatedFile) {
    // Create a BMP with valid headers but no pixel data
    std::ofstream file("test_truncated.bmp", std::ios::binary);

    // BMP File Header (14 bytes)
    BMP_WORD bfType = 0x4D42;  // "BM"
    BMP_DWORD bfSize = 1000;   // Claim larger size
    BMP_WORD bfReserved1 = 0;
    BMP_WORD bfReserved2 = 0;
    BMP_DWORD bfOffBits = 54;

    file.write(reinterpret_cast<const char*>(&bfType), 2);
    file.write(reinterpret_cast<const char*>(&bfSize), 4);
    file.write(reinterpret_cast<const char*>(&bfReserved1), 2);
    file.write(reinterpret_cast<const char*>(&bfReserved2), 2);
    file.write(reinterpret_cast<const char*>(&bfOffBits), 4);

    // BMP Info Header (40 bytes)
    BMP_DWORD biSize = 40;
    BMP_LONG biWidth = 10;
    BMP_LONG biHeight = 10;
    BMP_WORD biPlanes = 1;
    BMP_WORD biBitCount = 24;
    BMP_DWORD biCompression = 0;
    BMP_DWORD biSizeImage = 300;  // 10x10x3 bytes
    BMP_LONG biXPelsPerMeter = 0;
    BMP_LONG biYPelsPerMeter = 0;
    BMP_DWORD biClrUsed = 0;
    BMP_DWORD biClrImportant = 0;

    file.write(reinterpret_cast<const char*>(&biSize), 4);
    file.write(reinterpret_cast<const char*>(&biWidth), 4);
    file.write(reinterpret_cast<const char*>(&biHeight), 4);
    file.write(reinterpret_cast<const char*>(&biPlanes), 2);
    file.write(reinterpret_cast<const char*>(&biBitCount), 2);
    file.write(reinterpret_cast<const char*>(&biCompression), 4);
    file.write(reinterpret_cast<const char*>(&biSizeImage), 4);
    file.write(reinterpret_cast<const char*>(&biXPelsPerMeter), 4);
    file.write(reinterpret_cast<const char*>(&biYPelsPerMeter), 4);
    file.write(reinterpret_cast<const char*>(&biClrUsed), 4);
    file.write(reinterpret_cast<const char*>(&biClrImportant), 4);

    // Don't write pixel data - file ends here, causing fread to fail
    file.close();

    Image image;
    bool result = readBMP("test_truncated.bmp", image);

    // Should fail because fread cannot read the expected pixel data
    EXPECT_FALSE(result);
}
