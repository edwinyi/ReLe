/*
 * rele,
 *
 *
 * Copyright (C) 2015 Davide Tateo & Matteo Pirotta & Marcello Restelli
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
#ifndef BASISFUNCTIONS_H
#define BASISFUNCTIONS_H

#include <armadillo>

namespace ReLe
{

class BasisFunction
{
public:
	virtual double operator()(const arma::vec& input) = 0;

	/**
	 * @brief Write a complete description of the instance to
	 * a stream.
	 * @param out the output stream
	 */
	virtual void WriteOnStream(std::ostream& out) = 0;

	/**
	 * @brief Read the description of the basis function from
	 * a file and reset the internal state according to that.
	 * This function is complementary to WriteOnStream
	 * @param in the input stream
	 */
	virtual void ReadFromStream(std::istream& in) = 0;

	/**
	 * @brief Write the internal state to the stream.
	 * @see WriteOnStream
	 * @param out the output stream
	 * @param bf an instance of basis function
	 * @return the output stream
	 */
	friend std::ostream& operator<<(std::ostream& out, BasisFunction& bf)
	{
		bf.WriteOnStream(out);
		return out;
	}

	/**
	 * @brief Read the internal stream from a stream
	 * @see ReadFromStream
	 * @param in the input stream
	 * @param bf an instance of basis function
	 * @return the input stream
	 */
	friend std::istream& operator>>(std::istream& in, BasisFunction& bf)
	{
		bf.ReadFromStream(in);
		return in;
	}

	virtual ~BasisFunction()
	{
	}

};

class BasisFunctions: public std::vector<BasisFunction*>
{
public:
	BasisFunctions();
	virtual ~BasisFunctions();
	virtual arma::vec operator()(const arma::vec& input);
	virtual double dot(const arma::vec& input, const arma::vec &otherVector);

	/**
	 * Automatically generates polynomial basis functions up to the specified degree
	 * @param  degree The maximum degree of the polynomial
	 * @param  input_size Number of input dimensions
	 */
	void GeneratePolynomialBasisFunctions(unsigned int degree,
				unsigned int input_size);

	/**
	 * @brief Write the internal state to the stream.
	 * @see WriteOnStream
	 * @param out the output stream
	 * @param bf an instance of basis functions
	 * @return the output stream
	 */
	friend std::ostream& operator<<(std::ostream& out, BasisFunctions& bf);

	/**
	 * @brief Read the internal stream from a stream
	 * @see ReadFromStream
	 * @param in the input stream
	 * @param bf an instance of basis functions
	 * @return the input stream
	 */
	friend std::istream& operator>>(std::istream& in, BasisFunctions& bf);

private:

	void display(std::vector<unsigned int> v);

	/**
	 * Function to generate combinations
	 * @param  deg  Vector of polynomial degrees
	 * @param  dim  Vector that lists dimensions
	 * @param  place  position to be modified
	 */
	void GeneratePolynomials(std::vector<unsigned int> deg,
				std::vector<unsigned int>& dim, unsigned int place);

	/**
	 * Function to generate permutations
	 * @param  deg  Vector of polynomial degrees
	 * @param  dim  Vector that lists dimensions
	 */
	void GeneratePolynomialsPermutations(std::vector<unsigned int> deg,
				std::vector<unsigned int>& dim);
};

} //end namespace

#endif //BASISFUNCTIONS_H
