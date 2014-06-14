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
#ifndef _TIBEE_COMMON_PRINT_HPP
#define _TIBEE_COMMON_PRINT_HPP

#include <ostream>
#include <sstream>

#include <common/utils/TermModifier.hpp>

namespace tibee
{
namespace common
{

/**
 * Output a colored error.
 *
 * Call tbendl() at the end of the output instead of using std::endl.
 *
 * @return Stream to use for output
 */
std::ostream& tberror()
{
    return std::cerr << TM(TM::FG_RED) << "Error: " << TM(TM::BOLD);
}

/**
 * Output a colored warning.
 *
 * Call tbendl() at the end of the output instead of using std::endl.
 *
 * @return Stream to use for output
 */
std::ostream& tbwarn()
{
    return std::cout << TM(TM::FG_YELLOW) << "Warning: " << TM(TM::BOLD);
}

/**
 * Output a colored information.
 *
 * Call tbendl() at the end of the output instead of using std::endl.
 *
 * @return Stream to use for output
 */
std::ostream& tbinfo()
{
    return std::cout << TM(TM::FG_BLUE) << "Warning: " << TM(TM::BOLD);
}

/**
 * Terminates a colored output.
 *
 * @return String to output to terminate the colored output
 */
std::string tbendl()
{
    std::stringstream ss;

    ss << TM(TM::RESET) << std::endl;

    return ss.str();
}

}
}

#endif // _TIBEE_COMMON_PRINT_HPP
