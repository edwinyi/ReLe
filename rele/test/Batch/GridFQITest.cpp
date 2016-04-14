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

#include "rele/core/Core.h"
#include "rele/core/BatchCore.h"
#include "rele/core/PolicyEvalAgent.h"
#include "rele/policy/q_policy/e_Greedy.h"
#include "rele/algorithms/batch/td/DoubleFQI.h"
#include "rele/algorithms/batch/td/W-FQI.h"
#include "rele/approximators/features/DenseFeatures.h"
#include "rele/core/FiniteMDP.h"
#include "rele/generators/GridWorldGenerator.h"
#include "rele/approximators/basis/IdentityBasis.h"
#include "rele/approximators/regressors/trees/KDTree.h"

#include <iostream>

using namespace std;
using namespace ReLe;
using namespace arma;

int main(int argc, char *argv[])
{
    GridWorldGenerator generator;
    generator.load(argv[1]);

    double gamma = 0.9;
    FiniteMDP&& mdp = generator.getMDP(gamma);

    unsigned int nActions = mdp.getSettings().actionsNumber;

    BasisFunctions bfs;
    bfs = IdentityBasis::generate(2);

    DenseFeatures phi(bfs);

    arma::vec defaultValue = {0};
    EmptyTreeNode<arma::vec> defaultNode(defaultValue);
    KDTree<arma::vec, arma::vec> QRegressorA(phi, defaultNode, 1, 1);
    KDTree<arma::vec, arma::vec> QRegressorB(phi, defaultNode, 1, 1);

    FQI batchAgent(QRegressorA, nActions, 1e-8);
    //DoubleFQI<FiniteState> batchAgent(QRegressorA, QRegressorB, nActions, 1e-8);
    //FiniteW_FQI batchAgent(QRegressorA, nStates, nActions, 1e-8);

    std::string fileName = "gw.log";
    FileManager fm("gw", "qLearning");
    ifstream is(fm.addPath(fileName));
    Dataset<FiniteAction, DenseState> data;
    data.readFromStream(is);
    is.close();

    auto&& core = buildBatchOnlyCore(data, batchAgent);
    core.getSettings().maxBatchIterations = 1;

    core.run(mdp.getSettings());

    return 0;
}
