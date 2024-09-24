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

#include <tidop/core/exception.h>
#include <tidop/core/flags.h>
#include <tidop/core/path.h>
#include <tidop/core/utils.h>
#include <tidop/core/endian.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/graphic/color.h>

#include <fstream>
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

    PlyProperty(std::string name, Type type)
      : mName(std::move(name)),
        mType(type) {}

public:

    auto type() const -> Type { return mType; }

    virtual size_t size() const = 0;
    virtual void resize(size_t size) = 0;
    virtual void reserve(size_t size) = 0;
    virtual void setValue(size_t index, const std::string &value) = 0;
    virtual void addValue(const std::string &value) = 0;
    virtual int bytes() const = 0;
    virtual void read(std::fstream *stream, size_t id, bool littleEndian = true) = 0;
    virtual void write(std::fstream *stream, size_t id, bool littleEndian) = 0;
    virtual void write(std::fstream *stream, size_t id) = 0;

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

    auto size() const -> size_t override;
    void resize(size_t size) override;
    void reserve(size_t size) override;
    auto value(size_t index) const -> T;
    auto bytes() const -> int override;
    void setValue(size_t index, const std::string &value) override;
    void setValue(size_t index, T value);
    void addValue(const std::string &value) override;
    void addValue(T value);

    void read(std::fstream *stream, size_t id, bool littleEndian) override;
    void write(std::fstream *stream, size_t id) override;
    void write(std::fstream *stream, size_t id, bool littleEndian) override;

private:

    std::vector<T> mValue;
};




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
    Ply(tl::Path file, OpenMode mode = OpenMode::in);
    ~Ply();

    void open(const tl::Path &file, OpenMode mode);
    void read();
    void save(bool binary = true, bool littleEndian = tl::endianness::native == tl::endianness::little_endian);
    void close() const;
    void resize(size_t size);
    void reserve(size_t size) const;
    void setProperty(const std::string &name, PlyProperty::Type type);

    auto size() const -> size_t;

    auto hasColors() const -> bool;
    auto hasNormals() const -> bool;
    auto hasScalarFields() const -> bool;

    template<typename T> auto point(size_t index) const -> tl::Point3<T>;
    template<typename T> auto addPoint(const tl::Point3<T>& point) -> size_t;
    template<typename T> auto normals(size_t index) const -> tl::Point3<T>;
    template<typename T> void addNormals(const tl::Point3<T>& normals);

    auto color(size_t index) const -> tl::Color;
    void addColor(const tl::Color &color);

    auto property(const std::string& propertyName) const -> std::shared_ptr<PlyProperty>;

    template<typename T> auto addProperty(const std::string& name, T value) -> size_t;

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

    auto findType(const std::string& type) const -> PlyProperty::Type;
    auto findStringType(PlyProperty::Type type) const -> std::string;

    template<typename T>
    T property(size_t index, const std::string &propertyName) const;

    auto createProperty(const std::string& name, PlyProperty::Type type) -> std::shared_ptr<PlyProperty>;

private:

    tl::Path _file;
    std::fstream *stream;
    tl::EnumFlags<OpenMode> flags;
    bool mIsBinary;
    bool mIsLittleEndian;
    bool mHasColors;
    bool mHasNormals;
    bool mHasScalarFields;
    size_t mSize;
    size_t mFaceSize;
    std::map<std::string, std::shared_ptr<PlyProperty>> mProperties;
    std::vector<std::string> mPropertiesOrder;
    std::map<std::string, PlyProperty::Type> mStringTypes;
    std::vector<tl::Point3<int>> mFaces;

};
ALLOW_BITWISE_FLAG_OPERATIONS(Ply::OpenMode)





template <typename T>
auto PlyPropertyImp<T>::size() const -> size_t
{
    return mValue.size();
}

template <typename T>
void PlyPropertyImp<T>::resize(size_t size)
{
    mValue.resize(size);
}

template <typename T>
void PlyPropertyImp<T>::reserve(size_t size)
{
    mValue.reserve(size);
}

template <typename T>
auto PlyPropertyImp<T>::value(size_t index) const -> T
{
    // Comprobar que este en rango
    return mValue.at(index);
}

template <typename T>
auto PlyPropertyImp<T>::bytes() const -> int
{
    return sizeof(T);
}

template <typename T>
void PlyPropertyImp<T>::setValue(size_t index, const std::string& value)
{
    // Comprobar que este en rango
    mValue[index] = tl::convertStringTo<T>(value);
}

template <typename T>
void PlyPropertyImp<T>::setValue(size_t index, T value)
{
    // Comprobar que este en rango
    mValue[index] = value;
}

template <typename T>
void PlyPropertyImp<T>::addValue(const std::string& value)
{
    // Comprobar que este en rango
    mValue.push_back(tl::convertStringTo<T>(value));
}

template <typename T>
void PlyPropertyImp<T>::addValue(T value)
{
    // Comprobar que este en rango
    mValue.push_back(value);
}

template <typename T>
void PlyPropertyImp<T>::read(std::fstream* stream, size_t id, bool littleEndian)
{
    stream->read(reinterpret_cast<char *>(&mValue[id]), sizeof(T));
    if ((littleEndian && tl::endianness::native == tl::endianness::big_endian) ||
        (!littleEndian && tl::endianness::native == tl::endianness::little_endian))
        mValue[id] = tl::swapEndian(mValue[id]);
}

