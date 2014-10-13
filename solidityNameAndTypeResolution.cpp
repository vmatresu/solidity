/*
    This file is part of cpp-ethereum.

    cpp-ethereum is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cpp-ethereum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @author Christian <c@ethdev.com>
 * @date 2014
 * Unit tests for the name and type resolution of the solidity parser.
 */

#include <string>

#include <libdevcore/Log.h>
#include <libsolidity/Scanner.h>
#include <libsolidity/Parser.h>
#include <libsolidity/NameAndTypeResolver.h>
#include <boost/test/unit_test.hpp>

namespace dev {
namespace solidity {
namespace test {

namespace {
	void parseTextAndResolveNames(const std::string& _source)
	{
		Parser parser;
		ptr<ContractDefinition> contract = parser.parse(
												std::make_shared<Scanner>(CharStream(_source)));
		NameAndTypeResolver resolver;
		resolver.resolveNamesAndTypes(*contract);
	}
}

BOOST_AUTO_TEST_SUITE(SolidityNameAndTypeResolution)

BOOST_AUTO_TEST_CASE(smoke_test)
{
	char const* text = "contract test {\n"
					   "  uint256 stateVariable1;\n"
					   "  function fun(uint256 arg1) { var x = 2; uint256 y = 3; x = 1; }"
					   "}\n";
	BOOST_CHECK_NO_THROW(parseTextAndResolveNames(text));
}

BOOST_AUTO_TEST_CASE(double_stateVariable_declaration)
{
	char const* text = "contract test {\n"
					   "  uint256 variable;\n"
					   "  uint128 variable;\n"
					   "}\n";
	BOOST_CHECK_THROW(parseTextAndResolveNames(text), std::exception);
}

BOOST_AUTO_TEST_CASE(double_function_declaration)
{
	char const* text = "contract test {\n"
					   "  function fun() { var x = 2; }\n"
					   "  function fun() { var y = 9; }\n"
					   "}\n";
	BOOST_CHECK_THROW(parseTextAndResolveNames(text), std::exception);
}

BOOST_AUTO_TEST_CASE(double_variable_declaration)
{
	char const* text = "contract test {\n"
					   "  function f() { uint256 x = 9; if (true)  { uint256 x = 2;} x = 3; }\n"
					   "}\n";
	BOOST_CHECK_THROW(parseTextAndResolveNames(text), std::exception);
}

BOOST_AUTO_TEST_CASE(name_shadowing)
{
	char const* text = "contract test {\n"
					   "  uint256 variable;\n"
					   "  function f() { uint8 variable = 2; }"
					   "}\n";
	BOOST_CHECK_NO_THROW(parseTextAndResolveNames(text));
}

BOOST_AUTO_TEST_CASE(name_references)
{
	char const* text = "contract test {\n"
					   "  uint256 variable;\n"
					   "  function f() { variable = 2; f(); test; }"
					   "}\n";
	BOOST_CHECK_NO_THROW(parseTextAndResolveNames(text));
}

BOOST_AUTO_TEST_CASE(undeclared_name)
{
	char const* text = "contract test {\n"
					   "  uint256 variable;\n"
					   "  function f() { notfound = 2; }"
					   "}\n";
	BOOST_CHECK_THROW(parseTextAndResolveNames(text), std::exception);
}

BOOST_AUTO_TEST_SUITE_END()

} } } // end namespaces

