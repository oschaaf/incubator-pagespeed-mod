/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


#include "base/logging.h"
#include "pagespeed/kernel/base/gtest.h"
#include "pagespeed/kernel/base/message_handler.h"
#include "pagespeed/kernel/base/mock_message_handler.h"
#include "pagespeed/kernel/base/null_mutex.h"
#include "pagespeed/kernel/base/string.h"
#include "pagespeed/kernel/base/string_util.h"
#include "pagespeed/kernel/image/image_converter.h"
#include "pagespeed/kernel/image/image_util.h"
#include "pagespeed/kernel/image/png_optimizer.h"
#include "pagespeed/kernel/image/read_image.h"
#include "pagespeed/kernel/image/test_utils.h"
#include "pagespeed/kernel/image/webp_optimizer.h"

namespace {

using net_instaweb::MockMessageHandler;
using net_instaweb::NullMutex;
using pagespeed::image_compression::FrameSpec;
using pagespeed::image_compression::ImageConverter;
using pagespeed::image_compression::ImageSpec;
using pagespeed::image_compression::IMAGE_GIF;
using pagespeed::image_compression::IMAGE_PNG;
using pagespeed::image_compression::IMAGE_WEBP;
using pagespeed::image_compression::kPngSuiteTestDir;
using pagespeed::image_compression::kMessagePatternPixelFormat;
using pagespeed::image_compression::kMessagePatternStats;
using pagespeed::image_compression::kMessagePatternWritingToWebp;
using pagespeed::image_compression::kTestRootDir;
using pagespeed::image_compression::kWebpTestDir;
using pagespeed::image_compression::size_px;
using pagespeed::image_compression::PixelFormat;
using pagespeed::image_compression::PngScanlineReaderRaw;
using pagespeed::image_compression::QUIRKS_CHROME;
using pagespeed::image_compression::ReadFile;
using pagespeed::image_compression::ReadTestFile;
using pagespeed::image_compression::RGB_888;
using pagespeed::image_compression::RGBA_8888;
using pagespeed::image_compression::SCANLINE_STATUS_INVOCATION_ERROR;
using pagespeed::image_compression::SCANLINE_STATUS_SUCCESS;
using pagespeed::image_compression::ScanlineReaderInterface;
using pagespeed::image_compression::ScanlineStatus;
using pagespeed::image_compression::ScanlineWriterInterface;
using pagespeed::image_compression::WebpConfiguration;
using pagespeed::image_compression::WebpScanlineReader;

struct ImageInfo {
  const char* original_file;
  const char* gold_file;
};

// Each file in kValidImages is saved in both PNG format and WebP format.
// alpha_32x32.webp is lossless, while opaque_32x20.webp is lossy.
// alpha_32x32.png and opaque_32x20.png were generated by 'dwebp' from the
// corresponding WebP images. pagespeed_32x32_gray.png and
// pagespeed_32x32_rgb.png have the same pixel values, but are stored in
// different formats.
const ImageInfo kValidImages[] = {
  {"alpha_32x32", "alpha_32x32"},         // size 32-by-32 with alpha.
  {"opaque_32x20", "opaque_32x20"},       // size 32-by-20 without alpha.
  {"gray_saved_as_gray", "gray_saved_as_rgb"},  // images of same contents.
};
const size_t kValidImageCount = arraysize(kValidImages);

const char kFileCorruptedHeader[] = "corrupt_header";
const char kFileCorruptedBody[] = "corrupt_body";
const double kMinPSNR = 33.0;

// Message to ignore.
const char kMessagePatternInvalidWebPData[] = "Invalid WebP data.";

class WebpScanlineOptimizerTest : public testing::Test {
 public:
  WebpScanlineOptimizerTest()
    : message_handler_(new NullMutex),
      reader_(&message_handler_),
      scanline_(nullptr) {
  }

  bool Initialize(const char* file_name) {
    if (!ReadTestFile(kWebpTestDir, file_name, "webp", &input_image_)) {
      PS_LOG_ERROR((&message_handler_), "Failed to read file: %s", file_name);
      return false;
    }
    return reader_.Initialize(input_image_.c_str(), input_image_.length());
  }

