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
#ifndef _TIBEE_COMMON_PYTHONSTATEPROVIDER_HPP
#define _TIBEE_COMMON_PYTHONSTATEPROVIDER_HPP

#include <boost/filesystem.hpp>

#include <common/trace/EventValueType.hpp>
#include <common/trace/AbstractEventValue.hpp>
#include "AbstractStateProviderFile.hpp"

namespace tibee
{
namespace common
{

/**
 * A state provider which loads a Python user script and calls specific
 * functions to obtain state informations.
 *
 * @author Philippe Proulx
 */
class PythonStateProvider :
    public AbstractStateProviderFile
{
public:
    /**
     * Builds a Python state provider.
     *
     * @param path   Python script path
     * @param config State provider configuration
     */
    PythonStateProvider(const boost::filesystem::path& path,
                        const StateProviderConfig& config);

    ~PythonStateProvider();

private:
    void onInitImpl(CurrentState& state);
    void onEventImpl(CurrentState& state, const Event& event);
    void onFiniImpl(CurrentState& state);
};

}
}

#endif // _TIBEE_COMMON_PYTHONSTATEPROVIDER_HPP
