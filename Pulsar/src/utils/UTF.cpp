#include "UTF.h"

#include <stdexcept>

static constexpr int UTF_B1_CAP = 0b1000'0000; // 0x80
static constexpr int UTF_B1_ERASURE = 0b0000'0000; // 0x00
static constexpr int UTF_B2_CAP = 0b1110'0000; // 0xE0
static constexpr int UTF_B2_MASK = 0b0001'1111; // 0x1F
static constexpr int UTF_B2_ERASURE = 0b1100'0000; // 0xC0
static constexpr int UTF_B3_CAP = 0b1111'0000; // 0xF0
static constexpr int UTF_B3_MASK = 0b0000'1111; // 0x0F
static constexpr int UTF_B3_ERASURE = 0b1110'0000; // 0xE0
static constexpr int UTF_B4_CAP = 0b1111'1000; // 0xF8
static constexpr int UTF_B4_MASK = 0b0000'0111; // 0x07
static constexpr int UTF_B4_ERASURE = 0b1111'0000; // 0xF0
static constexpr int UTF_CONT_MASK = 0b0011'1111; // 0x3F
static constexpr int UTF_CONT_HEAD = 0b1000'0000; // 0x80
static constexpr int UTF_CONT_CAPTURE = 0b1100'0000; // 0xC0
static constexpr int UTF_SURR_OFFSET = 0x0001'0000; // 0x10000 = 0b00000000'00000001'00000000'00000000
static constexpr int UTF_SURR_SHIFT = 10;
static constexpr int UTF_SURR_HIGH_OFFSET = 0xD800; // 0xD800 = 0b11011000'00000000
static constexpr int UTF_SURR_HIGH_MAX = 0xDBFF; // 0xDBFF = 0b11011011'11111111
static constexpr int UTF_SURR_LOW_OFFSET = 0xDC00; // 0xDC00 = 0b11011100'00000000
static constexpr int UTF_SURR_LOW_MAX = 0xDFFF; // 0xDFFF = 0b11011111'11111111
static constexpr int UTF_SURR_LOW_MASK = 0x03FF; // 0x3FF = 0b00000011'11111111
static constexpr int CODEPOINT_B1_MAX = 0x0000'007F; // 0x7F = 0b00000000'00000000'00000000'01111111
static constexpr int CODEPOINT_B2_MAX = 0x0000'07FF; // 0x7FF = 0b00000000'00000000'00000111'11111111
static constexpr int CODEPOINT_B3_MAX = 0x0000'FFFF; // 0xFFFF = 0b00000000'00000000'11111111'11111111
static constexpr int CODEPOINT_B4_MAX = 0x0010'FFFF; // 0x10FFFF = 0b00000000'00010000'11111111'11111111

template<int _bits> struct utf_bytes { using type = char; };
template<> struct utf_bytes<8> { using type = char8_t; };
template<> struct utf_bytes<16> { using type = char16_t; };
template<> struct utf_bytes<32> { using type = char32_t; };
template<int _bits> using utf_bytes_t = utf_bytes<_bits>::type;
template<int _bits> static constexpr utf_bytes_t<_bits> CH(char c) { return static_cast<utf_bytes_t<_bits>>(c); }
template<int _bits> static constexpr utf_bytes_t<_bits> CH(char8_t c) { return static_cast<utf_bytes_t<_bits>>(c); }
template<int _bits> static constexpr utf_bytes_t<_bits> CH(char16_t c) { return static_cast<utf_bytes_t<_bits>>(c); }
template<int _bits> static constexpr utf_bytes_t<_bits> CH(char32_t c) { return static_cast<utf_bytes_t<_bits>>(c); }
template<int _bits> static constexpr utf_bytes_t<_bits> CH(unsigned char c) { return static_cast<utf_bytes_t<_bits>>(c); }
template<int _bits> static constexpr utf_bytes_t<_bits> CH(int c) { return static_cast<utf_bytes_t<_bits>>(c); }
template<int _bits> static constexpr utf_bytes_t<_bits> CH(unsigned int c) { return static_cast<utf_bytes_t<_bits>>(c); }
static constexpr unsigned char UC(char c) { return static_cast<unsigned char>(c); }
static constexpr unsigned char UC(char8_t c) { return static_cast<unsigned char>(c); }
static constexpr unsigned char UC(char16_t c) { return static_cast<unsigned char>(c); }
static constexpr unsigned char UC(char32_t c) { return static_cast<unsigned char>(c); }
static constexpr unsigned char UC(unsigned char c) { return static_cast<unsigned char>(c); }
static constexpr unsigned char UC(int c) { return static_cast<unsigned char>(c); }
static constexpr unsigned char UC(unsigned int c) { return static_cast<unsigned char>(c); }

