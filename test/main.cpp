#include "cpplibsocket/Socket.h"

#include <gmock/gmock.h>

using namespace cpplibsocket;

TEST(SocketTest, basic) {
    Socket<IPProto::TCP>{IPVer::IPV4};
}

int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
