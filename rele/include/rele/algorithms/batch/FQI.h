/*
*  rele,
*
*
*  Copyright (C) 2015 Davide Tateo & Matteo Pirotta
*  Versione 1.0
*
*  This file is part of rele.
*
*  rele is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  rele is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with rele.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Written by: Alessandro Nuara, Carlo D'Eramo
 */

#ifndef FQI_H_
#define FQI_H_

namespace ReLe
{
/*
 * Fitted Q-Iteration (FQI)
 *
 * "Tree-Based Batch Mode Reinforcement Learning"
 * Damien Ernst, Pierre Geurts, Louis Wehenkel
 * Journal of Machine Learning Research, 6, 2006, pp. 503-556.
 */

// A template is used for states in order to manage both dense and finite states.
template<class StateC>
class FQI
{
public:

    /* This class implements the FQI algorithm. As a batch algorithm, it takes
     * a dataset of (s, a, r, s') transitions, together with a regressor that
     * it is used to approximate the target distribution of Q values.
     */
    FQI(Dataset<FiniteAction, StateC>& data, BatchRegressor& QRegressor,
        unsigned int nStates, unsigned int nActions, double gamma, unsigned int nMiniBatches = 1) :
        data(data),
        QRegressor(QRegressor),
        nSamples(0),
        nStates(nStates),
        nActions(nActions),
        gamma(gamma),
		nMiniBatches(nMiniBatches)
    {
        // Compute the overall number of samples
        unsigned int nEpisodes = data.size();
        for(unsigned int k = 0; k < nEpisodes; k++)
            nSamples += data[k].size();

        QTable.zeros(nStates, nActions);

        /*
         * This vector is used for the terminal condition evaluation. It will
         * contain the approximated Q values of each sample in the dataset.
         */
        QHat.zeros(nSamples);
    }

    virtual void run(Features& phi, unsigned int maxiterations, double epsilon)
    {
        /* This is the function to be called to run the FQI algorithm. It takes
         * the features phi that are used to compute the input of the regressor.
         */

        // Rewards are extracted from the dataset
        arma::mat rewards = data.rewardAsMatrix();
        // Input of the regressor are computed using provided features
        arma::mat input = data.featuresAsMatrix(phi);
        /* Output vector is initialized. It will contain the Q values, found
         * with the optimal Bellman equation, that are used in regression as
         * target values.
         */
        arma::mat output(1, nSamples, arma::fill::zeros);

        unsigned int i = 0;
        arma::vec nextStates(nSamples, arma::fill::zeros);
        for(auto& episode : data)
        	for(auto& tr : episode)
        	{
        		nextStates(i) = tr.xn;
        		i++;
        	}


        /* First iteration of FQI is performed here training
         * the regressor with a dataset that has the rewards as
         * output. The dataset is divided into two minibatches
         * with shuffled samples.
         */
        BatchDataSimple featureDatasetStart(input, rewards);
        const std::vector<MiniBatchData*> miniBatches = (featureDatasetStart.getNMiniBatches(nMiniBatches));
        for(unsigned int i = 0; i < miniBatches.size(); i++)
		{
			QRegressor.trainFeatures(*miniBatches[i]);
			nextStatesMiniBatch.push_back(nextStates(miniBatches[i]->getIndexes()));
		}

        // Initial QHat is stored before update
        arma::vec prevQHat = QHat;

        // Update QHat
        computeQHat(data);

        unsigned int iteration = 0;
        double J;
        // Print info
        std::cout << std::endl << "*********************************************************" << std::endl;
        std::cout << "FQI iteration: " << iteration << std::endl;
        std::cout << "*********************************************************" << std::endl;
        printInfo(rewards, prevQHat, J);

        // Main FQI loop
        while(iteration < maxiterations)// && J > epsilon)
        {
            // Update and print the iteration number
            iteration++;
            std::cout << std::endl << "*********************************************************" << std::endl;
            std::cout << "FQI iteration: " << iteration << std::endl;
            std::cout << "*********************************************************" << std::endl;

            // Iteration of FQI
            step(miniBatches, output);

            // Previous Q approximated values are stored
            prevQHat = QHat;
            /* New QHat values are computed using the regressor trained with the
             * new output values.
             */
            computeQHat(data);

            // Print info
            printInfo(output, prevQHat, J);
        }

        // Print final info
        std::cout << std::endl << "*********************************************************" << std::endl;
        if(J > epsilon)
            /* The algorithm has not converged and terminated for exceeding
             * the maximum number of transitions.
             */
            std::cout << "FQI finished in " << iteration <<
                      " iterations WITHOUT CONVERGENCE to a fixed point" << std::endl;
        else
            // Error below the maximum desired error: the algorithm has converged
            std::cout << "FQI converged in " << iteration << " iterations" << std::endl <<
                      "********************************************************* " << std::endl;

        // Print the policy found according to QHat values in the Q-Table
        printPolicy();
    }

