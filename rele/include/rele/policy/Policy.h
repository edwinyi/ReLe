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

#ifndef POLICY_H_
#define POLICY_H_

#include "BasicsTraits.h"

namespace ReLe
{

template<class ActionC, class StateC>
class Policy
{
    static_assert(std::is_base_of<Action, ActionC>::value, "Not valid Action class as template parameter");
    static_assert(std::is_base_of<State, StateC>::value, "Not a valid State class as template parameter");

public:
    virtual typename action_type<ActionC>::type operator() (const typename state_type<StateC>::type state) = 0;
    virtual double operator() (const typename state_type<StateC>::type state, const typename action_type<ActionC>::type action) = 0;

    virtual ~Policy()
    {

    }
};

template<class ActionC, class StateC>
class NonParametricPolicy: public Policy<ActionC, StateC>
{

};

template<class ActionC, class StateC>
class ParametricPolicy: public Policy<ActionC, StateC>
{
public:
    arma::vec diff(const typename state_type<StateC>::type state, const typename action_type<ActionC>::type action) = 0;
    arma::vec difflog(const typename state_type<StateC>::type state, const typename action_type<ActionC>::type action) = 0;

    inline const arma::vec& getParameters() const
    {
        return w;
    }

protected:
    arma::vec w;

};

template<class StateC>
class ActionValuePolicy: public NonParametricPolicy<FiniteAction, StateC>
{

};


}

#endif /* POLICY_H_ */
