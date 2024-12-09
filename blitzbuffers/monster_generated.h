#pragma once

#include "blitzbuffers.h"

using blitzbuffers::BufferArray;
using blitzbuffers::BufferArrayIter;
using blitzbuffers::BufferArrayWriter;
using blitzbuffers::BufferWriterBase;
using blitzbuffers::offset_t;
using blitzbuffers::PrimitiveBufferArray;
using blitzbuffers::PrimitiveBufferArrayIter;
using blitzbuffers::PrimitiveBufferArrayWriter;
using blitzbuffers::ref;
using blitzbuffers::ref_const;

// Forward declarations
namespace bench
{
    enum class Color : uint8_t;
}

namespace bench::Vec3::_internal
{
    class _Raw;
    class _Viewer;

    template <class>
    class _Builder;
}

namespace bench::Weapon::_internal
{
    class _Raw;
    class _Viewer;

    template <class>
    class _Builder;
}

namespace bench::Monster::_internal
{
    class _Raw;
    class _Viewer;

    template <class>
    class _Builder;
}

namespace bench::MonstersList::_internal
{
    class _Raw;
    class _Viewer;

    template <class>
    class _Builder;
}


// Declarations
namespace bench
{
enum class Color : uint8_t
{
    None = 0,
    Red = 1,
    Green = 2,
    Blue = 3,
};
}

namespace bench::Vec3
{
    constexpr static uint32_t size()
    {
        return 12;
    }

    namespace _internal {
        class _Raw {
        public:
            float x;
            float y;
            float z;
        };

        class _Viewer {
        private:
            const uint8_t* __buffer;

        public:
            const float& x;
            const float& y;
            const float& z;

            _Viewer(const uint8_t* buffer);


        };

        template <class BufferBuilder>
        class _Builder : BufferWriterBase<BufferBuilder> {
        private:

        public:
            constexpr static uint32_t size()
            {
                return 12;
            }

            float& x;
            float& y;
            float& z;

            _Builder<BufferBuilder>& operator=(_Raw _raw);

            _Builder(BufferBuilder& _buffer_builder, uint8_t* _buffer, uint32_t _self_offset);

        };

    }

    template <class BufferBuilder>
    static _internal::_Builder<BufferBuilder> new_on(BufferBuilder& buffer_builder)
    {
        auto [offset, buffer] = buffer_builder.add_buffer(::bench::Vec3::size());
        return _internal::_Builder(buffer_builder, buffer, offset);
    }

    static _internal::_Viewer view(const uint8_t* buffer)
    {
        return _internal::_Viewer(buffer);
    }

    static ::bench::Vec3::_internal::_Raw raw()
    {
        return ::bench::Vec3::_internal::_Raw {};
    }

    static ::bench::Vec3::_internal::_Raw raw(::bench::Vec3::_internal::_Raw _raw)
    {
        return _raw;
    }
}

namespace bench::Weapon
{
    constexpr static uint32_t size()
    {
        return 6;
    }

    namespace _internal {
        class _Raw {
        public:
            const char* name;
            int16_t damage;
        };

        class _Viewer {
        private:
            const uint8_t* __buffer;
            const offset_t& __name_offset;

        public:
            const int16_t& damage;

            _Viewer(const uint8_t* buffer);

            
            const char* name() const
            {
                if (__name_offset == 0)
                {
                    return "";
                }
                return (const char*)(__buffer + __name_offset);
            }


        };

        template <class BufferBuilder>
        class _Builder : BufferWriterBase<BufferBuilder> {
        private:
            offset_t& __name_offset;

        public:
            constexpr static uint32_t size()
            {
                return 6;
            }

            int16_t& damage;

            _Builder<BufferBuilder>& operator=(_Raw _raw);

            _Builder(BufferBuilder& _buffer_builder, uint8_t* _buffer, uint32_t _self_offset);

            
            void insert_name(const std::string& value)
            {
                this->__buffer_builder.add_string_and_set_offset(value.c_str(), value.size(), this->__self_offset, this->__name_offset);
            }

            void insert_name(const char* value)
            {
                this->__buffer_builder.add_string_and_set_offset(value, strlen(value), this->__self_offset, this->__name_offset);
            }

        };

    }

    template <class BufferBuilder>
    static _internal::_Builder<BufferBuilder> new_on(BufferBuilder& buffer_builder)
    {
        auto [offset, buffer] = buffer_builder.add_buffer(::bench::Weapon::size());
        return _internal::_Builder(buffer_builder, buffer, offset);
    }

    static _internal::_Viewer view(const uint8_t* buffer)
    {
        return _internal::_Viewer(buffer);
    }