  void ConvertPngToWebp(const GoogleString& png_image,
                        const WebpConfiguration& webp_config,
                        GoogleString* webp_image) {
    PngScanlineReaderRaw png_reader(&message_handler_);
    // Initialize a PNG reader for reading the original image.
    ASSERT_TRUE(png_reader.Initialize(
        png_image.data(), png_image.length()));

    // Get the sizes and pixel format of the original image.
    const size_t width = png_reader.GetImageWidth();
    const size_t height = png_reader.GetImageHeight();
    const PixelFormat pixel_format = png_reader.GetPixelFormat();

    // Create a WebP writer.
    std::unique_ptr<ScanlineWriterInterface> webp_writer(
        CreateScanlineWriter(pagespeed::image_compression::IMAGE_WEBP,
                             pixel_format, width, height, &webp_config,
                             webp_image, &message_handler_));
    ASSERT_NE(reinterpret_cast<ScanlineWriterInterface*>(NULL),
              webp_writer.get());

    // Read the scanlines from the original image and write them to the new one.
    while (png_reader.HasMoreScanLines()) {
      uint8* scanline = nullptr;
      ASSERT_TRUE(png_reader.ReadNextScanline(
          reinterpret_cast<void**>(&scanline)));
      ASSERT_TRUE(webp_writer->WriteNextScanline(
          reinterpret_cast<void*>(scanline)));
    }
    ScanlineStatus status = webp_writer->FinalizeWriteWithStatus();
    ASSERT_TRUE(status.Success()) << "Status=" << status.ToString();
  }

 protected:
  void SetUp() override {
    message_handler_.AddPatternToSkipPrinting(kMessagePatternInvalidWebPData);
    message_handler_.AddPatternToSkipPrinting(kMessagePatternPixelFormat);
    message_handler_.AddPatternToSkipPrinting(kMessagePatternStats);
    message_handler_.AddPatternToSkipPrinting(kMessagePatternWritingToWebp);
  }

 protected:
  MockMessageHandler message_handler_;
  WebpScanlineReader reader_;
  GoogleString input_image_;
  void* scanline_;

