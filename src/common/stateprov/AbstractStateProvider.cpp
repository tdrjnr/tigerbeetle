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
#include <boost/regex.hpp>

#include <common/stateprov/AbstractStateProvider.hpp>

namespace tibee
{
namespace common
{

AbstractStateProvider::AbstractStateProvider(const StateProviderConfig& config) :
    _config {config}
{
}

AbstractStateProvider::~AbstractStateProvider()
{
}

void AbstractStateProvider::onInit(CurrentState& state,
                                   const TraceSet* traceSet)
{
    _curTraceSet = traceSet;

    // clear the infamous map, ready for a new run
    _infamousMap.clear();

    // delegate to implementation
    this->onInitImpl(state, traceSet);
}

bool AbstractStateProvider::onEvent(CurrentState& state,
                                    const Event& event)
{
    // try finding a matching event callback function
    auto traceId = event.getTraceId();
    auto eventId = event.getId();

    auto callbackMapIt = _infamousMap.find(traceId);

    if (callbackMapIt != _infamousMap.end()) {
        const auto& callbackMap = callbackMapIt->second;

        auto callbackIt = callbackMap.find(eventId);

        if (callbackIt != callbackMap.end()) {
            const auto& callback = callbackIt->second;

            if (callback) {
                // match!
                return callback(state, event);
            }
        }
    }

    // no match: continue
    return true;
}

void AbstractStateProvider::onFini(CurrentState& state)
{
    this->onFiniImpl(state);

    // clear infamous map here
    _infamousMap.clear();
}

void AbstractStateProvider::onInitImpl(CurrentState& state,
                                       const TraceSet* traceSet)
{
    // implemented here so that it's not mandatory for concrete providers
}

void AbstractStateProvider::onFiniImpl(CurrentState& state)
{
    // implemented here so that it's not mandatory for concrete providers
}

bool AbstractStateProvider::registerEventCallback(const std::string& traceType,
                                                  const std::string& eventName,
                                                  const OnEventFunc& onEvent)
{
    const auto& tracesInfos = _curTraceSet->getTracesInfos();

    bool matchLatch = false;

    for (const auto& traceInfos : tracesInfos) {
        EventIdCallbackMap callbackMap;

        if (AbstractStateProvider::namesMatchSimple(traceType, traceInfos->getTraceType())) {
            for (const auto& eventNameIdPair : *traceInfos->getEventMap()) {
                if (AbstractStateProvider::namesMatchSimple(eventName, eventNameIdPair.first)) {

                    auto traceId = traceInfos->getId();
                    auto eventId = eventNameIdPair.second->getId();

                    if (!_infamousMap[traceId][eventId]) {
                        _infamousMap[traceId][eventId] = onEvent;
                        matchLatch = true;
                    }
                }
            }
        }
    }

    return matchLatch;
}

bool AbstractStateProvider::registerEventCallbackRegex(const std::string& traceTypeRe,
                                                       const std::string& eventNameRe,
                                                       const OnEventFunc& onEvent)
{
    // try building/compiling regular expressions now
    boost::regex traceTypeBre;
    boost::regex eventNameBre;

    try {
        traceTypeBre = traceTypeRe;
        eventNameBre = eventNameRe;
    } catch (const std::exception& ex) {
        return false;
    }

    // find matches
    const auto& tracesInfos = _curTraceSet->getTracesInfos();

    bool matchLatch = false;

    for (const auto& traceInfos : tracesInfos) {
        EventIdCallbackMap callbackMap;

        if (boost::regex_search(traceInfos->getTraceType(), traceTypeBre)) {
            for (const auto& eventNameIdPair : *traceInfos->getEventMap()) {
                if (boost::regex_search(eventNameIdPair.first, eventNameBre)) {
                    auto traceId = traceInfos->getId();
                    auto eventId = eventNameIdPair.second->getId();

                    if (!_infamousMap[traceId][eventId]) {
                        _infamousMap[traceId][eventId] = onEvent;
                        matchLatch = true;
                    }
                }
            }
        }
    }

    return matchLatch;
}

bool AbstractStateProvider::namesMatchSimple(const std::string& asked,
                                             const std::string& candidate)
{
    return asked.empty() || asked == candidate;
}

}
}
