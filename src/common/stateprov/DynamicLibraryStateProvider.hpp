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
#ifndef _TIBEE_COMMON_DYNAMICLIBRARYSTATEPROVIDER_HPP
#define _TIBEE_COMMON_DYNAMICLIBRARYSTATEPROVIDER_HPP

#include <string>
#include <boost/filesystem.hpp>

#include <common/trace/EventValueType.hpp>
#include <common/trace/AbstractEventValue.hpp>
#include <common/stateprov/AbstractStateProviderFile.hpp>

namespace tibee
{
namespace common
{

/**
 * A state provider which loads a dynamic library, finds specific
 * symbols and call them to obtain state informations.
 *
 * @author Philippe Proulx
 */
class DynamicLibraryStateProvider :
    public AbstractStateProviderFile
{
public:
    /**
     * This is a façade for dynamically loaded state providers.
     *
     * It's safer to pass this to dynamically loaded state providers
     * than the whole DynamicLibraryStateProvider since the set of
     * methods are limited here. Also, this restricted interface is
     * simpler to understand.
     */
    class StateProviderConfig
    {
        friend class DynamicLibraryStateProvider;

    public:
        /**
         * Calls to this method are delegated to
         * AbstractStateProvider::registerEventCallback().
         *
         * @see AbstractStateProvider::registerEventCallback()
         */
        bool registerEventCallback(const std::string& traceType,
                                   const std::string& eventName,
                                   const OnEventFunc& onEvent);

        /**
         * Calls to this method are delegated to
         * AbstractStateProvider::registerEventCallbackRegex().
         *
         * @see AbstractStateProvider::registerEventCallbackRegex()
         */
        bool registerEventCallbackRegex(const std::string& traceType,
                                        const std::string& eventName,
                                        const OnEventFunc& onEvent);

        /**
         * Calls to this method are delegated to
         * AbstractStateProvider::getInstanceName().
         *
         * @see AbstractStateProvider::getInstanceName()
         */
        const std::string& getInstanceName();

    private:
        /**
         * Builds a state provider configuration.
         *
         * This constructor is private to only allow the
         * DynamicLibraryStateProvider parent to build this.
         *
         * Methods of StateProviderConfig are forwarded to the
         * DynamicLibraryStateProvider parent.
         *
         * @param stateProvider Parent dynamic library state provider
         */
        StateProviderConfig(DynamicLibraryStateProvider* stateProvider);

    private:
        // state provider parent
        DynamicLibraryStateProvider* _stateProvider;
    };

public:
    /**
     * Builds a dynamic library state provider.
     *
     * @param path     Dynamic library path
     * @param instance State provider instance name
     */
    DynamicLibraryStateProvider(const boost::filesystem::path& path,
                                const std::string& instance);

    ~DynamicLibraryStateProvider();

protected:
    static std::string getErrorMsg(const std::string& base);

private:
    static constexpr const char* ON_INIT_SYMBOL_NAME() {
        return "onInit";
    }

    static constexpr const char* ON_EVENT_SYMBOL_NAME() {
        return "onEvent";
    }

    static constexpr const char* ON_FINI_SYMBOL_NAME() {
        return "onFini";
    }

    void onInitImpl(CurrentState& state, const TraceSet* traceSet);
    void onEventImpl(CurrentState& state, const Event& event);
    void onFiniImpl(CurrentState& state);

private:
    // DL handle
    void* _dlHandle;

    // DL resolved symbols
    void (*_dlOnInit)(CurrentState&, const TraceSet*, StateProviderConfig&);
    void (*_dlOnFini)(CurrentState&);
};

}
}

#endif // _TIBEE_COMMON_DYNAMICLIBRARYSTATEPROVIDER_HPP
