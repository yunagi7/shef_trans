#include "agent.h"

Agentclass::Agentclass(repast::AgentId id): id_(id), c(100), total(200){ }

Agentclass::Agentclass(repast::AgentId id, double newC, double newTotal): id_(id), c(newC), total(newTotal){ }

Agentclass::Agentclass(repast::AgentId id, double w1, double w2, double w3, double p1, double p2, double p3, double s, int state): id_(id), w1(w1), w2(w2), w3(w3), p1(p1), p2(p2), p3(p3), s(s), state(state){ }

Agentclass::~Agentclass(){ }

void Agentclass::change(int newstate){
    state = newstate;
}

void Agentclass::set(int currentRank, double newC, double newTotal){
    id_.currentRank(currentRank);
    c     = newC;
    total = newTotal;
}

bool Agentclass::cooperate(){
	return repast::Random::instance()->nextDouble() < c/total;
}

void Agentclass::play(repast::SharedContext<Agentclass>* context){ //, double w1, double w2, double w3
    std::set<Agentclass*> agentsToPlay;

    agentsToPlay.insert(this); // Prohibit playing against self

    context->selectAgents(3, agentsToPlay, true);

    s = w1 * p1 + w2 * p2 + w3 * p3;
    std::cout << getId() << "s: " << s << std::endl;
    if (s > 3)
        state = 1;
    /*    
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
    switch (state){
        case 0:
            std::cout << getId() << "state: no" << std::endl;
        case 1:
            std::cout << getId() << "state: yes" << std::endl;
    }
 //   std::cout << getId() << "w1: " << w1 << std::endl;
 //   std::cout << getId() << "w2: " << w2 << std::endl;
 //   std::cout << getId() << "w3: " << w3 << std::endl;
 //   std::cout << getId() << "p1: " << p1 << std::endl;
 //   std::cout << getId() << "p2: " << p2 << std::endl;
 //   std::cout << getId() << "p3: " << p3 << std::endl;    


//    std::cout << " c " << getC() << std::endl;
//    std::cout << " total " << getTotal() << std::endl;
}

/* Serializable Agent Package Data */

AgentPackage::AgentPackage(){ }

AgentPackage::AgentPackage(int _id, int _rank, int _type, int _currentRank, double _c, double _total):
id(_id), rank(_rank), type(_type), currentRank(_currentRank), c(_c), total(_total){ }

