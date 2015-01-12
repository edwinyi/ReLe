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

#include "FiniteMDP.h"
#include "td/SARSA.h"
#include "Core.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	const size_t statesNumber = 5;
	const size_t actionsNumber = 2;

	const double Rdata[actionsNumber][statesNumber][2] =
	{
				{
							{0,0},
							{0,0},
							{1,0},
							{0,0},
							{0,0}
				},
				{
							{0,0},
							{0,0},
							{1,0},
							{0,0},
							{0,0}
				}
	};

	const double Pdata[actionsNumber][statesNumber][statesNumber] =
	{
				{
							{0.2, 0.8, 0, 0, 0},
							{0, 0.2, 0.8, 0, 0},
							{0, 0, 0.2, 0.8, 0},
							{0, 0, 0, 0.2, 0.8},
							{0, 0, 0, 0, 1}
				},
				{
							{1, 0, 0, 0, 0},
							{0.8, 0.2, 0, 0, 0},
							{0, 0.8, 0.2, 0, 0},
							{0, 0, 0.8, 0.2, 0},
							{0, 0, 0, 0.8, 0.2}
				}
	};

	ReLe::FiniteMDP mdp(Pdata, Rdata);
	ReLe::SARSA_lambda agent(0.8);
	ReLe::Core<ReLe::FiniteAction, ReLe::FiniteState> core(mdp, agent);

	core.getSettings().episodeLenght = 300;
	core.runEpisode();

	//TODO levami
	ReLe::FiniteState x;
	ReLe::Action a;
	mdp.getInitialState(x);
	cout << "x: " << x.getStateN() << endl;

}
