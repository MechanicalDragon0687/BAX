#include <cstdint>
#include <stdexcept>

#include <lz4.h>
#include <lz4hc.h>

uint8_t *CompressData(const uint8_t *in, size_t in_sz, size_t& out_sz, int lvl)
{
	uint8_t *out_buffer;
	size_t bound, compressed_size;

	bound = LZ4_compressBound(in_sz);

	/**
	 * The idea here is despite requesting `bound` bytes from the heap,
	 * they won't actually be assigned to the process until it actually
	 * tries to write to it, and considering compression ratios are
	 * fairly good here, the actually used memory will be much lower
	 * than (bound * 2 * framecount)
	 *
	 * It's not ideal since it assumes too much about the host OS but
	 * it saves another memory allocation + memory copy
	 *
	 * Considering most frames are ~20KiB each on average, maybe
	 * it's a better idea to use a secondary buffer that's only
	 * the needed size, but that's to be tested in the future
	 */
	out_buffer = new uint8_t[bound];
	if (out_buffer == nullptr)
		throw std::bad_alloc();

	compressed_size = LZ4_compress_HC(reinterpret_cast<const char*> (in),
									reinterpret_cast<char*> (out_buffer),
									in_sz, bound, lvl);
	if (compressed_size == 0)
		throw std::runtime_error("failed to compress buffer");

	out_sz = compressed_size;
	return out_buffer;
}
