/*
 * libnbt++ - A library for the Minecraft Named Binary Tag format.
 * Copyright (C) 2013, 2015  ljfa-ag
 *
 * This file is part of libnbt++.
 *
 * libnbt++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libnbt++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libnbt++.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TAG_ARRAY_H_INCLUDED
#define TAG_ARRAY_H_INCLUDED

#include "crtp_tag.h"
#include "io/stream_reader.h"
#include "io/stream_writer.h"
#include <type_traits>
#include <vector>
#include <istream>

namespace nbt
{

///@cond
namespace detail
{
    ///Meta-struct that holds the tag_type value for a specific array type
    template<class T> struct get_array_type
    { static_assert(sizeof(T) != sizeof(T), "Invalid type paramter for tag_array, can only use byte or int"); };

    template<> struct get_array_type<int8_t>  : public std::integral_constant<tag_type, tag_type::Byte_Array> {};
    template<> struct get_array_type<int32_t> : public std::integral_constant<tag_type, tag_type::Int_Array> {};
    template<> struct get_array_type<int64_t> : public std::integral_constant<tag_type, tag_type::Long_Array> {};
}
///@cond

/**
 * @brief Tag that contains an array of byte or int values
 *
 * Common class for tag_byte_array, tag_int_array and tag_long_array.
 */
template<class T>
class tag_array final : public detail::crtp_tag<tag_array<T>>
{
public:
    //Iterator types
    typedef typename std::vector<T>::iterator iterator;
    typedef typename std::vector<T>::const_iterator const_iterator;

    ///The type of the contained values
    typedef T value_type;

    ///The type of the tag
    static constexpr tag_type type = detail::get_array_type<T>::value;

    ///Constructs an empty array
    tag_array() {}

    ///Constructs an array with the given values
    tag_array(std::initializer_list<T> init): data(init) {}
    tag_array(std::vector<T>&& vec) noexcept: data(std::move(vec)) {}

    ///Returns a reference to the vector that contains the values
    std::vector<T>& get() { return data; }
    const std::vector<T>& get() const { return data; }

    /**
     * @brief Accesses a value by index with bounds checking
     * @throw std::out_of_range if the index is out of range
     */
    T& at(size_t i) { return data.at(i); }
    T at(size_t i) const { return data.at(i); }

    /**
     * @brief Accesses a value by index
     *
     * No bounds checking is performed.
     */
    T& operator[](size_t i) { return data[i]; }
    T operator[](size_t i) const { return data[i]; }

    ///Appends a value at the end of the array
    void push_back(T val) { data.push_back(val); }

    ///Removes the last element from the array
    void pop_back() { data.pop_back(); }

    ///Returns the number of values in the array
    size_t size() const { return data.size(); }

    ///Erases all values from the array.
    void clear() { data.clear(); }

    //Iterators
    iterator begin() { return data.begin(); }
    iterator end()   { return data.end(); }
    const_iterator begin() const  { return data.begin(); }
    const_iterator end() const    { return data.end(); }
    const_iterator cbegin() const { return data.cbegin(); }
    const_iterator cend() const   { return data.cend(); }

    void read_payload(io::stream_reader& reader) override;
    /**
     * @inheritdoc
     * @throw std::length_error if the array is too large for NBT
     */
    void write_payload(io::stream_writer& writer) const override;

private:
    std::vector<T> data;
};

template<class T> bool operator==(const tag_array<T>& lhs, const tag_array<T>& rhs)
{ return lhs.get() == rhs.get(); }
template<class T> bool operator!=(const tag_array<T>& lhs, const tag_array<T>& rhs)
{ return !(lhs == rhs); }

//Slightly different between byte_array and int_array
//Reading
template<>
inline void tag_array<int8_t>::read_payload(io::stream_reader& reader)
{
    int32_t length;
    reader.read_num(length);
    if(length < 0)
        reader.get_istr().setstate(std::ios::failbit);
    if(!reader.get_istr())
        throw io::input_error("Error reading length of tag_byte_array");

    data.resize(length);
    reader.get_istr().read(reinterpret_cast<char*>(data.data()), length);
    if(!reader.get_istr())
        throw io::input_error("Error reading contents of tag_byte_array");
}

template<typename T>
inline void tag_array<T>::read_payload(io::stream_reader& reader)
{
    int32_t length;
    reader.read_num(length);
    if(length < 0)
        reader.get_istr().setstate(std::ios::failbit);
    if(!reader.get_istr())
        throw io::input_error("Error reading length of generic array tag");

    data.clear();
    data.reserve(length);
    for(T i = 0; i < length; ++i)
    {
        T val;
        reader.read_num(val);
        data.push_back(val);
    }
    if(!reader.get_istr())
        throw io::input_error("Error reading contents of generic array tag");
}

template<>
inline void tag_array<int64_t>::read_payload(io::stream_reader& reader)
{
    int32_t length;
    reader.read_num(length);
    if(length < 0)
        reader.get_istr().setstate(std::ios::failbit);
    if(!reader.get_istr())
        throw io::input_error("Error reading length of tag_long_array");

    data.clear();
    data.reserve(length);
    for(int32_t i = 0; i < length; ++i)
    {
        int64_t val;
        reader.read_num(val);
        data.push_back(val);
    }
    if(!reader.get_istr())
        throw io::input_error("Error reading contents of tag_long_array");
}

//Writing
template<>
inline void tag_array<int8_t>::write_payload(io::stream_writer& writer) const
{
    if(size() > io::stream_writer::max_array_len)
    {
        writer.get_ostr().setstate(std::ios::failbit);
        throw std::length_error("Byte array is too large for NBT");
    }
    writer.write_num(static_cast<int32_t>(size()));
    writer.get_ostr().write(reinterpret_cast<const char*>(data.data()), data.size());
}

template<typename T>
inline void tag_array<T>::write_payload(io::stream_writer& writer) const
{
    if(size() > io::stream_writer::max_array_len)
    {
        writer.get_ostr().setstate(std::ios::failbit);
        throw std::length_error("Generic array is too large for NBT");
    }
    writer.write_num(static_cast<int32_t>(size()));
    for(T i: data)
        writer.write_num(i);
}

template<>
inline void tag_array<int64_t>::write_payload(io::stream_writer& writer) const
{
    if(size() > io::stream_writer::max_array_len)
    {
        writer.get_ostr().setstate(std::ios::failbit);
        throw std::length_error("Long array is too large for NBT");
    }
    writer.write_num(static_cast<int32_t>(size()));
    for(int64_t i: data)
        writer.write_num(i);
}

//Typedefs that should be used instead of the template tag_array.
typedef tag_array<int8_t> tag_byte_array;
typedef tag_array<int32_t> tag_int_array;
typedef tag_array<int64_t> tag_long_array;

}

#endif // TAG_ARRAY_H_INCLUDED
