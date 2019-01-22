#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>

#include "bax.h"
#include "end.h"
#include "main.h"
#include "compress.h"

#define BAX_HDR_VERSION	(1)
#define BAX_HDR_SIZE	(0x100)
#define BAX_FHDR_SIZE	(8)

#define BAX_MAX_WIDTH	(720)
#define BAX_MAX_HEIGHT	(240)
#define BAX_MAX_FRATE	(60)

#define BAX_MAX_FDIM	(BAX_MAX_WIDTH * BAX_MAX_HEIGHT)
#define BAX_MAX_FSIZE	(BAX_MAX_FDIM * 2)

namespace BAX {
	static const uint8_t Header_Magic[] = {'B', 'A', 'X', BAX_HDR_VERSION};

	Frame::Frame(const uint16_t *raw_data, size_t raw_size, unsigned clvl)
	{
		uint8_t *comp_buffer;
		size_t comp_size;

		if (raw_data == nullptr)
			throw std::invalid_argument("got null raw_data ptr");

		comp_buffer = CompressData(reinterpret_cast<const uint8_t*> (raw_data),
									raw_size, comp_size, clvl);

		this->data = comp_buffer;
		this->raw_size = raw_size;
		this->comp_size = comp_size;
	}

	Frame::~Frame(void)
	{
		delete[] this->data;
	}


	Header::Header(unsigned int x_offset, unsigned int x_length)
	{
		if ((x_offset + x_length) >= BAX_MAX_WIDTH)
			throw std::invalid_argument("invalid header dimensions");

		this->auth = "makebax";
		this->info = "Unknown";

		this->bg_color = RGB565(0, 0, 0);
		this->x_offset = x_offset;
		this->x_length = x_length;

		this->total_size = BAX_HDR_SIZE;
	}

	Header::~Header(void)
	{
		for (BAX::Frame *f : this->frames)
			delete f;
	}

	void Header::AddFrame(BAX::Frame *frame, unsigned int i)
	{
		this->lock.lock();

		// TODO - fix this garbage
		if (i >= this->frames.size())
			this->frames.resize(i + 1);

		this->total_size += frame->Compressed_Size() + BAX_FHDR_SIZE;
		this->frames[i] = frame;
		this->lock.unlock();
	}

	void Header::DumpFile(std::ofstream &file)
	{
		// Build header in memory
		union {
			uint8_t b[0x100];
			uint16_t s[0x80];
			uint32_t w[0x40];
		} header = {};

		std::vector<std::array<uint32_t, 2>> offlen;
		uint32_t total_offset;

		this->lock.lock();

		// Header magic
		for (auto i = 0; i < 4; i++)
			header.b[i] = Header_Magic[i];

		// Special flags
		header.w[1] = le32(0);

		// Frame count
		header.w[2] = le32(this->frames.size());

		// Frame rate
		header.w[3] = le32(this->rate);

		// Background color
		header.s[8] = le16(this->bg_color);

		// Line offset
		header.w[6] = le32(this->x_offset);

		// Line count
		header.w[7] = le32(this->x_length);

		// Reserved field
		for (auto i = 0x12; i < 0x18; i++)
			header.b[i] = 0;

		// Author and description fields
		std::strncat(reinterpret_cast<char*> (&header.b[0x20]), this->auth.c_str(), BAX_HDR_MAXAUTH - 1);
		std::strncat(reinterpret_cast<char*> (&header.b[0x40]), this->info.c_str(), BAX_HDR_MAXINFO - 1);

		// Build up frame offsets and sizes
		total_offset = sizeof(header) + (this->frames.size() * sizeof(uint32_t) * 2);
		offlen.resize(this->frames.size());
		for (unsigned i = 0; i < this->frames.size(); i++) {
			// Frame offset
			offlen[i][0] = total_offset;

			// Frame size
			offlen[i][1] = this->frames[i]->Compressed_Size();

			// Update offset within file, needs to be aligned to 4 bytes
			total_offset += AlignUp(offlen[i][1], 4);
		}

		// Write everything to the file
		file.seekp(0);
		file.write(reinterpret_cast<const char*> (&header), sizeof(header));

		file.seekp(sizeof(header));
		file.write(reinterpret_cast<const char*> (&offlen[0]), sizeof(uint32_t) * 2 * offlen.size());

		for (unsigned i = 0; i < this->frames.size(); i++) {
			file.seekp(offlen[i][0]);
			file.write(reinterpret_cast<const char*> (this->frames[i]->Data()), this->frames[i]->Compressed_Size());
		}

		this->lock.unlock();

	}
}
