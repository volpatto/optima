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

// Optima includes
#include <Optima/MasterMatrix.hpp>
#include <Optima/MasterVector.hpp>
#include <Optima/ResidualVector.hpp>
using namespace Optima;

void exportResidualVector(py::module& m)
{
    auto update = [](ResidualVector& self,
        MasterMatrix const& M,
        VectorConstRef x,
        VectorConstRef p,
        VectorConstRef y,
        VectorConstRef z,
        VectorConstRef g,
        VectorConstRef v,
        VectorConstRef b,
        VectorConstRef h)
    {
        self.update({M, x, p, y, z, g, v, b, h});
    };

    py::class_<ResidualVector>(m, "ResidualVector")
        .def(py::init<Index, Index, Index, Index>())
        .def(py::init<const ResidualVector&>())
        .def("update", update)
        .def("canonicalVector", &ResidualVector::canonicalVector)
        ;
}
