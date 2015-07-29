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

#ifndef PGIRL_H_
#define PGIRL_H_

#include "IRLAlgorithm.h"
#include "Policy.h"
#include "Transition.h"
#include "algorithms/GIRL.h" //TODO togliere, sola dipendenza gradient type
#include "ArmadilloExtensions.h"

#include <nlopt.hpp>
#include <cassert>

namespace ReLe
{

template<class ActionC, class StateC>
class PlaneGIRL : public IRLAlgorithm<ActionC, StateC>
{
public:

    PlaneGIRL(Dataset<ActionC,StateC>& dataset,
              DifferentiablePolicy<ActionC,StateC>& policy,
              BasisFunctions& rewardBasis,
              double gamma, IRLGradType aType)
        : policy(policy), data(dataset), rewardBasis(rewardBasis),
          gamma(gamma), atype(aType)
    {
        nbFunEvals = 0;
    }

    virtual ~PlaneGIRL()
    {

    }

    virtual arma::vec getWeights()
    {
        return weights;
    }

    virtual Policy<ActionC, StateC>* getPolicy()
    {
        return &policy;
    }

    void setData(Dataset<ActionC,StateC>& dataset)
    {
        data = dataset;
    }


    arma::vec ReinforceGradient(BasisFunction& rewardf)
    {
        int dp  = policy.getParametersSize();
        arma::vec sumGradLog(dp), localg;
        arma::vec gradient_J(dp, arma::fill::zeros);
        double Rew;

        int nbEpisodes = data.size();
        for (int i = 0; i < nbEpisodes; ++i)
        {
            //core setup
            int nbSteps = data[i].size();


            // *** REINFORCE CORE *** //
            sumGradLog.zeros();
            double df = 1.0;
            Rew = 0.0;
            // ********************** //

            //iterate the episode
            for (int t = 0; t < nbSteps; ++t)
            {
                Transition<ActionC, StateC>& tr = data[i][t];
                //            std::cout << tr.x << " " << tr.u << " " << tr.xn << " " << tr.r[0] << std::endl;

                // *** REINFORCE CORE *** //
                localg = policy.difflog(tr.x, tr.u);
                sumGradLog += localg;
                Rew += df * rewardf(vectorize(tr.x, tr.u, tr.xn));
                // ********************** //

                df *= gamma;

                if (tr.xn.isAbsorbing())
                {
                    assert(nbSteps == t+1);
                    break;
                }
            }

            // *** REINFORCE CORE *** //
            for (int p = 0; p < dp; ++p)
            {
                gradient_J[p] += Rew * sumGradLog(p);
            }
            // ********************** //

        }
        // compute mean values
        gradient_J /= nbEpisodes;

        return gradient_J;
    }

    arma::vec ReinforceBaseGradient(BasisFunction& rewardf)
    {
        int dp  = policy.getParametersSize();
        int nbEpisodes = data.size();

        arma::vec sumGradLog(dp), localg;
        arma::vec gradient_J(dp, arma::fill::zeros);
        double Rew;

        arma::vec baseline_J_num(dp, arma::fill::zeros);
        arma::vec baseline_den(dp, arma::fill::zeros);
        arma::vec return_J_ObjEp(nbEpisodes);
        arma::mat sumGradLog_CompEp(dp,nbEpisodes);

        for (int i = 0; i < nbEpisodes; ++i)
        {
            //core setup
            int nbSteps = data[i].size();


            // *** REINFORCE CORE *** //
            sumGradLog.zeros();
            double df = 1.0;
            Rew = 0.0;
            // ********************** //

            //iterate the episode
            for (int t = 0; t < nbSteps; ++t)
            {
                Transition<ActionC, StateC>& tr = data[i][t];
                //            std::cout << tr.x << " " << tr.u << " " << tr.xn << " " << tr.r[0] << std::endl;

                // *** REINFORCE CORE *** //
                localg = policy.difflog(tr.x, tr.u);
                for (int p = 0; p < dp; ++p)
                    assert(!isinf(localg(p)));
                sumGradLog += localg;
                Rew += df * rewardf(vectorize(tr.x, tr.u, tr.xn));
                // ********************** //

                df *= gamma;

                if (tr.xn.isAbsorbing())
                {
                    assert(nbSteps == t+1);
                    break;
                }
            }

            // *** REINFORCE BASE CORE *** //

            // store the basic elements used to compute the gradients

            return_J_ObjEp(i) = Rew;

            for (int p = 0; p < dp; ++p)
            {
                sumGradLog_CompEp(p,i) = sumGradLog(p);
            }

            // compute the baselines
            for (int p = 0; p < dp; ++p)
            {
                baseline_J_num(p) += Rew * sumGradLog(p) * sumGradLog(p);
                baseline_den(p) += sumGradLog(p) * sumGradLog(p);
                assert(!isinf(baseline_J_num(p)));
            }

            // ********************** //

        }

        // *** REINFORCE BASE CORE *** //

        // compute the gradients
        for (int p = 0; p < dp; ++p)
        {

            double baseline_J = 0;
            if (baseline_den(p) != 0)
            {
                baseline_J = baseline_J_num(p) / baseline_den(p);
            }

            for (int ep = 0; ep < nbEpisodes; ++ep)
            {
                double a =return_J_ObjEp(ep);
                double b = sumGradLog_CompEp(p,ep);
                assert(!isnan(a));
                assert(!isnan(b));
                assert(!isnan(baseline_J));
                gradient_J[p] += (return_J_ObjEp(ep) - baseline_J) * sumGradLog_CompEp(p,ep);
            }
        }

        // ********************** //

        // compute mean values
        gradient_J /= nbEpisodes;

        return gradient_J;
    }

