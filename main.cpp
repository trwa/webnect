#include <cstdint>
#include <iostream>

#include "libfreenect.h"

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

#include "frame_generated.h"

void testFlatBuffers() {
  std::vector<Webnect::V1::Row> rows{};
  rows.reserve(480);
  for (auto i = 0; i < 480; i++) {
    std::vector<Webnect::V1::Pixel> rowData = {};
    rowData.reserve(640);
    for (auto j = 0; j < 640; ++j) {
      auto color = Webnect::V1::Color(255, 0, 0);
      auto point = Webnect::V1::Point(0.0f, 1.0f, 2.0f);
      auto pixel = Webnect::V1::Pixel(color, point);
      rowData.push_back(pixel);
    }
    auto row = Webnect::V1::Row(::flatbuffers::span<const Webnect::V1::Pixel, 640>(rowData.data(), 640));
    rows.push_back(row);
  }
  const auto image = Webnect::V1::Image(::flatbuffers::span<const Webnect::V1::Row, 480>(rows.data(), 480));
  flatbuffers::FlatBufferBuilder builder;
  auto frameBuilder = Webnect::V1::FrameBuilder(builder);
  frameBuilder.add_image(&image);
  frameBuilder.add_msvideo(123456);
  frameBuilder.add_msdepth(654321);
  const auto frameOffset = frameBuilder.Finish();
  builder.Finish(frameOffset);
  uint8_t *buf = builder.GetBufferPointer();
  const auto size = builder.GetSize();
  std::cout << size << std::endl;
}

int main(int argc, char *argv[]) {
  int32_t ret = 0;


  std::cout << "TESTING FLATBUFFERS" << std::endl;
  testFlatBuffers();
  std::cout << "FLATBUFFERS TEST COMPLETE" << std::endl;

  cv::namedWindow("depth", cv::WINDOW_NORMAL);
  cv::resizeWindow("depth", 640, 480);
  //cv::namedWindow("video", cv::WINDOW_NORMAL);
  //cv::resizeWindow("video", 640, 480);

  auto const on_depth = [](freenect_device *dev, void *data, uint32_t timestamp) {
    auto data_u16 = (uint16_t *) data;
    float data_f32[480 * 640];
    for (size_t i = 0; i < 480 * 640; ++i) {
      data_f32[i] = data_u16[i] / 2048.0;
    }
    auto const frame = cv::Mat(480, 640, CV_32FC1, (void *) data_f32);
    cv::imshow("depth", frame);
  };
  auto const on_video = [](freenect_device *dev, void *data, uint32_t timestamp) {
    auto const frame = cv::Mat(480, 640, CV_8UC3, data);
    cv::imshow("video", frame);
  };
  freenect_set_depth_callback(dev, on_depth);
  freenect_set_video_callback(dev, on_video);

  ret = freenect_start_depth(dev);
  if (ret != 0) {
    std::cerr << "error: could not start depth acquisition with error " << ret << '\n';
    return 1;
  }

  //ret = freenect_start_video(dev);
  if (ret != 0) {
    std::cerr << "error: could not start video acquisition with error " << ret << '\n';
    return 1;
  }

  auto running = true;
  while (running) {
    freenect_process_events(ctx);
    if (cv::waitKey(30) == 27) {
      running = false;
    }
  }

  ret = freenect_shutdown(ctx);
  if (ret != 0) {
    std::cerr << "error: could not shutdown with error " << ret << '\n';
    return 1;
  }

  return 0;
}
