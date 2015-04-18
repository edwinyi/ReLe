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

#include "Core.h"
#include "parametric/differentiable/LinearPolicy.h"
#include "BasisFunctions.h"
#include "DifferentiableNormals.h"
#include "basis/PolynomialFunction.h"

#include "LQR.h"
#include "PolicyEvalAgent.h"
#include "algorithms/MWAL.h"
#include "policy_search/PGPE/PGPE.h"
#include "ParametricRewardMDP.h"

using namespace std;
using namespace ReLe;
using namespace arma;


class MWALBasis : public BasisFunction
{

public:
    MWALBasis(unsigned int index, double max, double min) :
        index(index), min(min), max(max)
    {

    }

    virtual double operator()(const arma::vec& input)
    {
        double value = input[index];
        if(value > min && value < max)
            return +1.0;
        else
            return 0;
    }

    virtual void writeOnStream(std::ostream& out)
    {

    }

    virtual void readFromStream(std::istream& in)
    {

    }

private:
    unsigned int index;
    double min;
    double max;
};

int main(int argc, char *argv[])
{
    /* Learn lqr correct policy */
    LQR mdp(1,1); //with these settings the optimal value is -0.6180 (for the linear policy)
    vec initialState(1);
    initialState[0] = -5;
    mdp.setInitialState(initialState);

    PolynomialFunction* pf = new PolynomialFunction(1,1);
    DenseFeatures basis(pf);
    LinearApproximator expertRegressor(mdp.getSettings().continuosStateDim, basis);
    DetLinearPolicy<DenseState> expertPolicy(&expertRegressor);
    vec param(1);
    param[0] = -0.6180;
    expertPolicy.setParameters(param);
    PolicyEvalAgent<DenseAction, DenseState> expert(expertPolicy);

    /* Generate LQR expert dataset */
    Core<DenseAction, DenseState> expertCore(mdp, expert);
    CollectorStrategy<DenseAction, DenseState> collection;
    expertCore.getSettings().loggerStrategy = &collection;
    expertCore.getSettings().episodeLenght = 50;
    expertCore.getSettings().testEpisodeN = 1000;
    expertCore.runTestEpisodes();


    /* Learn weight with MWAL */

    //Create features vector
    BasisFunctions rewardBF;
    for(int i = 0; i < 3; i++)
    {
        double mean = 2.5*i;
        double delta = 1.25;
        double min = mean + delta;
        double max = mean - delta;
        MWALBasis* bfP = new MWALBasis(0, min, max);
        rewardBF.push_back(bfP);

        if(i != 0)
        {
            MWALBasis* bfN = new MWALBasis(0, -max, -min);
            rewardBF.push_back(bfN);
        }
    }

    DenseFeatures rewardPhi(rewardBF);

    LinearApproximator rewardRegressor(mdp.getSettings().continuosStateDim, rewardPhi);


    //Compute expert feature expectations
    arma::vec muE = collection.data.computefeatureExpectation(rewardPhi, mdp.getSettings().gamma);

    //Create an agent to solve the mdp direct problem
    DetLinearPolicy<DenseState> policy(&expertRegressor);
    int nparams = basis.rows();
    arma::vec mean(nparams, fill::zeros);
    expertRegressor.setParameters(mean);

    int nbepperpol = 1, nbpolperupd = 100;
    arma::mat cov(nparams, nparams, arma::fill::eye);
    cov *= 0.1;
    ParametricNormal dist(mean, cov);
    AdaptiveStep steprule(0.1);
    PGPE<DenseAction, DenseState> agent(dist, policy, nbepperpol, nbpolperupd, steprule, true);

    //Setup the solver
    int nbUpdates = 600;
    int episodes  = nbUpdates*nbepperpol*nbpolperupd;

    IRLAgentSolver<DenseAction, DenseState> solver(agent, mdp, policy, rewardPhi, rewardRegressor);
    solver.setLearningParams(episodes, 50);
    solver.setTestParams(1000, 50);

    //Run MWAL
    unsigned int T = 30;

    MWAL<DenseAction, DenseState> irlAlg(T, muE, solver);
    irlAlg.run();
    arma::vec w = irlAlg.getWeights();

    cout << "Computed weights: " << endl << w.t() << endl;
    arma::vec meanFinal = dist.getMean();
    cout <<  "Policy learned" << endl << meanFinal.t() << endl;


    return 0;
}
