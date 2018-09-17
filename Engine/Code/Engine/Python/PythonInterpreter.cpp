#include "Engine/Python/PythonInterpreter.hpp"
#include "Engine/Core/Console.hpp"

PYBIND11_EMBEDDED_MODULE( zzzUtils, m ) {
    // `m` is a `py::module` which is used to bind functions and classes
    m.def( "quit", PythonInterpreter::CloseShell );
}


PythonInterpreter* PythonInterpreter::GetInstance()
{
    static PythonInterpreter* s_instance = new PythonInterpreter();
    return s_instance;
}

void PythonInterpreter::Start()
{
    if( !m_started )
    {
        py::initialize_interpreter();
        StartShell();
        InitPath();
        BindQuit();
        m_started = true;
    }
}

void PythonInterpreter::Stop()
{
    if( m_started )
    {
        py::finalize_interpreter();
        m_started = false;
    }
}

void PythonInterpreter::PushToShell( std::string text )
{
    py::object pyText = py::cast( text );
    m_shellPush( pyText );
}

std::string PythonInterpreter::ReadFromShell()
{
    return py::eval( "outputCatcher.pop()" ).cast<std::string>();
}

void PythonInterpreter::CloseShell()
{
    Console::GetDefault()->UsePython( false );
    LOG_INFO_TAG( "Console", "Leaving Python" );
}

PythonInterpreter::~PythonInterpreter()
{
    Stop();
}

void PythonInterpreter::StartShell()
{
    py::exec( R"(
import sys
import code

shell = code.InteractiveConsole(globals())

class OutputCatcher:
    def __init__(self):
        self.value = ''
    def write(self, txt):
        self.value += txt
    def pop(self):
        temp = self.value
        self.value = ''
        return temp

outputCatcher = OutputCatcher()
sys.stdout = outputCatcher
sys.stderr = outputCatcher

    )" );

    m_shellPush = py::globals()["shell"].attr( "push" );
}

void PythonInterpreter::InitPath()
{
    py::exec( R"(
import sys
import os

sys.path.insert( 1, os.getcwd() + '\\Data\\Scripts' )

    )" );
}

void PythonInterpreter::BindQuit()
{
    py::exec( R"(
import zzzUtils
quit = zzzUtils.quit
    )" );
}
