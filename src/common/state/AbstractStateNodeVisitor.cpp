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
#include <common/state/AbstractStateNodeVisitor.hpp>

namespace tibee
{
namespace common
{

AbstractStateNodeVisitor::~AbstractStateNodeVisitor()
{
}

void AbstractStateNodeVisitor::visitReadEnter(quark_t quark, const StateNode& node)
{
    this->visitReadEnterImpl(quark, node);
}

void AbstractStateNodeVisitor::visitReadLeave(quark_t quark, const StateNode& node)
{
    this->visitReadLeaveImpl(quark, node);
}

void AbstractStateNodeVisitor::visitUpdateEnter(quark_t quark, StateNode& node)
{
    this->visitUpdateEnterImpl(quark, node);
}

void AbstractStateNodeVisitor::visitUpdateLeave(quark_t quark, StateNode& node)
{
    this->visitUpdateLeaveImpl(quark, node);
}

void AbstractStateNodeVisitor::visitReadEnterImpl(quark_t quark, const StateNode& node)
{
}

void AbstractStateNodeVisitor::visitReadLeaveImpl(quark_t quark, const StateNode& node)
{
}

void AbstractStateNodeVisitor::visitUpdateEnterImpl(quark_t quark, StateNode& node)
{
}

void AbstractStateNodeVisitor::visitUpdateLeaveImpl(quark_t quark, StateNode& node)
{
}

}
}
