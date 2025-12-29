#include "../include/middleware/v4l2_device.hpp"

using namespace jetson_middleware;

int main() {
	// Create and configure device
	V4L2Device camera(
		"/dev/video0",
		1920,
		1080,
		V4L2_PIX_FMT_YUYV
	);

	// Allocate driver-owned buffers and map them to GPU
	camera.request_buffers(4);

	// Queue all buffers so capture can begin
	camera.prime_buffers();

	// Start camera streaming
	camera.start_stream();

	// Main processing loop
	while (true) {
		// Acquire a filled frame
		auto& buffer = camera.acquire_buffer();

		// GPU processing would occur here using buffer.device_ptr
		// launch_kernel(buffer.device_ptr, buffer.size);

		// Return buffer to the camera after GPU use
		camera.release_buffer(buffer.index);
	}

	// Stop streaming (unreachable in this outline)
	camera.stop_stream();

	return 0;
}
