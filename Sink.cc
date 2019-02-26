#include <omnetpp.h>
#include <PriorityMessage_m.h>

using namespace omnetpp;


class Sink : public cSimpleModule
{
  private:
    // Global
    simsignal_t responseTimeSignal;

    // Per-class
    simsignal_t responseTimeSignal0;
    simsignal_t responseTimeSignal1;
    simsignal_t responseTimeSignal2;
    simsignal_t responseTimeSignal3;
    simsignal_t responseTimeSignal4;

    simsignal_t arrivedMsgSignal;
    int nb_arrivedMsg;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Sink);


void Sink::initialize()
{
    // Global
    responseTimeSignal = registerSignal("responseTime");

    // Per-class
    responseTimeSignal0 = registerSignal("responseTime0");
    responseTimeSignal1 = registerSignal("responseTime1");
    responseTimeSignal2 = registerSignal("responseTime2");
    responseTimeSignal3 = registerSignal("responseTime3");
    responseTimeSignal4 = registerSignal("responseTime4");

    arrivedMsgSignal = registerSignal("arrivedMsg");
    nb_arrivedMsg = 0;
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t lifetime = simTime() - msg->getCreationTime();
    EV << "Sink Received " << msg->getName() << ", lifetime: " << lifetime << "s" << endl;

    // Emit the global average lifetime
    emit(responseTimeSignal, lifetime);

    // Emit per-class lifetimes
    PriorityMessage* prioMsg = (PriorityMessage*)msg;
    switch (prioMsg->getPriority()){
        case 0: emit(responseTimeSignal0, lifetime); break;
        case 1: emit(responseTimeSignal1, lifetime); break;
        case 2: emit(responseTimeSignal2, lifetime); break;
        case 3: emit(responseTimeSignal3, lifetime); break;
        case 4: emit(responseTimeSignal4, lifetime); break;
        default: break;
    }

    nb_arrivedMsg ++;
    emit(arrivedMsgSignal, nb_arrivedMsg);
    delete msg;
}
