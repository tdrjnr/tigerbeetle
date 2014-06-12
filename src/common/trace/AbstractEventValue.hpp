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
#ifndef _TIBEE_COMMON_ABSTRACTEVENTVALUE_HPP
#define _TIBEE_COMMON_ABSTRACTEVENTVALUE_HPP

#include <boost/utility.hpp>
#include <cstdint>
#include <memory>

#include <common/BasicTypes.hpp>
#include <common/trace/EventValueType.hpp>

namespace tibee
{
namespace common
{

class EventValueFactory;
class SintEventValue;
class UintEventValue;
class FloatEventValue;
class EnumEventValue;
class StringEventValue;
class ArrayEventValue;
class DictEventValue;

/**
 * Abstract event value. All event values must inherit this class.
 *
 * @author Philippe Proulx
 */
class AbstractEventValue :
    boost::noncopyable
{
public:
    /// Unique pointer to abstract event value
    typedef std::unique_ptr<AbstractEventValue> UP;

public:
    /**
     * Builds an event value with type \p type.
     *
     * @param type         Event value type
     * @param valueFactory Value factory used to create other event values
     */
    AbstractEventValue(EventValueType type,
                       const EventValueFactory* valueFactory) :
        _type {type},
        _valueFactory {valueFactory}
    {
    }

    virtual ~AbstractEventValue() = 0;

    /**
     * Returns a string representation of this event value.
     *
     * This method may be really slow and is only useful for
     * debugging.
     *
     * @returns String representation of this event value.
     */
    std::string toString() const;

    /**
     * Returns the event value type.
     *
     * @returns Event value type.
     */
    EventValueType getType() const
    {
        return _type;
    }

    /**
     * Statically casts this event value to a signed integer event
     * value.
     *
     * No runtime check is performed.
     *
     * @returns This event value as a signed integer event value
     */
    const SintEventValue* asSintValue() const;

    /**
     * Applies asSintValue() and returns the actual integer value of
     * this object.
     *
     * No runtime check is performed, so if the actual event value type
     * is not SintEventValue, the behaviour is undefined.
     *
     * @returns This event value as a signed integer
     */
    std::int64_t asSint() const;

    /**
     * Statically casts this event value to an unsigned integer event
     * value.
     *
     * No runtime check is performed.
     *
     * @returns This event value as an unsigned integer event value
     */
    const UintEventValue* asUintValue() const;

    /**
     * Applies asUintValue() and returns the actual integer value of
     * this object.
     *
     * No runtime check is performed, so if the actual event value type
     * is not UintEventValue, the behaviour is undefined.
     *
     * @returns This event value as an unsigned integer
     */
    std::uint64_t asUint() const;

    /**
     * Statically casts this event value to a floating point number
     * event value.
     *
     * No runtime check is performed.
     *
     * @returns This event value as a floating point number event value
     */
    const FloatEventValue* asFloatValue() const;

    /**
     * Applies asFloatValue() and returns the actual floating point number
     * value of this object.
     *
     * No runtime check is performed, so if the actual event value type
     * is not FloatEventValue, the behaviour is undefined.
     *
     * @returns This event value as a floating point number
     */
    double asFloat() const;

    /**
     * Statically casts this event value to an enumeration event
     * value.
     *
     * No runtime check is performed.
     *
     * @returns This event value as an enumeration event value
     */
    const EnumEventValue* asEnumValue() const;

    /**
     * Applies asEnumValue() and returns the actual integer value of
     * the enumeration.
     *
     * No runtime check is performed, so if the actual event value type
     * is not EnumEventValue, the behaviour is undefined.
     *
     * @returns This event value as an enumeration integer value
     */
    std::uint64_t asEnumInt() const;

    /**
     * Applies asEnumValue() and returns the actual string label of
     * the enumeration.
     *
     * No runtime check is performed, so if the actual event value type
     * is not EnumEventValue, the behaviour is undefined.
     *
     * @returns This event value as an enumeration string label
     */
    const char* asEnumLabel() const;

    /**
     * Statically casts this event value to a string event
     * value.
     *
     * No runtime check is performed.
     *
     * @returns This event value as a string event value
     */
    const StringEventValue* asStringValue() const;

    /**
     * Applies asStringValue() and returns the actual string value of
     * this object.
     *
     * No runtime check is performed, so if the actual event value type
     * is not StringEventValue, the behaviour is undefined.
     *
     * @returns This event value as a string value
     */
    const char* asString() const;

    /**
     * Statically casts this event value to an array event
     * value.
     *
     * No runtime check is performed.
     *
     * @returns This event value as an array event value
     */
    const ArrayEventValue* asArray() const;

    /**
     * Statically casts this event value to a dictionary event
     * value.
     *
     * No runtime check is performed.
     *
     * @returns This event value as a dictionary event value
     */
    const DictEventValue* asDict() const;

    /**
     * Returns whether or not this is a signed integer event value.
     *
     * @returns True if this is a signed integer event value
     */
    bool isSint() const
    {
        return _type == EventValueType::SINT;
    }

    /**
     * Returns whether or not this is an unsigned integer event value.
     *
     * @returns True if this is an unsigned integer event value
     */
    bool isUint() const
    {
        return _type == EventValueType::UINT;
    }

    /**
     * Returns whether or not this is a floating point number event value.
     *
     * @returns True if this is a floating point number integer event value
     */
    bool isFloat() const
    {
        return _type == EventValueType::FLOAT;
    }

    /**
     * Returns whether or not this is an enumeration event value.
     *
     * @returns True if this is an enumeration event value
     */
    bool isEnum() const
    {
        return _type == EventValueType::ENUM;
    }

    /**
     * Returns whether or not this is a string event value.
     *
     * @returns True if this is a string event value
     */
    bool isString() const
    {
        return _type == EventValueType::STRING;
    }

    /**
     * Returns whether or not this is an array event value.
     *
     * @returns True if this is an array event value
     */
    bool isArray() const
    {
        return _type == EventValueType::ARRAY;
    }

    /**
     * Returns whether or not this is a dictionary event value.
     *
     * @returns True if this is a dictionary event value
     */
    bool isDict() const
    {
        return _type == EventValueType::DICT;
    }

    /**
     * Returns whether or not this is a null event value.
     *
     * @returns True if this is a null event value
     */
    bool isNull() const
    {
        return _type == EventValueType::NUL;
    }

    /**
     * Not isNull().
     *
     * Returns true if this event value is not null.
     *
     * @see isNull()
     *
     * @returns True if this event value is not null
     */
    explicit operator bool() const
    {
        return !this->isNull();
    }

    /**
     * This is a convenience method which returns the field of this
     * event value having the name \p name. If this event value type is
     * not DictEventValue, it shall return a null event value.
     *
     * If there's no such field with name \p name, the method shall
     * return a null event value.
     *
     * This method is not very efficient since it searches linearly
     * through the keys of a dictionary. Should you know the field
     * index in advance, prefer using operator[](std::size_t).
     *
     * @param name Name of field value to retrieve
     * @returns    Retrieved field value or null event value if not found
     */
    const AbstractEventValue& operator[](const char* name) const;

    /**
     * @see operator[](const char*)
     */
    const AbstractEventValue& operator[](const std::string& name) const;

    /**
     * This is a convenience method which returns the field of this
     * event value at index \p index. If this event value type is
     * not ArrayEventValue nor DictEventValue, it shall return a null
     * event value.
     *
     * Same as operator[](const char* name), but using the numeric
     * index of the field value to retrieve.
     *
     * If the supplied index \p index is greater or equal than the
     * dictionary/array size, this method shall return a null event
     * value.
     *
     * @param index Index of field value to retrieve
     * @returns     Retrieved field value or null event value if not found
     */
    const AbstractEventValue& operator[](field_index_t index) const;

protected:
    const EventValueFactory* getValueFactory() const
    {
        return _valueFactory;
    }

private:
    virtual std::string toStringImpl() const = 0;
    virtual const AbstractEventValue& getFieldImpl(const char* name) const;
    virtual const AbstractEventValue& getFieldImpl(field_index_t index) const;

private:
    EventValueType _type;
    const EventValueFactory* _valueFactory;
};

}
}

#endif // _TIBEE_COMMON_ABSTRACTEVENTVALUE_HPP
