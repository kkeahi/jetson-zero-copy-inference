#include "../../include/middleware/v4l2_device.hpp"
#include "../../include/middleware/v4l2_utils.hpp"

#include <linux/videodev2.h>

#include <iostream> 	// debugging/logging
#include <filesystem>	// check path
#include <stdexcept>	// error handling
#include <fcntl.h> 	// open()
#include <unistd.h>	// close(), read(), write()
#include <stdio.h>
#include <errno.h>
#include <string.h> 	// strerror()

namespace jetson_middleware { 

V4L2Device::V4L2Device(const char* path, uint32_t width, uint32_t height)
	: _path(path), _width(width), _height(height) 
{
	_fd = open(path, 0);
	if (_fd < 0) {} // err
	

	// query capabilities //
	v4l2_capability cap {};
	if (xioctl(_fd, VIDIOC_QUERYCAP, &cap) < 0) {} // err
	
	const bool is_capture = 
		(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) || 	// make sure device supports video capture
		(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE); 	// check physical device & specific opened fd
	
	const bool is_streaming = 
		(cap.capabilities & V4L2_CAP_STREAMING) || 	// make sure device supports streaming I/O via user pointers
		(cap.device_caps & V4L2_CAP_STREAMING);

	if (!is_capture) {} 	// err
	if (!is_streaming) {} 	// err
	
	// set device format //
	v4l2_format fmt {};
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = _width;
	fmt.fmt.pix.height = _height;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;	// may need customization
	fmt.fmt.pix.field = V4L2_FIELD_NONE;		// may need customization
	
	if (xioctl(_fd, VIDEOC_S_FMT, &fmt) < 0) {} // err
	
	// VIDIO_C_FMT may change fmt
	_width = fmt.fmt.pix.width;
	_height = fmt.fmt.pix.height;
	// may need one for pixelformat
}

V4L2Device::~V4L2Device() {
}

void V4L2Device::request_buffers(uint32_t count) {
	
}

void V4L2Device::enqueue_frame() {}

// DeviceBuffer& V4L2Device::dequeue_frame() {}

void V4L2Device::start_stream() {}

void V4L2Device::stop_stream() {}

void V4L2Device::_set_format() {
	v4l2_format fmt;

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = _width;
	fmt.fmt.pix.height = _height;
	// fmt.fmt.pixelformat = V4L2_PIX_FMT_UYVY;
	// fmt.fmt.field = V4L2_FIELD_INTERLACED;

	if (xioctl(_fd, VIDIOC_S_FMT, &fmt) == -1) throw std::runtime_error("FORMAT");

	// VIDIOC_S_FMT may alter fmt. probably verify width/height after
}

void _set_params(const char* path, uint32_t width, uint32_t height) {
	const std::filesystem::path dir(path);

	if (!std::filesystem::exists(dir)) {
		std::cerr << path << " does not exist\n";
		throw std::runtime_error("PATH");
	}
	_path = path;

	if (width < 1) {
		std::cerr << "Frame width must be at least 1px\n";
		throw std::runtime_error("WIDTH");
	}
	_width = width;

	if (height < 1) {
		std::cerr << "Frame height must be at least 1px\n";
		throw std::runtime_error("HEIGHT");
	}
	_height = height;

	_fd = open(path, 0);
	if (_fd == -1) {
		int err = errno;
		fprintf(stderr, "ERROR: failed to open path '%s': %s\n", path, strerror(err));
		throw std::runtime_error("PATH");
	}
}

void _validate_capabilities(int fd) {
	struct v4l2_capability cap;
	if (xioctl(_fd, VIDIOC_QUERYCAP, &cap) == -1) throw std::runtime_error("DEVICE CAPABILITIES");
	
	if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE == 0) {
		fprintf(stderr, "ERROR: device does not support video capture");
		throw std::runtime_error("DEVICE VIDEO CAPTURE");
	}
}
}