 private:
  DISALLOW_COPY_AND_ASSIGN(WebpScanlineOptimizerTest);
};

// Test both WebP writer and reader. It is done by encoding an image to WebP
// format, then decoding the image and comparing with the gold image. The image
// is encoded as lossy format.
TEST_F(WebpScanlineOptimizerTest, ConvertToAndReadLossyWebp) {
  WebpConfiguration webp_config;
  webp_config.lossless = false;
  webp_config.quality = 90;

  for (size_t i = 0; i < kValidImageCount; ++i) {
    GoogleString original_image, gold_image, webp_image;
    GoogleString file_name = kValidImages[i].original_file;
    ReadTestFile(kWebpTestDir, file_name.c_str(), "png", &original_image);
    ConvertPngToWebp(original_image, webp_config, &webp_image);
    ReadTestFile(kWebpTestDir, kValidImages[i].gold_file, "png",
                 &gold_image);
    DecodeAndCompareImagesByPSNR(IMAGE_PNG, gold_image.c_str(),
                                 gold_image.length(), IMAGE_WEBP,
                                 webp_image.c_str(), webp_image.length(),
                                 kMinPSNR,
                                 true,  // ignore_transparent_rgb
                                 false,  // do not expand colors
                                 &message_handler_);
  }
}

// Test both WebP writer and reader. It is done by encoding an image to WebP
// format, then decoding the image and comparing with the gold image. The image
// is encoded as lossless format.
TEST_F(WebpScanlineOptimizerTest, ConvertToAndReadLosslessWebp) {
  WebpConfiguration webp_config;
  webp_config.lossless = true;

  for (size_t i = 0; i < kValidImageCount; ++i) {
    GoogleString original_image, gold_image, webp_image;
    ReadTestFile(kWebpTestDir, kValidImages[i].original_file, "png",
                 &original_image);
    ConvertPngToWebp(original_image, webp_config, &webp_image);
    ReadTestFile(kWebpTestDir, kValidImages[i].gold_file, "png",
                 &gold_image);
    DecodeAndCompareImages(IMAGE_PNG, gold_image.c_str(), gold_image.length(),
                           IMAGE_WEBP, webp_image.c_str(),
                           webp_image.length(),
                           true,  // ignore_transparent_rgb
                           &message_handler_);
  }
}

// Verify that decoded image is accurate as determined by PSNR.
// The gold data was loaded from disk.
TEST_F(WebpScanlineOptimizerTest, CompareToWebpGolds) {
  WebpConfiguration webp_config;
  webp_config.lossless = true;

  for (size_t i = 0; i < kValidImageCount; ++i) {
    GoogleString png_image, webp_image;
    ReadTestFile(kWebpTestDir, kValidImages[i].gold_file, "png", &png_image);
    ReadTestFile(kWebpTestDir, kValidImages[i].gold_file, "webp", &webp_image);
    DecodeAndCompareImagesByPSNR(IMAGE_PNG, png_image.c_str(),
                                 png_image.length(), IMAGE_WEBP,
                                 webp_image.c_str(), webp_image.length(), 55,
                                 true,  // ignore_transparent_rgb
                                 false,  // do not expand colors
                                 &message_handler_);
  }
}

TEST_F(WebpScanlineOptimizerTest, InitializeWithoutRead) {
  ASSERT_TRUE(Initialize(kValidImages[0].original_file));
}

TEST_F(WebpScanlineOptimizerTest, ReadOneRow) {
  ASSERT_TRUE(Initialize(kValidImages[0].original_file));
  ASSERT_TRUE(reader_.ReadNextScanline(&scanline_));
}

TEST_F(WebpScanlineOptimizerTest, ReinitializeAfterOneRow) {
  ASSERT_TRUE(Initialize(kValidImages[0].original_file));
  ASSERT_TRUE(reader_.ReadNextScanline(&scanline_));
  ASSERT_TRUE(Initialize(kValidImages[1].original_file));
  ASSERT_TRUE(reader_.ReadNextScanline(&scanline_));
}

TEST_F(WebpScanlineOptimizerTest, ReInitializeAfterLastRow) {
  ASSERT_TRUE(Initialize(kValidImages[0].original_file));
  while (reader_.HasMoreScanLines()) {
    ASSERT_TRUE(reader_.ReadNextScanline(&scanline_));
  }

  // After depleting the scanlines, any further call to
  // ReadNextScanline leads to death in debugging mode, or a
  // false in release mode.
#ifdef NDEBUG
  EXPECT_FALSE(reader_.ReadNextScanline(&scanline_));
#else
  EXPECT_DEATH(reader_.ReadNextScanline(&scanline_),
               "The reader was not initialized or the image does not "
               "have any more scanlines.");
#endif

  ASSERT_TRUE(Initialize(kValidImages[1].original_file));
  ASSERT_TRUE(reader_.ReadNextScanline(&scanline_));
}

TEST_F(WebpScanlineOptimizerTest, InvalidWebpHeader) {
  ASSERT_FALSE(Initialize(kFileCorruptedHeader));
}

TEST_F(WebpScanlineOptimizerTest, InvalidWebpBody) {
  ASSERT_TRUE(Initialize(kFileCorruptedBody));
  ASSERT_FALSE(reader_.ReadNextScanline(&scanline_));
}


class AnimatedWebpTest : public testing::Test {
 public:
  AnimatedWebpTest() : message_handler_(new NullMutex) {}

  void ConvertGifToWebp(const char* filename,
                        const GoogleString& input_image,
                        WebpConfiguration* webp_config,
                        GoogleString* webp_image) {
    ScanlineStatus status;
    reader_.reset(
        CreateImageFrameReader(IMAGE_GIF,
                               input_image.c_str(), input_image.length(),
                               QUIRKS_CHROME, &message_handler_, &status));
    ASSERT_TRUE(status.Success());

    writer_.reset(CreateImageFrameWriter(IMAGE_WEBP, webp_config, webp_image,
                                         &message_handler_, &status));
    ASSERT_TRUE(status.Success());

    EXPECT_TRUE(
        ImageConverter::ConvertMultipleFrameImage(reader_.get(),
                                                  writer_.get()).Success())
        << " for '" << filename << "'";
  }

