#include "Engine/ShapeGrammar/ShapeRulesetLoader.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Python/PythonInterpreter.hpp"
#include "Engine/Core/ErrorUtils.hpp"
#include "Engine/GameObject/GameObject.hpp"
#include "Engine/GameObject/Transform.hpp"
#include "Engine/String/StringUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat4.hpp"

#include "ThirdParty/pybind11/embed.h"
#include "ThirdParty/pybind11/operators.h"
#include "ThirdParty/pybind11/stl.h"

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


    py::exec( R"(
from zzzEngine import *
    )" );
    ;
}

void ShapeRulesetLoader::Load( const string& file )
{
    string msg = LoadRuleset( file ).cast<string>();
    LOG_INFO_TAG("ShapeRule", msg.c_str() )
}

bool ShapeRulesetLoader::DidCurrentRuleChange()
{
    return DidFileChange().cast<bool>();
}


PYBIND11_EMBEDDED_MODULE( zzzEngine, m ) {


    py::class_<GameObject, std::unique_ptr<GameObject, py::nodelete>>( m, "GameObject" )
        .def( py::init<const string&>() )
        .def( "GetTransform", py::overload_cast<>( &GameObject::GetTransform ), py::return_value_policy::reference_internal )
        .def( "SetParent", &GameObject::SetParent<GameObject> )
        .def( "SetParentKeepWorldTransform", &GameObject::SetParentKeepWorldTransform<GameObject> )
        .def( "SetShouldDie", &GameObject::SetShouldDie )
        .def( "SetType", &GameObject::SetType )
        .def( "GetType", &GameObject::GetType )
        .def( "SetVisible", &GameObject::SetVisible )
        .def( "IsVisible", &GameObject::IsVisible );


    py::class_<Vec3>( m, "Vec3" )
        .def( py::init<float, float, float>() )
        .def_readwrite( "x", &Vec3::x )
        .def_readwrite( "y", &Vec3::y )
        .def_readwrite( "z", &Vec3::z )
        .def( py::self + py::self )
        .def( py::self - py::self )
        .def( py::self * float() )
        .def( py::self / float() )
        .def( py::self /= float() )
        .def( py::self += py::self )
        .def( py::self -= py::self )
        .def( py::self *= float() )
        .def( float() * py::self )
        .def( py::self * py::self )
        .def( py::self == py::self )
        .def( py::self != py::self )
        .def( "__repr__", &Vec3::ToString );

    m.def( "MakeCube", &GameObject::MakeCube, "side_len"_a = Vec3::ONES, "pivot"_a = Vec3::ZEROS );

    py::class_<Mat4>( m, "Mat4" )
        .def( py::init<>() )
        .def( py::self * py::self )
        .def( py::self * Vec3() )
        .def( py::self == py::self )
        .def( py::self != py::self )
        .def( "TransformPosition", &Mat4::TransformPosition )
        .def( "TransformDisplacement", &Mat4::TransformDisplacement )
        .def( "GetRotationalPart", &Mat4::GetRotationalPart )
        .def( "DecomposeEuler", &Mat4::DecomposeEuler )
        .def( "DecomposeScale", &Mat4::DecomposeScale )
        .def( "IsAnyInf", &Mat4::IsAnyInf )
        .def( "IsAnyNaN", &Mat4::IsAnyNaN )
        .def( "Invert", &Mat4::Invert )
        .def( "Inverse", &Mat4::Inverse )
        .def( "Transpose", &Mat4::Transpose )
        .def( "Transposed", &Mat4::Transposed )
        .def( "Translate", &Mat4::Translate )
        .def( "Scale", &Mat4::Scale )
        .def( "__repr__", &Mat4::ToString );

    py::class_<Transform>( m, "Transform" )
        .def( py::init<>() )
        .def( "SetIdentity", &Transform::SetIdentity )
        .def( "GetForward", &Transform::GetForward )
        .def( "GetUp", &Transform::GetUp )
        .def( "GetRight", &Transform::GetRight )
        .def( "GetLocalForward", &Transform::GetLocalForward )
        .def( "GetLocalUp", &Transform::GetLocalUp )
        .def( "GetLocalRight", &Transform::GetLocalRight )
        .def( "GetLocalToParent", &Transform::GetLocalToParent, py::return_value_policy::reference_internal )
        .def( "SetLocalToParent", &Transform::SetLocalToParent )
        .def( "GetLocalToWorld", &Transform::GetLocalToWorld, py::return_value_policy::reference_internal )
        .def( "GetWorldToParent", &Transform::GetWorldToParent, py::return_value_policy::reference_internal )
        .def( "GetParentToWorld", &Transform::GetParentToWorld, py::return_value_policy::reference_internal )
        .def( "GetWorldToLocal", &Transform::GetWorldToLocal, py::return_value_policy::reference_internal )
        .def( "GetParentToLocal", &Transform::GetParentToLocal, py::return_value_policy::reference_internal )

        .def( "GetLocalPosition", &Transform::GetLocalPosition )
        .def( "SetLocalPosition", &Transform::SetLocalPosition )
        .def( "TranslateLocal", &Transform::TranslateLocal )

        .def( "GetWorldPosition", &Transform::GetWorldPosition )
        .def( "SetWorldPosition", &Transform::SetWorldPosition )
        .def( "TranslateWorld", &Transform::TranslateWorld )

        .def( "GetLocalEuler", &Transform::GetLocalEuler )
        .def( "SetLocalEuler", &Transform::SetLocalEuler )
        .def( "RotateLocalEuler", &Transform::RotateLocalEuler )

        .def( "GetWorldEuler", &Transform::GetWorldEuler )
        .def( "SetWorldEuler", &Transform::SetWorldEuler )
        .def( "RotateWorldEuler", &Transform::RotateWorldEuler )

        .def( "GetLocalScale", &Transform::GetLocalScale )
        .def( "SetLocalScale", &Transform::SetLocalScale )
        .def( "SetLocalScaleUniform", &Transform::SetLocalScaleUniform )
        .def( "GetWorldScale", &Transform::GetWorldScale )

        .def( "LookAt", &Transform::LookAt )
        .def( "HasParent", &Transform::HasParent )
        .def( "HasChildren", &Transform::HasChildren )
        .def( "GetParent", &Transform::GetParent, py::return_value_policy::reference_internal )
        .def( "GetChildren", &Transform::GetChildren, py::return_value_policy::reference_internal )

        .def( "SetParentKeepWorldTransform", &Transform::SetParentKeepWorldTransform )
        .def( "SetParent", &Transform::SetParent );


}