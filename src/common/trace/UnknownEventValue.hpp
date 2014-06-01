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
#ifndef _TIBEE_COMMON_UNKNOWNEVENTVALUE_HPP
#define _TIBEE_COMMON_UNKNOWNEVENTVALUE_HPP

#include <cstdint>
#include <babeltrace/ctf/events.h>

#include <common/trace/AbstractEventValue.hpp>
#include <common/trace/EventValueType.hpp>

namespace tibee
{
namespace common
{

/**
 * Unknown event value.
 *
 * @author Philippe Proulx
 */
class UnknownEventValue :
    public AbstractEventValue
{
public:
    /**
     * Builds an unknown value.
     *
     * @param def BT field definition
     */
    UnknownEventValue();

    /**
     * @see AbstractEventValue::toString()
     */
    std::string toString() const;
};

}
}

#endif // _TIBEE_COMMON_UNKNOWNEVENTVALUE_HPP
