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

#ifndef GRAPHOS_PROPERTIES_READER_INTERFACE_H
#define GRAPHOS_PROPERTIES_READER_INTERFACE_H


#include <unordered_map>
#include <memory>

#include <QString>

namespace graphos
{


class PropertiesParser
{

public:

    using Properties = std::unordered_map<QString, std::list<std::pair<QString, QString>>>;

public:

    PropertiesParser(const QString &name) : mName(name){}
    virtual ~PropertiesParser() = default;

    virtual Properties parse(const QString &fileName) = 0;
    QString name() const { return mName; }
	
private:

    QString mName;
	
};



class PropertiesParserFactory
{

private:

    PropertiesParserFactory() {}

public:

    static PropertiesParserFactory &instace();
    void addParser(const std::shared_ptr<PropertiesParser> &parser);
    static std::shared_ptr<PropertiesParser> create(const QString &name);
	
private:

    std::list<std::shared_ptr<PropertiesParser>> parsers;
	
};



} // namespace graphos


#endif // GRAPHOS_PROPERTIES_READER_INTERFACE_H
