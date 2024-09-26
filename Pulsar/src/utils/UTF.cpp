#include "UTF.h"

#include <stdexcept>

std::string UTF::encode(const std::u16string& utf16, bool ignore_invalid_chars)
{
	std::string utf8;

	for (size_t i = 0; i < utf16.size(); ++i) {
		char16_t ch = utf16[i];

		if (ch <= 0x7F) {
			// 1-byte (ASCII)
			utf8.push_back(static_cast<char>(ch));
		}
		else if (ch <= 0x7FF) {
			// 2-byte
			utf8.push_back(static_cast<char>((ch >> 6) | 0xC0));
			utf8.push_back(static_cast<char>((ch & 0x3F) | 0x80));
		}
		else if (ch >= 0xD800 && ch <= 0xDBFF) {
			// Surrogate pair (4-byte UTF-8)
			if (i + 1 >= utf16.size())
			{
				if (!ignore_invalid_chars)
					throw std::runtime_error("Invalid UTF-16 sequence");
				continue;
			}
			char16_t low_surrogate = utf16[++i];

			if (low_surrogate < 0xDC00 || low_surrogate > 0xDFFF) {
				if (!ignore_invalid_chars)
					throw std::runtime_error("Invalid UTF-16 surrogate pair");
				continue;
			}

			char32_t codepoint = ((ch - 0xD800) << 10) + (low_surrogate - 0xDC00) + 0x10000;
			utf8.push_back(static_cast<char>((codepoint >> 18) | 0xF0));
			utf8.push_back(static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80));
			utf8.push_back(static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80));
			utf8.push_back(static_cast<char>((codepoint & 0x3F) | 0x80));
		}
		else {
			// 3-byte
			utf8.push_back(static_cast<char>((ch >> 12) | 0xE0));
			utf8.push_back(static_cast<char>(((ch >> 6) & 0x3F) | 0x80));
			utf8.push_back(static_cast<char>((ch & 0x3F) | 0x80));
		}
	}

	return utf8;
}

