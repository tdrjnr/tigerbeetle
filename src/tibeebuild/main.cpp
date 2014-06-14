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
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <boost/program_options.hpp>

#include "BuilderBeetle.hpp"
#include "Arguments.hpp"
#include "ex/InvalidArgument.hpp"
#include "ex/BuilderBeetleError.hpp"
#include "common/utils/print.hpp"

using tibee::common::tberror;
using tibee::common::tbendl;

namespace
{

/**
 * Parses the command line arguments passed to the program.
 *
 * @param argc Number of arguments in \p argv
 * @param argv Command line arguments
 * @param args Arguments values to fill
 *
 * @returns    0 to continue, 1 if there's a command line error
 */
int parseOptions(int argc, char* argv[], tibee::Arguments& args)
{
    namespace bpo = boost::program_options;

    bpo::options_description desc;

    desc.add_options()
        ("help,h", "help")
        ("traces", bpo::value<std::vector<std::string>>())
        ("verbose,v", bpo::bool_switch()->default_value(false))
        ("stateprov,s", bpo::value<std::vector<std::string>>())
        ("bind-progress,b", bpo::value<std::string>())
        ("db-dir,d", bpo::value<std::string>())
        ("force,f", bpo::bool_switch()->default_value(false))
    ;

    bpo::positional_options_description pos;

    pos.add("traces", -1);

    bpo::variables_map vm;

    try {
        auto cliParser = bpo::command_line_parser(argc, argv);
        auto parsedOptions = cliParser.options(desc).positional(pos).run();

        bpo::store(parsedOptions, vm);
    } catch (const std::exception& ex) {
        tberror() << "command line error: " << ex.what() << tbendl();
        return 1;
    }

    if (!vm["help"].empty()) {
        std::cout <<
            "usage: " << argv[0] << " [options] <trace path>..." << std::endl <<
            std::endl <<
            "options:" << std::endl <<
            std::endl <<
            "  -h, --help                  print this help message" << std::endl <<
            "  -b, --bind-progress <addr>  bind address for build progress (default: none)" << std::endl <<
            "  -d, --db-dir <path>         write database in this directory" << std::endl <<
            "                              (default: \"./tibee\")" << std::endl <<
            "  -f, --force                 force database writing, even if the output" << std::endl <<
            "                              directory already exists" << std::endl <<
            "  -s <provider path>          state provider file path (at least one)" << std::endl <<
            "  -v, --verbose               verbose" << std::endl;

        return -1;
    }

    try {
        vm.notify();
    } catch (const std::exception& ex) {
        tberror() << "command line error: " << ex.what() << tbendl();
        return 1;
    }

    // traces
    if (vm["traces"].empty()) {
        tberror() << "command line error: need at least one trace file to work with" << tbendl();
        return 1;
    }

    args.traces = vm["traces"].as<std::vector<std::string>>();

    // database directory
    if (!vm["db-dir"].empty()) {
        args.dbDir = vm["db-dir"].as<std::string>();
    }

    // state providers
    if (vm["stateprov"].empty()) {
        tberror() << "command line error: need at least one state provider to work with" << tbendl();
        return 1;
    }

    args.stateProviders = vm["stateprov"].as<std::vector<std::string>>();

    // bind progress
    if (!vm["bind-progress"].empty()) {
        args.bindProgress = vm["bind-progress"].as<std::string>();
    }

    // verbose
    args.verbose = vm["verbose"].as<bool>();

    // force
    args.force = vm["force"].as<bool>();

    return 0;
}

}

int main(int argc, char* argv[])
{
    tibee::Arguments args;

    int ret = parseOptions(argc, argv, args);

    if (ret < 0) {
        return 0;
    } else if (ret > 0) {
        return ret;
    }

    // create the builder beetle and run it
    try {
        std::unique_ptr<tibee::BuilderBeetle> builderBeetle {new tibee::BuilderBeetle {args}};

        return builderBeetle->run() ? 0 : 1;
    } catch (const tibee::ex::InvalidArgument& ex) {
        tberror() << "invalid argument: " << ex.what() << tbendl();
    } catch (const tibee::ex::BuilderBeetleError& ex) {
        tberror() << "build error: " << ex.what() << tbendl();
    } catch (const std::exception& ex) {
        tberror() << "unknown error: " << ex.what() << tbendl();
    }

    return 1;
}