    arma::vec GpomdpGradient(BasisFunction& rewardf)
    {
        int dp  = policy.getParametersSize();
        arma::vec sumGradLog(dp), localg;
        arma::vec gradient_J(dp, arma::fill::zeros);
        double Rew;

        int nbEpisodes = data.size();
        for (int i = 0; i < nbEpisodes; ++i)
        {
            //core setup
            int nbSteps = data[i].size();


            // *** GPOMDP CORE *** //
            sumGradLog.zeros();
            double df = 1.0;
            Rew = 0.0;
            // ********************** //

            //iterate the episode
            for (int t = 0; t < nbSteps; ++t)
            {
                Transition<ActionC, StateC>& tr = data[i][t];
                //            std::cout << tr.x << " " << tr.u << " " << tr.xn << " " << tr.r[0] << std::endl;

                // *** GPOMDP CORE *** //
                localg = policy.difflog(tr.x, tr.u);
                sumGradLog += localg;
                double creward = rewardf(vectorize(tr.x, tr.u, tr.xn));
                Rew += df * creward;

                // compute the gradients
                Rew += df * creward;
                for (int p = 0; p < dp; ++p)
                {
                    gradient_J[p] += df * creward * sumGradLog(p);
                }
                // ********************** //

                df *= gamma;

                if (tr.xn.isAbsorbing())
                {
                    assert(nbSteps == t+1);
                    break;
                }
            }

        }
        // compute mean values
        gradient_J /= nbEpisodes;

        return gradient_J;
    }

