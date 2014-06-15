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
#include <common/state/StateNode.hpp>
#include <common/state/StateNodeIterator.hpp>

namespace tibee
{
namespace common
{

StateNodeIterator::StateNodeIterator(const std::unordered_map<quark_t, std::unique_ptr<StateNode>>::iterator& it,
                                     const std::unordered_map<quark_t, std::unique_ptr<StateNode>>::iterator& end) :
    _it {it},
    _end {end}
{
    // increment iterator if the pointed node is marked as removed
    this->findNextValidNode();
}

StateNodeIterator::StateNodeIterator(const StateNodeIterator& it) :
    _it {it._it},
    _end {it._end}
{
    this->findNextValidNode();
}

StateNodeIterator& StateNodeIterator::operator=(const StateNodeIterator& rhs)
{
    _it = rhs._it;
    _end = rhs._end;

    this->findNextValidNode();

    return *this;
}

void StateNodeIterator::findNextValidNode()
{
    if (_it != _end) {
        if (_it->second->isNull()) {
            this->operator++();
        }
    }
}

StateNodeIterator& StateNodeIterator::StateNodeIterator::operator++()
{
    _it++;

    /* Raison d'être of StateNodeIterator: some state nodes are
     * null, and we don't want to consider them here, since a null
     * node means a non-existing node (at least we're not supposed
     * to be interested into getting it since it has no set value).
     */
    for (; _it != _end; ++_it) {
        if (*(_it->second)) {
            return *this;
        }
    }

    // nothing found, we reached the end
    _it = _end;

    return *this;
}

bool StateNodeIterator::operator==(const StateNodeIterator& rhs)
{
    return _it == rhs._it;
}

bool StateNodeIterator::operator!=(const StateNodeIterator& rhs)
{
    return _it != rhs._it;
}

quark_t StateNodeIterator::operator*()
{
    return _it->first;
}

}
}