template <typename T>
void PlyPropertyImp<T>::write(std::fstream* stream, size_t id)
{
    *stream << mValue.at(id);
}

template <typename T>
void PlyPropertyImp<T>::write(std::fstream* stream, size_t id, bool littleEndian)
{
    T value = mValue.at(id);
    if ((littleEndian && tl::endianness::native == tl::endianness::big_endian) ||
        (!littleEndian && tl::endianness::native == tl::endianness::little_endian))
        value = tl::swapEndian(value);
    stream->write(reinterpret_cast<char *>(&value), sizeof(T));
}




inline auto Ply::size() const -> size_t
{
    return mSize;
}

inline auto Ply::hasColors() const -> bool
{
    return mHasColors;
}

inline auto Ply::hasNormals() const -> bool
{
    return mHasNormals;
}

inline auto Ply::hasScalarFields() const -> bool
{
    return mHasScalarFields;
}

template <typename T>
auto Ply::point(size_t index) const -> tl::Point3<T>
{
    tl::Point3<T> point(property<T>(index, "x"),
                        property<T>(index, "y"),
                        property<T>(index, "z"));
    return point;
}

template <typename T>
auto Ply::addPoint(const tl::Point3<T>& point) -> size_t
{
    auto index = addProperty("x", point.x);
    addProperty("y", point.y);
    addProperty("z", point.z);

    mSize = index + 1;

    return index;
}

template <typename T>
auto Ply::normals(size_t index) const -> tl::Point3<T>
{
    tl::Point3<T> normals(property<T>(index, "nx"),
                          property<T>(index, "ny"),
                          property<T>(index, "nz"));

    return normals;
}

template <typename T>
void Ply::addNormals(const tl::Point3<T>& normals)
{
    mHasNormals = true; // Esto se tiene que definir antes
    addProperty("nx", normals.x);
    addProperty("ny", normals.y);
    addProperty("nz", normals.z);
}

template <typename T>
auto Ply::addProperty(const std::string& name, T value) -> size_t
{
    size_t index = mProperties[name]->size();
    auto type = PlyTraits<T>::property_type;

    switch (type) {
    case PlyProperty::ply_unknown:
        break;
    case PlyProperty::ply_int8:
        std::dynamic_pointer_cast<PlyPropertyImp<char>>(mProperties[name])->addValue(value);
        break;
    case PlyProperty::ply_uint8:
        std::dynamic_pointer_cast<PlyPropertyImp<unsigned char>>(mProperties[name])->addValue(value);
        break;
    case PlyProperty::ply_int16:
        std::dynamic_pointer_cast<PlyPropertyImp<short>>(mProperties[name])->addValue(value);
        break;
    case PlyProperty::ply_uint16:
        std::dynamic_pointer_cast<PlyPropertyImp<unsigned short>>(mProperties[name])->addValue(value);
        break;
    case PlyProperty::ply_int32:
        std::dynamic_pointer_cast<PlyPropertyImp<int>>(mProperties[name])->addValue(value);
        break;
    case PlyProperty::ply_uin32:
        std::dynamic_pointer_cast<PlyPropertyImp<unsigned int>>(mProperties[name])->addValue(value);
        break;
    case PlyProperty::ply_float32:
        std::dynamic_pointer_cast<PlyPropertyImp<float>>(mProperties[name])->addValue(value);
        break;
    case PlyProperty::ply_float64:
        std::dynamic_pointer_cast<PlyPropertyImp<double>>(mProperties[name])->addValue(value);
        break;
    default:
        break;
    }

    return index;
}

template <typename T>
T Ply::property(size_t index, const std::string& propertyName) const
{
    auto type = mProperties.at(propertyName)->type();
    if (PlyTraits<T>::property_type < type) {
        tl::Message::warning("Conversión de {} a {}. Posible perdida de datos",
                             findStringType(PlyTraits<T>::property_type),
                             findStringType(type));
    }

    auto _property = mProperties.at(propertyName);

    T value{};

    switch (type) {
    case graphos::PlyProperty::ply_unknown:
        break;
    case graphos::PlyProperty::ply_int8:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<char>>(_property)->value(index));
        break;
    case graphos::PlyProperty::ply_uint8:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<unsigned char>>(_property)->value(index));
        break;
    case graphos::PlyProperty::ply_int16:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<short>>(_property)->value(index));
        break;
    case graphos::PlyProperty::ply_uint16:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<unsigned short>>(_property)->value(index));
        break;
    case graphos::PlyProperty::ply_int32:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<int>>(_property)->value(index));
        break;
    case graphos::PlyProperty::ply_uin32:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<unsigned int>>(_property)->value(index));
        break;
    case graphos::PlyProperty::ply_float32:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<float>>(_property)->value(index));
        break;
    case graphos::PlyProperty::ply_float64:
        value = static_cast<T>(std::dynamic_pointer_cast<PlyPropertyImp<double>>(_property)->value(index));
        break;
    default:
        break;
    }

    return value;
}
} // end namespace graphos

#endif // GRAPHOS_CORE_PLY_H