    static ::bench::Weapon::_internal::_Raw raw()
    {
        return ::bench::Weapon::_internal::_Raw {};
    }

    static ::bench::Weapon::_internal::_Raw raw(::bench::Weapon::_internal::_Raw _raw)
    {
        return _raw;
    }
}

namespace bench::Monster
{
    constexpr static uint32_t size()
    {
        return 39;
    }

    namespace _internal {
        class _Raw {
        public:
            ::bench::Vec3::_internal::_Raw pos;
            int16_t mana;
            int16_t hp;
            const char* name;
            std::vector<uint8_t> inventory;
            Color color;
            std::vector<::bench::Weapon::_internal::_Raw> weapons;
            ::bench::Weapon::_internal::_Raw equipped;
            std::vector<::bench::Vec3::_internal::_Raw> path;
        };

        class _Viewer {
        private:
            const uint8_t* __buffer;
            const offset_t& __name_offset;
            const offset_t& __inventory_offset;
            const offset_t& __weapons_offset;
            const offset_t& __path_offset;

        public:
            const ::bench::Vec3::_internal::_Viewer pos;
            const int16_t& mana;
            const int16_t& hp;
            const bench::Color& color;
            const ::bench::Weapon::_internal::_Viewer equipped;

            _Viewer(const uint8_t* buffer);

            
            const char* name() const
            {
                if (__name_offset == 0)
                {
                    return "";
                }
                return (const char*)(__buffer + __name_offset);
            }

            
            PrimitiveBufferArray<uint8_t> inventory() const
            {
                return PrimitiveBufferArray<uint8_t>(__buffer + __inventory_offset);
            }

            
            BufferArray<::bench::Weapon::_internal::_Viewer> weapons() const
            {
                return BufferArray<::bench::Weapon::_internal::_Viewer>(__buffer + __weapons_offset, 6);
            }

            
            BufferArray<::bench::Vec3::_internal::_Viewer> path() const
            {
                return BufferArray<::bench::Vec3::_internal::_Viewer>(__buffer + __path_offset, 12);
            }


        };

        template <class BufferBuilder>
        class _Builder : BufferWriterBase<BufferBuilder> {
        private:
            offset_t& __name_offset;
            offset_t& __inventory_offset;
            offset_t& __weapons_offset;
            offset_t& __path_offset;

        public:
            constexpr static uint32_t size()
            {
                return 39;
            }

            ::bench::Vec3::_internal::_Builder<BufferBuilder> pos;
            int16_t& mana;
            int16_t& hp;
            bench::Color& color;
            ::bench::Weapon::_internal::_Builder<BufferBuilder> equipped;

            _Builder<BufferBuilder>& operator=(_Raw _raw);

            _Builder(BufferBuilder& _buffer_builder, uint8_t* _buffer, uint32_t _self_offset);

            
            void insert_name(const std::string& value)
            {
                this->__buffer_builder.add_string_and_set_offset(value.c_str(), value.size(), this->__self_offset, this->__name_offset);
            }

            void insert_name(const char* value)
            {
                this->__buffer_builder.add_string_and_set_offset(value, strlen(value), this->__self_offset, this->__name_offset);
            }

            
            PrimitiveBufferArrayWriter<uint8_t> insert_inventory(offset_t amount)
            {
                return PrimitiveBufferArrayWriter<uint8_t>::make_and_set_offset(this->__buffer_builder, amount, this->__self_offset, this->__inventory_offset);
            }

            
            BufferArrayWriter<::bench::Weapon::_internal::_Builder<BufferBuilder>, BufferBuilder> insert_weapons(offset_t _size)
            {
                return BufferArrayWriter<::bench::Weapon::_internal::_Builder<BufferBuilder>, BufferBuilder>::make_and_set_offset(this->__buffer_builder, _size, this->__self_offset, this->__weapons_offset);
            }

            void insert_weapons(const std::vector<::bench::Weapon::_internal::_Raw>& _raw_vec)
            {
                auto array_writer = BufferArrayWriter<::bench::Weapon::_internal::_Builder<BufferBuilder>, BufferBuilder>::make_and_set_offset(this->__buffer_builder, _raw_vec.size(), this->__self_offset, this->__weapons_offset);
                for (size_t i = 0; i < _raw_vec.size(); i++)
                {
                    array_writer[i] = _raw_vec[i];
                }
            }

            
            BufferArrayWriter<::bench::Vec3::_internal::_Builder<BufferBuilder>, BufferBuilder> insert_path(offset_t _size)
            {
                return BufferArrayWriter<::bench::Vec3::_internal::_Builder<BufferBuilder>, BufferBuilder>::make_and_set_offset(this->__buffer_builder, _size, this->__self_offset, this->__path_offset);
            }

