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

#include "MAB/SimpleMAB.h"


namespace ReLe
{

SimpleMAB::SimpleMAB(arma::vec P, arma::vec R, double gamma, unsigned int horizon) :
    P(P),
    R(R),
    MAB(gamma, horizon)
{
    EnvironmentSettings& task = getWritableSettings();
    task.finiteActionDim = P.n_elem;
    task.continuosActionDim = 0;
}

SimpleMAB::SimpleMAB(arma::vec P, double r, double gamma, unsigned int horizon) :
    P(P),
    MAB(gamma, horizon)
{
    R = arma::vec(P.n_elem, arma::fill::ones) * r;

    EnvironmentSettings& task = getWritableSettings();
    task.finiteActionDim = P.n_elem;
    task.continuosActionDim = 0;
}

SimpleMAB::SimpleMAB(unsigned int nArms, double r, double gamma, unsigned int horizon) :
    MAB(gamma, horizon)
{
    P = arma::randu(nArms);
    R = arma::vec(P.n_elem, arma::fill::ones) * r;

    EnvironmentSettings& task = getWritableSettings();
    task.finiteActionDim = nArms;
    task.continuosActionDim = 0;
}

SimpleMAB::SimpleMAB(unsigned int nArms, double gamma, unsigned int horizon) :
    MAB(gamma, horizon)
{
    P = arma::randu(nArms);
    R = arma::randn(nArms);

    EnvironmentSettings& task = getWritableSettings();
    task.finiteActionDim = nArms;
    task.continuosActionDim = 0;
}

void SimpleMAB::step(const FiniteAction& action, FiniteState& nextState, Reward& reward)
{
    nextState.setStateN(0);
    reward[0] = 0;

    if(RandomGenerator::sampleEvent(P(action)))
        reward[0] += R(action);
    else
        reward[0] += 0;
}

}
