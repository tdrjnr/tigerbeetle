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
#ifndef _TIBEE_COMMON_STATEPROVIDERPARAMVALUE_HPP
#define _TIBEE_COMMON_STATEPROVIDERPARAMVALUE_HPP

#include <string>
#include <cstdint>

namespace tibee
{
namespace common
{

/**
 * State provider parameter value.
 *
 * @author Philippe Proulx
 */
class StateProviderParamValue
{
public:
    /**
     * Builds a state provider parameter value.
     *
     * @param val String value of this parameter
     */
    StateProviderParamValue(const std::string& val);

    /**
     * Returns the parameter value as a string.
     *
     * @returns Parameter value as a string
     */
    const std::string& asString() const
    {
        return _val;
    }

    /**
     * Returns the parameter value as a signed integer.
     *
     * No check is performed. If the internal value string does not
     * represent an actual signed integer, the return value
     * is undefined.
     *
     * @returns Parameter value as a signed integer
     */
    std::int64_t asSint() const;

    /**
     * Returns the parameter value as an unsigned integer.
     *
     * No check is performed. If the internal value string does not
     * represent an actual unsigned integer, the return value
     * is undefined.
     *
     * @returns Parameter value as an unsigned integer
     */
    std::uint64_t asUint() const;

    /**
     * Returns the parameter value as a floating point number.
     *
     * No check is performed. If the internal value string does not
     * represent an actual floating point number, the return value
     * is undefined.
     *
     * @returns Parameter value as a floating point number
     */
    double asFloat() const;

    /**
     * Returns the parameter value as a boolean.
     *
     * No check is performed. If the internal value string does not
     * represent an actual boolean, the return value
     * is undefined.
     *
     * @returns Parameter value as a boolean
     */
    bool asBool() const;

private:
    std::string _val;
};

}
}

#endif // _TIBEE_COMMON_STATEPROVIDERPARAMVALUE_HPP