            void insert_path(const std::vector<::bench::Vec3::_internal::_Raw>& _raw_vec)
            {
                auto array_writer = BufferArrayWriter<::bench::Vec3::_internal::_Builder<BufferBuilder>, BufferBuilder>::make_and_set_offset(this->__buffer_builder, _raw_vec.size(), this->__self_offset, this->__path_offset);
                for (size_t i = 0; i < _raw_vec.size(); i++)
                {
                    array_writer[i] = _raw_vec[i];
                }
            }

        };

    }

    template <class BufferBuilder>
    static _internal::_Builder<BufferBuilder> new_on(BufferBuilder& buffer_builder)
    {
        auto [offset, buffer] = buffer_builder.add_buffer(::bench::Monster::size());
        return _internal::_Builder(buffer_builder, buffer, offset);
    }

    static _internal::_Viewer view(const uint8_t* buffer)
    {
        return _internal::_Viewer(buffer);
    }

    static ::bench::Monster::_internal::_Raw raw()
    {
        return ::bench::Monster::_internal::_Raw {};
    }

    static ::bench::Monster::_internal::_Raw raw(::bench::Monster::_internal::_Raw _raw)
    {
        return _raw;
    }
}

namespace bench::MonstersList
{
    constexpr static uint32_t size()
    {
        return 4;
    }

    namespace _internal {
        class _Raw {
        public:
            std::vector<::bench::Monster::_internal::_Raw> data;
        };

        class _Viewer {
        private:
            const uint8_t* __buffer;
            const offset_t& __data_offset;

        public:

            _Viewer(const uint8_t* buffer);

            
            BufferArray<::bench::Monster::_internal::_Viewer> data() const
            {
                return BufferArray<::bench::Monster::_internal::_Viewer>(__buffer + __data_offset, 39);
            }


        };

        template <class BufferBuilder>
        class _Builder : BufferWriterBase<BufferBuilder> {
        private:
            offset_t& __data_offset;

        public:
            constexpr static uint32_t size()
            {
                return 4;
            }


            _Builder<BufferBuilder>& operator=(_Raw _raw);

            _Builder(BufferBuilder& _buffer_builder, uint8_t* _buffer, uint32_t _self_offset);

            
            BufferArrayWriter<::bench::Monster::_internal::_Builder<BufferBuilder>, BufferBuilder> insert_data(offset_t _size)
            {
                return BufferArrayWriter<::bench::Monster::_internal::_Builder<BufferBuilder>, BufferBuilder>::make_and_set_offset(this->__buffer_builder, _size, this->__self_offset, this->__data_offset);
            }

            void insert_data(const std::vector<::bench::Monster::_internal::_Raw>& _raw_vec)
            {
                auto array_writer = BufferArrayWriter<::bench::Monster::_internal::_Builder<BufferBuilder>, BufferBuilder>::make_and_set_offset(this->__buffer_builder, _raw_vec.size(), this->__self_offset, this->__data_offset);
                for (size_t i = 0; i < _raw_vec.size(); i++)
                {
                    array_writer[i] = _raw_vec[i];
                }
            }

        };

    }

    template <class BufferBuilder>
    static _internal::_Builder<BufferBuilder> new_on(BufferBuilder& buffer_builder)
    {
        auto [offset, buffer] = buffer_builder.add_buffer(::bench::MonstersList::size());
        return _internal::_Builder(buffer_builder, buffer, offset);
    }

    static _internal::_Viewer view(const uint8_t* buffer)
    {
        return _internal::_Viewer(buffer);
    }

    static ::bench::MonstersList::_internal::_Raw raw()
    {
        return ::bench::MonstersList::_internal::_Raw {};
    }

    static ::bench::MonstersList::_internal::_Raw raw(::bench::MonstersList::_internal::_Raw _raw)
    {
        return _raw;
    }
}



// Definitions

namespace bench::Vec3::_internal
{
    inline _Viewer::_Viewer(const uint8_t* _buffer)
    : __buffer(_buffer)
    , x(ref_const<float>(_buffer, 0))
    , y(ref_const<float>(_buffer, 4))
    , z(ref_const<float>(_buffer, 8))
    {
    }

    template <class BufferBuilder>
    inline _Builder<BufferBuilder>::_Builder(BufferBuilder& _buffer_builder, uint8_t* _buffer, uint32_t _self_offset)
    : BufferWriterBase<BufferBuilder>(_buffer_builder, _self_offset)
    , x(ref<float>(_buffer, 0))
    , y(ref<float>(_buffer, 4))
    , z(ref<float>(_buffer, 8))
    {
    }