  void CheckGifVsWebP(const char* filename, WebpConfiguration* webp_config,
                      bool check_pixels) {
    GoogleString input_path = StrCat(net_instaweb::GTestSrcDir(),
                                                   kTestRootDir, filename);

    GoogleString input_image;
    ASSERT_TRUE(ReadFile(input_path, &input_image));
    DVLOG(1) << "Read image: " << input_path;

    GoogleString output_image;
    ConvertGifToWebp(filename, input_image, webp_config, &output_image);

    GoogleString output_path =
        StrCat(net_instaweb::GTestTempDir(),
                             kTestRootDir,
                             filename, ".webp");

    if (check_pixels) {
      EXPECT_TRUE(
          pagespeed::image_compression::CompareAnimatedImages(
              filename, output_image, &message_handler_));
    }
  }

  void PrepareWriterFor5x5Image(size_px num_frames) {
    webp_config_.lossless = false;
    ScanlineStatus status;
    writer_.reset(
        CreateImageFrameWriter(IMAGE_WEBP, &webp_config_, &output_image_,
                               &message_handler_, &status));
    ASSERT_TRUE(status.Success());

    image_spec_.width = 5;
    image_spec_.height = 5;
    image_spec_.num_frames = num_frames;
    image_spec_.loop_count = 1;
    EXPECT_TRUE(writer_->PrepareImage(&image_spec_, &status));
  }

 protected:
  MockMessageHandler message_handler_;
  std::unique_ptr<
    pagespeed::image_compression::MultipleFrameWriter> writer_;

 private:
  std::unique_ptr<
    pagespeed::image_compression::MultipleFrameReader> reader_;