    arma::vec GpomdpBaseGradient(BasisFunction& rewardf)
    {
        int dp  = policy.getParametersSize();
        int nbEpisodes = data.size();

        int maxSteps = 0;
        for (int i = 0; i < nbEpisodes; ++i)
        {
            int nbSteps = data[i].size();
            if (maxSteps < nbSteps)
                maxSteps = nbSteps;
        }

        arma::vec sumGradLog(dp), localg;
        arma::vec gradient_J(dp, arma::fill::zeros);
        double Rew;

        arma::mat baseline_J_num(dp, maxSteps, arma::fill::zeros);
        arma::mat baseline_den(dp, maxSteps, arma::fill::zeros);
        arma::mat reward_J_ObjEpStep(nbEpisodes, maxSteps);
        arma::cube sumGradLog_CompEpStep(dp,nbEpisodes, maxSteps);
        arma::vec  maxsteps_Ep(nbEpisodes);

        for (int ep = 0; ep < nbEpisodes; ++ep)
        {
            //core setup
            int nbSteps = data[ep].size();


            // *** GPOMDP CORE *** //
            sumGradLog.zeros();
            double df = 1.0;
            Rew = 0.0;
            // ********************** //

            //iterate the episode
            for (int t = 0; t < nbSteps; ++t)
            {
                Transition<ActionC, StateC>& tr = data[ep][t];
                //            std::cout << tr.x << " " << tr.u << " " << tr.xn << " " << tr.r[0] << std::endl;

                // *** GPOMDP CORE *** //
                localg = policy.difflog(tr.x, tr.u);
                sumGradLog += localg;

                // store the basic elements used to compute the gradients
                double creward = rewardf(vectorize(tr.x, tr.u, tr.xn));
                Rew += df * creward;
                reward_J_ObjEpStep(ep,t) = df * creward;


                for (int p = 0; p < dp; ++p)
                {
                    sumGradLog_CompEpStep(p,ep,t) = sumGradLog(p);
                }

                // compute the baselines
                for (int p = 0; p < dp; ++p)
                {
                    baseline_J_num(p,t) += df * creward * sumGradLog(p) * sumGradLog(p);
                }

                for (int p = 0; p < dp; ++p)
                {
                    baseline_den(p,t) += sumGradLog(p) * sumGradLog(p);
                }
                // ********************** //

                df *= gamma;

                if (tr.xn.isAbsorbing())
                {
                    assert(nbSteps == t+1);
                    break;
                }
            }

            // store the actual length of the current episode (<= maxsteps)
            maxsteps_Ep(ep) = nbSteps;

        }

        // *** GPOMDP BASE CORE *** //

        // compute the gradients
        for (int p = 0; p < dp; ++p)
        {
            for (int ep = 0; ep < nbEpisodes; ++ep)
            {
                for (int t = 0; t < maxsteps_Ep(ep); ++t)
                {

                    double baseline_J = 0;
                    if (baseline_den(p,t) != 0)
                    {
                        baseline_J = baseline_J_num(p,t) / baseline_den(p,t);
                    }

                    gradient_J[p] += (reward_J_ObjEpStep(ep,t) - baseline_J) * sumGradLog_CompEpStep(p,ep,t);
                }
            }
        }
        // ************************ //

        // compute mean values
        gradient_J /= nbEpisodes;

        return gradient_J;
    }

    arma::vec NaturalGradient(BasisFunction& rewardf)
    {
        int dp  = policy.getParametersSize();
        arma::vec localg;
        arma::mat fisher(dp,dp, arma::fill::zeros);

        int nbEpisodes = data.size();
        for (int i = 0; i < nbEpisodes; ++i)
        {
            //core setup
            int nbSteps = data[i].size();

            //iterate the episode
            for (int t = 0; t < nbSteps; ++t)
            {
                Transition<ActionC, StateC>& tr = data[i][t];
                //            std::cout << tr.x << " " << tr.u << " " << tr.xn << " " << tr.r[0] << std::endl;

                // *** eNAC CORE *** //
                localg = policy.difflog(tr.x, tr.u);
                fisher += localg * localg.t();
                // ********************** //

                if (tr.xn.isAbsorbing())
                {
                    assert(nbSteps == t+1);
                    break;
                }
            }

        }
        fisher /= nbEpisodes;

        arma::vec gradient;
        if (atype == IRLGradType::NATR)
        {
            gradient = ReinforceGradient(rewardf);
        }
        else if (atype == IRLGradType::NATRB)
        {
            gradient = ReinforceBaseGradient(rewardf);
        }
        else if (atype == IRLGradType::NATG)
        {
            gradient = GpomdpGradient(rewardf);
        }
        else if (atype == IRLGradType::NATGB)
        {
            gradient = GpomdpBaseGradient(rewardf);
        }

        arma::vec nat_grad;
        int rnk = arma::rank(fisher);
        //        std::cout << rnk << " " << fisher << std::endl;
        if (rnk == fisher.n_rows)
        {
            nat_grad = arma::solve(fisher, gradient);
        }
        else
        {
            std::cerr << "WARNING: Fisher Matrix is lower rank (rank = " << rnk << ")!!! Should be " << fisher.n_rows << std::endl;

            arma::mat H = arma::pinv(fisher);
            nat_grad = H * gradient;
        }

        return nat_grad;
    }

