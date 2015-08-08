/*
 * rele,
 *
 *
 * Copyright (C) 2015 Davide Tateo & Matteo Pirotta
 * Versione 1.0
 *
 * This file is part of rele.
 *
 * rele is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rele is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with rele.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <nlopt.hpp>
#include <iostream>

double f(unsigned int n, const double* x, double* grad, void*)
{
    static int it = 0;

    double a = x[0];
    double b = x[1];
    double c = x[2];

    grad[0] = 2*a*b + 2*a*c + b*b + c*c + 1;
    grad[1] = a*a + c*c + 2*b*a + 2*b*c + 1;
    grad[2] = a*a + b*b + 2*c*a + 2*c*b + 1;

    std::cout << it++ << std::endl;

    std::cout << "J = " << a*a*b + a*a*c + b*b*a + b*b*c + c*c*a + c*c*b + a + b + c << std::endl;
    std::cout << "x = " << x[0] << ", " << x[1] << ", " << x[2] << std::endl;
    std::cout << "grad = " << grad[0] << ", " << grad[1] << ", " << grad[2] << std::endl;

    return a*a*b + a*a*c + b*b*a + b*b*c + c*c*a + c*c*b + a + b + c;

}

double g(unsigned int n, const double* x, double* grad, void*)
{
    static int it = 0;

    double a = x[0];
    double b = x[1];
    double c = x[2];

    grad[0] = 2*(a - 3)*std::exp((a - 3)*(a - 3) - b*b + c*c) - 2*(a - 3)*std::exp(-(a - 3)*(a - 3) + b*b - c*c + 3);
    grad[1] = -2*b*std::exp((a - 3)*(a - 3) - b*b + c*c) + 2*b*std::exp(-(a - 3)*(a - 3) + b*b - c*c + 3);
    grad[2] = 2*c*std::exp((a - 3)*(a - 3) - b*b + c*c) - 2*c*std::exp(-(a - 3)*(a - 3) + b*b - c*c + 3);

    std::cout << it++ << std::endl;

    std::cout << "J = " << std::exp((a - 3)*(a - 3) - b*b + c*c) + std::exp(-(a - 3)*(a - 3) + b*b - c*c + 3) << std::endl;
    std::cout << "x = " << x[0] << ", " << x[1] << ", " << x[2] << std::endl;
    std::cout << "grad = " << grad[0] << ", " << grad[1] << ", " << grad[2] << std::endl;

    return std::exp((a - 3)*(a - 3) - b*b + c*c) + std::exp(-(a - 3)*(a - 3) + b*b - c*c + 3);

}

double h(unsigned int n, const double* x, double* grad, void*)
{
    if(grad != nullptr)
    {
        grad[0] = 1;
        grad[1] = 1;
    }


    return x[0]+x[1];
}

double constraint(unsigned int n, const double *x, double *grad, void *)
{
    if(grad != nullptr)
    {
        grad[0] = 2*x[0];
        grad[1] = 2*x[1];
    }

    double val = x[0]*x[0]+x[1]*x[1]-1;
    std::cout << "val: " << val << std::endl;
    return val;
}

/*int main()
{
    nlopt::opt optimizator;
    int size = 3;
    optimizator = nlopt::opt(nlopt::algorithm::LD_LBFGS, size);
    //optimizator = nlopt::opt(nlopt::algorithm::LD_MMA, size);
    //optimizator.set_min_objective(g, NULL);
    optimizator.set_min_objective(f, NULL);
    optimizator.set_xtol_rel(1e-8);
    optimizator.set_ftol_rel(1e-12);
    optimizator.set_maxeval(5);

    std::vector<double> x(size, 10);
    double J;
    optimizator.optimize(x, J);

    std::cout << "x = " << x[0] << ", " << x[1] << ", " << x[2] << std::endl;
    std::cout << "J = " << J << std::endl;

}*/

int main()
{
    nlopt::opt optimizator;
    nlopt::opt localOptimizator;
    int size = 2;
    optimizator = nlopt::opt(nlopt::algorithm::LN_AUGLAG_EQ, size);
    optimizator.set_min_objective(h, nullptr);
    optimizator.set_xtol_rel(1e-8);
    optimizator.set_ftol_rel(1e-12);
    optimizator.add_equality_constraint(constraint, nullptr, 0.001);
    optimizator.set_maxeval(600);

    localOptimizator = nlopt::opt(nlopt::algorithm::LD_MMA, size);
    localOptimizator.set_xtol_rel(1e-8);
    localOptimizator.set_ftol_rel(1e-12);

    //optimizator.set_local_optimizer(localOptimizator);

    std::vector<double> x(size, 0);
    x[1] = 1;
    double J;

    try
    {
        optimizator.optimize(x, J);
    }
    catch(...)
    {

    }

    std::cout << "x = " << x[0] << ", " << x[1] << std::endl;
    std::cout << "J = " << J << std::endl;

}