    virtual void step(const std::vector<MiniBatchData*> miniBatches, arma::mat& output)
    {
    	arma::mat input = miniBatches[0]->getFeatures();
    	arma::mat rewards = miniBatches[0]->getOutputs();

    	for(unsigned int i = 0; i < input.n_cols; i++)
    	{
			/* In order to be able to fill the output vector (i.e. regressor
			 * target values), we need to compute the Q values for each
			 * s' sample in the dataset and for each action in the
			 * set of actions of the problem. Recalling the fact that the values
			 * are zero for each action in an absorbing state, we check if s' is
			 * absorbing and, if it is, we leave the Q-values fixed to zero.
			 */
			arma::vec Q_xn(nActions, arma::fill::zeros);
			FiniteState nextState = FiniteState(nextStatesMiniBatch[0](i));
			if(!nextState.isAbsorbing())
				for(unsigned int u = 0; u < nActions; u++)
					Q_xn(u) = arma::as_scalar(QRegressor(nextState,
														 FiniteAction(u)));

            /* For the current s', Q values for each action are stored in
             * Q_xn. The optimal Bellman equation can be computed
             * finding the maximum value inside Q_xn. They are zero if
             * xn is an absorbing state.
             */
            output(i) = rewards(0, i) + this->gamma * arma::max(Q_xn);
    	}

        // The regressor is trained
        BatchDataSimple featureDataset(input, output);
        QRegressor.trainFeatures(featureDataset);
    }

    virtual void computeQHat(Dataset<FiniteAction, StateC>& data)
    {
        // Computation of Q values approximation with the updated regressor
        unsigned int i = 0;
        for(auto& episode : data)
            for(auto& tr : episode)
            {
                QHat(i) = arma::as_scalar(QRegressor(tr.x, tr.u));
                i++;
            }
    }

    virtual void computeQTable()
    {
        for(unsigned int i = 0; i < nStates; i++)
            for(unsigned int j = 0; j < nActions; j++)
                QTable(i, j) = arma::as_scalar(QRegressor(FiniteState(i), FiniteAction(j)));
    }

    virtual void printInfo(arma::mat output,
						   arma::vec prevQHat,
						   double& J1)
    {
        double J2;

        /* Evaluate the distance between the previous approximation of Q
         * and the current one.
         */
        J1 = arma::norm(QHat - prevQHat);
        /* Evaluate the error mean squared error between the current approximation
         * of Q and the target output in the dataset.
         */
        J2 = arma::sum(arma::square(QHat - output.t()))  / nSamples;

        std::cout << std::endl << std::endl << "Bellman Q-values: " << std::endl << output.cols(0, 40) << std::endl;
        std::cout << "Approximated Q-values: " << std::endl << QHat.rows(0, 40).t() << std::endl;
        std::cout << "QHat - Previous QHat: " << J1 << std::endl;
        std::cout << "QHat - Q Bellman: " << J2 << std::endl;
    }

    virtual void printPolicy()
    {
        computeQTable();
        std::cout << std::endl << "Policy:" << std::endl;
        for(unsigned int i = 0; i < nStates; i++)
        {
            arma::uword policy;
            QTable.row(i).max(policy);
            std::cout << "policy(" << i << ") = " << policy << std::endl;
        }
    }

    virtual ~FQI()
    {
    }

protected:
    Dataset<FiniteAction, StateC>& data;
    arma::vec QHat;
    arma::mat QTable;
    BatchRegressor& QRegressor;
    unsigned int nSamples;
    unsigned int nStates;
    unsigned int nActions;
    double gamma;
    unsigned int nMiniBatches;
    std::vector<arma::vec> nextStatesMiniBatch;
};

}

#endif //FQI_H
