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

#ifndef BASICS_H_
#define BASICS_H_

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <armadillo>
#include <iomanip>

#define OS_PRECISION 10

namespace ReLe
{

struct EnvironmentSettings
{
    bool isFiniteHorizon;
    double gamma;
    unsigned int horizon;

    bool isAverageReward;
    bool isEpisodic;

    size_t finiteStateDim;
    unsigned int finiteActionDim;

    unsigned int continuosStateDim;
    unsigned int continuosActionDim;

    unsigned int rewardDim;

    arma::vec max_obj; //TODO: possiamo mettere un range? (usato per normalizzazione in matlab, default 1)

    inline void WriteToStream(std::ostream& out) const
    {
        out << std::setprecision(OS_PRECISION);
        out << finiteStateDim << "\t" << finiteActionDim << std::endl;
        out << continuosStateDim << "\t" << continuosActionDim << std::endl;
        out << rewardDim << std::endl;
        out << gamma << "\t" << isFiniteHorizon << "\t" << horizon << "\t";
        out << isEpisodic << "\t" << isAverageReward << std::endl;
    }

    inline void ReadFromStream(std::istream& in)
    {
        in >> finiteStateDim >> finiteActionDim;
        in >> continuosStateDim >> continuosActionDim;
        in >> rewardDim;
        in >> gamma >> isFiniteHorizon >> horizon;
        in >> isEpisodic >> isAverageReward;
    }
};


class AgentOutputData
{
public:
    AgentOutputData(bool final = false) : final(final), step(0)
    {

    }

    virtual void writeData(std::ostream& os) = 0;
    virtual void writeDecoratedData(std::ostream& os) = 0;

    virtual ~AgentOutputData()
    {
    }

    inline bool isFinal() const
    {
        return final;
    }

    inline unsigned int getStep() const
    {
        return step;
    }

    inline void setStep(unsigned int step)
    {
        this->step = step;
    }

private:
    unsigned int step;
    bool final;

};

class Action
{
public:
    inline virtual std::string to_str() const
    {
        return "action";
    }

    virtual inline int serializedSize()
    {
        return 0;
    }

    virtual ~Action()
    {

    }

    static inline Action generate(std::vector<std::string>::iterator& begin,
                                  std::vector<std::string>::iterator& end)
    {
        return Action();
    }
};

class FiniteAction: public Action
{
public:
    FiniteAction(unsigned int n = 0)
    {
        actionN = n;
    }

    operator unsigned int&()
    {
        return actionN;
    }

    operator const unsigned int&() const
    {
        return actionN;
    }

    inline unsigned int getActionN() const
    {
        return actionN;
    }

    inline void setActionN(unsigned int actionN)
    {
        this->actionN = actionN;
    }

    inline virtual std::string to_str() const
    {
        return std::to_string(actionN);
    }

    inline int serializedSize()
    {
        return 1;
    }

    virtual ~FiniteAction()
    {

    }

    static inline FiniteAction generate(std::vector<std::string>::iterator& begin,
                                        std::vector<std::string>::iterator& end)
    {
        int actionN = std::stoul(*begin);
        return FiniteAction(actionN);
    }

    inline virtual bool isAlmostEqual(const FiniteAction& other) const
    {
        return this->getActionN() == other.getActionN();
    }

    inline static std::vector<FiniteAction> generate(size_t actionN)
    {
        std::vector<FiniteAction> actions;
        for(int i = 0; i < actionN; ++i)
            actions.push_back(FiniteAction(i));
        return actions;
    }

private:
    unsigned int actionN;
};

class DenseAction: public Action, public arma::vec
{
public:
    DenseAction ()
    { }

    DenseAction(std::size_t size) :
        arma::vec(size)
    {    }

    DenseAction(arma::vec& other) :
        arma::vec(other.n_elem)
    {
        this->set_size(other.n_elem);
        for (int i = 0; i < other.n_elem; ++i)
            this->at(i) = other[i];
    }

    inline virtual std::string to_str() const
    {
        const arma::vec& self = *this;
        std::stringstream ss;
        ss << std::setprecision(OS_PRECISION);

        size_t i;
        for (i = 0; i + 1 < self.n_elem; i++)
            ss << self[i] << ",";

        ss << self[i];

        return ss.str();
    }

