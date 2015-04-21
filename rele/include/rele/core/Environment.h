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

#ifndef ENVIRORMENT_H_
#define ENVIRORMENT_H_

#include <vector>

#include "Basics.h"

namespace ReLe
{

template<class ActionC, class StateC>
class Environment
{
    static_assert(std::is_base_of<Action, ActionC>::value, "Not valid Action class as template parameter");
    static_assert(std::is_base_of<State, StateC>::value, "Not a valid State class as template parameter");
public:

    Environment()
        : settings(new EnvironmentSettings()), clearEnvSettings(true)
    {
    }

    Environment(EnvironmentSettings* settings, bool clear = false)
        : settings(settings), clearEnvSettings(clear)
    {
    }

    virtual void step(const ActionC& action, StateC& nextState,
                      Reward& reward) = 0;
    virtual void getInitialState(StateC& state) = 0;

    inline const EnvironmentSettings& getSettings() const
    {
        return *settings;
    }

    virtual ~Environment()
    {
        if (clearEnvSettings)
            delete settings;
    }

protected:
    inline EnvironmentSettings& getWritableSettings()
    {
        return *settings;
    }

protected:
    EnvironmentSettings* settings;
    bool clearEnvSettings;
};

}

#endif /* ENVIRORMENT_H_ */
