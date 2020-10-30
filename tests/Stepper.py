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
from numpy.linalg import norm
from numpy.testing import assert_allclose
from pytest import approx, mark
from itertools import product

from utils.matrices import assemble_matrix_Ax, matrix_non_singular, pascal_matrix
from utils.stability import create_expected_stability, check_stability


# The number of primal variables x
nx = 6

# Tested number of parameter variables p
# tested_np = [0, 5]
tested_np = [0]

# Tested number of Lagrange multipliers y (i.e., number of rows in A = [Ax Ap])
# tested_ny = [4, 8]
tested_ny = [3]

# Tested number of Lagrange multipliers z (i.e., number of rows in J = [Jx Jp])
# tested_nz = [0, 5]
tested_nz = [0]

# Tested number of unstable/fixed basic variables
# tested_nbu = [0, 1, 2]
tested_nbu = [0]

# Tested number of linearly dependent rows in Ax
# tested_nl = [0, 1, 2]
tested_nl = [0]

# Tested cases for the indices of fixed variables
tested_ifixed = [
    [],
    # [1],
    # [1, 3, 7, 9]
]

# Tested cases for the indices of variables with lower bounds
tested_ilower = [
    [],
    # [1],
    # [1, 3, 7, 9],
    # list(range(nx))  # all x variables with lower bounds
]

# Tested cases for the indices of variables with upper bounds
tested_iupper = [
    [],
    # [1],
    # [1, 3, 7, 9],
    # list(range(nx))  # all x variables with upper bounds
]

# Tested cases for the saddle point methods
tested_methods = [
    SaddlePointMethod.Fullspace,
    # SaddlePointMethod.Nullspace,
    # SaddlePointMethod.Rangespace,
]