    inline int serializedSize()
    {
        return this->n_elem;
    }

    virtual ~DenseAction()
    {

    }

    inline virtual void copy_vec(const arma::vec& other)
    {
        this->set_size(other.n_elem);
        for (int i = 0; i < other.n_elem; ++i)
            this->at(i) = other[i];
    }

    inline virtual bool isAlmostEqual(const arma::vec& other, double epsilon = 1e-6) const
    {
        int i, dim = this->n_elem;
        if (other.n_elem != dim)
            return false;
        for (i = 0; i < dim; ++i)
            if (fabs(this->at(i) - other[i]) > epsilon)
                return false;

        return true;
    }

    static inline DenseAction generate(std::vector<std::string>::iterator& begin,
                                       std::vector<std::string>::iterator& end)
    {
        int size = std::distance(begin, end);
        DenseAction action(size);
        for(unsigned int i = 0; i < size; i++)
        {
            action[i] = std::stod(*(begin +i));
        }

        return action;
    }
};

class State
{
public:
    State() :
        absorbing(false)
    {

    }

    inline bool isAbsorbing() const
    {
        return absorbing;
    }

    inline void setAbsorbing(bool absorbing = true)
    {
        this->absorbing = absorbing;
    }

    inline virtual std::string to_str() const
    {
        return "state";
    }

    virtual inline int serializedSize()
    {
        return 0;
    }

    virtual ~State()
    {

    }

    static inline State generate(std::vector<std::string>::iterator& begin,
                                 std::vector<std::string>::iterator& end)
    {
        return State();
    }

private:
    bool absorbing;
};

class FiniteState: public State
{
public:
    FiniteState(unsigned int n = 0)
    {
        stateN = n;
    }

    operator size_t&()
    {
        return stateN;
    }

    operator const size_t&() const
    {
        return stateN;
    }

    inline std::size_t getStateN() const
    {
        return stateN;
    }

    inline void setStateN(std::size_t stateN)
    {
        this->stateN = stateN;
    }

    inline virtual std::string to_str() const
    {
        return std::to_string(stateN);
    }

    inline int serializedSize()
    {
        return 1;
    }

    virtual ~FiniteState()
    {

    }

    static inline FiniteState generate(std::vector<std::string>::iterator& begin,
                                       std::vector<std::string>::iterator& end)
    {
        int stateN = std::stoul(*begin);
        return FiniteState(stateN);
    }

private:
    std::size_t stateN;

};

class DenseState: public State, public arma::vec
{
public:
    DenseState()
    {
    }

    DenseState(std::size_t size) :
        arma::vec(size)
    {
    }

    inline virtual std::string to_str() const
    {
        const arma::vec& self = *this;
        std::stringstream ss;
        ss << std::setprecision(OS_PRECISION);

        size_t i;
        for (i = 0; i + 1 < self.n_elem; i++)
            ss << self[i] << ",";

        ss << self[i];

        return ss.str();
    }

    inline int serializedSize()
    {
        return this->n_elem;
    }

    virtual ~DenseState()
    {

    }

    static inline DenseState generate(std::vector<std::string>::iterator& begin,
                                      std::vector<std::string>::iterator& end)
    {
        int size = std::distance(begin, end);
        DenseState state(size);
        for(unsigned int i = 0; i < size; i++)
        {
            state[i] = std::stod(*(begin +i));
        }

        return state;
    }

};

typedef std::vector<double> Reward;
inline Reward generate(std::vector<std::string>::iterator& begin,
                       std::vector<std::string>::iterator& end)
{
    Reward reward;

    for(auto it = begin; it != end; it++)
    {
        double value = std::stod(*it);
        reward.push_back(value);
    }

    return reward;

}

inline std::ostream& operator<<(std::ostream& os, const Action& action)
{
    os << action.to_str();
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const State& state)
{
    os << state.to_str();
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Reward& reward)
{
    if(!reward.empty())
    {
        size_t i;
        for (i = 0; i + 1 < reward.size(); i++)
            os << reward[i] << ",";

        os << reward[i];
    }
    return os;
}

}

#endif /* BASICS_H_ */
