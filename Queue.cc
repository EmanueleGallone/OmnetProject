#include <omnetpp.h>
#include <PriorityMessage_m.h>

using namespace omnetpp;


class Queue : public cSimpleModule
{
  protected:
    cMessage *msgServiced;
    cMessage *endServiceMsg;

    int numPrio;

    cArray queues;

    simsignal_t qlenSignal;
    simsignal_t busySignal;
    simsignal_t queueingTimeSignal;
    simsignal_t responseTimeSignal;

  public:
    Queue();
    virtual ~Queue();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual int getMsgToServe();
    virtual double getServiceTimeForPriority(int priority);
    virtual double randomTime();
};

Define_Module(Queue);


Queue::Queue()
{
    msgServiced = endServiceMsg = nullptr;
}

Queue::~Queue()
{
    delete msgServiced;
    cancelAndDelete(endServiceMsg);
}

void Queue::initialize()
{
    endServiceMsg = new cMessage("end-service");

    numPrio = par("numPrio"); //number of priority queues

    for(int i = 0; i < numPrio; i++){
       queues.add(new cQueue(std::to_string(i).c_str()));
   }

    qlenSignal = registerSignal("qlen");
    busySignal = registerSignal("busy");
    queueingTimeSignal = registerSignal("queueingTime");
    responseTimeSignal = registerSignal("responseTime");

    emit(qlenSignal, check_and_cast<cQueue*>(queues.get(0))->getLength());
    emit(busySignal, false);
}

void Queue::handleMessage(cMessage *msg)
{
    if (msg == endServiceMsg) { // Self-message arrived

        EV << "Completed service of " << msgServiced->getName() << endl;
        send(msgServiced, "out");

        //Response time: time from msg arrival timestamp to time msg ends service (now)
        emit(responseTimeSignal, simTime() - msgServiced->getTimestamp());
        msgServiced = nullptr;

        if (getMsgToServe() == 0) { // Empty queue, server goes in IDLE

            EV << "Empty queue, server goes IDLE" <<endl;
            msgServiced = nullptr;
            emit(busySignal, false);

        }
        else { // Queue contains users

            int notEmpty = 0;
            if(!((notEmpty = getMsgToServe()) == 0)){ //queue is not empty!
                cQueue *queue = check_and_cast<cQueue*>(queues.get(notEmpty));

                PriorityMessage *m = (PriorityMessage*)(queue->pop());
                msgServiced = m;
                emit(qlenSignal, queue->getLength()); //Queue length changed, emit new length!

                //Waiting time: time from msg arrival to time msg enters the server (now)
                emit(queueingTimeSignal, simTime() - msgServiced->getTimestamp());

                EV << "Starting service of " << m->getName() << endl;
                simtime_t serviceTime = getServiceTimeForPriority(m->getPriority());
                scheduleAt(simTime()+serviceTime, endServiceMsg);
            }
            else {
                EV << "something wrong!" << endl;
            }

        }

    }
    else { // Data msg has arrived

        //Setting arrival timestamp as msg field
        msg->setTimestamp();

        if (!msgServiced) { //No message in service (server IDLE) ==> No queue ==> Direct service


            PriorityMessage *m = check_and_cast<PriorityMessage*>(msg);
            msgServiced = m;
            emit(queueingTimeSignal, SIMTIME_ZERO);

            EV << "Starting service of " << msgServiced->getName() << endl;
            simtime_t serviceTime = getServiceTimeForPriority(m->getPriority());
            scheduleAt(simTime()+serviceTime, endServiceMsg);
            emit(busySignal, true);
        }
        else {  //Message in service (server BUSY) ==> Queuing

            EV << "Queuing " << msg->getName() << endl;

            int prio = ((PriorityMessage*)msg)->getPriority();
            ((cQueue*)(queues.get(prio)))->insert(((PriorityMessage*)msg));

       }
    }
}

int Queue::getMsgToServe(){
    //scan sequentially from priority 0 (the most important) to the last and get the next message to Serve
    for(int i = 0; i <= queues.size()-1; i++){
        cQueue *c = (cQueue*)(queues.get(i));
        if(!c->isEmpty()){
            ASSERT(c->getLength() > 0);
            return i;
        }
    }
    //if they are all empty, return 0
    return 0;
}

double Queue::getServiceTimeForPriority(int priority){
    if(priority > 0 && priority < numPrio){
            switch (priority) {
                case 0:
                    return par("serviceTime1");
                case 1:
                        return par("serviceTime2");
                case 2:
                        return par("serviceTime3");
                case 3:
                        return par("serviceTime4");
                case 4:
                        return par("serviceTime5");
                default:
                        return Queue::randomTime();
            }
        }

        return 0;
}

double Queue::randomTime(){
    switch (rand() % numPrio){
    case 0:
        return par("serviceTime1");
    case 1:
        return par("serviceTime2");
    case 2:
        return par("serviceTime3");
    case 3:
        return par("serviceTime4");
    case 4:
        return par("serviceTime5");
    default:
        return par("serviceTime1");

    }
}
