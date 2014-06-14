/* Copyright (c) 2014 Philippe Proulx <eepp.ca>
 *
 * This file is part of tigerbeetle.
 *
 * tigerbeetle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tigerbeetle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tigerbeetle.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _UNKNOWNSTATEPROVIDERTYPEEX_HPP
#define _UNKNOWNSTATEPROVIDERTYPEEX_HPP

#include <string>
#include <stdexcept>

namespace tibee
{
namespace ex
{

class UnknownStateProviderType :
    public std::runtime_error
{
public:
    UnknownStateProviderType(const std::string& name) :
        std::runtime_error {"unknown state provider type"},
        _name {name}
    {
    }

    const std::string& getName() const {
        return _name;
    }

private:
    std::string _name;
};

}
}

#endif // _UNKNOWNSTATEPROVIDERTYPEEX_HPP
