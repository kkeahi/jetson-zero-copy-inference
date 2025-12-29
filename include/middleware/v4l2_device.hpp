#ifndef JETSON_VIDEO_MIDDLEWARE_HPP
#define JETSON_VIDEO_MIDDLEWARE_HPP

#include <cstdint> // uint32_t
#include <cstddef> // size_t
#include <vector>

#include <cuda_runtime.h>

namespace jetson_middleware {

// Buffer (frame) shared between camera and GPU
struct DeviceBuffer {
	uint32_t index;		// V4L2 buffer index in vector
	size_t size;		// Size of frame data (bytes)

	int dmabuf_fd; 		// Exported DMA_BUF file descriptor

	cudaExternalMemory_t ext_mem	// CUDA external memory handle
	void* device_ptr		// GPU pointer mapped from DMA-BUF
};

class V4L2Device {
public:
	V4L2Device(const char* path, uint32_t width, uint32_t height, uint32_t format);
	~V4L2Device();

	void request_buffers(uint32_t count);	// Allocate V4L2 driver-owned buffers
	void prime_buffers();			// Fill vector with empty buffers

	void start_stream();	// Start camera streaming
	void stop_stream();	// Stop camera streaming

	DeviceBuffer& aquire_buffer();	// Dequeue filled buffer
	void release_buffer();		// Requeue buffer after GPU processing

private:
	int _fd = -1;		// Video device file descriptor
	const char* _path;	// Device path (/dev/deviceX)

	uint32_t _width;	// Frame width (px)
	uint32_t _height;	// Frame height (px)
	uint32_t _format;	// V4L2 pixel format

	std::vector<DeviceBuffer> _buffers;	// Buffer pool
};

}

#endif
