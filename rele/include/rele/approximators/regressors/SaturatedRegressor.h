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

#ifndef INCLUDE_RELE_APPROXIMATORS_REGRESSORS_SATURATEDREGRESSOR_H_
#define INCLUDE_RELE_APPROXIMATORS_REGRESSORS_SATURATEDREGRESSOR_H_

#include "Features.h"
#include <armadillo>
#include <vector>
#include <cassert>
#include "Regressors.h"

namespace ReLe
{

template<class InputC, bool denseOutput = true>
class SaturatedRegressor_: public ParametricRegressor_<InputC, denseOutput>
{
public:
    SaturatedRegressor_(Features_<InputC, denseOutput>& bfs, const arma::vec& uMin, const arma::vec& uMax)
        : ParametricRegressor_<InputC>(bfs.cols()), basis(bfs), uMin(uMin), uMax(uMax),
          parameters(bfs.rows(), arma::fill::zeros)
    {
    }

    ~SaturatedRegressor_()
    {
    }

    arma::vec operator()(const InputC& input)
    {
        arma::vec weights = arma::exp(basis(input).t()*parameters);
        weights = weights/(weights + 1);

        return (uMax - uMin)%weights + uMin;
    }

    arma::vec diff(const InputC& input)
    {
        arma::mat features = basis(input);
        arma::vec weights = arma::exp(features.t()*parameters);
        arma::mat output = features*(weights/arma::square(weights +1))*arma::diagmat(uMax - uMin);
        return vectorise(output);
    }

    inline Features& getBasis()
    {
        return basis;
    }

    inline arma::vec getParameters() const
    {
        return parameters;
    }

    inline void setParameters(const arma::vec& params)
    {
        assert(params.n_elem == parameters.n_elem);
        parameters = params;
    }

    inline unsigned int getParametersSize() const
    {
        return parameters.n_elem;
    }

private:
    arma::vec parameters;
    Features_<InputC, denseOutput>& basis;
    arma::vec uMin;
    arma::vec uMax;

};

typedef SaturatedRegressor_<arma::vec> SaturatedRegressor;

}

#endif /* INCLUDE_RELE_APPROXIMATORS_REGRESSORS_SATURATEDREGRESSOR_H_ */
