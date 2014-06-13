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
#include <cassert>

#include <common/trace/AbstractEventValue.hpp>
#include <common/trace/SintEventValue.hpp>
#include <common/trace/UintEventValue.hpp>
#include <common/trace/FloatEventValue.hpp>
#include <common/trace/EnumEventValue.hpp>
#include <common/trace/StringEventValue.hpp>
#include <common/trace/ArrayEventValue.hpp>
#include <common/trace/DictEventValue.hpp>
#include <common/trace/EventValueFactory.hpp>

namespace tibee
{
namespace common
{

AbstractEventValue::~AbstractEventValue()
{
}

const SintEventValue& AbstractEventValue::asSintValue() const
{
    assert(this->isSint());

    return *static_cast<const SintEventValue*>(this);
}

std::int64_t AbstractEventValue::asSint() const
{
    return this->asSintValue().getValue();
}

const UintEventValue& AbstractEventValue::asUintValue() const
{
    assert(this->isUint());

    return *static_cast<const UintEventValue*>(this);
}

std::uint64_t AbstractEventValue::asUint() const
{
    return this->asUintValue().getValue();
}

const FloatEventValue& AbstractEventValue::asFloatValue() const
{
    assert(this->isFloat());

    return *static_cast<const FloatEventValue*>(this);
}

double AbstractEventValue::asFloat() const
{
    return this->asFloatValue().getValue();
}

const EnumEventValue& AbstractEventValue::asEnumValue() const
{
    assert(this->isEnum());

    return *static_cast<const EnumEventValue*>(this);
}

std::uint64_t AbstractEventValue::asEnumInt() const
{
    return this->asEnumValue().getIntValue();
}

const char* AbstractEventValue::asEnumLabel() const
{
    return this->asEnumValue().getLabel();
}

const StringEventValue& AbstractEventValue::asStringValue() const
{
    assert(this->isString());

    return *static_cast<const StringEventValue*>(this);
}

const char* AbstractEventValue::asString() const
{
    return this->asStringValue().getValue();
}

const ArrayEventValue& AbstractEventValue::asArray() const
{
    assert(this->isArray());

    return *static_cast<const ArrayEventValue*>(this);
}

const DictEventValue& AbstractEventValue::asDict() const
{
    assert(this->isDict());

    return *static_cast<const DictEventValue*>(this);
}

std::string AbstractEventValue::toString() const
{
    return this->toStringImpl();
}

const AbstractEventValue& AbstractEventValue::operator[](const char* name) const
{
    return this->getFieldImpl(name);
}

const AbstractEventValue& AbstractEventValue::operator[](const std::string& name) const
{
    return this->operator[](name.c_str());
}

const AbstractEventValue& AbstractEventValue::operator[](field_index_t index) const
{
    return this->getFieldImpl(index);
}

const AbstractEventValue& AbstractEventValue::getFieldImpl(const char* name) const
{
    return *_valueFactory->getNull();
}

const AbstractEventValue& AbstractEventValue::getFieldImpl(field_index_t index) const
{
    return *_valueFactory->getNull();
}

std::int64_t AbstractEventValue::operator+(std::int64_t val) const
{
    if (this->isSint()) {
        return this->asSintValue() + val;
    } else if (this->isUint()) {
        return this->asUintValue() + val;
    }

    return 0;
}

std::int64_t AbstractEventValue::operator+(std::uint64_t val) const
{
    if (this->isSint()) {
        return this->asSintValue() + val;
    } else if (this->isUint()) {
        return this->asUintValue() + val;
    }

    return 0;
}

std::int64_t AbstractEventValue::operator+(const AbstractEventValue& val) const
{
    if (this->isSint()) {
        if (val.isSint()) {
            return this->asSintValue() + val.asSintValue();
        } else if (val.isUint()) {
            return this->asSintValue() + val.asUintValue();
        }
    } else if (this->isUint()) {
        if (val.isSint()) {
            return this->asUintValue() + val.asSintValue();
        } else if (val.isUint()) {
            return this->asUintValue() + val.asUintValue();
        }
    }

    return 0;
}

std::int64_t AbstractEventValue::operator-(std::int64_t val) const
{
    if (this->isSint()) {
        return this->asSintValue() - val;
    } else if (this->isUint()) {
        return this->asUintValue() - val;
    }

    return 0;
}

std::int64_t AbstractEventValue::operator-(std::uint64_t val) const
{
    if (this->isSint()) {
        return this->asSintValue() - val;
    } else if (this->isUint()) {
        return this->asUintValue() - val;
    }

    return 0;
}

std::int64_t AbstractEventValue::operator-(const AbstractEventValue& val) const
{
    if (this->isSint()) {
        if (val.isSint()) {
            return this->asSintValue() - val.asSintValue();
        } else if (val.isUint()) {
            return this->asSintValue() - val.asUintValue();
        }
    } else if (this->isUint()) {
        if (val.isSint()) {
            return this->asUintValue() - val.asSintValue();
        } else if (val.isUint()) {
            return this->asUintValue() - val.asUintValue();
        }
    }

    return 0;
}

std::int64_t AbstractEventValue::operator*(std::int64_t val) const
{
    if (this->isSint()) {
        return this->asSintValue() * val;
    } else if (this->isUint()) {
        return this->asUintValue() * val;
    }

    return 0;
}

std::int64_t AbstractEventValue::operator*(std::uint64_t val) const
{
    if (this->isSint()) {
        return this->asSintValue() * val;
    } else if (this->isUint()) {
        return this->asUintValue() * val;
    }

    return 0;
}

std::int64_t AbstractEventValue::operator*(const AbstractEventValue& val) const
{
    if (this->isSint()) {
        if (val.isSint()) {
            return this->asSintValue() * val.asSintValue();
        } else if (val.isUint()) {
            return this->asSintValue() * val.asUintValue();
        }
    } else if (this->isUint()) {
        if (val.isSint()) {
            return this->asUintValue() * val.asSintValue();
        } else if (val.isUint()) {
            return this->asUintValue() * val.asUintValue();
        }
    }

    return 0;
}

std::int64_t AbstractEventValue::operator/(std::int64_t val) const
{
    if (this->isSint()) {
        return this->asSintValue() / val;
    } else if (this->isUint()) {
        return this->asUintValue() / val;
    }

    return 0;
}

std::int64_t AbstractEventValue::operator/(std::uint64_t val) const
{
    if (this->isSint()) {
        return this->asSintValue() / val;
    } else if (this->isUint()) {
        return this->asUintValue() / val;
    }

    return 0;
}

std::int64_t AbstractEventValue::operator/(const AbstractEventValue& val) const
{
    if (this->isSint()) {
        if (val.isSint()) {
            return this->asSintValue() / val.asSintValue();
        } else if (val.isUint()) {
            return this->asSintValue() / val.asUintValue();
        }
    } else if (this->isUint()) {
        if (val.isSint()) {
            return this->asUintValue() / val.asSintValue();
        } else if (val.isUint()) {
            return this->asUintValue() / val.asUintValue();
        }
    }

    return 0;
}

std::uint64_t AbstractEventValue::operator&(std::uint64_t val) const
{
    return this->asUintValue() & val;
}

std::uint64_t AbstractEventValue::operator&(const AbstractEventValue& val) const
{
    return this->asUintValue() & val.asUintValue();
}

std::uint64_t AbstractEventValue::operator|(std::uint64_t val) const
{
    return this->asUintValue() | val;
}

std::uint64_t AbstractEventValue::operator|(const AbstractEventValue& val) const
{
    return this->asUintValue() | val.asUintValue();
}

std::uint64_t AbstractEventValue::operator^(std::uint64_t val) const
{
    return this->asUintValue() ^ val;
}

std::uint64_t AbstractEventValue::operator^(const AbstractEventValue& val) const
{
    return this->asUintValue() ^ val.asUintValue();
}

std::uint64_t AbstractEventValue::operator~() const
{
    return ~this->asUintValue();
}

}
}
