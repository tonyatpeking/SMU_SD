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

    static void CloseShell();

private:

    PythonInterpreter() {};
    ~PythonInterpreter();

    void StartShell();
    void InitPath();
    void BindQuit();

    bool m_started = false;

    py::object m_shellPush;

};

