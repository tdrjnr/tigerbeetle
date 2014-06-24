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
#include <memory>
#include <cassert>

#include <common/state/StateValueType.hpp>
#include <common/state/AbstractStateValue.hpp>
#include <common/state/Sint32StateValue.hpp>
#include <common/state/Uint32StateValue.hpp>
#include <common/state/Sint64StateValue.hpp>
#include <common/state/Uint64StateValue.hpp>
#include <common/state/Float32StateValue.hpp>
#include <common/state/QuarkStateValue.hpp>

namespace tibee
{
namespace common
{

AbstractStateValue::AbstractStateValue(StateValueType type) :
    _type {type}
{
}

AbstractStateValue::~AbstractStateValue()
{
}

const Sint32StateValue& AbstractStateValue::asSint32Value() const
{
    assert(this->isSint32());

    return static_cast<const Sint32StateValue&>(*this);
}

std::int32_t AbstractStateValue::asSint32() const
{
    return this->asSint32Value().getValue();
}

const Uint32StateValue& AbstractStateValue::asUint32Value() const
{
    assert(this->isUint32());

    return static_cast<const Uint32StateValue&>(*this);
}

std::uint32_t AbstractStateValue::asUint32() const
{
    return this->asUint32Value().getValue();
}

const Sint64StateValue& AbstractStateValue::asSint64Value() const
{
    assert(this->isSint64());

    return static_cast<const Sint64StateValue&>(*this);
}

std::int64_t AbstractStateValue::asSint64() const
{
    return this->asSint64Value().getValue();
}

const Uint64StateValue& AbstractStateValue::asUint64Value() const
{
    assert(this->isUint64());

    return static_cast<const Uint64StateValue&>(*this);
}

std::uint64_t AbstractStateValue::asUint64() const
{
    return this->asUint64Value().getValue();
}

const Float32StateValue& AbstractStateValue::asFloat32Value() const
{
    assert(this->isFloat32());

    return static_cast<const Float32StateValue&>(*this);
}

float AbstractStateValue::asFloat32() const
{
    return this->asFloat32Value().getValue();
}

const QuarkStateValue& AbstractStateValue::asQuarkValue() const
{
    assert(this->isQuark());

    return static_cast<const QuarkStateValue&>(*this);
}

Quark AbstractStateValue::asQuark() const
{
    return this->asQuarkValue().getValue();
}

}
}
