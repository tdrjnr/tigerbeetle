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
#include <memory>
#include <string>
#include <set>
#include <vector>
#include <sstream>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>

#include <common/trace/TraceSet.hpp>
#include <common/stateprov/StateProviderConfig.hpp>
#include <common/ex/WrongStateProvider.hpp>
#include "StateHistoryBuilder.hpp"
#include "ProgressPublisher.hpp"
#include "TraceDeck.hpp"
#include "Arguments.hpp"
#include "BuilderBeetle.hpp"
#include "ex/MqBindError.hpp"
#include "ex/UnknownStateProviderType.hpp"
#include "ex/InvalidArgument.hpp"
#include "ex/BuilderBeetleError.hpp"
#include "ex/StateProviderNotFound.hpp"

namespace bfs = boost::filesystem;

namespace tibee
{

BuilderBeetle::BuilderBeetle(const Arguments& args)
{
    // validate arguments as soon as possible (will throw if anything wrong)
    this->validateSaveArguments(args);
}

void BuilderBeetle::validateSaveArguments(const Arguments& args)
{
    // make sure all traces actually exist and create paths
    for (const auto& pathStr : args.traces) {
        auto tracePath = bfs::path {pathStr};

        // make sure this trace exists (at least, may still be invalid)
        if (!bfs::exists(tracePath)) {
            std::stringstream ss;

            ss << "trace " << tracePath << " does not exist";

            throw ex::InvalidArgument {ss.str()};
        }

        // append trace path
        _tracesPaths.push_back(tracePath);
    }

    // create default database output directory if not specified
    if (args.dbDir.empty()) {
        _dbDir = bfs::current_path() / "tibee";
    } else {
        _dbDir = args.dbDir;
    }

    // make sure the database directory doesn't exist, unless force is enabled
    if (!args.force && bfs::exists(_dbDir)) {
        std::stringstream ss;

        ss << "the specified database directory " <<
              _dbDir << " exists already" << std::endl <<
              "  (use -f to overwrite files)";

        throw ex::InvalidArgument {ss.str()};
    } else if (args.force && !bfs::is_directory(_dbDir)) {
        throw ex::InvalidArgument {"database directory has to be a directory"};
    }

    // create specified directory now
    bfs::create_directories(_dbDir);

    // extract instance names from state provider names and keep them
    boost::regex re {"([A-Za-z0-9_][A-Za-z0-9_-]*):(.+)"};

    for (const auto& fullStateProvider : args.stateProviders) {
        // has instance name?
        boost::smatch m;
        std::string instance;
        std::string name;

        auto hasInstance = boost::regex_match(fullStateProvider, m, re);

        if (hasInstance) {
            instance = m[1];
            name = m[2];
        } else {
            name = fullStateProvider;
        }

        _stateProviders.push_back({
            name,
            instance,
            common::StateProviderConfig::Params {}
        });
    }

    // make sure all state provider instance names are unique
    std::set<std::string> set;

    for (const auto& stateProviderConfig : _stateProviders) {
        const auto& instance = stateProviderConfig.getInstanceName();

        if (!instance.empty()) {
            if (set.find(instance) != set.end()) {
                std::stringstream ss;

                ss << "duplicate state provider instance name: \"" <<
                      instance << "\"";

                throw ex::InvalidArgument {ss.str()};
            }

            set.insert(instance);
        }
    }


    // bind address for progress publishing
    _bindProgress = args.bindProgress;
}

bool BuilderBeetle::run()
{
    // create a trace set
    std::unique_ptr<common::TraceSet> traceSet {new common::TraceSet};

    // add traces to trace set
    for (const auto& tracePath : _tracesPaths) {
        if (!traceSet->addTrace(tracePath)) {
            std::stringstream ss;

            ss << "could not add trace " << tracePath << " (internal error)";

            throw ex::BuilderBeetleError {ss.str()};
        }
    }

    // create a list of trace listeners
    std::vector<AbstractTracePlaybackListener::UP> listeners;

    // create a state history builder
    std::unique_ptr<StateHistoryBuilder> stateHistoryBuilder;
    try {
        stateHistoryBuilder = std::unique_ptr<StateHistoryBuilder> {
            new StateHistoryBuilder {
                _dbDir,
                _stateProviders
            }
        };
    } catch (const common::ex::WrongStateProvider& ex) {
        std::stringstream ss;

        ss << "wrong state provider: \"" << ex.getName() << "\"" << std::endl <<
              "  " << ex.what();

        throw ex::BuilderBeetleError {ss.str()};
    } catch (const ex::UnknownStateProviderType& ex) {
        std::stringstream ss;

        ss << "unknown state provider type: \"" << ex.getName() << "\"";

        throw ex::BuilderBeetleError {ss.str()};
    } catch (const ex::StateProviderNotFound& ex) {
        std::stringstream ss;

        ss << "cannot find state provider \"" << ex.getName() << "\"";

        throw ex::BuilderBeetleError {ss.str()};
    } catch (...) {
        throw ex::BuilderBeetleError {"unknown error"};
    }

    listeners.push_back(std::move(stateHistoryBuilder));

    // create a progress publisher
    if (!_bindProgress.empty()) {
        std::unique_ptr<ProgressPublisher> progressPublisher;
        try {
            progressPublisher = std::unique_ptr<ProgressPublisher> {
                new ProgressPublisher {
                    _bindProgress,
                    traceSet->getBegin(),
                    traceSet->getEnd(),
                    _tracesPaths,
                    _stateProviders,
                    stateHistoryBuilder.get(),
                    2801,
                    200
                }
            };
        } catch (const ex::MqBindError& ex) {
            std::stringstream ss;

            ss << "cannot bind to address \"" << ex.getBindAddr() << "\"";

            throw ex::BuilderBeetleError {ss.str()};
        }

        listeners.push_back(std::move(progressPublisher));
    }

    // ready for the deck
    return _traceDeck.play(traceSet.get(), listeners);
}

void BuilderBeetle::stop()
{
    _traceDeck.stop();
}

}
