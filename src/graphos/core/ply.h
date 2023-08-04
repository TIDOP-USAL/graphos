/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_CORE_PLY_H
#define GRAPHOS_CORE_PLY_H


//#include "graphos/graphos_global.h"

#include <tidop/core/exception.h>
#include <tidop/core/flags.h>
#include <tidop/core/path.h>
#include <tidop/core/utils.h>
#include <tidop/core/endian.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/graphic/color.h>

#include <sstream>
#include <fstream>
#include <array>
#include <memory>
#include <map>

namespace graphos
{


class PlyProperty
{

public:

    enum Type
    {
        ply_unknown,
        ply_int8,
        ply_uint8,
        ply_int16,
        ply_uint16,
        ply_int32,
        ply_uin32,
        ply_float32,
        ply_float64,
        ply_char = ply_int8,
        ply_uchar = ply_uint8,
        ply_short = ply_int16,
        ply_ushort = ply_uint16,
        ply_int = ply_int32,
        ply_uint = ply_uin32,
        ply_float = ply_float32,
        ply_double = ply_float64
    };

protected:

    PlyProperty(std::string name,
                Type type);

    // No tiene constructor de copia ni de movimiento ....

public:

    virtual void setValue(const std::string &value) = 0;

    Type type() const;
    virtual int bytes() const = 0;
    virtual void read(std::fstream *stream, bool littleEndian = true) = 0;
    virtual void write(std::fstream *stream, bool littleEndian) = 0;
    virtual void write(std::fstream *stream) = 0;

    static std::shared_ptr<PlyProperty> create(const std::string &name,
                                               Type type);
    template<typename T>
    static std::shared_ptr<PlyProperty> create(const std::string &name,
                                               T value);
    // {
    //   auto ply_property = std::make_shared<PlyPropertyImp<T>>(name, PlyTraits<T>::property_type);
    //   ply_property->setValue(value);
    //   return ply_property;
    // }

private:

    std::string mName;
    Type mType;

};


template<typename T>
struct PlyTraits
{
    static constexpr auto property_type = PlyProperty::Type::ply_unknown;
};

template<>
struct PlyTraits<float>
{
    using value_type = float;
    static constexpr auto property_type = PlyProperty::Type::ply_float;
};

template<>
struct PlyTraits<double>
{
    using value_type = double;
    static constexpr auto property_type = PlyProperty::Type::ply_double;
};

template<>
struct PlyTraits<char>
{
    using value_type = char;
    static constexpr auto property_type = PlyProperty::Type::ply_char;
};

template<>
struct PlyTraits<unsigned char>
{
    using value_type = unsigned char;
    static constexpr auto property_type = PlyProperty::Type::ply_uchar;
};

template<>
struct PlyTraits<short>
{
    using value_type = short;
    static constexpr auto property_type = PlyProperty::Type::ply_short;
};

template<>
struct PlyTraits<unsigned short>
{
    using value_type = unsigned short;
    static constexpr auto property_type = PlyProperty::Type::ply_ushort;
};

template<>
struct PlyTraits<int>
{
    using value_type = int;
    static constexpr auto property_type = PlyProperty::Type::ply_int;
};

template<>
struct PlyTraits<unsigned int>
{
    using value_type = unsigned int;
    static constexpr auto property_type = PlyProperty::Type::ply_uint;
};

template<typename T>
class PlyPropertyImp
    : public PlyProperty
{

public:

    PlyPropertyImp(const std::string &name, Type type)
        : PlyProperty(name, type),
        mValue(0)
    {
    }

    T value()
    {
        return mValue;
    }

    int bytes() const override
    {
        return sizeof(T);
    }

    void setValue(const std::string &value) override
    {
        mValue = tl::convertStringTo<T>(value);
    }

    void setValue(T value)
    {
        mValue = value;
    }

    void read(std::fstream *stream, bool littleEndian) override
    {
        stream->read(reinterpret_cast<char *>(&mValue), sizeof(T));
        if ((littleEndian && tl::endianness::native == tl::endianness::big_endian) ||
            (!littleEndian && tl::endianness::native == tl::endianness::little_endian))
            mValue = tl::swapEndian(mValue);
    }

    void write(std::fstream *stream) override
    {
        *stream << mValue;
    }

    void write(std::fstream *stream, bool littleEndian) override
    {
        T value = mValue;
        if ((littleEndian && tl::endianness::native == tl::endianness::big_endian) ||
            (!littleEndian && tl::endianness::native == tl::endianness::little_endian))
            value = tl::swapEndian(value);
        stream->write(reinterpret_cast<char *>(&mValue), sizeof(T));
    }

private:

    T mValue;
};

//class PlyElement
//{
//
//
//  std::map<std::string, PlyProperty> mProperties;
//};

class Ply
{
public:

    enum class OpenMode : uint8_t
    {
        in = 1 << 0,
        out = 1 << 1/*,
        binary = 1 << 2*/
    };

public:

    Ply();
    Ply(const std::string &file, OpenMode mode = OpenMode::in);
    ~Ply();

    void open(const std::string &file, OpenMode mode);
    void save(bool binary = true);
    void close();

    void resize(size_t size);
    void reserve(size_t size);
    size_t size() const;

    bool hasColors() const;
    bool hasNormals() const;
    bool hasScalarFields() const;

