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

#include "Rocky.h"
#include "policy_search/REPS/EpisodicREPS.h"
#include "DifferentiableNormals.h"
#include "Core.h"
#include "parametric/differentiable/LinearPolicy.h"
#include "BasisFunctions.h"

#include <iostream>

using namespace std;
using namespace ReLe;
using namespace arma;

namespace ReLe
{
class RockyCircularPolicy : public ParametricPolicy<DenseAction, DenseState>
{
public:

    RockyCircularPolicy() : v(1)
    {

    }

    //Policy
    virtual vec operator() (const vec& state)
    {
        vec u(3);
        u[0] = 10;
        u[1] = M_PI;
        u[2] = 0;

        return u;
    }
    virtual double operator() (const vec& state, const vec& action)
    {
        return 0;
    }

    virtual std::string getPolicyName()
    {
        return "Circular fake";
    }

    virtual std::string getPolicyHyperparameters()
    {
        return "";
    }

    virtual std::string printPolicy()
    {
        return "";
    }

    //ParametricPolicy
    virtual const arma::vec& getParameters() const
    {
        return v;
    }

    virtual const unsigned int getParametersSize() const
    {
        return 1;
    }

    virtual void setParameters(arma::vec& w)
    {

    }


private:
    vec v;
};

}

int main(int argc, char *argv[])
{
    Rocky rocky;

    //Low level policy
    /*int dim = rocky.getSettings().continuosStateDim;
    DenseBasisVector basis;
    basis.generatePolynomialBasisFunctions(1, dim);
    LinearApproximator regressor(dim, basis);

    DetLinearPolicy<DenseState> policy(&regressor);*/
    RockyCircularPolicy policy;


    //high level policy
    int dimPolicy = policy.getParametersSize();
    arma::vec mean(dimPolicy);
    arma::mat cov(dimPolicy, dimPolicy, arma::fill::eye);

    ParametricNormal dist(mean, cov);


    EpisodicREPS agent(dist, policy);

    Core<DenseAction, DenseState> core(rocky, agent);

    /*int episodes = 40;
    for (int i = 0; i < episodes; i++)
    {
        core.getSettings().episodeLenght = 100000;
        cout << "starting episode" << endl;
        core.runEpisode();
    }*/

    core.getSettings().episodeLenght = 100000;
    core.getSettings().loggerStrategy = new WriteStrategy<DenseAction, DenseState>("/home/dave/prova.txt");
    cout << "starting episode" << endl;
    core.runTestEpisode();

    return 0;

}