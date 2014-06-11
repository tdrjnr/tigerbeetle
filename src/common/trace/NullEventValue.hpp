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
#ifndef _TIBEE_COMMON_NULLEVENTVALUE_HPP
#define _TIBEE_COMMON_NULLEVENTVALUE_HPP

#include <string>
#include <babeltrace/ctf/events.h>

#include <common/trace/AbstractEventValue.hpp>
#include <common/trace/EventValueType.hpp>

namespace tibee
{
namespace common
{

/**
 * Null event value: used by methods returning references to signify
 * "no value".
 *
 * @author Philippe Proulx
 */
class NullEventValue :
    public AbstractEventValue
{
public:
    /**
     * Builds an enumeration item value out of a field definition.
     *
     * @param valueFactory Value factory used to create other event values
     */
    NullEventValue(const EventValueFactory* valueFactory);

private:
    std::string toStringImpl() const;
};

}
}

#endif // _TIBEE_COMMON_NULLEVENTVALUE_HPP
