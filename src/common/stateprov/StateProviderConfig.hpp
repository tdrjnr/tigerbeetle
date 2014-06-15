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
#ifndef _TIBEE_COMMON_STATEPROVIDERCONFIG_HPP
#define _TIBEE_COMMON_STATEPROVIDERCONFIG_HPP

#include <string>
#include <unordered_map>

#include <common/stateprov/StateProviderParamValue.hpp>

namespace tibee
{
namespace common
{

/**
 * Immutable state provider configuration.
 */
class StateProviderConfig
{
public:
    /// State provider parameters
    typedef std::unordered_map<std::string, StateProviderParamValue> Params;

public:
    /**
     * Builds a state provider configuration.
     *
     * The \p instance parameter may be an empty string, in which case
     * the state provider is considered to not have any instance name.
     *
     * @param name     State provider name
     * @param instance State provider unique instance name
     * @param params   State provider parameters
     */
    StateProviderConfig(const std::string& name,
                        const std::string& instance,
                        const Params& params) :
        _name {name},
        _instance {instance},
        _params {params}
    {
    }

    /**
     * Returns the state provider name.
     *
     * @returns State provider name
     */
    const std::string& getName() const
    {
        return _name;
    }

    /**
     * Returns whether or not the state provider has an instance name.
     *
     * @returns True if the state provider has an instance name
     */
    bool hasInstanceName() const
    {
        return !_instance.empty();
    }

    /**
     * Returns the state provider instance name.
     *
     * If the state provider has no instance name, an empty string is
     * returned.
     *
     * @returns State provider instance name
     */
    const std::string& getInstanceName() const
    {
        return _instance;
    }

    /**
     * Returns the state provider parameters.
     *
     * @returns State provider parameters
     */
    const Params& getParams() const
    {
        return _params;
    }

private:
    std::string _name;
    std::string _instance;
    Params _params;
};

}
}

#endif // _TIBEE_COMMON_STATEPROVIDERCONFIG_HPP
