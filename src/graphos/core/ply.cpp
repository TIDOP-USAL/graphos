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

#include "graphos/core/ply.h"


namespace graphos
{


PlyProperty::PlyProperty(std::string name, Type type)
  : mName(std::move(name)),
    mType(type)
{
}

PlyProperty::Type PlyProperty::type() const
{
  return mType;
}

std::shared_ptr<PlyProperty> PlyProperty::create(const std::string &name, Type type)
{
  std::shared_ptr<PlyProperty> ply_property;

  switch (type) {
  case graphos::PlyProperty::ply_unknown:
    //TODO: Devolver excepción
    break;
  case graphos::PlyProperty::ply_char:
    ply_property = std::make_shared<PlyPropertyImp<int8_t>>(name, type);
    break;
  case graphos::PlyProperty::ply_uchar:
    ply_property = std::make_shared<PlyPropertyImp<uint8_t>>(name, type);
    break;
  case graphos::PlyProperty::ply_short:
    ply_property = std::make_shared<PlyPropertyImp<int16_t>>(name, type);
    break;
  case graphos::PlyProperty::ply_ushort:
    ply_property = std::make_shared<PlyPropertyImp<uint16_t>>(name, type);
    break;
  case graphos::PlyProperty::ply_int:
    ply_property = std::make_shared<PlyPropertyImp<int32_t>>(name, type);
    break;
  case graphos::PlyProperty::ply_uint:
    ply_property = std::make_shared<PlyPropertyImp<uint32_t>>(name, type);
    break;
  case graphos::PlyProperty::ply_float:
    ply_property = std::make_shared<PlyPropertyImp<float>>(name, type);
    break;
  case graphos::PlyProperty::ply_double:
    ply_property = std::make_shared<PlyPropertyImp<double>>(name, type);
    break;
  default:
    break;
  }

  return ply_property;
}


Ply::Ply()
  : stream(nullptr),
    mIsBinary(false),
    mIsLittleEndian(false),
    mHasColors(false),
    mHasNormals(false),
    mHasScalarFields(false),
    mSize(0),
    mReserveSize(0)
{
  init();
}

Ply::Ply(const std::string &file,
         OpenMode mode)
  : stream(nullptr),
    mIsBinary(false),
    mIsLittleEndian(false),
    mHasColors(false),
    mHasNormals(false),
    mHasScalarFields(false)
{
  init();
  open(file, mode);
}

Ply::~Ply()
{
  if (stream) {
    delete stream;
    stream = nullptr;
  }
}



void Ply::open(const std::string &file,
               OpenMode mode)
{
  flags = mode;

  bool file_exists = tl::Path::exists(file);

  std::ios_base::openmode open_mode = 0;
  if (flags.isEnabled(OpenMode::in)) open_mode += std::ios_base::in;
  if (flags.isEnabled(OpenMode::out)) open_mode += std::ios_base::out;
  if (flags.isEnabled(OpenMode::binary) || file_exists) open_mode += std::ios_base::binary;
  
  mIsBinary = flags.isEnabled(OpenMode::binary);
  if (mIsBinary) mIsLittleEndian = true;

  stream = new std::fstream(file, open_mode);

  if (file_exists && flags.isEnabled(OpenMode::in)) {
    readHeader();
    readBody();
  }

}

void Ply::save()
{
  writeHeader();
  writeBody();
}

void Ply::close()
{
  if (stream->is_open())  
    stream->close();
}

void Ply::resize(size_t size)
{
  mSize = size;
  for (auto property_name : mPropertiesNames)
    mProperties[property_name].resize(size);
}

void Ply::reserve(size_t size)
{
  mReserveSize = size;
  for (auto property_name : mPropertiesNames)
    mProperties[property_name].reserve(size);
}

void Ply::init()
{
  mStringTypes["unknown"] = PlyProperty::Type::ply_unknown;
  mStringTypes["int8"] = PlyProperty::Type::ply_int8;
  mStringTypes["uint8"] = PlyProperty::Type::ply_uint8;
  mStringTypes["int16"] = PlyProperty::Type::ply_int16;
  mStringTypes["uint16"] = PlyProperty::Type::ply_uint16;
  mStringTypes["int32"] = PlyProperty::Type::ply_int32;
  mStringTypes["uint32"] = PlyProperty::Type::ply_uin32;
  mStringTypes["float32"] = PlyProperty::Type::ply_float32;
  mStringTypes["float64"] = PlyProperty::Type::ply_float64;
  mStringTypes["char"] = PlyProperty::Type::ply_char;
  mStringTypes["uchar"] = PlyProperty::Type::ply_uchar;
  mStringTypes["short"] = PlyProperty::Type::ply_short;
  mStringTypes["ushort"] = PlyProperty::Type::ply_ushort;
  mStringTypes["int"] = PlyProperty::Type::ply_int;
  mStringTypes["uint"] = PlyProperty::Type::ply_uint;
  mStringTypes["float"] = PlyProperty::Type::ply_float;
  mStringTypes["double"] = PlyProperty::Type::ply_double;
}

void Ply::readHeader()
{
  std::string line;

  std::getline(*stream, line);

  TL_ASSERT(line == "ply", "Invalid Ply file");

  std::string name;
  std::string type;
  std::string value;

  int coordinates_dimension{};
  int normals_dimension{};
  int color_dimension{};

  while (std::getline(*stream, line)) {

    if (line == "end_header") break;

    std::istringstream isstream(line);
    isstream >> name >> type >> value;

    if (name == "format") {

      if (type == "binary_little_endian") {
        mIsBinary = true;
        mIsLittleEndian = true;
      } else if (type == "binary_big_endian") {
        mIsBinary = true;
        mIsLittleEndian = false;
      } else if (type == "ascii") {
        mIsBinary = false;
      } else {
        /// TODO: devolver error
      }

    } else if (name == "element") {

      if (type == "vertex") {
        mSize = tl::convertStringTo<size_t>(value);
      } else {
        msgError("Error reading Ply: Only vertex elements supported");
      }

    } else if (name == "property") {

      mProperties[value].reserve(mSize);
      mPropertiesNames.push_back(value);
      mPropertiesTypes.push_back(findType(type));

      if (value == "x" || value == "y" ||  value == "z") {

        coordinates_dimension++;

      } else if (value == "red" || value == "green" || value == "blue") {

        color_dimension++;

      } else if (value == "nx" || value == "ny" || value == "nz") {

        normals_dimension++;

      } else {

        mHasScalarFields = true;

      }

    } else {
      // Comment
    }
  }

  TL_ASSERT(coordinates_dimension == 3, "Invalid Ply file");
  mHasColors = color_dimension == 3;
  mHasNormals = normals_dimension == 3;
}

size_t Ply::size() const
{
  return mSize;
}

void Ply::readBody()
{
  if (mIsBinary) {
    readBinaryBody();
  } else {
    readTextBody();
  }
}

void Ply::readTextBody()
{
  std::string line;

  size_t counter{};

  while (std::getline(*stream, line)) {

    while (line.empty()) {
      std::getline(*stream, line);
    }

    std::stringstream line_stream(line);

    std::string item;
    std::vector<std::string> items;
    size_t property_index = 0;

    while (!line_stream.eof()) {

      std::getline(line_stream, item, ' ');
      std::string property_name = mPropertiesNames[property_index++];
      PlyProperty::Type property_type = mPropertiesTypes[property_index++];
      auto property = PlyProperty::create(property_name, property_type);
      property->setValue(item);
      mProperties[property_name].push_back(property);

    }

    counter++;
  }
}

void Ply::readBinaryBody()
{
  for (size_t i = 0; i < mSize; ++i) {
    for (size_t j = 0; j < mPropertiesNames.size(); j++) {

      std::string property_name = mPropertiesNames[j];
      PlyProperty::Type property_type = mPropertiesTypes[j];
      auto property = PlyProperty::create(property_name, property_type);
      property->read(stream);
      mProperties[property_name].push_back(property);

    }
  }
}

void Ply::writeHeader()
{
  TL_ASSERT(stream->is_open(), "File not open");

  std::string endianness;
  if (mIsBinary && mIsLittleEndian)
    endianness = "binary_little_endian";
  else if (mIsBinary && !mIsLittleEndian)
    endianness = "binary_big_endian";
  else
    endianness = "ascii";

  *stream << "ply\n"
          << "format " << endianness << " 1.0\n"
          << "element vertex " << mSize << std::endl;

  *stream << "comment Created by GRAPHOS" << std::endl; 

  for (size_t i = 0; i < mPropertiesNames.size(); i++) {
    std::string property_name = mPropertiesNames[i];
    std::string property_type = findStringType(mPropertiesTypes[i]);
    *stream << "property " << property_type << " " << property_name << "\n";
  }

  *stream << "end_header" << std::endl;
}

void Ply::writeBody()
{
  if (mIsBinary) {
    writeBinaryBody();
  } else {
    writeTextBody();
  }
}

void Ply::writeTextBody()
{
  for (size_t i = 0; i < mSize; ++i) {
    
    for (auto &name : mPropertiesNames) {
      auto &property = mProperties.at(name).at(i);
      property->write(stream);
      *stream << " ";
    }
          
    *stream << "\n";
  }
      
  *stream << std::endl;
}

void Ply::writeBinaryBody()
{
  for (size_t i = 0; i < mSize; ++i) {
    for (auto &name : mPropertiesNames) {
      auto &property = mProperties.at(name).at(i);
      property->write(stream, mIsLittleEndian);
    }
  }
}

PlyProperty::Type Ply::findType(const std::string &sType) const
{
  PlyProperty::Type type = PlyProperty::Type::ply_unknown;

  auto string_type = mStringTypes.find(sType);
  if (string_type != mStringTypes.end()) {
    type = string_type->second;
  }

  return type;
}

std::string Ply::findStringType(PlyProperty::Type type) const
{
  std::string string_type = "unknown";

  for (auto &_type : mStringTypes) {
    if (_type.second == type) {
      string_type = _type.first;
      break;
    }
  }

  return string_type;
}



bool Ply::hasColors() const
{
  return mHasColors;
}

bool Ply::hasNormals() const
{
  return mHasNormals;
}

bool Ply::hasScalarFields() const
{
  return false;
}

tl::graph::Color Ply::color(size_t index) const
{
  return tl::graph::ColorRGB(property<int>(index, "red"),
                             property<int>(index, "green"), 
                             property<int>(index, "blue"));
}

void Ply::addColor(const tl::graph::Color &color)
{
  mHasColors = true;
  addProperty("red", color.red());
  addProperty("green", color.green());
  addProperty("blue", color.blue());
}

std::shared_ptr<PlyProperty> Ply::property(size_t index,
                                           const std::string &propertyName) const
{
  return mProperties.at(propertyName).at(index);
}



} // end namespace graphos
