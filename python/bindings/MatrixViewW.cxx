// Optima is a C++ library for solving linear and non-linear constrained optimization problems
//
// Copyright (C) 2014-2018 Allan Leal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

// pybind11 includes
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
namespace py = pybind11;

// pybindx includes
#include "pybindx.hpp"
namespace pyx = pybindx;

// Optima includes
#include <Optima/MatrixViewW.hpp>
using namespace Optima;

void exportMatrixViewW(py::module& m)
{
    py::class_<MatrixViewW>(m, "MatrixViewW")
        .def(py::init<MatrixConstRef4py, MatrixConstRef4py, MatrixConstRef4py, MatrixConstRef4py, MatrixConstRef4py, MatrixConstRef4py>(),
            pyx::keep_argument_alive<0>(),
            pyx::keep_argument_alive<1>(),
            pyx::keep_argument_alive<2>(),
            pyx::keep_argument_alive<3>(),
            pyx::keep_argument_alive<4>(),
            pyx::keep_argument_alive<5>())
        .def_readonly("Wx" , &MatrixViewW::Wx)
        .def_readonly("Wp" , &MatrixViewW::Wp)
        .def_readonly("Ax" , &MatrixViewW::Ax)
        .def_readonly("Ap" , &MatrixViewW::Ap)
        .def_readonly("Jx" , &MatrixViewW::Jx)
        .def_readonly("Jp" , &MatrixViewW::Jp)
        ;
}
