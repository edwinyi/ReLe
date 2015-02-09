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

#ifndef CONTINUOUSMDP_H_
#define CONTINUOUSMDP_H_

#include "Envirorment.h"

namespace ReLe
{

class ContinuousMDP: public Envirorment<DenseAction, DenseState>
{
public:
    ContinuousMDP(std::size_t stateSize, std::size_t actionSize, std::size_t rewardSize, bool isFiniteHorizon,
                  bool isEpisodic, double gamma = 1.0, unsigned int horizon = 0);

private:
    void setupEnvirorment(std::size_t stateSize, std::size_t actionSize, std::size_t rewardSize,
                          bool isFiniteHorizon, bool isEpisodic, unsigned int horizon,
                          double gamma);

protected:
    DenseState currentState;

};

}

#endif /* CONTINUOUSMDP_H_ */