    arma::vec ENACGradient(BasisFunction& rewardf)
    {
        int dp  = policy.getParametersSize();
        arma::vec localg;
        double Rew;
        arma::vec g(dp+1, arma::fill::zeros), phi(dp+1);
        arma::mat fisher(dp+1,dp+1, arma::fill::zeros);
        //        double Jpol = 0.0;

        int nbEpisodes = data.size();
        for (int i = 0; i < nbEpisodes; ++i)
        {
            //core setup
            int nbSteps = data[i].size();


            // *** eNAC CORE *** //
            double df = 1.0;
            Rew = 0.0;
            phi.zeros();
            //    #ifdef AUGMENTED
            phi(dp) = 1.0;
            //    #endif
            // ********************** //

            //iterate the episode
            for (int t = 0; t < nbSteps; ++t)
            {
                Transition<ActionC, StateC>& tr = data[i][t];
                //            std::cout << tr.x << " " << tr.u << " " << tr.xn << " " << tr.r[0] << std::endl;

                // *** eNAC CORE *** //
                localg = policy.difflog(tr.x, tr.u);
                double creward = rewardf(vectorize(tr.x, tr.u, tr.xn));
                Rew += df * creward;

                //Construct basis functions
                for (unsigned int i = 0; i < dp; ++i)
                    phi[i] += df * localg[i];
                // ********************** //

                df *= gamma;

                if (tr.xn.isAbsorbing())
                {
                    assert(nbSteps == t+1);
                    break;
                }
            }

            fisher += phi * phi.t();
            g += Rew * phi;

        }


        arma::vec nat_grad;
        int rnk = arma::rank(fisher);
        //        std::cout << rnk << " " << fisher << std::endl;
        if (rnk == fisher.n_rows)
        {
            nat_grad = arma::solve(fisher, g);
        }
        else
        {
            std::cerr << "WARNING: Fisher Matrix is lower rank (rank = " << rnk << ")!!! Should be " << fisher.n_rows << std::endl;

            arma::mat H = arma::pinv(fisher);
            nat_grad = H * g;
        }

        return nat_grad.rows(0,dp-1);
    }

    arma::vec ENACBaseGradient(BasisFunction& rewardf)
    {
        int dp  = policy.getParametersSize();
        arma::vec localg;
        double Rew;
        arma::vec g(dp+1, arma::fill::zeros), eligibility(dp+1, arma::fill::zeros), phi(dp+1);
        arma::mat fisher(dp+1,dp+1, arma::fill::zeros);
        double Jpol = 0.0;

        int nbEpisodes = data.size();
        for (int i = 0; i < nbEpisodes; ++i)
        {
            //core setup
            int nbSteps = data[i].size();


            // *** eNAC CORE *** //
            double df = 1.0;
            Rew = 0.0;
            phi.zeros();
            //    #ifdef AUGMENTED
            phi(dp) = 1.0;
            //    #endif
            // ********************** //

            //iterate the episode
            for (int t = 0; t < nbSteps; ++t)
            {
                Transition<ActionC, StateC>& tr = data[i][t];
                //            std::cout << tr.x << " " << tr.u << " " << tr.xn << " " << tr.r[0] << std::endl;

                // *** eNAC CORE *** //
                localg = policy.difflog(tr.x, tr.u);
                double creward = rewardf(tr.x, tr.u, tr.xn);
                Rew += df * creward;

                //Construct basis functions
                for (unsigned int i = 0; i < dp; ++i)
                    phi[i] += df * localg[i];
                // ********************** //

                df *= gamma;

                if (tr.xn.isAbsorbing())
                {
                    assert(nbSteps == t+1);
                    break;
                }
            }

            Jpol += Rew;
            fisher += phi * phi.t();
            g += Rew * phi;
            eligibility += phi;

        }

        // compute mean value
        fisher /= nbEpisodes;
        g /= nbEpisodes;
        eligibility /= nbEpisodes;
        Jpol /= nbEpisodes;

        arma::vec nat_grad;
        int rnk = arma::rank(fisher);
        //        std::cout << rnk << " " << fisher << std::endl;
        if (rnk == fisher.n_rows)
        {
            arma::mat tmp = arma::solve(nbEpisodes * fisher - eligibility * eligibility.t(), eligibility);
            arma::mat Q = (1 + eligibility.t() * tmp) / nbEpisodes;
            arma::mat b = Q * (Jpol - eligibility.t() * arma::solve(fisher, g));
            arma::vec grad = g - eligibility * b;
            nat_grad = arma::solve(fisher, grad);
        }
        else
        {
            std::cerr << "WARNING: Fisher Matrix is lower rank (rank = " << rnk << ")!!! Should be " << fisher.n_rows << std::endl;

            arma::mat H = arma::pinv(fisher);
            arma::mat b = (1 + eligibility.t() * arma::pinv(nbEpisodes * fisher - eligibility * eligibility.t()) * eligibility)
                          * (Jpol - eligibility.t() * H * g)/ nbEpisodes;
            arma::vec grad = g - eligibility * b;
            nat_grad = H * (grad);
        }

        return nat_grad.rows(0,dp-1);
    }

