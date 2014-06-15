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
#include <common/stateprov/StateProviderParamValue.hpp>

namespace tibee
{
namespace common
{

StateProviderParamValue::StateProviderParamValue(const std::string& val) :
    _val {val}
{
}

std::int64_t StateProviderParamValue::asSint() const
{
    try {
        return std::stoll(_val);
    } catch (...) {
        return -1;
    }
}

std::uint64_t StateProviderParamValue::asUint() const
{
    try {
        return std::stoull(_val);
    } catch (...) {
        return -1;
    }
}

double StateProviderParamValue::asFloat() const
{
    try {
        return std::stod(_val);
    } catch (...) {
        return -1;
    }
}

bool StateProviderParamValue::asBool() const
{
    if (_val == "true" || _val == "TRUE" || _val == "True") {
        return true;
    } else if (_val == "false" || _val == "FALSE" || _val == "False") {
        return false;
    }

    return static_cast<bool>(this->asUint());
}

}
}
