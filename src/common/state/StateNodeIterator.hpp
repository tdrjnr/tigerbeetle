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
#ifndef _TIBEE_COMMON_STATENODEITERATOR_HPP
#define _TIBEE_COMMON_STATENODEITERATOR_HPP

#include <memory>
#include <iterator>

namespace tibee
{
namespace common
{

class StateNode;

/**
 * A state node iterator.
 *
 * Do not build the iterator; use one created and returned by
 * StateNode.
 *
 * A state node iterator remains valid and usable as long as the
 * children of the associated state node remain unchanged.
 *
 * @author Philippe Proulx
 */
class StateNodeIterator :
    public std::iterator<
        std::input_iterator_tag,
        quark_t
    >
{
public:
    StateNodeIterator(const std::unordered_map<quark_t, std::unique_ptr<StateNode>>::iterator& it,
                      const std::unordered_map<quark_t, std::unique_ptr<StateNode>>::iterator& end);
    StateNodeIterator(const StateNodeIterator& it);

    StateNodeIterator& operator=(const StateNodeIterator& rhs);
    StateNodeIterator& operator++();
    bool operator==(const StateNodeIterator& rhs);
    bool operator!=(const StateNodeIterator& rhs);

    /**
     * Returns the state node currently pointed to by this iterator.
     *
     * @returns Current event
     */
    quark_t operator*();

private:
    void findNextValidNode();

private:
    std::unordered_map<quark_t, std::unique_ptr<StateNode>>::iterator _it;
    std::unordered_map<quark_t, std::unique_ptr<StateNode>>::iterator _end;
};

}
}

#endif // _TIBEE_COMMON_STATENODEITERATOR_HPP
