#include <omnetpp.h>
#include <cstdlib>
#include <PriorityMessage_m.h>

using namespace omnetpp;


class Source : public cSimpleModule
{
  private:
    PriorityMessage *priorityMessage;

    int numPrio;

    int generatedMsgCounter[100] = {0}; //obviously need to limit the max number of priority queues

  public:
    Source();
    virtual ~Source();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual double getPriorityTime(int priority);
    virtual double randomTime();
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

    priorityMessage = new PriorityMessage("dataPriorityMessage");
    scheduleAt(simTime(), priorityMessage);
}

void Source::handleMessage(cMessage *msg)
{

    ASSERT(msg == priorityMessage);

    char msgname[60];
    int priority = (rand() % numPrio); //generating priority number from parameter
    sprintf(msgname, "message-%d-priority:%d", ++generatedMsgCounter[priority], priority);
    PriorityMessage *message = new PriorityMessage(msgname);
    message->setPriority(priority);

    send(message, "out");

    scheduleAt(simTime() + getPriorityTime(priority), priorityMessage);

}

double Source::getPriorityTime(int priority){

    if(priority > 0 && priority < numPrio){
        switch (priority) {
            case 0:
                return par("interArrivalTime1");
            case 1:
                    return par("interArrivalTime2");
            case 2:
                    return par("interArrivalTime3");
            case 3:
                    return par("interArrivalTime4");
            case 4:
                    return par("interArrivalTime5");
            default:
                    return Source::randomTime();
        }
    }

    return 0;
}

double Source::randomTime(){
    switch (rand() % numPrio){
    case 0:
        return par("interArrivalTime1");
    case 1:
        return par("interArrivalTime2");
    case 2:
        return par("interArrivalTime3");
    case 3:
        return par("interArrivalTime4");
    case 4:
        return par("interArrivalTime5");
    default:
        return par("interArrivalTime1");

    }
}