static void encode_into(char32_t codepoint, char8_t quad[4], std::u8string& utf8, bool ignore_invalid_chars)
{
	if (codepoint <= CODEPOINT_B1_MAX)
	{
		// 1-byte (ASCII)
		quad[0] = CH<8>(codepoint);
		utf8.append(quad, 1);
	}
	else if (codepoint <= CODEPOINT_B2_MAX)
	{
		// 2-byte
		quad[0] = CH<8>((codepoint >> 6) | UTF_B2_ERASURE);
		quad[1] = CH<8>((codepoint & UTF_CONT_MASK) | UTF_CONT_HEAD);
		utf8.append(quad, 2);
	}
	else if (codepoint <= CODEPOINT_B3_MAX)
	{
		// 3-byte
		quad[0] = CH<8>((codepoint >> 12) | UTF_B3_ERASURE);
		quad[1] = CH<8>(((codepoint >> 6) & UTF_CONT_MASK) | UTF_CONT_HEAD);
		quad[2] = CH<8>((codepoint & UTF_CONT_MASK) | UTF_CONT_HEAD);
		utf8.append(quad, 3);
	}
	else if (codepoint <= CODEPOINT_B4_MAX)
	{
		// 4-byte
		quad[0] = CH<8>((codepoint >> 18) | UTF_B4_ERASURE);
		quad[1] = CH<8>(((codepoint >> 12) & UTF_CONT_MASK) | UTF_CONT_HEAD);
		quad[2] = CH<8>(((codepoint >> 6) & UTF_CONT_MASK) | UTF_CONT_HEAD);
		quad[3] = CH<8>((codepoint & UTF_CONT_MASK) | UTF_CONT_HEAD);
		utf8.append(quad, 4);
	}
	else if (!ignore_invalid_chars)
	{
		quad[0] = quad[1] = quad[2] = quad[3] = 0;
		throw std::runtime_error("Invalid Unicode codepoint");
	}
	quad[0] = quad[1] = quad[2] = quad[3] = 0;
}

std::u8string UTF::encode(const std::u16string& utf16, bool ignore_invalid_chars)
{
	std::u8string utf8;
	char8_t quad[4]{ 0, 0, 0, 0 };
	char32_t codepoint = 0;

	for (size_t i = 0; i < utf16.size(); ++i)
	{
		codepoint = CH<32>(utf16[i]);
		
		if (codepoint >= UTF_SURR_HIGH_OFFSET && codepoint <= UTF_SURR_HIGH_MAX)
		{
			// Surrogate pair (4-byte UTF-8)
			if (i + 1 >= utf16.size())
			{
				if (!ignore_invalid_chars)
					throw std::out_of_range("Invalid UTF-16 sequence");
				continue;
			}
			char32_t low_surrogate = CH<32>(utf16[i + 1]);

			if (low_surrogate < UTF_SURR_LOW_OFFSET || low_surrogate > UTF_SURR_LOW_MAX) {
				if (!ignore_invalid_chars)
					throw std::runtime_error("Invalid UTF-16: unpaired high surrogate");
				continue;
			}
			++i;
			codepoint = ((codepoint - UTF_SURR_HIGH_OFFSET) << UTF_SURR_SHIFT) + (low_surrogate - UTF_SURR_LOW_OFFSET) + UTF_SURR_OFFSET;
		}
		else if (codepoint >= UTF_SURR_LOW_OFFSET && codepoint <= UTF_SURR_LOW_MAX)
		{
			if (!ignore_invalid_chars)
				throw std::runtime_error("Invalid UTF-16: unpaired low surrogate");
			continue;
		}

		encode_into(codepoint, quad, utf8, ignore_invalid_chars);
	}

	return utf8;
}

