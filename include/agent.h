#ifndef AGENT_H
#define AGENT_H

#include "repast_hpc/AgentId.h"
#include "repast_hpc/SharedContext.h"


/* Agents */
class Agentclass{

private:
    repast::AgentId   id_;
    double              c;
    double          total;

    int state; // Agent state
    double s; // Score

    double w1; // Weight 1
    double w2; // Weight 2
    double w3; // Weight 3

    double p1; // Property 1
    double p2; // Property 2
    double p3; // Property 3

public:
    Agentclass(repast::AgentId id);

    Agentclass(repast::AgentId id, double newC, double newTotal);

    Agentclass(repast::AgentId id, double w1, double w2, double w3, double p1, double p2, double p3, double s, int state);

    ~Agentclass();

    /* Required Getters */
    virtual repast::AgentId& getId(){                   return id_;    }
    virtual const repast::AgentId& getId() const {      return id_;    }

    /* Getters specific to this kind of Agent */
    double getC(){                                      return c;      }
    double getTotal(){                                  return total;  }
    double getw1(){                                     return w1;}
    double getw2(){                                     return w2;}
    double getw3(){                                     return w3;}
    double getp1(){                                     return p1;}
    double getp2(){                                     return p2;}
    double getp3(){                                     return p3;}
    
    /* Setter */
    void set(int currentRank, double newC, double newTotal);
    void change(int newstate);

    /* Actions */
    bool cooperate();                                                 // Will indicate whether the agent cooperates or not; probability determined by = c / total
//    void play(repast::SharedContext<Agentclass>* context, double w1, double w2, double w3);    // Choose three other agents from the given context and see if they cooperate or not
    void play(repast::SharedContext<Agentclass>* context);
};

/* Serializable Agent Package */
struct AgentPackage {

public:
    int    id;
    int    rank;
    int    type;
    int    currentRank;
    double c;
    double total;

    /* Constructors */
    AgentPackage(); // For serialization
    AgentPackage(int _id, int _rank, int _type, int _currentRank, double _c, double _total);


    /* For archive packaging */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version){
        ar & id;
        ar & rank;
        ar & type;
        ar & currentRank;
        ar & c;
        ar & total;
    }

};

#endif // AGENT_H_INCLUDED
