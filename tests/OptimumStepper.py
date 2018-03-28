# Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
#
# Copyright (C) 2014-2018 Allan Leal
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

from optima import *
from numpy import *
from pytest import approx, mark

def test_optimum_stepper():
    n = 10
    m = 5

    A = eigen.random(m, n)

    structure = OptimumStructure(A)
    params = OptimumParams(structure)
    state = OptimumState()
    params.b = eigen.random(m)
    
    stepper = OptimumStepper(structure)
    
    stepper.decompose(params, state)
    stepper.solve(params, state)
    
    step = stepper.step().array()
    residual = stepper.residual().array()
    lhs = stepper.matrix().array()
    
    assert norm(lhs.dot(step) - residual) / norm(residual) == approx(0.0)
