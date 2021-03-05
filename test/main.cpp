#include "gtest/gtest.h"
#if defined(NC_OS_WIN)
//#  include <vld.h>
#endif

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int rtn = RUN_ALL_TESTS();
  return rtn;
}
