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
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main()
{
    // top level test suite from the registry
    auto& registry = CppUnit::TestFactoryRegistry::getRegistry();
    auto suite = registry.makeTest();

    // add test suite to new text UI runner
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(suite);

    // change the default outputter to a compiler error format outputter
    auto compilerOutput = new CppUnit::CompilerOutputter(&runner.result(),
                                                         std::cerr);
    runner.setOutputter(compilerOutput);

    // run all tests
    bool success = runner.run();

    return success ? 0 : 1;
}