    template<typename T> tl::Point3<T> point(size_t index) const;
    template<typename T> size_t addPoint(const tl::Point3<T> &point);

    template<typename T> tl::Point3<T> normals(size_t index) const;
    template<typename T> void addNormals(const tl::Point3<T> &normals);

    tl::Color color(size_t index) const;
    void addColor(const tl::Color &color);


    std::shared_ptr<PlyProperty> property(size_t index,
                                          const std::string &propertyName) const;

    template<typename T> size_t addProperty(const std::string &name, T value);

    template<typename T> void setProperty(size_t index, const std::string &name, T value);

private:

    void init();
    void readHeader();
    void readBody();
    void readTextBody();
    void readBinaryBody();
    void writeHeader();
    void writeBody();
    void writeTextBody();
    void writeBinaryBody();

    PlyProperty::Type findType(const std::string &type) const;
    std::string findStringType(PlyProperty::Type type) const;

    template<typename T>
    T property(size_t index, const std::string &propertyName) const;

private:

    std::string _file;
    std::fstream *stream;
    tl::EnumFlags<OpenMode> flags;
    bool mIsBinary;
    bool mIsLittleEndian;
    bool mHasColors;
    bool mHasNormals;
    bool mHasScalarFields;
    size_t mSize;
    size_t mFaceSize;
    size_t mReserveSize;
    std::map<std::string, std::vector<std::shared_ptr<PlyProperty>>> mProperties;
    std::vector<std::string> mPropertiesNames;
    std::vector<PlyProperty::Type> mPropertiesTypes;
    std::map<std::string, PlyProperty::Type> mStringTypes;
    std::vector<tl::Point3<int>> mFaces;

};
ALLOW_BITWISE_FLAG_OPERATIONS(Ply::OpenMode)






template<typename T>
std::shared_ptr<PlyProperty> PlyProperty::create(const std::string &name,
                                                 T value)
{
    //auto ply_property = std::make_shared<PlyPropertyImp<T>>(name, PlyTraits<T>::property_type);
    //ply_property->setValue(value);
    //return ply_property;
    return nullptr;
}






template<typename T>
inline tl::Point3<T> Ply::point(size_t index) const
{
    tl::Point3<T> point(property<T>(index, "x"),
                        property<T>(index, "y"),
                        property<T>(index, "z"));
    return point;
}

template<typename T>
size_t Ply::addPoint(const tl::Point3<T> &point)
{
    auto index = addProperty("x", point.x);
    addProperty("y", point.y);
    addProperty("z", point.z);

    mSize = index + 1;

    return index;
}

template<typename T>
inline tl::Point3<T> Ply::normals(size_t index) const
{
    tl::Point3<T> normals(property<T>(index, "nx"),
                          property<T>(index, "ny"),
                          property<T>(index, "nz"));

    return normals;
}

template<typename T>
void Ply::addNormals(const tl::Point3<T> &normals)
{
    mHasNormals = true;
    addProperty("nx", normals.x);
    addProperty("ny", normals.y);
    addProperty("nz", normals.z);
}

template<typename T>
T Ply::property(size_t index, const std::string &propertyName) const
{
    auto type = property(index, propertyName)->type();

    if (PlyTraits<T>::property_type < type) {
        tl::Message::warning("Conversión de {} a {}. Posible perdida de datos",
                             findStringType(PlyTraits<T>::property_type),
                             findStringType(type));
    }

    auto _property = property(index, propertyName);

    T value{};

    switch (type) {
    case graphos::PlyProperty::ply_unknown:
        break;
    case graphos::PlyProperty::ply_int8:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<char>>(_property)->value());
        break;
    case graphos::PlyProperty::ply_uint8:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<unsigned char>>(_property)->value());
        break;
    case graphos::PlyProperty::ply_int16:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<short>>(_property)->value());
        break;
    case graphos::PlyProperty::ply_uint16:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<unsigned short>>(_property)->value());
        break;
    case graphos::PlyProperty::ply_int32:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<int>>(_property)->value());
        break;
    case graphos::PlyProperty::ply_uin32:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<unsigned int>>(_property)->value());
        break;
    case graphos::PlyProperty::ply_float32:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<float>>(_property)->value());
        break;
    case graphos::PlyProperty::ply_float64:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<double>>(_property)->value());
        break;
    default:
        break;
    }

    return value;
}

template<typename T>
size_t Ply::addProperty(const std::string &name, T value)
{
    size_t index = mProperties[name].size();
    auto type = PlyTraits<T>::property_type;

    if (index == 0) {
        mPropertiesNames.push_back(name);
        mPropertiesTypes.push_back(type);
        mProperties[name].reserve(mReserveSize);
    }

    mProperties[name].push_back(PlyProperty::create(name, value));
    return index;
}

template<typename T>
void Ply::setProperty(size_t index, const std::string &name, T value)
{
    auto type = PlyTraits<T>::property_type;

    TL_ASSERT(!mProperties[name].empty(), "Property not found");
    //if (mProperties[name].empty()) {
    //  mPropertiesNames.push_back(name);
    //  mPropertiesTypes.push_back(type);
    //}
    std::dynamic_pointer_cast<PlyPropertyImp<T>>(mProperties[name].at(index))->setValue(value);
    //mProperties[name].at(index) = PlyProperty::create(name, value);
}

} // end namespace graphos

#endif // GRAPHOS_CORE_PLY_H