@mark.parametrize("np"    , tested_np)
@mark.parametrize("ny"    , tested_ny)
@mark.parametrize("nz"    , tested_nz)
@mark.parametrize("nbu"   , tested_nbu)
@mark.parametrize("nl"    , tested_nl)
@mark.parametrize("ifixed", tested_ifixed)
@mark.parametrize("ilower", tested_ilower)
@mark.parametrize("iupper", tested_iupper)
@mark.parametrize("method", tested_methods)
def test_active_stepper(np, ny, nz, nbu, nl, ifixed, ilower, iupper, method):

    # Due to a current limitation in the algorithm, if number of parameter
    # variables is non-zero and number of linearly dependent or number of basic
    # unstable variables is non-zero, skip the test.
    if np > 0 and nbu + nl > 0:
        return

    # Skip if there are no unstable/fixed variables, and the number of unstable
    # basic variables is non-zero
    if nbu > 0 and len(ifixed) == 0:
        return

    # Skip if there are more Lagrange multipliers than primal variables
    if nx < ny + nz:
        return

    # The number of Lagrange multipliers in w = (y, z)
    nw = ny + nz

    # Add the indices of fixed variables to those that have lower and upper bounds
    # since fixed variables are those that have identical lower and upper bounds
    ilower = list(set(ilower + ifixed))
    iupper = list(set(iupper + ifixed))

    # The number variables with finite lower and upper bounds, and fixed variables
    nlower = len(ilower)
    nupper = len(iupper)
    nfixed = len(ifixed)

    # The total number of variables x, p, z, y
    t = nx + np + nz + ny

    # Assemble the coefficient matrices Ax and Ap
    Ax = assemble_matrix_Ax(ny, nx, nbu, nl, ifixed)
    Ap = linspace(1, ny*np, ny*np).reshape((ny, np))

    # Assemble the coefficient matrix J = [Jx Jp]
    J = linspace(0, 1, nz*(nx + np)).reshape((nz, nx + np))

    # Extract the blocks of J = [Jx Jp]
    Jx = J[:, :nx]
    Jp = J[:, nx:]

    # Create vectors for the lower and upper bounds of the x variables
    xlower = full(nx, -inf)
    xupper = full(nx,  inf)

    # Create vectors for the lower and upper bounds of the p variables
    plower = full(np, -inf)
    pupper = full(np,  inf)

    # Set lower and upper bounds of x to negative and positive sequence respectively
    xlower[ilower] = -linspace(1.0, nlower, nlower) * 100
    xupper[iupper] =  linspace(1.0, nupper, nupper) * 100

    # Set lower and upper bounds to equal values for fixed x variables
    xlower[ifixed] = xupper[ifixed] = linspace(1, nfixed, nfixed) * 10

    # Create vectors x, p, z, y
    x = linspace(1.0, nx, nx)
    p = linspace(1.0, np, np)
    z = linspace(1.0, nz, nz)
    y = linspace(1.0, ny, ny)

    # Create the blocks Hxx, Hxp
    Hxx = matrix_non_singular(nx)
    Hxp = pascal_matrix(nx, np)

    # Create the blocks Vpx, Vpp
    Vpx = pascal_matrix(np, nx)
    Vpp = matrix_non_singular(np)

    # Ensure Hxx is diagonal in case Rangespace method is used
    if method == SaddlePointMethod.Rangespace:
        Hxx = abs(diag(linspace(1, nx, num=nx)))

    # Auxiliary functions to get head and tail of a sequence in a set
    head = lambda seq: [seq[ 0]] if len(seq) > 0 else []
    tail = lambda seq: [seq[-1]] if len(seq) > 0 else []

    # Set head and tail variables with lower/upper bounds to be unstable as well as all fixed variables
    iunstable_lower = list(set(head(ilower) + tail(ilower) + ifixed))
    iunstable_upper = list(set(head(iupper) + tail(iupper) + ifixed))
    iunstable = list(set(iunstable_lower + iunstable_upper))
    istable = list(set(range(nx)) - set(iunstable))

    # The number of unstable variables
    nu = len(iunstable)

    # Create vector u = (dx, dp, dy, dz) with the expected Newton step values
    u = u_expected = linspace(1.0, t, t)  # introduce `u` as an alias for `u_expected`

    # Get references to the segments dx, dp, dy, dz in u
    dx, dp, dy, dz = split(u, [nx, nx + np, nx + np + ny])

    # Set to zero the entries in dx corresponding to lower/upper unstable variables
    dx[iunstable] = 0.0

    # Set lower/upper unstable variables in x to their respective lower/upper bounds
    x[iunstable_lower] = xlower[iunstable_lower]
    x[iunstable_upper] = xupper[iunstable_upper]

    # Ensure rows/cols in Hxx, Hxp, Vpx take into account fixed/unstable variables
    Hxx[iunstable, :] = 0.0          # zero out rows in Hxx corresponding to fixed variables
    Hxx[:, iunstable] = 0.0          # zero out cols in Hxx corresponding to fixed variables
    Hxx[iunstable, iunstable] = 1.0  # set one to diagonal entries in Hxx corresponding to fixed variables

    Hxp[iunstable, :] = 0.0       # zero out rows in Hxx corresponding to fixed variables

    # The AxT = tr(Ax) and JxT = tr(Jx) matrix blocks in M
    AxT = copy(Ax.T)
    JxT = copy(Jx.T)

    AxT[iunstable, :] = 0.0      # zero out rows in AxT corresponding to fixed variables
    JxT[iunstable, :] = 0.0      # zero out rows in JxT corresponding to fixed variables

    # Create the zero blocks Opy, Opz, Oyy, Oyz, Ozy, Ozz
    Opy = zeros((np, ny))
    Opz = zeros((np, nz))
    Oyy = zeros((ny, ny))
    Oyz = zeros((ny, nz))
    Ozy = zeros((nz, ny))
    Ozz = zeros((nz, nz))

    # Assemble the coefficient matrix M
    M = block([
        [Hxx, Hxp, AxT, JxT],
        [Vpx, Vpp, Opy, Opz],
        [ Ax,  Ap, Oyy, Oyz],
        [ Jx,  Jp, Ozy, Ozz]]
    )

    # Compute the right-hand side vector r = M*u
    r = M @ u

    # Get references to rx, rp, ry, rz in r where rx := -(g + tr(Ax)*y + tr(Jx)*z), rp := -(v), ry := -(Ax*x + Ap*p - b), rz := -h
    rx, rp, ry, rz = split(r, [nx, nx + np, nx + np + ny])

    # Compute the gradient vector remembering that rx := -(g + tr(Ax)*y + tr(Jx)*z) for stable variables, zero otherwise
    g = -(rx + Ax.T @ y + Jx.T @ z)

    # Ensure g has values for lower/upper unstable variables that make these recognized as unstable indeed
    # Note that:
    #  - x[i] is lower unstable when x[i] == xlower[i] and z[i] > 0, where z = g + tr(W)*y
    #  - x[i] is upper unstable when x[i] == xupper[i] and z[i] < 0, where z = g + tr(W)*y
    g[iunstable_lower] = +1.0e+2
    g[iunstable_upper] = -1.0e+2

    # Compute the residual vector v of the external nonlinear constraints v(x, p) = 0 knowing that rp := -v
    v = -rp

    # Compute the residual vector h of the nonlinear equality constraints h(x, p) = 0 knowing that rz := -h
    h = -rz

    # Compute the right-hand side vector b remembering that ry = -(Ax*x + Ap*p - b)
    b = ry + Ax @ x + Ap @ p

    # The solution of the Newton step calculation
    dx   = zeros(nx)  # The Newton step for the primal variables *x*.
    dp   = zeros(np)  # The Newton step for the parameter variables *p*.
    dy   = zeros(ny)  # The Newton step for the Lagrange multipliers *y*.
    dz   = zeros(nz)  # The Newton step for the Lagrange multipliers *z*.
    resx = zeros(nx)  # The residuals of the first-order optimality conditions.
    resp = zeros(np)  # The residuals of the external constraints v(x, p) = 0.
    resw = zeros(nw)  # The residuals of the linear and nonlinear constraints.
    errx = zeros(nx)  # The relative residual errors of the first-order optimality conditions.
    errp = zeros(np)  # The relative residual errors of the external constraints v(x, p) = 0.
    errw = zeros(nw)  # The relative residual errors of the linear and nonlinear constraints.
    s    = zeros(nx)  # The *stabilities* of the variables defined as s = g + tr(Jx)z + tr(Ax)y.

    # Create the stability state of the variables
    stability = Stability()

    # Set options for the Newton step calculation
    options = Options()
    options.kkt.method = method

    set_printoptions(linewidth=10000)
    print(f"Ax = \n{Ax}")
    print(f"Ap = \n{Ap}")

    # Create a Newton step calculator
    stepper = Stepper(nx, np, ny, nz, Ax, Ap)
    stepper.setOptions(options)

    # Initialize, decompose the saddle point matrix, and solve the Newton step
    stepper.initialize(b, xlower, xupper, plower, pupper, x, stability)
    stepper.canonicalize(x, p, y, z, g, Hxx, Hxp, Vpx, Vpp, Jx, Jp, xlower, xupper, plower, pupper, stability)
    stepper.residuals(x, p, y, z, b, h, v, g, Jx, resx, resp, resw, errx, errp, errw, s)
    stepper.decompose()
    stepper.solve(x, p, y, z, g, b, h, v, stability, dx, dp, dy, dz)

    #==============================================================
    # Compute the sensitivity derivatives of the optimal solution
    #==============================================================

    # The number of w parameters considered in the sensitivity calculations
    nw = 5

    # The expected solution of the sensitivity matrix dudw = [dxdw; dpdw; dydw; dzdw]
    dudw_expected = linspace(1.0, t*nw, t*nw).reshape((t, nw))

    # The block views in dudw = [dxdw; dpdw; dydw; dzdw]
    dxdw_expected, dpdw_expected, dydw_expected, dzdw_expected = vsplit(dudw_expected, [nx, nx + np, nx + np + ny])

    # Set rows in dxdw corresponding to unstable variables to zero (because dxudw === 0)
    dxdw_expected[iunstable, :] = 0.0

    # Compute the right-hand side matrix in M*dudw = drdw
    drdw_expected = M @ dudw_expected

    # The block views in drdw = [drxdw; drpdw; drydw; drzdw] = [-dgdw; -dvdw; dbdw; -dhdw]
    drxdw, drpdw, drydw, drzdw = vsplit(drdw_expected, [nx, nx + np, nx + np + ny])

    dgdw = -drxdw
    dvdw = -drpdw
    dbdw =  drydw
    dhdw = -drzdw

    # Set the rows of dgdw corresponding to unstable variables
    dgdw[iunstable, :] = linspace(1.0, nu*nw, nu*nw).reshape((nu, nw))

    # The actual/calculated sensitivity derivatives dudw = [dxdw; dpdw; dydw; dzdw] and dzdw
    dxdw = zeros((nx, nw))
    dpdw = zeros((np, nw))
    dydw = zeros((ny, nw))
    dzdw = zeros((nz, nw))
    dsdw = zeros((nx, nw))

    # Compute the sensitivity matrices dxdw, dpdw, dydw, dzdw, dsdw
    stepper.sensitivities(dgdw, dhdw, dbdw, dvdw, stability, dxdw, dpdw, dydw, dzdw, dsdw)

    # Assemble dudw = [dxdw; dpdw; dydw; dzdw] and compute drdw = M*dudw
    dudw = block([[dxdw], [dpdw], [dydw], [dzdw]])
    drdw = M @ dudw

    # Calculate the expected sensitivity derivatives dsdw = dgdw + tr(Ax)dydw + tr(Jx)dzdw
    dsdw_expected = dgdw + Ax.T @ dydw + Jx.T @ dzdw  # do not use dydw_expected here because it causes failure when there are basic fixed variables (degeneracy)!!!
    dsdw_expected[istable, :] = 0.0  # dzdw is zero for stable variables

    # Print state variables
    def print_state():
        ux, up, uy, uz = split(u, [nx, nx+np, nx+np+ny])
        set_printoptions(suppress=True, linewidth=1000, precision=3)
        print()
        print(f"nx          = {nx}")
        print(f"np          = {np}")
        print(f"ny          = {ny}")
        print(f"nz          = {nz}")
        print(f"nbu         = {nbu}")
        print(f"nl          = {nl}")
        print(f"ifixed      = {ifixed}")
        print(f"ilower      = {ilower}")
        print(f"iupper      = {iupper}")
        print(f"method      = {method}")
        print()
        print(f"M = \n{M}")
        print(f"dx(actual)     = {dx}")
        print(f"dx(expected)   = {ux}")
        print(f"dp(actual)     = {dp}")
        print(f"dp(expected)   = {up}")
        print(f"dy(actual)     = {dy}")
        print(f"dy(expected)   = {uy}")
        print(f"dz(actual)     = {dz}")
        print(f"dz(expected)   = {uz}")
        print(f"dxdw(actual)   = \n{dxdw}")
        print(f"dxdw(expected) = \n{dxdw_expected}")
        print(f"dpdw(actual)   = \n{dpdw}")
        print(f"dpdw(expected) = \n{dpdw_expected}")
        print(f"dydw(actual)   = \n{dydw}")
        print(f"dydw(expected) = \n{dydw_expected}")
        print(f"dzdw(actual)   = \n{dzdw}")
        print(f"dzdw(expected) = \n{dzdw_expected}")
        print(f"dsdw(actual)   = \n{dsdw}")
        print(f"dsdw(expected) = \n{dsdw_expected}")
        print()

    # Compare the actual and expected Newton steps dx, dp, dy, dz
    u_actual = concatenate([dx, dp, dy, dz])

    print_state()

    assert_allclose(M @ u_actual, r)

    # Compare the actual and expected sensitivity derivatives dxdw, dpdw, dydw, dzdw, dsdw
    # assert_allclose(drdw_expected, drdw)
    # assert_allclose(dsdw_expected, dsdw)

    # # Get the canonicalization matrix R
    # R = stepper.info().R  # TODO: Stepper has no info method yet.

    # # The expected optimality and feasibility residuals
    # resx_expected = abs(rx)
    # resy_expected = abs(R * ry)  # The feasibility residuals in canonical form!
    # z_expected = g + W.T @ y

    # assert_allclose(resx, resx_expected)
    # assert_allclose(resy, resy_expected)
    # assert_allclose(z, z_expected)

    # Create a Stability object with expected state
    # expected_stability = create_expected_stability(Ax, Ap, x, p, b, s, xlower, xupper, plower, pupper)

    # check_stability(stability, expected_stability)
