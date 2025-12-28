#include "../include/middleware/v4l2_device.hpp"

using namespace jetson_middleware;

int main() {
	V4L2Device dvc("C://abcd/aca", 256, 256);

	return 0;
}