  WebpConfiguration webp_config_;
  GoogleString output_image_;
  ImageSpec image_spec_;
};

struct ProgressData {
  net_instaweb::MessageHandler* handler;
  int times_called;
};

bool UpdateProgress(int percent, void* user_data) {
  ProgressData* progress_data = static_cast<ProgressData*>(user_data);
  progress_data->times_called++;
  return true;
}

TEST_F(AnimatedWebpTest, ConvertGifs) {
  ProgressData progress_data;
  progress_data.handler = &message_handler_;

  WebpConfiguration webp_config;
  webp_config.lossless = true;
  webp_config.quality = 50;
  webp_config.progress_hook = UpdateProgress;
  webp_config.user_data = &progress_data;

  progress_data.times_called = 0;
  CheckGifVsWebP("gif/animated.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("gif/completely_transparent.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("gif/square2loop.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("gif/full2loop.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("gif/interlaced.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("gif/red_empty_screen.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("gif/red_unused_invalid_background.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("gif/transparent.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("gif/zero_size_animation.gif", &webp_config, false);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("webp/multiple_frame_opaque.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);

  progress_data.times_called = 0;
  CheckGifVsWebP("webp/multiple_frame_opaque_gray.gif", &webp_config, true);
  EXPECT_LT(3, progress_data.times_called);
}

TEST_F(AnimatedWebpTest, RequireFirstScanline) {
  PrepareWriterFor5x5Image(2);
  ScanlineStatus status;

  FrameSpec frame_spec;
  frame_spec.width = 5;
  frame_spec.height = 5;
  frame_spec.top = 0;
  frame_spec.left = 0;
  frame_spec.pixel_format = RGB_888;
  EXPECT_TRUE(writer_->PrepareNextFrame(&frame_spec, &status));

#ifndef NDEBUG
  ASSERT_DEATH(writer_->PrepareNextFrame(&frame_spec, &status),
               "FRAME_WEBPWRITER/SCANLINE_STATUS_INVOCATION_ERROR "
               "CacheCurrentFrame: not all scanlines written");
#else
  EXPECT_FALSE(writer_->PrepareNextFrame(&frame_spec, &status));
  EXPECT_EQ(SCANLINE_STATUS_INVOCATION_ERROR, status.type());
#endif
}

TEST_F(AnimatedWebpTest, RequireAllScanlines) {
  PrepareWriterFor5x5Image(2);
  ScanlineStatus status;

  FrameSpec frame_spec;
  frame_spec.width = 5;
  frame_spec.height = 5;
  frame_spec.top = 0;
  frame_spec.left = 0;
  frame_spec.pixel_format = RGB_888;
  EXPECT_TRUE(writer_->PrepareNextFrame(&frame_spec, &status));

  uint8_t scanline[300];
  memset(scanline, 0x80, GetBytesPerPixel(RGB_888)*frame_spec.width);
  EXPECT_TRUE(writer_->WriteNextScanline(scanline, &status));

#ifndef NDEBUG
  ASSERT_DEATH(writer_->PrepareNextFrame(&frame_spec, &status),
               "FRAME_WEBPWRITER/SCANLINE_STATUS_INVOCATION_ERROR "
               "CacheCurrentFrame: not all scanlines written");
#else
  EXPECT_FALSE(writer_->PrepareNextFrame(&frame_spec, &status));
  EXPECT_EQ(SCANLINE_STATUS_INVOCATION_ERROR, status.type());
#endif
}

TEST_F(AnimatedWebpTest, RejectExtraScanlines) {
  PrepareWriterFor5x5Image(1);
  ScanlineStatus status;

  FrameSpec frame_spec;
  frame_spec.width = 3;
  frame_spec.height = 3;
  frame_spec.top = 0;
  frame_spec.left = 0;
  frame_spec.pixel_format = RGB_888;
  EXPECT_TRUE(writer_->PrepareNextFrame(&frame_spec, &status));

  uint8_t scanline[300];
  memset(scanline, 0x80, GetBytesPerPixel(RGB_888)*frame_spec.width);
  for (int j = 0; j < frame_spec.height; ++j) {
    EXPECT_TRUE(writer_->WriteNextScanline(scanline, &status));
  }
#ifndef NDEBUG
  ASSERT_DEATH(writer_->WriteNextScanline(scanline, &status),
               "FRAME_WEBPWRITER/SCANLINE_STATUS_INVOCATION_ERROR "
               "WriteNextScanline: too many scanlines");
#else
  EXPECT_FALSE(writer_->WriteNextScanline(scanline, &status));
#endif
}

TEST_F(AnimatedWebpTest, FrameAtOriginFallingOffImageFails) {
  PrepareWriterFor5x5Image(1);

  ScanlineStatus status;

  FrameSpec frame_spec;
  frame_spec.top = 0;
  frame_spec.left = 0;
  frame_spec.pixel_format = RGB_888;

  frame_spec.width = 6;
  frame_spec.height = 5;
  EXPECT_FALSE(writer_->PrepareNextFrame(&frame_spec, &status));
  EXPECT_EQ(SCANLINE_STATUS_INVOCATION_ERROR, status.type());

  status = ScanlineStatus(SCANLINE_STATUS_SUCCESS);
  frame_spec.width = 5;
  frame_spec.height = 6;
  EXPECT_FALSE(writer_->PrepareNextFrame(&frame_spec, &status));
  EXPECT_EQ(SCANLINE_STATUS_INVOCATION_ERROR, status.type());

  status = ScanlineStatus(SCANLINE_STATUS_SUCCESS);
  frame_spec.width = 5;
  frame_spec.height = 5;
  EXPECT_TRUE(writer_->PrepareNextFrame(&frame_spec, &status));
}

TEST_F(AnimatedWebpTest, FrameInMiddleFallingOffImageFails) {
  PrepareWriterFor5x5Image(1);
  ScanlineStatus status;

  FrameSpec frame_spec;
  frame_spec.width = 5;
  frame_spec.height = 5;
  frame_spec.pixel_format = RGB_888;

  frame_spec.top = 1;
  frame_spec.left = 0;
  EXPECT_FALSE(writer_->PrepareNextFrame(&frame_spec, &status));
  EXPECT_EQ(SCANLINE_STATUS_INVOCATION_ERROR, status.type());

  status = ScanlineStatus(SCANLINE_STATUS_SUCCESS);
  frame_spec.top = 0;
  frame_spec.left = 1;
  EXPECT_FALSE(writer_->PrepareNextFrame(&frame_spec, &status));
  EXPECT_EQ(SCANLINE_STATUS_INVOCATION_ERROR, status.type());

  status = ScanlineStatus(SCANLINE_STATUS_SUCCESS);
  frame_spec.top = 0;
  frame_spec.left = 0;
  EXPECT_TRUE(writer_->PrepareNextFrame(&frame_spec, &status));
}

}  // namespace
