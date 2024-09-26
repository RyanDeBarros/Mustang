#pragma once

#include <string>
#include <vector>
#include <variant>

namespace UTF
{
	extern std::string encode(const std::u16string& utf16, bool ignore_invalid_chars = false);
	extern std::u16string decode_utf16(const std::string& utf8);
	extern std::string encode(const std::u32string& utf32, bool ignore_invalid_chars = false);
	extern std::u32string decode_utf32(const std::string& utf8);

	class String
	{
		friend class Iterator;
		std::string str;

	public:
		String(const std::string& str) : str(str) {}
		String(std::string&& str) : str(std::move(str)) {}
		String(const std::u16string& str) : str(UTF::encode(str)) {}
		String(const std::u32string& str) : str(UTF::encode(str)) {}
		String(const char* str) : str(str) {}
		String(const char16_t* str) : str(UTF::encode(str)) {}
		String(const char32_t* str) : str(UTF::encode(str)) {}
		String() = default;

		class Iterator
		{
			const String& string;
			size_t i;

			friend class UTF::String;
			Iterator(const String& string, size_t i) : string(string), i(i) {}

		public:
			int codepoint() const;
			Iterator& operator++() { i += num_bytes(); return *this; }
			Iterator& operator++(int) const { Iterator iter(string, i + num_bytes()); return iter; }
			bool operator==(const Iterator& other) const { return string.str == other.string.str && i == other.i; }
			bool operator!=(const Iterator& other) const { return string.str != other.string.str || i != other.i; }
			char num_bytes() const;
			operator bool() const { return i < string.str.size(); }
			int advance();
		};

		Iterator begin() const { return Iterator(*this, 0); }
		Iterator end() const { return Iterator(*this, str.size()); }
		size_t size() const { return str.size(); }

		void push_back(int codepoint);

		// TODO Bi-Iterator? for going backwards keep reference to the previous Bi-Iterator. Also const versions

		// TODO other string operations that propogate to underlying str member.
	};
}
