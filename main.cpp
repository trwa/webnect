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

  ret = freenect_shutdown(ctx);
  if (ret != 0) {
    std::cerr << "error: could not shutdown with error " << ret << '\n';
    return 1;
  }

  return 0;
}
