#include "gtest/gtest.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int rtn = RUN_ALL_TESTS();
  return rtn;
}
