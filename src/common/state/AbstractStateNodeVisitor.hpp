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
#ifndef _TIBEE_COMMON_ABSTRACTSTATENODEVISITOR_HPP
#define _TIBEE_COMMON_ABSTRACTSTATENODEVISITOR_HPP

#include <common/BasicTypes.hpp>

namespace tibee
{
namespace common
{

class StateNode;

/**
 * Abstract state node visitor.
 *
 * Concrete state node visitors are used to perform all sorts
 * of things on a whole state tree or on a subtree.
 *
 * @author Philippe Proulx
 */
class AbstractStateNodeVisitor
{
public:
    virtual ~AbstractStateNodeVisitor();

    /**
     * Visits the node \p node with subpath quark \p quark.
     *
     * @param quark Subpath quark of node \p node
     * @param node  Node to visit
     */
    void visitEnter(quark_t quark, const StateNode& node);

    /**
     * Leaves the visit of node \p node with subpath quark \p quark.
     *
     * @param quark Left subpath quark of node \p node
     * @param node  Left node
     */
    void visitLeave(quark_t quark, const StateNode& node);

private:
    virtual void visitEnterImpl(quark_t quark, const StateNode& node);
    virtual void visitLeaveImpl(quark_t quark, const StateNode& node);
};

}
}

#endif // _TIBEE_COMMON_ABSTRACTSTATENODEVISITOR_HPP
