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
#ifndef _TIBEE_COMMON_ABSTRACTSTATEPROVIDERFILE_HPP
#define _TIBEE_COMMON_ABSTRACTSTATEPROVIDERFILE_HPP

#include <boost/filesystem.hpp>

#include <common/stateprov/AbstractStateProvider.hpp>

namespace tibee
{
namespace common
{

/**
 * An abstract state provider file. Any state provider which uses an
 * external file to provider state changes must inherit this class.
 *
 * @author Philippe Proulx
 */
class AbstractStateProviderFile :
    public AbstractStateProvider
{
public:
    /**
     * Builds a state provider file.
     *
     * @param path   State provider file path
     * @param config State provider configuration
     */
    AbstractStateProviderFile(const boost::filesystem::path& path,
                              const StateProviderConfig& config);

    /**
     * Returns this provider path.
     *
     * @returns Provider path
     */
    const boost::filesystem::path& getPath() const
    {
        return _path;
    }

private:
    boost::filesystem::path _path;
};

}
}

#endif // _TIBEE_COMMON_ABSTRACTSTATEPROVIDERFILE_HPP
