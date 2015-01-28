/*
 * rele,
 *
 *
 * Copyright (C) 2015 Davide Tateo
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

#ifndef E_GREEDY_H_
#define E_GREEDY_H_

#include "Policy.h"
#include "Approximators.h"

namespace ReLe
{

class e_Greedy: public ActionValuePolicy<FiniteState>
{
public:
    e_Greedy(arma::mat* Q);
    virtual ~e_Greedy();

    virtual int operator() (int state);
    virtual double operator() (int state, int action);

    void setEpsilon(double eps)
    {
        this->eps = eps;
    }

    double getEpsilon()
    {
        return this->eps;
    }

protected:
    arma::mat* Q;
    double eps;
};

class e_GreedyApproximate: public ActionValuePolicy<DenseState>
{
public:
    e_GreedyApproximate(Regressor* Q, unsigned int nactions);
    virtual ~e_GreedyApproximate();

    virtual int operator() (arma::vec& state);
    virtual double operator() (arma::vec& state, int action);

    void setEpsilon(double eps)
    {
        this->eps = eps;
    }

    double getEpsilon()
    {
        return this->eps;
    }

protected:
    Regressor* Q;
    double eps;
    unsigned int nactions;
};

}
#endif /* E_GREEDY_H_ */
