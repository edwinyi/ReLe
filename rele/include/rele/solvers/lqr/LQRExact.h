/*
 * rele,
 *
 *
 * Copyright (C) 2016 Davide Tateo
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

#ifndef INCLUDE_RELE_SOLVERS_LQR_LQREXACT_H_
#define INCLUDE_RELE_SOLVERS_LQR_LQREXACT_H_

#include <armadillo>
#include "rele/environments/LQR.h"

namespace ReLe
{

class LQRExact
{
public:
    LQRExact(double gamma, arma::mat A,
             arma::mat B,
             std::vector<arma::mat> Q,
             std::vector<arma::mat> R,
             arma::vec x0);

    LQRExact(LQR& lqr);

    arma::mat computeP(const arma::mat& K, unsigned int r = 0);

    arma::mat riccatiRHS(const arma::vec& k, const arma::mat& P, unsigned int r);

    arma::mat computeJ(const arma::vec& k, const arma::mat& Sigma);
    arma::mat computeGradient(const arma::vec& k, const arma::mat& Sigma, unsigned int r = 0);
    arma::mat computeJacobian(const arma::vec& k, const arma::mat& Sigma);
    arma::mat computeHesian(const arma::vec& k, const arma::mat& Sigma, unsigned int r = 0);


private:
    arma::mat computeM(const arma::mat& K);
    arma::mat compute_dM(const arma::mat& K, unsigned int i);
    arma::mat computeHM(unsigned int i, unsigned int j);

    arma::mat computeL(const arma::mat& K, unsigned int r);
    arma::mat compute_dL(const arma::mat& K, unsigned int r, unsigned int i);
    arma::mat computeHL(unsigned int r, unsigned int i, unsigned int j);

    inline arma::vec to_vec(const arma::mat& m)
    {
    	arma::vec v = reshape(m, n_dim*n_dim, 1);
        return v;
    }

    inline arma::mat to_mat(const arma::vec& v)
    {
    	arma::mat M = reshape(v, n_dim, n_dim);
        return M;
    }

private:
    unsigned int n_rewards;
    unsigned int n_dim;
    double gamma;
    arma::mat A;
    arma::mat B;
    std::vector<arma::mat> Q;
    std::vector<arma::mat> R;
    arma::vec x0;
};

}

#endif /* INCLUDE_RELE_SOLVERS_LQR_LQREXACT_H_ */
