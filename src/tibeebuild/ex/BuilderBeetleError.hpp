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
#ifndef _BUILDERBEETLEERROREX_HPP
#define _BUILDERBEETLEERROREX_HPP

#include <string>
#include <stdexcept>

namespace tibee
{
namespace ex
{

class BuilderBeetleError :
    public std::runtime_error
{
public:
    BuilderBeetleError(const std::string& err) :
        std::runtime_error {err}
    {
    }
};

}
}

#endif // _BUILDERBEETLEERROREX_HPP
