#include "agent.h"

Agentclass::Agentclass(repast::AgentId id): id_(id), c(100), total(200){ }

Agentclass::Agentclass(repast::AgentId id, double newC, double newTotal): id_(id), c(newC), total(newTotal){ }

Agentclass::Agentclass(repast::AgentId id, double w1, double w2, double w3, double w4, double w5, double p1, double p2, double p3, double p4, double p5, double s, int state): id_(id), w1(w1), w2(w2), w3(w3), w4(w4), w5(w5), p1(p1), p2(p2), p3(p3), p4(p4), p5(p5), s(s), state(state){ }

Agentclass::~Agentclass(){ }

void Agentclass::setw1(double W1){w1 = W1;}
void Agentclass::setw2(double W2){w2 = W2;}
void Agentclass::setw3(double W3){w3 = W3;}
void Agentclass::setw4(double W4){w4 = W4;}
void Agentclass::setw5(double W5){w5 = W5;}
void Agentclass::setp5(double P5){p5 = P5;}
void Agentclass::setp3(double P3){p3 = P3;}
void Agentclass::setp4(double P4){p4 = P4;}

void Agentclass::setAgent(double W1, double W2, double W3, double W4, double W5, double P1, double P2, double P3, double P4, double P5){
    w1 = W1;
    w2 = W2;
    w3 = W3;
    w4 = W4;
    w5 = W5;

    p1 = P1;
    p2 = P2;
    p3 = P3;
    p4 = P4;
    p5 = P5;
}

void Agentclass::set(int currentRank, double newC, double newTotal){
    id_.currentRank(currentRank);
    c     = newC;
    total = newTotal;
}

// Interaction moved to model.h
double Agentclass::interaction(repast::SharedContext<Agentclass>* context){ 
    // e.g. 
    std::set<Agentclass*> agentsToPlay;
    repast::AgentId currentAgentId; 
    double p3temp;
//    agentsToPlay.insert(this); // Prohibit playing against self
    context->selectAgents(2, agentsToPlay, true);
    std::set<Agentclass*>::iterator agentToPlay = agentsToPlay.begin();
    currentAgentId = (*agentToPlay)-> getId();
    while(agentToPlay != agentsToPlay.end()){
    p3temp = (*agentToPlay)-> getp3();
    agentToPlay++;
    (*agentToPlay)-> setp3(p3temp);
    }
    return p3temp;
}

bool Agentclass::decisionmaking(repast::SharedContext<Agentclass>* context){
    bool cycling_flag = 0;
 //   if (repast::Random::instance()->nextDouble() < 0.1) interaction(context);
    s = w1 * p1 + w2 * p2 + w3 * p3 + w4 * p4 + w5 * p5;

    if (s > 0.5)
        state = 1;

    if (state){
//           std::cout << getId() << ", state = 1" << std::endl;
           cycling_flag = 1;
     }

    state = 0;

//    std::cout << getId() << s << std::endl; Print out score of each agent
    return cycling_flag;
}

/* Serializable Agent Package Data */

AgentPackage::AgentPackage(){ }

AgentPackage::AgentPackage(int _id, int _rank, int _type, int _currentRank, double _c, double _total):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), c(_c), total(_total){ }

    /*    
        agentsToPlay.insert(this); // Prohibit playing against self

    context->selectAgents(3, agentsToPlay, true);
    double cPayoff     = 0;
    double totalPayoff = 0;
    std::set<Agentclass*>::iterator agentToPlay = agentsToPlay.begin();
    while(agentToPlay != agentsToPlay.end()){
        bool iCooperated = cooperate();                          // Do I cooperate?
        double payoff = (iCooperated ?
                         ((*agentToPlay)->cooperate() ?  7 : 1) :     // If I cooperated, did my opponent?
                         ((*agentToPlay)->cooperate() ? 10 : 3));     // If I didn't cooperate, did my opponent?
        if(iCooperated) cPayoff += payoff;
        totalPayoff             += payoff;
        agentToPlay++;
    }
    c      += cPayoff;
    total  += totalPayoff;
    */