std::u16string UTF::decode_utf16(const std::string& utf8)
{
	std::u16string utf16;

	for (size_t i = 0; i < utf8.size();) {
		unsigned char byte = static_cast<unsigned char>(utf8[i]);

		if ((byte & 0x80) == 0) {
			// 1-byte (ASCII)
			utf16.push_back(static_cast<char16_t>(byte));
			i += 1;
		}
		else if ((byte & 0xE0) == 0xC0) {
			// 2-byte
			if (i + 1 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			char32_t codepoint = ((byte & 0x1F) << 6) | (static_cast<unsigned char>(utf8[i + 1]) & 0x3F);
			utf16.push_back(static_cast<char16_t>(codepoint));
			i += 2;
		}
		else if ((byte & 0xF0) == 0xE0) {
			// 3-byte
			if (i + 2 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			char32_t codepoint = ((byte & 0x0F) << 12) |
				((static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 6) |
				(static_cast<unsigned char>(utf8[i + 2]) & 0x3F);
			utf16.push_back(static_cast<char16_t>(codepoint));
			i += 3;
		}
		else if ((byte & 0xF8) == 0xF0) {
			// 4-byte (surrogate pair)
			if (i + 3 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			char32_t codepoint = ((byte & 0x07) << 18) |
				((static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 12) |
				((static_cast<unsigned char>(utf8[i + 2]) & 0x3F) << 6) |
				(static_cast<unsigned char>(utf8[i + 3]) & 0x3F);

			// Encode as UTF-16 surrogate pair
			codepoint -= 0x10000;
			char16_t high_surrogate = static_cast<char16_t>((codepoint >> 10) + 0xD800);
			char16_t low_surrogate = static_cast<char16_t>((codepoint & 0x3FF) + 0xDC00);
			utf16.push_back(high_surrogate);
			utf16.push_back(low_surrogate);
			i += 4;
		}
		else {
			throw std::runtime_error("Invalid UTF-8 sequence");
		}
	}

	return utf16;
}

std::string UTF::encode(const std::u32string& utf32, bool ignore_invalid_chars)
{
	std::string utf8;

	for (char32_t codepoint : utf32)
	{
		if (codepoint <= 0x7F)
		{
			// 1-byte (ASCII)
			utf8.push_back(static_cast<char>(codepoint));
		}
		else if (codepoint <= 0x7FF)
		{
			// 2-byte
			utf8.push_back(static_cast<char>((codepoint >> 6) | 0xC0));
			utf8.push_back(static_cast<char>((codepoint & 0x3F) | 0x80));
		}
		else if (codepoint <= 0xFFFF)
		{
			// 3-byte
			utf8.push_back(static_cast<char>((codepoint >> 12) | 0xE0));
			utf8.push_back(static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80));
			utf8.push_back(static_cast<char>((codepoint & 0x3F) | 0x80));
		}
		else if (codepoint <= 0x10FFFF)
		{
			// 4-byte
			utf8.push_back(static_cast<char>((codepoint >> 18) | 0xF0));
			utf8.push_back(static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80));
			utf8.push_back(static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80));
			utf8.push_back(static_cast<char>((codepoint & 0x3F) | 0x80));
		}
		else
		{
			if (!ignore_invalid_chars)
				throw std::runtime_error("Invalid Unicode codepoint");
		}
	}

	return utf8;
}

std::u32string UTF::decode_utf32(const std::string& utf8)
{
	std::u32string utf32;

	size_t i = 0;
	while (i < utf8.size()) {
		unsigned char byte = static_cast<unsigned char>(utf8[i]);

		if ((byte & 0x80) == 0) {
			// 1-byte (ASCII)
			utf32.push_back(static_cast<char32_t>(byte));
			i += 1;
		}
		else if ((byte & 0xE0) == 0xC0) {
			// 2-byte
			if (i + 1 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			char32_t codepoint = ((byte & 0x1F) << 6) | (static_cast<unsigned char>(utf8[i + 1]) & 0x3F);
			utf32.push_back(codepoint);
			i += 2;
		}
		else if ((byte & 0xF0) == 0xE0) {
			// 3-byte character
			if (i + 2 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			char32_t codepoint = ((byte & 0x0F) << 12) |
				((static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 6) |
				(static_cast<unsigned char>(utf8[i + 2]) & 0x3F);
			utf32.push_back(codepoint);
			i += 3;
		}
		else if ((byte & 0xF8) == 0xF0) {
			// 4-byte character
			if (i + 3 >= utf8.size()) throw std::runtime_error("Invalid UTF-8 sequence");
			char32_t codepoint = ((byte & 0x07) << 18) |
				((static_cast<unsigned char>(utf8[i + 1]) & 0x3F) << 12) |
				((static_cast<unsigned char>(utf8[i + 2]) & 0x3F) << 6) |
				(static_cast<unsigned char>(utf8[i + 3]) & 0x3F);
			utf32.push_back(codepoint);
			i += 4;
		}
		else {
			throw std::runtime_error("Invalid UTF-8 sequence");
		}
	}

	return utf32;
}

int UTF::String::Iterator::codepoint() const
{
	if (i >= string.str.size()) throw std::out_of_range("End of string");
	unsigned char first = static_cast<unsigned char>(string.str[i]);
	char32_t codepoint = 0;

	if (first < 0x80)
	{
		codepoint = first;
	}
	else if (first < 0xE0)
	{
		if (i + 1 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint = (first & 0x1F) << 6;
		codepoint |= (static_cast<unsigned char>(string.str[i + 1]) & 0x3F);
	}
	else if (first < 0xF0)
	{
		if (i + 2 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint = (first & 0x0F) << 12;
		codepoint |= (static_cast<unsigned char>(string.str[i + 1]) & 0x3F) << 6;
		codepoint |= (static_cast<unsigned char>(string.str[i + 2]) & 0x3F);
	}
	else
	{
		if (i + 3 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint = (first & 0x07) << 18;
		codepoint |= (static_cast<unsigned char>(string.str[i + 1]) & 0x3F) << 12;
		codepoint |= (static_cast<unsigned char>(string.str[i + 2]) & 0x3F) << 6;
		codepoint |= (static_cast<unsigned char>(string.str[i + 3]) & 0x3F);
	}

	return codepoint;
}

char UTF::String::Iterator::num_bytes() const
{
	unsigned char first = static_cast<unsigned char>(string.str[i]);
	if (first < 0x80)
		return 1;
	else if (first < 0xE0)
		return 2;
	else if (first < 0xF0)
		return 3;
	else
		return 4;
}

int UTF::String::Iterator::advance()
{
	if (i >= string.str.size()) throw std::out_of_range("End of string");
	unsigned char first = static_cast<unsigned char>(string.str[i]);
	char32_t codepoint = 0;

	if (first < 0x80)
	{
		codepoint = first;
		i += 1;
	}
	else if (first < 0xE0)
	{
		if (i + 1 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint = (first & 0x1F) << 6;
		codepoint |= (static_cast<unsigned char>(string.str[i + 1]) & 0x3F);
		i += 2;
	}
	else if (first < 0xF0)
	{
		if (i + 2 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint = (first & 0x0F) << 12;
		codepoint |= (static_cast<unsigned char>(string.str[i + 1]) & 0x3F) << 6;
		codepoint |= (static_cast<unsigned char>(string.str[i + 2]) & 0x3F);
		i += 3;
	}
	else
	{
		if (i + 3 >= string.str.size()) throw std::out_of_range("Invalid UTF-8");
		codepoint = (first & 0x07) << 18;
		codepoint |= (static_cast<unsigned char>(string.str[i + 1]) & 0x3F) << 12;
		codepoint |= (static_cast<unsigned char>(string.str[i + 2]) & 0x3F) << 6;
		codepoint |= (static_cast<unsigned char>(string.str[i + 3]) & 0x3F);
		i += 4;
	}

	return codepoint;
}

void UTF::String::push_back(int codepoint)
{
	if (codepoint <= 0x7F)
	{
		str.push_back(static_cast<char>(codepoint));
	}
	else if (codepoint <= 0x7FF)
	{
		str.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
		str.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
	}
	else if (codepoint <= 0xFFFF)
	{
		str.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
		str.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
		str.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
	}
	else if (codepoint <= 0x10FFFF)
	{
		str.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
		str.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
		str.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
		str.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
	}
	else
	{
		throw std::out_of_range("Codepoint is out of valid UTF-8 range");
	}
}