std::u16string UTF::decode_utf16(const std::u8string& utf8)
{
	std::u16string utf16;

	for (size_t i = 0; i < utf8.size();)
	{
		unsigned char byte = UC(utf8[i]);
		char32_t character = 0;

		if ((byte & UTF_B1_CAP) == UTF_B1_ERASURE) {
			// 1-byte (ASCII)
			character |= byte;
			i += 1;
		}
		else if ((byte & UTF_B2_CAP) == UTF_B2_ERASURE) {
			// 2-byte
			if (i + 1 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			character |= (byte & UTF_B2_MASK) << 6;
			character |= UC(utf8[i + 1]) & UTF_CONT_MASK;
			i += 2;
		}
		else if ((byte & UTF_B3_CAP) == UTF_B3_ERASURE) {
			// 3-byte
			if (i + 2 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			character |= (byte & UTF_B3_MASK) << 12;
			character |= (UC(utf8[i + 1]) & UTF_CONT_MASK) << 6;
			character |= UC(utf8[i + 2]) & UTF_CONT_MASK;
			i += 3;
		}
		else if ((byte & UTF_B4_CAP) == UTF_B4_ERASURE) {
			// 4-byte (surrogate pair)
			if (i + 3 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			character |= (byte & UTF_B4_MASK) << 18;
			character |= (UC(utf8[i + 1]) & UTF_CONT_MASK) << 12;
			character |= (UC(utf8[i + 2]) & UTF_CONT_MASK) << 6;
			character |= UC(utf8[i + 3]) & UTF_CONT_MASK;

			// Encode as UTF-16 surrogate pair
			character -= UTF_SURR_OFFSET;
			utf16.push_back(CH<16>((character >> UTF_SURR_SHIFT) + UTF_SURR_HIGH_OFFSET)); // high surrogate
			utf16.push_back(CH<16>((character & UTF_SURR_LOW_MASK) + UTF_SURR_LOW_OFFSET)); // low surrogate
			i += 4;
			continue;
		}
		else
			throw std::runtime_error("Invalid UTF-8 sequence");
		
		utf16.push_back(CH<16>(character));
	}

	return utf16;
}

std::u8string UTF::encode(const std::u32string& utf32, bool ignore_invalid_chars)
{
	std::u8string utf8;
	char8_t quad[4]{ 0, 0, 0, 0 };
	for (char32_t codepoint : utf32)
		encode_into(codepoint, quad, utf8, ignore_invalid_chars);
	return utf8;
}

std::u32string UTF::decode_utf32(const std::u8string& utf8)
{
	std::u32string utf32;
	size_t i = 0;
	while (i < utf8.size())
	{
		unsigned char byte = UC(utf8[i]);
		char32_t character = 0;

		if ((byte & UTF_B1_CAP) == UTF_B1_ERASURE)
		{
			// 1-byte (ASCII)
			character |= CH<32>(byte);
			i += 1;
		}
		else if ((byte & UTF_B2_CAP) == UTF_B2_ERASURE)
		{
			// 2-byte
			if (i + 1 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			character |= (byte & UTF_B2_MASK) << 6;
			character |= UC(utf8[i + 1]) & 0x3F;
			i += 2;
		}
		else if ((byte & UTF_B3_CAP) == UTF_B3_ERASURE)
		{
			// 3-byte
			if (i + 2 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			character |= (byte & UTF_B3_MASK) << 12;
			character |= (UC(utf8[i + 1]) & UTF_CONT_MASK) << 6;
			character |= UC(utf8[i + 2]) & UTF_CONT_MASK;
			i += 3;
		}
		else if ((byte & UTF_B4_CAP) == UTF_B4_ERASURE)
		{
			// 4-byte
			if (i + 3 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			character |= (byte & UTF_B4_MASK) << 18;
			character |= (UC(utf8[i + 1]) & UTF_CONT_MASK) << 12;
			character |= (UC(utf8[i + 2]) & UTF_CONT_MASK) << 6;
			character |= UC(utf8[i + 3]) & UTF_CONT_MASK;
			i += 4;
		}
		else
			throw std::runtime_error("Invalid UTF-8 sequence");

		utf32.push_back(character);
	}

	return utf32;
}

std::u8string UTF::convert(const std::string& str)
{
	return std::u8string(str.begin(), str.end());
}

std::string UTF::convert(const std::u8string& utf8)
{
	return std::string(utf8.begin(), utf8.end());
}

int UTF::String::Iterator::codepoint() const
{
	if (i >= string.str.size()) throw std::out_of_range("End of string");
	unsigned char first = UC(string.str[i]);
	char32_t codepoint = 0;

	if (first < UTF_B1_CAP)
	{
		codepoint |= first;
	}
	else if (first < UTF_B2_CAP)
	{
		if (i + 1 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint |= (first & UTF_B2_MASK) << 6;
		codepoint |= (UC(string.str[i + 1]) & UTF_CONT_MASK);
	}
	else if (first < UTF_B3_CAP)
	{
		if (i + 2 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint |= (first & UTF_B3_MASK) << 12;
		codepoint |= (UC(string.str[i + 1]) & UTF_CONT_MASK) << 6;
		codepoint |= (UC(string.str[i + 2]) & UTF_CONT_MASK);
	}
	else if (first < UTF_B4_CAP)
	{
		if (i + 3 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint |= (first & UTF_B4_MASK) << 18;
		codepoint |= (UC(string.str[i + 1]) & UTF_CONT_MASK) << 12;
		codepoint |= (UC(string.str[i + 2]) & UTF_CONT_MASK) << 6;
		codepoint |= (UC(string.str[i + 3]) & UTF_CONT_MASK);
	}

	return codepoint;
}

UTF::String::Iterator& UTF::String::Iterator::operator--()
{
	if (i == 0)
		throw std::out_of_range("Start of string");
	--i;
	while ((UC(string.str[i]) & UTF_CONT_CAPTURE) == UTF_CONT_HEAD)
	{
		if (i == 0)
			throw std::runtime_error("UTF-8 invalid starting byte");
		--i;
	}
	return *this;
}

UTF::String::Iterator UTF::String::Iterator::operator--(int)
{
	Iterator it(string, i);
	if (i == 0)
		throw std::out_of_range("Start of string");
	--i;
	while ((UC(string.str[i]) & UTF_CONT_CAPTURE) == UTF_CONT_HEAD)
	{
		if (i == 0)
			throw std::runtime_error("UTF-8 invalid starting byte");
		--i;
	}
	return it;
}

char UTF::String::Iterator::num_bytes() const
{
	unsigned char first = UC(string.str[i]);
	if (first < UTF_B1_CAP)
		return 1;
	else if (first < UTF_B2_CAP)
		return 2;
	else if (first < UTF_B3_CAP)
		return 3;
	else if (first < UTF_B4_CAP)
		return 4;
	else
		throw std::runtime_error("Invalid UTF-8");
}

int UTF::String::Iterator::advance()
{
	if (i >= string.str.size()) throw std::out_of_range("End of string");
	unsigned char first = UC(string.str[i]);
	char32_t codepoint = 0;

	if (first < UTF_B1_CAP)
	{
		codepoint |= first;
		i += 1;
	}
	else if (first < UTF_B2_CAP)
	{
		if (i + 1 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint |= (first & UTF_B2_MASK) << 6;
		codepoint |= (UC(string.str[i + 1]) & UTF_CONT_MASK);
		i += 2;
	}
	else if (first < UTF_B3_CAP)
	{
		if (i + 2 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint |= (first & UTF_B3_MASK) << 12;
		codepoint |= (UC(string.str[i + 1]) & UTF_CONT_MASK) << 6;
		codepoint |= (UC(string.str[i + 2]) & UTF_CONT_MASK);
		i += 3;
	}
	else if (first < UTF_B4_CAP)
	{
		if (i + 3 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint |= (first & UTF_B4_MASK) << 18;
		codepoint |= (UC(string.str[i + 1]) & UTF_CONT_MASK) << 12;
		codepoint |= (UC(string.str[i + 2]) & UTF_CONT_MASK) << 6;
		codepoint |= (UC(string.str[i + 3]) & UTF_CONT_MASK);
		i += 4;
	}
	else
		throw std::runtime_error("Invalid UTF-8");

	return codepoint;
}

void UTF::String::push_back(int codepoint)
{
	char8_t quad[4]{ 0, 0, 0, 0 };
	if (codepoint <= CODEPOINT_B1_MAX)
	{
		quad[0] = CH<8>(codepoint);
		str.append(quad, 1);
	}
	else if (codepoint <= CODEPOINT_B2_MAX)
	{
		quad[0] = CH<8>(UTF_B2_ERASURE | (codepoint >> 6));
		quad[1] = CH<8>(UTF_CONT_HEAD | (codepoint & UTF_CONT_MASK));
		str.append(quad, 2);
	}
	else if (codepoint <= CODEPOINT_B3_MAX)
	{
		quad[0] = CH<8>(UTF_B3_ERASURE | (codepoint >> 12));
		quad[1] = CH<8>(UTF_CONT_HEAD | ((codepoint	>> 6) & UTF_CONT_MASK));
		quad[2] = CH<8>(UTF_CONT_HEAD | (codepoint & UTF_CONT_MASK));
		str.append(quad, 3);
	}
	else if (codepoint <= CODEPOINT_B4_MAX)
	{
		quad[0] = CH<8>(UTF_B4_ERASURE | (codepoint >> 18));
		quad[1] = CH<8>(UTF_CONT_HEAD | ((codepoint >> 12) & UTF_CONT_MASK));
		quad[2] = CH<8>(UTF_CONT_HEAD | ((codepoint >> 6) & UTF_CONT_MASK));
		quad[3] = CH<8>(UTF_CONT_HEAD | (codepoint & UTF_CONT_MASK));
		str.append(quad, 4);
	}
	else
		throw std::out_of_range("Codepoint is out of valid UTF-8 range");
}

UTF::String UTF::String::operator+(const String& rhs) const
{
	return UTF::String(str + rhs.str);
}

UTF::String& UTF::String::operator+=(const String& rhs)
{
	str += rhs.str;
	return *this;
}

UTF::String UTF::String::operator*(size_t n) const
{
	std::u8string temp;
	for (size_t i = 0; i < n; ++i)
		temp += str;
	return String(std::move(temp));
}

UTF::String& UTF::String::operator*=(size_t n)
{
	std::u8string temp;
	for (size_t i = 0; i < n; ++i)
		temp += str;
	str.swap(temp);
	return *this;
}

UTF::String::String(const Iterator& begin_, const Iterator& end_)
{
	if (&begin_.string == &end_.string)
		str = begin_.string.str.substr(begin_.i, end_.i - begin_.i);
	else
		throw std::runtime_error("UTF::String iterators are not compatible.");
}

UTF::String UTF::String::substr(size_t begin_, size_t end_) const
{
	Iterator index = begin();
	size_t i = 0;
	while (i < begin_)
	{
		++index;
		++i;
	}
	Iterator b = index;
	while (i < end_)
	{
		++index;
		++i;
	}
	return String(b, index);
}
