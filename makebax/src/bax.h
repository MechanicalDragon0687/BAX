#pragma once

#include <vector>
#include <mutex>

#include <cstdint>

#define BAX_HDR_MAXAUTH	(32)
#define BAX_HDR_MAXINFO	(192)

#define RGB565(r, g, b)	(((r) >> 3) << 11 | ((g) >> 2) << 5 | ((b) >> 3))

template<typename T>
static inline T AlignUp(T n, size_t a) {
	size_t msk = a - 1;
	return n + ((a - (n & msk)) & msk);
}

namespace BAX {
	class Frame {
	public:
		Frame(const uint16_t *raw_data, size_t raw_size, unsigned clvl);
		~Frame(void);

		uint8_t *Data(void) {
			return this->data;
		}

		size_t Raw_Size(void) {
			return this->raw_size;
		}

		size_t Compressed_Size(void) {
			return this->comp_size;
		}

	private:
		uint8_t *data;
		size_t raw_size;
		size_t comp_size;
	};

	class Header {
	public:
		Header(unsigned x_offset, unsigned x_length);
		~Header(void);

		void SetAuthor(std::string const& auth) {
			this->auth = auth;
		}

		void SetInfo(std::string const& info) {
			this->info = info;
		}

		void SetBackgroundColor(uint16_t color) {
			this->bg_color = color;
		}

		void SetFramerate(unsigned r) {
			this->rate = r;
		}

		size_t TotalSize(void) {
			return this->total_size;
		}

		void AddFrame(BAX::Frame *frame, unsigned i);

		void DumpFile(std::ofstream &file);

	private:
		std::string auth, info;

		size_t total_size;

		unsigned rate;
		unsigned x_offset;
		unsigned x_length;
		uint16_t bg_color;

		std::mutex lock;
		std::vector<BAX::Frame*> frames;
	};
}
