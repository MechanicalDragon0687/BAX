#include <iostream>
#include <stdexcept>

#include <opencv2/opencv.hpp>

#include "raw.h"

namespace RAW {
	Container::Container(std::string const& filename)
	{
		this->cap = new VideoCapture(filename);
		if (!this->cap->isOpened())
			throw std::runtime_error("failed to open video file");

		this->rate = this->cap->get(CAP_PROP_FPS);
		this->width = this->cap->get(CAP_PROP_FRAME_WIDTH);
		this->height = this->cap->get(CAP_PROP_FRAME_HEIGHT);
	}

	Container::~Container(void)
	{
		for (uint16_t *f : this->frames)
			delete[] f;
		delete this->cap;
	}

	static uint16_t ConvertFromBGR(uint8_t *px) {
		uint16_t b = px[0], g = px[1], r = px[2];

		b >>= 3;
		g >>= 2;
		r >>= 3;

		return (r << 11) | (g << 5) | (b << 0);
	}

	bool Container::ProcessNextFrame(void)
	{
		Mat frame;
		uint16_t *rgb_buffer;

		*(this->cap) >> frame;

		if (frame.empty())
			return false;

		rgb_buffer = new uint16_t[this->width * this->height];

		// Perform transposing + color conversion
		// Should probably let OpenCV do it all for me
		// because of SSE/NEON optimizations but whatever
		for (int x = 0; x < frame.cols; x++) {
			for (int y = 0; y < frame.rows; y++) {
				rgb_buffer[(x * this->height) + (this->height - y - 1)] = ConvertFromBGR(frame.ptr(y, x));
			}
		}

		this->frames.push_back(rgb_buffer);		
		return true;
	}
}