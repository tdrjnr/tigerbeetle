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
#ifndef _TIBEE_COMMON_ABSTRACTINTEGEREVENTVALUE_HPP
#define _TIBEE_COMMON_ABSTRACTINTEGEREVENTVALUE_HPP

#include <cstdint>
#include <babeltrace/ctf/events.h>

#include <common/trace/AbstractEventValue.hpp>
#include <common/trace/EventValueType.hpp>

namespace tibee
{
namespace common
{

/**
 * Abstract integer event value template, with common operations for
 * both signed and unsigned integers.
 *
 * @author Philippe Proulx
 */
template<typename T, EventValueType VT>
class AbstractIntegerEventValue :
    public AbstractEventValue
{
public:
    /**
     * Builds an abstract integer event value.
     *
     * @param def          BT field definition
     * @param valueFactory Value factory used to create other event values
     */
    AbstractIntegerEventValue(const ::bt_definition* def,
                              const EventValueFactory* valueFactory) :
        AbstractEventValue {VT, valueFactory},
        _btDef {def}
    {
    }

    /**
     * Returns the integer value.
     *
     * @returns Integer value
     */
    T getValue() const;

    /**
     * Returns the expected display base (radix) or -1 when not
     * available.
     *
     * @returns Expected display base or -1
     */
    int getDisplayBase() const
    {
        auto decl = ::bt_ctf_get_decl_from_def(_btDef);
        auto base = ::bt_ctf_get_int_base(decl);

        if (base < 0) {
            return -1;
        }

        return base;
    }

    /**
     * Returns the sum of this event value and a provided signed
     * integer.
     *
     * @param val Value to add
     * @returns   Sum of this event value and \p val
     */
    std::int64_t operator+(std::int64_t val) const
    {
        return this->getValue() + val;
    }

    /**
     * Returns the sum of this event value and a provided unsigned
     * integer.
     *
     * @param val Value to add
     * @returns   Sum of this event value and \p val
     */
    std::int64_t operator+(std::uint64_t val) const
    {
        return this->getValue() + val;
    }

    /**
     * Returns the sum of this event value and a provided signed
     * integer event value.
     *
     * @param val Value to add
     * @returns   Sum of this event value and \p val
     */
    std::int64_t operator+(const AbstractIntegerEventValue<std::int64_t, EventValueType::SINT>& val) const;

    /**
     * Returns the sum of this event value and a provided unsigned
     * integer event value.
     *
     * @param val Value to add
     * @returns   Sum of this event value and \p val
     */
    std::int64_t operator+(const AbstractIntegerEventValue<std::uint64_t, EventValueType::UINT>& val) const;

    /**
     * Returns the difference between this event value and a provided
     * signed integer.
     *
     * @param val Value to substract
     * @returns   Difference between this event value and \p val
     */
    std::int64_t operator-(std::int64_t val) const
    {
        return this->getValue() - val;
    }

    /**
     * Returns the difference between this event value and a provided
     * unsigned integer.
     *
     * @param val Value to substract
     * @returns   Difference between this event value and \p val
     */
    std::int64_t operator-(std::uint64_t val) const
    {
        return this->getValue() - val;
    }

    /**
     * Returns the difference between this event value and a provided
     * signed integer event value.
     *
     * @param val Value to substract
     * @returns   Difference between this event value and \p val
     */
    std::int64_t operator-(const AbstractIntegerEventValue<std::int64_t, EventValueType::SINT>& val) const;

    /**
     * Returns the difference between this event value and a provided
     * unsigned integer event value.
     *
     * @param val Value to substract
     * @returns   Difference between this event value and \p val
     */
    std::int64_t operator-(const AbstractIntegerEventValue<std::uint64_t, EventValueType::UINT>& val) const;

    /**
     * Returns the product of this event value by a provided signed
     * integer.
     *
     * @param val Value to multiply with
     * @returns   Product of this event value by \p val
     */
    std::int64_t operator*(std::int64_t val) const
    {
        return this->getValue() * val;
    }

    /**
     * Returns the product of this event value by a provided unsigned
     * integer.
     *
     * @param val Value to multiply with
     * @returns   Product of this event value by \p val
     */
    std::int64_t operator*(std::uint64_t val) const
    {
        return this->getValue() * val;
    }

    /**
     * Returns the product of this event value by a provided signed
     * integer event value.
     *
     * @param val Value to multiply with
     * @returns   Product of this event value by \p val
     */
    std::int64_t operator*(const AbstractIntegerEventValue<std::int64_t, EventValueType::SINT>& val) const;

    /**
     * Returns the product of this event value by a provided unsigned
     * integer event value.
     *
     * @param val Value to multiply with
     * @returns   Product of this event value by \p val
     */
    std::int64_t operator*(const AbstractIntegerEventValue<std::uint64_t, EventValueType::UINT>& val) const;

    /**
     * Returns the quotient of this event value and a provided signed
     * integer.
     *
     * @param val Value to divide by
     * @returns   Quotient of this event value and \p val
     */
    std::int64_t operator/(std::int64_t val) const
    {
        return this->getValue() / val;
    }

    /**
     * Returns the quotient of this event value and a provided unsigned
     * integer.
     *
     * @param val Value to divide by
     * @returns   Quotient of this event value and \p val
     */
    std::int64_t operator/(std::uint64_t val) const
    {
        return this->getValue() / val;
    }

    /**
     * Returns the quotient of this event value and a provided signed
     * integer event value.
     *
     * @param val Value to divide by
     * @returns   Quotient of this event value and \p val
     */
    std::int64_t operator/(const AbstractIntegerEventValue<std::int64_t, EventValueType::SINT>& val) const;

    /**
     * Returns the quotient of this event value and a provided unsigned
     * integer event value.
     *
     * @param val Value to divide by
     * @returns   Quotient of this event value and \p val
     */
    std::int64_t operator/(const AbstractIntegerEventValue<std::uint64_t, EventValueType::UINT>& val) const;

    /**
     * Compares the integer value of this event value and a provided
     * signed integer.
     *
     * @param value Value to compare with
     * @returns     True if both values are equal
     */
    bool operator==(std::int64_t value) const
    {
        return static_cast<std::int64_t>(this->getValue()) == value;
    }

    /**
     * Opposite of operator==(std::int64_t).
     *
     * @see operator==(std::int64_t)
     */
    bool operator!=(std::int64_t value) const
    {
        return !(*this == value);
    }

    /**
     * Compares the integer value of this event value and a provided
     * unsigned integer.
     *
     * @param value Value to compare with
     * @returns     True if both values are equal
     */
    bool operator==(std::uint64_t value) const
    {
        return static_cast<std::uint64_t>(this->getValue()) == value;
    }

    /**
     * Opposite of operator==(std::uint64_t).
     *
     * @see operator==(std::uint64_t)
     */
    bool operator!=(std::uint64_t value) const
    {
        return !(*this == value);
    }

protected:
    const ::bt_definition* getDef() const {
        return _btDef;
    }

private:
    virtual T getValueImpl() const = 0;

private:
    const ::bt_definition* _btDef;
};

template<typename T, EventValueType VT>
T AbstractIntegerEventValue<T, VT>::getValue() const
{
    return this->getValueImpl();
}

template<typename T, EventValueType VT>
std::int64_t AbstractIntegerEventValue<T, VT>::operator+(const AbstractIntegerEventValue<std::int64_t, EventValueType::SINT>& val) const
{
    return this->getValue() + val.getValue();
}

template<typename T, EventValueType VT>
std::int64_t AbstractIntegerEventValue<T, VT>::operator+(const AbstractIntegerEventValue<std::uint64_t, EventValueType::UINT>& val) const
{
    return this->getValue() + val.getValue();
}

template<typename T, EventValueType VT>
std::int64_t AbstractIntegerEventValue<T, VT>::operator-(const AbstractIntegerEventValue<std::int64_t, EventValueType::SINT>& val) const
{
    return this->getValue() - val.getValue();
}

template<typename T, EventValueType VT>
std::int64_t AbstractIntegerEventValue<T, VT>::operator-(const AbstractIntegerEventValue<std::uint64_t, EventValueType::UINT>& val) const
{
    return this->getValue() - val.getValue();
}

template<typename T, EventValueType VT>
std::int64_t AbstractIntegerEventValue<T, VT>::operator*(const AbstractIntegerEventValue<std::int64_t, EventValueType::SINT>& val) const
{
    return this->getValue() * val.getValue();
}

template<typename T, EventValueType VT>
std::int64_t AbstractIntegerEventValue<T, VT>::operator*(const AbstractIntegerEventValue<std::uint64_t, EventValueType::UINT>& val) const
{
    return this->getValue() * val.getValue();
}

template<typename T, EventValueType VT>
std::int64_t AbstractIntegerEventValue<T, VT>::operator/(const AbstractIntegerEventValue<std::int64_t, EventValueType::SINT>& val) const
{
    return this->getValue() / val.getValue();
}

template<typename T, EventValueType VT>
std::int64_t AbstractIntegerEventValue<T, VT>::operator/(const AbstractIntegerEventValue<std::uint64_t, EventValueType::UINT>& val) const
{
    return this->getValue() / val.getValue();
}

}
}

#endif // _TIBEE_COMMON_ABSTRACTINTEGEREVENTVALUE_HPP
