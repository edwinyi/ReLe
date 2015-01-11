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

#ifndef RANDOMGENERATOR_H_
#define RANDOMGENERATOR_H_

#include <boost/random.hpp>

namespace ReLe
{
class RandomGenerator
{
public:
	inline static double sampleNormal()
	{
		boost::random::normal_distribution<> dist;
		return dist(gen);
	}

	inline static double sampleNormal(double m, double sigma)
	{
		boost::random::normal_distribution<> dist(m, sigma);
		return dist(gen);
	}

	/*inline static double sampleUniform()
	{
		boost::random::uniform_01<> dist();
		return dist(gen);
	}*/

	inline static double sampleUniform(const double lo, const double hi)
	{
		boost::random::uniform_real_distribution<> dist(lo, hi);
		return dist(gen);
	}

	inline static int sampleUniformInt(const int lo, const int hi)
	{
		boost::random::uniform_int_distribution<> dist(lo, hi);
		return dist(gen);
	}

	inline static int sampleDiscrete(std::vector<double>& prob)
	{
		boost::random::discrete_distribution<> dist(prob.begin(), prob.end());
		return dist(gen);
	}

private:
	//random generators
	static boost::random::mt19937 gen;

};

}


#endif /* RANDOMGENERATOR_H_ */
