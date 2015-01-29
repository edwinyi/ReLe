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

#include "q_policy/ActionValuePolicy.h"

namespace ReLe
{

class e_Greedy: public ActionValuePolicy<FiniteState>
{
public:
    e_Greedy();
    virtual ~e_Greedy();

    virtual int operator() (const int state);
    virtual double operator() (const int state, const int action);

    inline void setEpsilon(double eps)
    {
        this->eps = eps;
    }

    inline double getEpsilon()
    {
        return this->eps;
    }

protected:
    double eps;
};

class e_GreedyApproximate: public ActionValuePolicy<DenseState>
{
public:
    e_GreedyApproximate();
    virtual ~e_GreedyApproximate();

    virtual int operator() (const arma::vec& state);
    virtual double operator() (const arma::vec& state, const int action);

    inline void setEpsilon(double eps)
    {
        this->eps = eps;
    }

    inline double getEpsilon()
    {
        return this->eps;
    }

protected:
    double eps;

};

}
#endif /* E_GREEDY_H_ */