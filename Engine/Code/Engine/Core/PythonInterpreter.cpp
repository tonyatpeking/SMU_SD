#include "Engine/Core/PythonInterpreter.hpp"


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
    py::eval( "shell.push('" + text + "')" );
}

std::string PythonInterpreter::ReadFromShell()
{
    return py::eval( "outputCatcher.pop()" ).cast<std::string>();
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
}