    template <class BufferBuilder>
    inline _Builder<BufferBuilder>& _Builder<BufferBuilder>::operator=(_Raw _raw)
    {
        this->x = _raw.x;
        this->y = _raw.y;
        this->z = _raw.z;
        return *this;
    }

}

namespace bench::Weapon::_internal
{
    inline _Viewer::_Viewer(const uint8_t* _buffer)
    : __buffer(_buffer)
    , __name_offset(ref_const<offset_t>(_buffer, 0))
    , damage(ref_const<int16_t>(_buffer, 4))
    {
    }

    template <class BufferBuilder>
    inline _Builder<BufferBuilder>::_Builder(BufferBuilder& _buffer_builder, uint8_t* _buffer, uint32_t _self_offset)
    : BufferWriterBase<BufferBuilder>(_buffer_builder, _self_offset)
    , __name_offset(ref<offset_t>(_buffer, 0))
    , damage(ref<int16_t>(_buffer, 4))
    {
    }

    template <class BufferBuilder>
    inline _Builder<BufferBuilder>& _Builder<BufferBuilder>::operator=(_Raw _raw)
    {
        this->insert_name(_raw.name);
        this->damage = _raw.damage;
        return *this;
    }

}

namespace bench::Monster::_internal
{
    inline _Viewer::_Viewer(const uint8_t* _buffer)
    : __buffer(_buffer)
    , pos(::bench::Vec3::_internal::_Viewer(_buffer + 0))
    , mana(ref_const<int16_t>(_buffer, 12))
    , hp(ref_const<int16_t>(_buffer, 14))
    , __name_offset(ref_const<offset_t>(_buffer, 16))
    , __inventory_offset(ref_const<offset_t>(_buffer, 20))
    , color(ref_const<bench::Color>(_buffer, 24))
    , __weapons_offset(ref_const<offset_t>(_buffer, 25))
    , equipped(::bench::Weapon::_internal::_Viewer(_buffer + 29))
    , __path_offset(ref_const<offset_t>(_buffer, 35))
    {
    }

    template <class BufferBuilder>
    inline _Builder<BufferBuilder>::_Builder(BufferBuilder& _buffer_builder, uint8_t* _buffer, uint32_t _self_offset)
    : BufferWriterBase<BufferBuilder>(_buffer_builder, _self_offset)
    , pos(::bench::Vec3::_internal::_Builder<BufferBuilder>(_buffer_builder, _buffer + 0, _self_offset + 0))
    , mana(ref<int16_t>(_buffer, 12))
    , hp(ref<int16_t>(_buffer, 14))
    , __name_offset(ref<offset_t>(_buffer, 16))
    , __inventory_offset(ref<offset_t>(_buffer, 20))
    , color(ref<bench::Color>(_buffer, 24))
    , __weapons_offset(ref<offset_t>(_buffer, 25))
    , equipped(::bench::Weapon::_internal::_Builder<BufferBuilder>(_buffer_builder, _buffer + 29, _self_offset + 29))
    , __path_offset(ref<offset_t>(_buffer, 35))
    {
    }

    template <class BufferBuilder>
    inline _Builder<BufferBuilder>& _Builder<BufferBuilder>::operator=(_Raw _raw)
    {
        this->pos = _raw.pos;
        this->mana = _raw.mana;
        this->hp = _raw.hp;
        this->insert_name(_raw.name);
        this->insert_inventory(_raw.inventory);
        this->color = _raw.color;
        this->insert_weapons(_raw.weapons);
        this->equipped = _raw.equipped;
        this->insert_path(_raw.path);
        return *this;
    }

}

namespace bench::MonstersList::_internal
{
    inline _Viewer::_Viewer(const uint8_t* _buffer)
    : __buffer(_buffer)
    , __data_offset(ref_const<offset_t>(_buffer, 0))
    {
    }

    template <class BufferBuilder>
    inline _Builder<BufferBuilder>::_Builder(BufferBuilder& _buffer_builder, uint8_t* _buffer, uint32_t _self_offset)
    : BufferWriterBase<BufferBuilder>(_buffer_builder, _self_offset)
    , __data_offset(ref<offset_t>(_buffer, 0))
    {
    }

    template <class BufferBuilder>
    inline _Builder<BufferBuilder>& _Builder<BufferBuilder>::operator=(_Raw _raw)
    {
        this->insert_data(_raw.data);
        return *this;
    }

}

