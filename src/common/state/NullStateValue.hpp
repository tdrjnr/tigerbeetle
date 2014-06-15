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
#ifndef _TIBEE_COMMON_NULLSTATEVALUE_HPP
#define _TIBEE_COMMON_NULLSTATEVALUE_HPP

#include <memory>
#include <cstdint>

#include <common/state/AbstractStateValue.hpp>
#include <common/state/StateValueType.hpp>

namespace tibee
{
namespace common
{

/**
 * Null state value, used for invalid state values.
 *
 * @author Philippe Proulx
 */
class NullStateValue :
    public AbstractStateValue
{
public:
    typedef std::shared_ptr<NullStateValue> SP;
    typedef std::unique_ptr<NullStateValue> UP;

public:
    NullStateValue() :
        AbstractStateValue {StateValueType::NUL}
    {
    }
};

}
}

#endif // _TIBEE_COMMON_NULLSTATEVALUE_HPP
