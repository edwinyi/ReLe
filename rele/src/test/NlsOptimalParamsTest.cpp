/*
 * rele,
 *
 *
 * Copyright (C) 2015  Davide Tateo & Matteo Pirotta
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

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cmath>

#include "NLS.h"
#include "DifferentiableNormals.h"
#include "Core.h"
#include "parametric/differentiable/NormalPolicy.h"
#include "BasisFunctions.h"
#include "basis/PolynomialFunction.h"
#include "RandomGenerator.h"
#include "PolicyEvalAgent.h"

using namespace std;
using namespace ReLe;
using namespace arma;

int main(int argc, char *argv[])
{
    NLS mdp;
    //with these settings
    //max in ( -3.58, 10.5 ) -> J = 8.32093
    //note that there are multiple optimal solutions
    //TODO: verificare, serve interfaccia core per valutare una politica

    int dim = mdp.getSettings().continuosStateDim;
    cout << "dim: " << dim << endl;

    //--- define policy (low level)
    DenseBasisVector basis;
    basis.generatePolynomialBasisFunctions(1,dim);
    delete basis.at(0);
    basis.erase(basis.begin());
    cout << "--- Mean regressor ---" << endl;
    cout << basis << endl;
    LinearApproximator meanRegressor(dim, basis);

    DenseBasisVector stdBasis;
    stdBasis.generatePolynomialBasisFunctions(1,dim);
    delete stdBasis.at(0);
    stdBasis.erase(stdBasis.begin());
    cout << "--- Standard deviation regressor ---" << endl;
    cout << stdBasis << endl;
    LinearApproximator stdRegressor(dim, stdBasis);
    arma::vec stdWeights(stdRegressor.getParametersSize());
    stdWeights.fill(0.5);
    stdRegressor.setParameters(stdWeights);


    NormalStateDependantStddevPolicy policy(&meanRegressor, &stdRegressor);
    //---

    PolicyEvalAgent<DenseAction, DenseState, NormalStateDependantStddevPolicy> agent(policy);
    ReLe::Core<DenseAction, DenseState> core(mdp, agent);
    core.getSettings().episodeLenght = mdp.getSettings().horizon;

    ofstream out("NLS_OptParamSpace.dat", ios_base::out);

    arma::vec w(2);
    if (out.is_open())
    {
        for (double p1 = -10; p1 < 0; p1 += 0.1)
        {
            w[0] = p1;
            for (double p2 = 0; p2 < 15; p2 += 0.1)
            {
                w[1] = p2;
                policy.setParameters(w);
                int testEpisodes = 1000;
                arma::vec J = core.runBatchTest(testEpisodes);
                out << p1 << "\t" << p2 <<  "\t" << J[0] << std::endl;
                cout << p1 << "\t" << p2 <<  "\t" << J[0] << std::endl;
            }
        }
        out.close();
    }
    return 0;
}