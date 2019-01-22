#pragma once

#include <cstdint>
#include <stdexcept>

#include <opencv2/opencv.hpp>

using namespace cv;

namespace RAW {
	/* NOT THREAD SAFE */
	class Container {
	public:
		Container(std::string const& filename);
		~Container(void);

		bool ProcessNextFrame(void);

		unsigned Framerate(void) {
			return this->rate;
		}

		unsigned Width(void) {
			return this->width;
		}

		unsigned Height(void) {
			return this->height;
		}

		unsigned Frames(void) {
			return this->frames.size();
		}

		uint16_t *Frame(unsigned n) {
			return this->frames[n];
		}

		void DumpInfo(void) {
			std::cout << "Framerate: " << this->rate << std::endl;
			std::cout << "Size: " << this->width << "x" << this->height << std::endl;
		}

	private:
		VideoCapture *cap;

		unsigned rate;
		unsigned width, height;

		std::vector<uint16_t*> frames;
	};
}
