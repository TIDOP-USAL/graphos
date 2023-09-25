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

#include "PropertiesParser.h"


namespace graphos
{


PropertiesParserFactory &PropertiesParserFactory::instace()
{
    static PropertiesParserFactory parser;
    return parser;
}

void PropertiesParserFactory::addParser(const std::shared_ptr<PropertiesParser> &parser)
{
    parsers.push_back(parser);
}

std::shared_ptr<PropertiesParser> PropertiesParserFactory::create(const QString &name)
{
    std::shared_ptr<PropertiesParser> properties_parser;
    ;
    for (auto &parser : PropertiesParserFactory::instace().parsers){
        if (parser->name() == name){
            properties_parser = parser;
        }
    }
    
    return properties_parser;
}

} // namespace graphos
