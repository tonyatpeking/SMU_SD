#pragma once
#include <string>

#include "ThirdParty/pybind11/embed.h"
namespace py = pybind11;
using namespace pybind11::literals;

class PythonInterpreter
{
public:
    static PythonInterpreter* GetInstance();

    void Start();
    void Stop();

    void PushToShell(std::string text);
    std::string ReadFromShell();

private:

    PythonInterpreter() {};
    ~PythonInterpreter();

    void StartShell();

    bool m_started = false;

};
