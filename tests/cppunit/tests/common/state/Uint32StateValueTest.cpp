/* Copyright (c) 2014 Francois Doray <francois.pierre-doray@polymtl.ca>
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

#include <cppunit/extensions/HelperMacros.h>

#include <common/state/Uint32StateValue.hpp>

using namespace tibee::common;

class Uint32StateValueTest :
    public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(Uint32StateValueTest);
        CPPUNIT_TEST(testConstructorAndGetter);
        CPPUNIT_TEST(testCast);
        CPPUNIT_TEST(testType);
    CPPUNIT_TEST_SUITE_END();

public:
    void testConstructorAndGetter();
    void testCast();
    void testType();
};

CPPUNIT_TEST_SUITE_REGISTRATION(Uint32StateValueTest);

void Uint32StateValueTest::testConstructorAndGetter()
{
    const Uint32StateValue value(42);
    CPPUNIT_ASSERT_EQUAL(42u, value.getValue());
}

void Uint32StateValueTest::testCast()
{
    const Uint32StateValue value(42);
    CPPUNIT_ASSERT_EQUAL(&value, &value.asUint32Value());
    CPPUNIT_ASSERT_EQUAL(42u, value.asUint32());
}

void Uint32StateValueTest::testType()
{
    const Uint32StateValue value(42);
    CPPUNIT_ASSERT(StateValueType::UINT32 == value.getType());
    CPPUNIT_ASSERT(!value.isSint32());
    CPPUNIT_ASSERT(!value.isSint64());
    CPPUNIT_ASSERT(value.isUint32());
    CPPUNIT_ASSERT(!value.isUint64());
    CPPUNIT_ASSERT(!value.isFloat32());
    CPPUNIT_ASSERT(!value.isQuark());
    CPPUNIT_ASSERT(!value.isNull());
    CPPUNIT_ASSERT(static_cast<bool>(value));
}
