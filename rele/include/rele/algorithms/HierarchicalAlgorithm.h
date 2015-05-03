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

#ifndef INCLUDE_RELE_ALGORITHMS_HIERARCHICALALGORITHM_H_
#define INCLUDE_RELE_ALGORITHMS_HIERARCHICALALGORITHM_H_

#include "Agent.h"
#include "Options.h"

namespace ReLe
{

template<class ActionC, class StateC>
class HierarchicalAlgorithm : public Agent<ActionC, StateC>
{
public:
    HierarchicalAlgorithm(Option<ActionC, StateC>& rootOption)
    {
        stack.push_back(&rootOption);
    }

    virtual void sampleAction(const StateC& state, ActionC& action)
    {
    	checkMultipleOptionTermination(state);
        sampleOptionAction(state, action);
    }

    virtual void initEpisode(const StateC& state, ActionC& action) = 0;

    virtual void step(const Reward& reward, const StateC& nextState,
                      ActionC& action) = 0;

    virtual void endEpisode(const Reward& reward) = 0;
    virtual void endEpisode() = 0;

    virtual ~HierarchicalAlgorithm()
    {

    }

protected:
    void sampleOptionAction(const StateC& state, ActionC& action)
    {

        auto& currentOption = *stack.back();

        if(currentOption.getOptionType() == Normal)
        {
            stack.push_back(&currentOption(state));
            sampleOptionAction(state, action);
        }
        else
        {
            currentOption(state, action);
        }
    }

    bool checkOptionTermination(const StateC& state)
    {
        auto& currentOption = *stack.back();

        if(currentOption.hasEnded(state))
        {
        	if(stack.size() > 1)
        		stack.pop_back();
            return true;
        }

        return false;
    }

    void checkMultipleOptionTermination(const StateC& state)
    {
        if(checkOptionTermination(state) && stack.size() > 1)
        {
            checkMultipleOptionTermination(state);
        }
    }

protected:
    OptionStack<ActionC, StateC> stack;
};


}

#endif /* INCLUDE_RELE_ALGORITHMS_HIERARCHICALALGORITHM_H_ */