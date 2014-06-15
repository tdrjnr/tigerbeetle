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
#include <common/state/CurrentState.hpp>
#include <common/state/StateHistorySink.hpp>
#include <common/state/StateNode.hpp>

namespace tibee
{
namespace common
{

CurrentState::CurrentState(StateHistorySink* sink) :
    _sink {sink}
{
}

quark_t CurrentState::getSubpathQuark(const char* subpath) const
{
    return _sink->getSubpathQuark(subpath);
}

quark_t CurrentState::getSubpathQuark(const std::string& subpath) const
{
    return _sink->getSubpathQuark(subpath);
}

quark_t CurrentState::getStringValueQuark(const char* string) const
{
    return _sink->getStringValueQuark(string);
}

quark_t CurrentState::getStringValueQuark(const std::string& string) const
{
    return _sink->getStringValueQuark(string);
}

const std::string& CurrentState::getSubpathString(quark_t quark) const
{
    return _sink->getSubpathString(quark);
}

const std::string& CurrentState::getStringValueString(quark_t quark) const
{
    return _sink->getStringValueString(quark);
}

std::size_t CurrentState::getStateChangesCount() const
{
    return _sink->getStateChangesCount();
}

std::size_t CurrentState::getNodesCount() const
{
    return _sink->getNodesCount();
}

StateNode& CurrentState::getRoot()
{
    return _sink->getRoot();
}

}
}
