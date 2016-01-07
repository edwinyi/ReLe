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

#ifndef REGRESSORS_H_
#define REGRESSORS_H_

#include "rele/core/Basics.h"
#include "rele/approximators/data/BatchData.h"
#include "rele/approximators/Features.h"

namespace ReLe
{

template<class InputC, class OutputC, bool denseOutput = true>
class Regressor_
{

public:

    Regressor_(Features_<InputC, denseOutput>& phi, unsigned int output = 1) :
        phi(phi), outputDimension(output)
    {
    }

    virtual OutputC operator() (const InputC& input) = 0;

    template<class Input1, class Input2>
    OutputC operator()(const Input1& input1, const Input2& input2)
    {
        auto& self = *this;
        return self(vectorize(input1, input2));
    }

    template<class Input1, class Input2, class Input3>
    OutputC operator()(const Input1& input1, const Input2& input2, const Input3& input3)
    {
        auto& self = *this;
        return self(vectorize(input1, input2, input3));
    }

    virtual ~Regressor_()
    {
    }

    int getOutputSize()
    {
        return outputDimension;
    }

    inline Features_<InputC, denseOutput>& getFeatures()
    {
        return phi;
    }

protected:
    Features_<InputC, denseOutput>& phi;
    unsigned int outputDimension;
};

typedef Regressor_<arma::vec, arma::vec> Regressor;

template<class InputC, bool denseOutput = true>
class ParametricRegressor_: public Regressor_<InputC, arma::vec, denseOutput>
{
public:
    ParametricRegressor_(Features_<InputC, denseOutput>& phi, unsigned int output = 1) :
        Regressor_<InputC, arma::vec, denseOutput>(phi, output)
    {
    }

    virtual void setParameters(const arma::vec& params) = 0;
    virtual arma::vec getParameters() const = 0;
    virtual unsigned int getParametersSize() const = 0;
    virtual arma::vec diff(const InputC& input) = 0;

    template<class Input1, class Input2>
    arma::vec diff(const Input1& input1, const Input2& input2)
    {
        return this->diff(vectorize(input1, input2));
    }

    template<class Input1, class Input2, class Input3>
    arma::vec diff(const Input1& input1, const Input2& input2, const Input3& input3)
    {
        return this->diff(vectorize(input1, input2, input3));
    }

    virtual ~ParametricRegressor_()
    {

    }

};

typedef ParametricRegressor_<arma::vec> ParametricRegressor;

template<class InputC, class OutputC, bool denseOutput=true>
class BatchRegressor_ : public Regressor_<InputC, OutputC, denseOutput>
{
    typedef typename input_traits<denseOutput>::type FeaturesCollection;
    typedef typename output_traits<OutputC>::type OutputCollection;
public:
    BatchRegressor_(Features_<InputC, denseOutput>& phi, unsigned int output = 1) :
        Regressor_<InputC, OutputC, denseOutput>(phi, output)
    {

    }

    virtual void train(const BatchDataRaw_<InputC, OutputC>& dataset)
    {
        unsigned int N = dataset.size();

        //compute features matrix
        FeaturesCollection features(this->phi.rows(), N);
        OutputCollection outputs(this->outputDimension, N);

        for(int i = 0; i < N; i++)
        {
            features.col(i) = this->phi(dataset.getInput(i));
            outputs.col(i) = dataset.getOutput(i);
        }

        BatchDataSimple_<OutputC, denseOutput> featureDataset(features, outputs);
        trainFeatures(featureDataset);
    }

    double computeJ(const BatchDataRaw_<InputC, arma::vec>& dataset)
    {
        unsigned int N = dataset.size();

        //compute features matrix
        FeaturesCollection features(this->phi.rows(), N);
        OutputCollection outputs(this->outputDimension, N);

        for(int i = 0; i < N; i++)
        {
            features.col(i) = this->phi(dataset.getInput(i));
            outputs.col(i) = dataset.getOutput(i);
        }

        BatchDataSimple_<OutputC, denseOutput> featureDataset(features, outputs);

        return computeJFeatures(featureDataset);
    }

    virtual void trainFeatures(const BatchData_<OutputC, denseOutput>& featureDataset) = 0;
    virtual double computeJFeatures(const BatchData_<OutputC, denseOutput>& dataset) = 0;

    virtual ~BatchRegressor_()
    {

    }

};

typedef BatchRegressor_<arma::vec, arma::vec> BatchRegressor;

template<class InputC, class OutputC, bool denseOutput = true>
class UnsupervisedBatchRegressor_ : public Regressor_<InputC, OutputC, denseOutput>
{
    typedef typename input_traits<denseOutput>::type FeaturesCollection;
public:
    UnsupervisedBatchRegressor_(Features_<InputC, denseOutput>& phi, unsigned int output = 1)
        : Regressor_<InputC, OutputC, denseOutput>(phi, output)
    {

    }

    virtual void train(const std::vector<InputC>& dataset)
    {
        unsigned int N = dataset.size();

        //compute features matrix
        arma::mat features(this->phi.rows(), N);
        for(int i = 0; i < N; i++)
        {
            features.col(i) = this->phi(dataset[i]);
        }

        trainFeatures(features);
    }

    virtual void trainFeatures(const FeaturesCollection& features) = 0;

    virtual ~UnsupervisedBatchRegressor_()
    {

    }

};

}

#define USE_REGRESSOR_MEMBERS(InputC, OutputC, denseOutput) \
    typedef Regressor_<InputC, OutputC, denseOutput> Base; \
    using Base::phi;

#define USE_PARAMETRIC_REGRESSOR_MEMBERS(InputC, OutputC, denseOutput) \
    typedef ParametricRegressor_<InputC, denseOutput> Base;

#define USE_BATCH_REGRESSOR_MEMBERS(InputC, OutputC, denseOutput) \
    typedef BatchRegressor_<InputC, OutputC, denseOutput> Base;

#define USE_UNSUPERVISED_REGRESSOR_MEMBERS(InputC, OutputC, denseOutput) \
    typedef UnsupervisedBatchRegressor_<InputC, OutputC, denseOutput> Base; \
 
#define DEFINE_FEATURES_TYPES(denseOutput) \
	typedef typename input_traits<denseOutput>::type FeaturesCollection; \
	typedef typename input_traits<denseOutput>::column_type FeaturesVector;

#endif /* REGRESSORS_H_ */
