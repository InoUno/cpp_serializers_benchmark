#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <variant>
#include <vector>
#include <iterator>
#include <cstddef>

namespace blitzbuffers
{
	// https://en.cppreference.com/w/cpp/utility/variant/visit
	template <class... Ts>
	struct overloaded : Ts...
	{
		using Ts::operator()...;
	};
	// explicit deduction guide (not needed as of C++20)
	template <class... Ts>
	overloaded(Ts...) -> overloaded<Ts...>;

	// https://stackoverflow.com/questions/64017982/c-equivalent-of-rust-enums
	template <typename Val, typename... Ts>
	auto match(Val val, Ts... ts)
	{
		return std::visit(overloaded{ ts... }, val);
	}

	using offset_t = uint32_t;

	template <typename T, typename U>
	T& ref(U* buf, std::size_t index)
	{
		return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(buf) + index);
	}

	template <typename T, typename U>
	const T& ref_const(const U* buf, std::size_t index)
	{
		return *reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(buf) + index);
	}

	template <class BufferBuilder>
	class BufferWriterBase
	{
	protected:
		const uint32_t __self_offset;
		BufferBuilder& __buffer_builder;

		BufferWriterBase(BufferBuilder& buffer_builder, uint32_t self_offset)
			: __buffer_builder(buffer_builder)
			, __self_offset(self_offset)
		{
		}
	};

	class FixedSizeBufferBuilder
	{
	private:
		uint8_t* builder_buffer;
		uint32_t current_size = 0;

	public:
	    FixedSizeBufferBuilder(const FixedSizeBufferBuilder&) = delete;            // No copying allowed
    	FixedSizeBufferBuilder& operator=(const FixedSizeBufferBuilder&) = delete; // No copy assignment
		FixedSizeBufferBuilder(FixedSizeBufferBuilder&&) = default;

		FixedSizeBufferBuilder(uint8_t* buffer)
			: builder_buffer(buffer)
		{
		}

		FixedSizeBufferBuilder(uint32_t max_size)
			: FixedSizeBufferBuilder(new uint8_t[max_size]{ 0 })
		{
		}

		~FixedSizeBufferBuilder()
		{
			delete[] builder_buffer;
		}

		std::pair<uint32_t, uint8_t*> add_buffer(uint32_t size)
		{
			auto offset = current_size;
			current_size += size;
			return { offset, builder_buffer + offset };
		}

		uint8_t* add_buffer_and_set_offset(uint32_t size, uint32_t buffer_offset, uint32_t& out_offset)
		{
			auto buffer = builder_buffer + current_size;
			out_offset = current_size - buffer_offset;
			current_size += size;
			return buffer;
		}

		uint8_t* add_string_and_set_offset(const char* value, uint32_t len, uint32_t buffer_offset, uint32_t& out_offset)
		{
			auto size = len + 1;
			uint8_t* buffer = builder_buffer + current_size;

			memcpy(buffer, value, len);
			buffer[size] = 0;

			out_offset = current_size - buffer_offset;
			current_size += size;
			return buffer;
		}

		uint32_t get_size()
		{
			return current_size;
		}

		void clear()
		{
			memset(builder_buffer, 0, current_size);
			current_size = 0;
		}

		std::pair<uint32_t, uint8_t*> build()
		{
			return { current_size, builder_buffer };
		}
	};

	struct BufferTracker {
		uint8_t* buffer;
		uint32_t size;
		uint32_t free;

		inline uint32_t used() {
			return size - free;
		}
	};

	class ExpandableBufferBuilder
	{
	private:
		BufferTracker current_tracker;
		std::vector<BufferTracker> previous_trackers = {};
		uint32_t current_size = 0;
		uint8_t* final_buffer = nullptr;

		const uint32_t default_buffer_size;
	public:
		ExpandableBufferBuilder(const ExpandableBufferBuilder&) = delete;            // No copying allowed
    	ExpandableBufferBuilder& operator=(const ExpandableBufferBuilder&) = delete; // No copy assignment
		ExpandableBufferBuilder(ExpandableBufferBuilder&&) = default;

		ExpandableBufferBuilder(uint32_t default_buffer_size_ = 1024)
			: default_buffer_size(default_buffer_size_)
			, current_tracker({ new uint8_t[default_buffer_size_]{ 0 }, default_buffer_size_, default_buffer_size_ })
		{
		}

		~ExpandableBufferBuilder()
		{
			for (auto& buffer_tracker : previous_trackers)
			{
				delete[] buffer_tracker.buffer;
			}
			previous_trackers.clear();

			delete[] current_tracker.buffer;
			current_tracker.buffer = nullptr;
			delete final_buffer;
		}

		std::pair<uint32_t, uint8_t*> add_buffer(uint32_t size)
		{
			ensure_capacity(size);

			uint8_t* buffer = current_tracker.buffer + current_tracker.used();
			current_tracker.free -= size;

			auto offset = current_size;
			current_size += size;

			return { offset, buffer };
		}

		uint8_t* add_buffer_and_set_offset(uint32_t size, uint32_t buffer_offset, uint32_t& out_offset)
		{
			ensure_capacity(size);

			uint8_t* buffer = current_tracker.buffer + current_tracker.used();
			current_tracker.free -= size;

			out_offset = current_size - buffer_offset;
			current_size += size;
			return buffer;
		}

		uint8_t* add_string_and_set_offset(const char* value, uint32_t len, uint32_t buffer_offset, uint32_t& out_offset)
		{
			auto size = len + 1;

			ensure_capacity(size);

			uint8_t* buffer = current_tracker.buffer + current_tracker.used();
			current_tracker.free -= size;

			memcpy(buffer, value, len);

			out_offset = current_size - buffer_offset;
			current_size += size;
			return buffer;
		}

		uint32_t get_size()
		{
			return current_size;
		}

		void ensure_capacity(uint32_t size) {
			if (current_tracker.free >= size) {
				return;
			}
			
			uint32_t size_to_use = size > default_buffer_size ? size : default_buffer_size;

			previous_trackers.push_back(current_tracker);
			current_tracker = {
				new uint8_t[size_to_use]{ 0 },
				size_to_use,
				size_to_use
			};
		}

		void clear()
		{
			delete[] final_buffer;
			final_buffer = nullptr;

			delete[] current_tracker.buffer;
			current_tracker = { new uint8_t[default_buffer_size]{ 0 }, default_buffer_size, default_buffer_size };

			for (auto& buffer_tracker : previous_trackers)
			{
				delete[] buffer_tracker.buffer;
			}
			previous_trackers.clear();

			current_size = 0;
		}

		std::pair<uint32_t, uint8_t*> build()
		{
			if (previous_trackers.size() == 0)
			{
				return { current_tracker.size, current_tracker.buffer };
			}

			delete final_buffer;
			final_buffer = new uint8_t[current_size];
			auto offset = 0;
			for (auto& buffer_tracker : previous_trackers)
			{
				memcpy(final_buffer + offset, buffer_tracker.buffer, buffer_tracker.used());
				offset += buffer_tracker.used();
			}
			memcpy(final_buffer + offset, current_tracker.buffer, current_tracker.used());
			offset += current_tracker.used();

			return { current_size, final_buffer };
		}
	};

	template <class T, class BufferBuilder>
	class BufferArrayWriter
	{
	private:
		BufferBuilder& buffer_builder;
		uint8_t* buffer;
		const uint32_t start_offset;
		const uint32_t element_size;

		BufferArrayWriter(BufferBuilder& _buffer_builder, uint8_t* _buffer, uint32_t _start_offset)
			: buffer_builder(_buffer_builder)
			, buffer(_buffer)
			, start_offset(_start_offset)
			, element_size(T::size())
		{
		}

	public:
		static BufferArrayWriter make_and_set_offset(BufferBuilder& buffer_builder, uint32_t size, uint32_t buffer_offset, uint32_t& out_offset)
		{
			auto [offset, buffer] = buffer_builder.add_buffer(sizeof(uint32_t) + T::size() * size);

			ref<uint32_t>(buffer, 0) = size;

			out_offset = offset - buffer_offset;
			return BufferArrayWriter<T, BufferBuilder>(buffer_builder, buffer + sizeof(uint32_t), offset + sizeof(uint32_t));
		}

		T operator[](int index)
		{
			auto element_offset = element_size * index;
			return T(buffer_builder, buffer + element_offset, start_offset + element_offset);
		}
	};

	template <typename T>
	class BufferArrayIter
	{
	private:
		const uint8_t* arr;
		const uint32_t& len;
		const uint32_t entry_size;
		uint32_t index = 0;

	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;

		BufferArrayIter(const uint8_t* arr_loc, uint32_t entry_size)
			: len(ref_const<uint32_t>(arr_loc, 0))
			, arr(arr_loc + sizeof(uint32_t))
			, entry_size(entry_size)
			, index(0)
		{
		}

		BufferArrayIter(const uint8_t* arr, uint32_t len, uint32_t entry_size, uint32_t index = 0)
			: arr(arr)
			, len(len)
			, entry_size(entry_size)
			, index(index)
		{
		}

		BufferArrayIter<T> operator++()
		{
			index += entry_size;
			return *this;
		}

		T operator*()
		{
			return T(arr + index);
		}

		friend bool operator==(const BufferArrayIter<T>& a, const BufferArrayIter<T>& b)
		{
			return a.arr == b.arr && a.index == b.index;
		}

		friend bool operator!=(const BufferArrayIter<T>& a, const BufferArrayIter<T>& b)
		{
			return a.arr != b.arr || a.index != b.index;
		}
	};

	template <typename T>
	class BufferArray
	{
	private:
		const uint8_t* arr;
		const uint32_t& len;
		const uint32_t entry_size;

	public:
		BufferArray(const uint8_t* arr_loc, uint32_t entry_size)
			: len(ref_const<uint32_t>(arr_loc, 0))
			, arr(arr_loc + sizeof(uint32_t))
			, entry_size(entry_size)
		{
		}

		BufferArrayIter<T> begin() const
		{
			return BufferArrayIter<T>(arr, len, entry_size);
		}

		BufferArrayIter<T> end() const
		{
			return BufferArrayIter<T>(arr, len, entry_size, len * entry_size);
		}

		uint32_t size() const
		{
			return len;
		}

		T operator[](int index) const
		{
			return T(arr + index * entry_size);
		}
	};

	template <class T>
	class PrimitiveBufferArrayWriter
	{
	private:
		uint8_t* buffer;
		const uint32_t element_size;

		PrimitiveBufferArrayWriter(uint8_t* _buffer)
			: buffer(_buffer)
			, element_size(sizeof(T))
		{
		}

	public:
		template <class BufferBuilder>
		static PrimitiveBufferArrayWriter make_and_set_offset(BufferBuilder& buffer_builder, uint32_t size, uint32_t buffer_offset, uint32_t& out_offset)
		{
			auto [offset, buffer] = buffer_builder.add_buffer(sizeof(uint32_t) + sizeof(T) * size);

			ref<offset_t>(buffer, 0) = size;

			out_offset = offset - buffer_offset;
			return PrimitiveBufferArrayWriter<T>(buffer + sizeof(offset_t));
		}

		T& operator[](int index) const
		{
			return ref<T>(buffer, element_size * index);
		}
	};

	template <typename T>
	class PrimitiveBufferArrayIter
	{
	private:
		const uint8_t* arr;
		const uint32_t& len;
		const uint32_t entry_size;
		uint32_t index = 0;

	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;

		PrimitiveBufferArrayIter(const uint8_t* arr_loc, uint32_t entry_size)
			: len(ref_const<offset_t>(arr_loc, 0))
			, arr(arr_loc + sizeof(offset_t))
			, entry_size(entry_size)
			, index(0)
		{
		}

		PrimitiveBufferArrayIter(const uint8_t* arr, uint32_t len, uint32_t entry_size, uint32_t index = 0)
			: arr(arr)
			, len(len)
			, entry_size(entry_size)
			, index(index)
		{
		}

		PrimitiveBufferArrayIter operator++()
		{
			index += entry_size;
			return *this;
		}

		const T& operator*() const
		{
			return ref_const<T>(arr, index * entry_size);
		}

		bool operator!=(const PrimitiveBufferArrayIter& i)
		{
			return arr != i.arr || index != i.index;
		}
	};

	template <typename T>
	class PrimitiveBufferArray
	{
	private:
		const uint8_t* arr;
		const uint32_t& len;
		const uint32_t entry_size;

	public:
		PrimitiveBufferArray(const uint8_t* arr_loc)
			: len(ref_const<uint32_t>(arr_loc, 0))
			, arr(arr_loc + sizeof(uint32_t))
			, entry_size(sizeof(T))
		{
		}

		PrimitiveBufferArrayIter<T> begin() const
		{
			return PrimitiveBufferArrayIter<T>(arr, len, entry_size);
		}

		PrimitiveBufferArrayIter<T> end() const
		{
			return PrimitiveBufferArrayIter<T>(arr, len, entry_size, len * entry_size);
		}

		inline uint32_t size() const
		{
			return len;
		}

		const T& operator[](int index) const
		{
			return ref_const<T>(arr, index * entry_size);
		}
	};

}
