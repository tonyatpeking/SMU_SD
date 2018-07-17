#include "Engine/Core/ShapeRulesetLoader.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/PythonBindings.hpp"
#include "Engine/Core/PythonInterpreter.hpp"
#include "Engine/Core/ErrorUtils.hpp"

#include "ThirdParty/pybind11/embed.h"

namespace py = pybind11;
using namespace pybind11::literals;

//Internal
namespace
{
py::object LoadRuleset;
py::object DidFileChange;
py::object os;
}

void ShapeRulesetLoader::Init()
{
    GUARANTEE_OR_DIE( PythonInterpreter::GetInstance()->IsStarted(),
                      "PythonInterpreter must be started first!" );

    py::exec( R"(
import zzz.ruleset_loader
from zzz.ruleset_loader import LoadRuleset, DidFileChange
    )" );

    LoadRuleset = py::module::import( "zzz.ruleset_loader" ).attr( "LoadRuleset" );
    DidFileChange = py::module::import( "zzz.ruleset_loader" ).attr( "DidFileChange" );
    os = py::module::import( "os" );
    ;
}

void ShapeRulesetLoader::Load( const String& file )
{
    String msg = LoadRuleset( file ).cast<String>();
    Console::DefaultConsole()->Print( msg );
}

bool ShapeRulesetLoader::DidCurrentRuleChange()
{
    return DidFileChange().cast<bool>();

}
