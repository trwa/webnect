#include <cstdint>
#include <iostream>

#include "libfreenect.h"

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

int main(int argc, char *argv[]) {
  int32_t ret = 0;
  freenect_context *ctx = nullptr;
  freenect_device *dev = nullptr;

  ret = freenect_init(&ctx, nullptr);
  if (ret != 0) {
    std::cerr << "error: could not initialize context with error " << ret << '\n';
    return 1;
  }
  if (ctx == NULL) {
    std::cerr << "error: could not get context\n";
    return 1;
  }

  ret = freenect_open_device(ctx, &dev, 0);
  if (ret != 0) {
    std::cerr << "error: could not open device with error " << ret << '\n';
    return 1;
  }
  if (dev == NULL) {
    std::cerr << "error: could not get device\n";
    return 1;
  }

  auto frame_mode = freenect_get_current_depth_mode(dev);
  frame_mode.depth_format = FREENECT_DEPTH_REGISTERED;

  ret = freenect_set_depth_mode(dev, frame_mode);
  if (ret != 0) {
    std::cerr << "error: could not set depth mode with error " << ret << '\n';
    return 1;
  }

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