    virtual void run()
    {
        int dp = policy.getParametersSize();
        int dr = rewardBasis.size();
        std::vector<arma::vec> gradients(dr, arma::vec());
        arma::mat A(dp,dr);
        for (int r = 0; r < dr; ++r)
        {
            if (atype == IRLGradType::R)
            {
                gradients[r] = ReinforceGradient(*(rewardBasis[r]));
            }
            else if (atype == IRLGradType::RB)
            {
                gradients[r] = ReinforceBaseGradient(*(rewardBasis[r]));
            }
            else if (atype == IRLGradType::G)
            {
                gradients[r] = GpomdpGradient(*(rewardBasis[r]));
            }
            else if (atype == IRLGradType::GB)
            {
                gradients[r] = GpomdpBaseGradient(*(rewardBasis[r]));
            }
            else if (atype == IRLGradType::ENAC)
            {
                gradients[r] = ENACGradient(*(rewardBasis[r]));
            }
            else if ((atype == IRLGradType::NATR) || (atype == IRLGradType::NATRB) ||
                     (atype == IRLGradType::NATG) || (atype == IRLGradType::NATGB))
            {
                gradients[r] = NaturalGradient(*(rewardBasis[r]));
            }
            else
            {
                std::cerr << "PGIRL ERROR" << std::endl;
                abort();
            }
            A.col(r) = gradients[r];
        }

        A.save("/tmp/ReLe/grad.log", arma::raw_ascii);

        std::cout << "Grads: \n" << A << std::endl;

        ////////////////////////////////////////////////
        /// PRE-PROCESSING
        ////////////////////////////////////////////////
        arma::mat Ared;         //reduced gradient matrix
        arma::uvec nonZeroIdx;  //nonzero elements of the reward weights
        int rnkG = rank(A);
        if ( rnkG < dr && A.n_rows >= A.n_cols )
        {
            // select linearly independent columns
            arma::mat Asub;
            nonZeroIdx = rref(A, Asub);
            std::cout << "Asub: \n" << Asub << std::endl;
            std::cout << "idx: \n" << nonZeroIdx.t()  << std::endl;
            Ared = A.cols(nonZeroIdx);
            assert(rank(Ared) == Ared.n_cols);
            //            //save idxs to be set to zero
            //            arma::vec tmp(A.n_cols);
            //            std::iota (std::begin(tmp), std::end(tmp), 0);
            //            std::vector<int> diff;
            //            std::set_difference(tmp.begin(), tmp.end(), nonZeroIdx.begin(), nonZeroIdx.end(),
            //                                   std::inserter(diff, diff.begin()));
            //            zeroIdx.set_size(diff.size());
            //            for (unsigned int i = 0, ie = diff.size(); i < ie; ++i)
            //            {
            //                zeroIdx(i) = diff[i];
            //            }
        }
        else
        {
            Ared = A;
            nonZeroIdx.set_size(A.n_cols);
            std::iota (std::begin(nonZeroIdx), std::end(nonZeroIdx), 0);
        }


        Ared.save("/tmp/ReLe/gradRed.log", arma::raw_ascii);

        ////////////////////////////////////////////////
        /// GRAM MATRIX AND NORMAL
        ////////////////////////////////////////////////
        arma::mat gramMatrix = Ared.t() * Ared;
        //        std::cout << "Gram: \n" << gramMatrix << std::endl;
        //        arma::mat X(dr-1, dr);
        //        for (int r = 0; r < dr-1; ++r)
        //        {
        //            for (int r2 = 0; r2 < dr; ++r2)
        //            {
        //                X(r, r2) = gramMatrix(r2, r) - gramMatrix(r2, dr-1);
        //            }
        //        }
        unsigned int lastr = gramMatrix.n_rows;
        arma::mat X = gramMatrix.rows(0, lastr-2) - arma::repmat(gramMatrix.row(lastr-1), lastr-1, 1);
        //        std::cerr << std::endl << "X: " << X;
        X.save("/tmp/ReLe/GM.log", arma::raw_ascii);


        // COMPUTE NULL SPACE
        Y = null(X);
        std::cout << "Y: " << Y << std::endl;
        Y.save("/tmp/ReLe/NullS.log", arma::raw_ascii);


        // prepare the output
        // reset weights
        weights.zeros(A.n_cols);


        if (Y.n_cols > 1)
        {
            ////////////////////////////////////////////////
            /// POST-PROCESSING (IF MULTIPLE SOLUTIONS)
            ////////////////////////////////////////////////

            //setup optimization algorithm
            nlopt::opt optimizator;
            int nbOptParams = Y.n_cols;
            optimizator = nlopt::opt(nlopt::algorithm::LN_COBYLA, nbOptParams);
            optimizator.set_min_objective(PlaneGIRL::wrapper, this);


            unsigned int maxFunEvals = 0;
            nbFunEvals = 0;
            if (maxFunEvals == 0)
                maxFunEvals = std::min(50*nbOptParams, 600);


            optimizator.set_xtol_rel(1e-8);
            optimizator.set_ftol_rel(1e-8);
            optimizator.set_ftol_abs(1e-8);
            optimizator.set_maxeval(maxFunEvals);

            optimizator.add_equality_constraint(PlaneGIRL::wrapper_constr, this, 1e-6);

            //optimize function
            std::vector<double> parameters(nbOptParams,0);
            double minf;
            if (optimizator.optimize(parameters, minf) < 0)
            {
                printf("nlopt failed!\n");
                abort();
            }
            else
            {
                //            printf("found minimum = %0.10g\n", minf);
                arma::vec finalP(nbOptParams);
                for(int i = 0; i < nbOptParams; ++i)
                {
                    finalP(i) = parameters[i];
                }

                weights(nonZeroIdx) = Y*finalP;
            }
        }
        else
        {
            weights(nonZeroIdx) = Y;
        }

        //Normalize (L1) weights
        weights /= arma::sum(weights);

    }

