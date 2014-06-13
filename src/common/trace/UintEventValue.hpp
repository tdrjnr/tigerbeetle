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
#ifndef _TIBEE_COMMON_UINTEVENTVALUE_HPP
#define _TIBEE_COMMON_UINTEVENTVALUE_HPP

#include <cstdint>
#include <babeltrace/ctf/events.h>

#include <common/trace/AbstractIntegerEventValue.hpp>

namespace tibee
{
namespace common
{

class SintEventValue;

/**
 * Event value carrying an unsigned integer.
 *
 * @author Philippe Proulx
 */
class UintEventValue :
    public AbstractIntegerEventValue<std::uint64_t, EventValueType::UINT>
{
public:
    /**
     * Builds an unsigned integer value out of a field definition.
     *
     * @param def          BT field definition
     * @param valueFactory Value factory used to create other event values
     */
    UintEventValue(const ::bt_definition* def,
                   const EventValueFactory* valueFactory);

    /**
     * Returns the result of a bitwise AND between this event value
     * and an unsigned integer.
     *
     * @param val Other operand
     * @returns   Result of bitwise AND between this event value and \p val
     */
    std::uint64_t operator&(std::uint64_t val) const
    {
        return this->getValue() & val;
    }

    /**
     * Returns the result of a bitwise AND between this event value
     * and an unsigned integer event value.
     *
     * @param val Other operand
     * @returns   Result of bitwise AND between this event value and \p val
     */
    std::uint64_t operator&(const UintEventValue& val) const
    {
        return this->getValue() & val.getValue();
    }

    /**
     * Returns the result of a bitwise OR between this event value
     * and an unsigned integer.
     *
     * @param val Other operand
     * @returns   Result of bitwise OR between this event value and \p val
     */
    std::uint64_t operator|(std::uint64_t val) const
    {
        return this->getValue() | val;
    }

    /**
     * Returns the result of a bitwise OR between this event value
     * and an unsigned integer event value.
     *
     * @param val Other operand
     * @returns   Result of bitwise OR between this event value and \p val
     */
    std::uint64_t operator|(const UintEventValue& val) const
    {
        return this->getValue() | val.getValue();
    }

    /**
     * Returns the result of a bitwise XOR between this event value
     * and an unsigned integer.
     *
     * @param val Other operand
     * @returns   Result of bitwise XOR between this event value and \p val
     */
    std::uint64_t operator^(std::uint64_t val) const
    {
        return this->getValue() ^ val;
    }

    /**
     * Returns the result of a bitwise XOR between this event value
     * and an unsigned integer event value.
     *
     * @param val Other operand
     * @returns   Result of bitwise XOR between this event value and \p val
     */
    std::uint64_t operator^(const UintEventValue& val) const
    {
        return this->getValue() ^ val.getValue();
    }

    /**
     * Returns the bitwise NOT value of this event value.
     *
     * @returns   Result of bitwise XOR between this event value and \p val
     */
    std::uint64_t operator~() const
    {
        return ~this->getValue();
    }

private:
    std::uint64_t getValueImpl() const;
    std::string toStringImpl() const;
};

}
}

#endif // _TIBEE_COMMON_UINTEVENTVALUE_HPP
