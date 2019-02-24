#include <omnetpp.h>
#include <cstdlib>
#include <PriorityMessage_m.h>

using namespace omnetpp;


class Source : public cSimpleModule
{
  private:
    PriorityMessage *priorityMessage;

    int numPrio;
    cMersenneTwister* rng; // random number generator
    std::vector<double> interArrivalTimes; // we default to exponential times

    //Needed for taking track of id generation of messages for each priority
    int generatedMsgCounter[100] = {0}; //obviously need to limit the max number of priority queues

  public:
    Source();
    virtual ~Source();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual double getPriorityTime(int priority);
};

Define_Module(Source);

Source::Source()
{
    priorityMessage = nullptr;
}

Source::~Source()
{
    cancelAndDelete(priorityMessage);
}

void Source::initialize()
{
    numPrio = par("numPrio").intValue(); //getting the numbers of n priorities from parameter

    rng = new cMersenneTwister();
    interArrivalTimes = cStringTokenizer(par("interArrivalTimes")).asDoubleVector();

    priorityMessage = new PriorityMessage("dataPriorityMessage");
    scheduleAt(simTime(), priorityMessage);
}

void Source::handleMessage(cMessage *msg)
{
    ASSERT(msg == priorityMessage);

    char msgname[60];
    int priority = (rand() % numPrio); //generating priority number from parameter
    sprintf(msgname, "message-%d-priority-%d", ++generatedMsgCounter[priority], priority);
    PriorityMessage *message = new PriorityMessage(msgname);
    message->setPriority(priority);
    message->setWorkTime(SIMTIME_ZERO);

    send(message, "out");

    auto prio = getPriorityTime(priority);

    scheduleAt(simTime() + prio, priorityMessage);
}

double Source::getPriorityTime(int priority){
    if(priority >= 0 && priority < numPrio && interArrivalTimes.size() > 0){
        if (priority <= (interArrivalTimes.size() - 1)) return omnetpp::exponential(rng, interArrivalTimes.at(priority)); // if the interArrivalTimes array has enough values, return the correct one
        else return omnetpp::exponential(rng, interArrivalTimes.at(rand() % interArrivalTimes.size())); // otherwise just return a random time out of all the available ones
    }

    return 0;
}