    ////////////////////////////////////////////////////////////////
    /// FUNCTIONS FOR THE OPTIMIZATION STEP
    ////////////////////////////////////////////////////////////////
    static double wrapper_constr(unsigned int n, const double* x, double* grad,
                                 void* o)
    {
        return reinterpret_cast<PlaneGIRL*>(o)->oneSumConstraint(n, x, grad);
    }

    double oneSumConstraint(unsigned int n, const double *x, double *grad)
    {
        grad = nullptr;
        arma::vec w(x,n);
        arma::vec p = Y*w;
        double val = arma::norm(p,1) - 1.0;
        return val;
    }

    static double wrapper(unsigned int n, const double* x, double* grad,
                          void* o)
    {
        return reinterpret_cast<PlaneGIRL*>(o)->objFunction(n, x, grad);
    }

    double objFunction(unsigned int n, const double* x, double* grad)
    {

        ++nbFunEvals;

        arma::vec w(x,n);
        arma::vec p = Y*w;

        if (grad != nullptr)
        {
            abort();
        }

        double norm1_2 = 0.0;
        for (unsigned int i = 0, ie = p.n_elem; i < ie; ++i)
        {
            norm1_2 += sqrt(abs(p(i)));
        }
        norm1_2 *= norm1_2;
        //        std::cerr << norm1_2 << std::endl;
        return norm1_2;

    }

protected:
    Dataset<ActionC,StateC>& data;
    DifferentiablePolicy<ActionC,StateC>& policy;
    BasisFunctions& rewardBasis;
    double gamma;
    arma::vec weights;
    IRLGradType atype;
    unsigned int nbFunEvals;
    arma::mat Y;

};


} //end namespace


#endif /* PGIRL_H_ */
