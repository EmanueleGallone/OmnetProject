#include <omnetpp.h>
#include <PriorityMessage_m.h>

using namespace omnetpp;


class Queue : public cSimpleModule
{
  protected:
    cMessage *msgServiced;
    cMessage *endServiceMsg;

    int numPrio;
    bool isPreemptive;
    bool preemptiveResume;
    simtime_t workEnd; // needed for preemptive resume
    cMersenneTwister* rng; // random number generator
    std::vector<double> serviceTimes;

    cArray queues; //array of queues; so to avoid scanning all the queue every time, we thought that
                   //splitting the queue in "sub-queues" based on priority will increase performance.

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
    virtual PriorityMessage* getMsgPtrToServe();
    virtual double getServiceTimeForPriority(int priority);
    virtual long getTotalQueueLength();
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

    isPreemptive = par("preemptive");
    preemptiveResume = par("resume");
    numPrio = par("numPrio"); //number of priority queues

    rng = new cMersenneTwister();
    serviceTimes = cStringTokenizer(par("serviceTimes")).asDoubleVector();

    for(int i = 0; i < numPrio; i++){
        //creating #queues that equals the # of priorities
        //NB the queues are ordered. The most important is queues[0] and than come the others
       queues.add(new cQueue(std::to_string(i).c_str())); //creating queue with name = priority
       workEnd = SIMTIME_ZERO;
   }

    qlenSignal = registerSignal("qlen");
    busySignal = registerSignal("busy");
    queueingTimeSignal = registerSignal("queueingTime");
    responseTimeSignal = registerSignal("responseTime");

    emit(qlenSignal, getTotalQueueLength());
    emit(busySignal, false);
}

void Queue::handleMessage(cMessage *msg)
{

    if (msg == endServiceMsg) { // Self-message arrived

        EV << "Completed service of " << msgServiced->getName() << endl;
        send(msgServiced, "out");

        //Response time: time from msg arrival timestamp to time msg ends service (now)
        emit(responseTimeSignal, simTime() - msgServiced->getTimestamp());

        if (getMsgToServe() == -1) { // Empty queue, server goes in IDLE

            EV << "Empty queue, server goes IDLE" <<endl;
            msgServiced = nullptr;
            emit(busySignal, false);

        }
        /*else { //good if you want to be more concise
            PriorityMessage *test;
            if ((test=getMsgPtrToServe())){
                msgServiced = test;

                //Waiting time: time from msg arrival to time msg enters the server (now)
                emit(queueingTimeSignal, simTime() - msgServiced->getTimestamp());

                EV << "Starting service of " << msgServiced->getName() << endl;
                simtime_t serviceTime = getServiceTimeForPriority(msgServiced->getPriority());

                if (isPreemptive && preemptiveResume) scheduleAt(simTime() + serviceTime - msgServiced->getWorkTime(), endServiceMsg);
                else scheduleAt(simTime() + serviceTime, endServiceMsg);
            }
        }*/

        else { // Queue contains users

            int notEmpty = 0;
            if((notEmpty = getMsgToServe()) != -1){ //queue is not empty!
                cQueue *queue = check_and_cast<cQueue*>(queues.get(notEmpty)); //taking the most important queue that is not empty

                PriorityMessage *m = (PriorityMessage*)(queue->pop());
                emit(qlenSignal, getTotalQueueLength()); //Queue length changed, emit new length!

                msgServiced = m; //serving the message

                //Waiting time: time from msg arrival to time msg enters the server (now)
                emit(queueingTimeSignal, simTime() - msgServiced->getTimestamp());

                EV << "Starting service of " << msgServiced->getName() << endl;
                simtime_t serviceTime = getServiceTimeForPriority(m->getPriority());
                EV << "with service time of " << serviceTime.str() << "s" << endl;

                auto time = SIMTIME_ZERO;
                if (isPreemptive && preemptiveResume && m->getWorkLeft() > 0) time = simTime() + m->getWorkLeft();
                else time = simTime() + serviceTime;

                workEnd = time;
                scheduleAt(time, endServiceMsg);

                emit(busySignal, true);
            }
        }
    }
    else { // Data msg has arrived
        if(isPreemptive){ //check if the server is preemptive

            PriorityMessage* msgInService = (PriorityMessage*)msgServiced;
            PriorityMessage* arrivedMsg = (PriorityMessage*)msg;

            if(msgServiced && msgInService->getPriority() > arrivedMsg->getPriority()){//NB look at the condition ">".
                //if there's someone with less priority, kick him away

                ((cQueue*)queues.get(msgInService->getPriority()))->insert(msgInService); //putting the msg in service away
                bubble("Preemption occurred!");
                EV << "Message " << msgServiced->getName() << " was thrown out because of preemption" << endl;
                emit(qlenSignal, getTotalQueueLength());
                EV << "Message " << msgServiced->getName() << " is back in queue" << endl;
                cancelEvent(endServiceMsg);

                if(preemptiveResume){
                    msgInService->setWorkLeft(workEnd - simTime()); // if we have to resume later, we save the work time that's already been done
                    EV << "Message " << msgInService->getName() << " has " << msgInService->getWorkLeft() << " work time left" << endl;
                }

                msgServiced = arrivedMsg;

                EV << "Starting service of " << msgServiced->getName() << endl;
                simtime_t serviceTime = getServiceTimeForPriority(arrivedMsg->getPriority());
                EV << "with service time of " << serviceTime.str() << "s" << endl;

                auto time = SIMTIME_ZERO;
                if (isPreemptive && preemptiveResume && arrivedMsg->getWorkLeft() > 0) time = simTime() + arrivedMsg->getWorkLeft();
                else time = simTime() + serviceTime;

                workEnd = time;
                scheduleAt(time, endServiceMsg);
            }

        }//end of if(isPreemptive)

        //Setting arrival timestamp as msg field
        msg->setTimestamp();

        if (!msgServiced) { //No message in service (server IDLE) ==> No queue ==> Direct service

            PriorityMessage *m = check_and_cast<PriorityMessage*>(msg);
            msgServiced = m;
            emit(queueingTimeSignal, SIMTIME_ZERO);

            EV << "Starting service of " << msgServiced->getName() << endl;
            simtime_t serviceTime = getServiceTimeForPriority(m->getPriority());

            auto time = SIMTIME_ZERO;
            if (isPreemptive && preemptiveResume && m->getWorkLeft() > 0) time = simTime() + m->getWorkLeft();
            else time = simTime() + serviceTime;

            workEnd = time;
            scheduleAt(time, endServiceMsg);

            emit(busySignal, true);
        }
        else if(strcmp(msgServiced->getName(), msg->getName()) != 0){  //if needed for preemption
            //Message in service (server BUSY) ==> Queuing

            EV << "Queuing " << msg->getName() << endl;

            int prio = ((PriorityMessage*)msg)->getPriority();
            ((cQueue*)(queues.get(prio)))->insert(((PriorityMessage*)msg));
            emit(qlenSignal, getTotalQueueLength());
       }
    }
}// end of handleMessage

int Queue::getMsgToServe(){
    //scan sequentially from priority 0 (the most important) to the last and get the next message to Serve
    for(int i = 0; i <= queues.size(); i++){
        cQueue *c = (cQueue*)(queues.get(i));
        if(c && !c->isEmpty()){
            ASSERT(c->getLength() > 0);
            return i;
        }
    }
    //if they are all empty, return -1
    return -1;
}

PriorityMessage* Queue::getMsgPtrToServe(){
    //other version. this one returns the message instead of index
    for(int i = 0; i <= queues.size()-1; i++){
            cQueue *c = (cQueue*)(queues.get(i));
            if(c && !c->isEmpty()){

                int len = c->getLength();
                ASSERT(len > 0);

                PriorityMessage *m = check_and_cast<PriorityMessage*>(c->pop());

                emit(qlenSignal, getTotalQueueLength()); //Queue length changed, emit new length!
                ASSERT(len-1 == c->getLength()); // checking if the element was really popped
                return m;
            }
        }
        //if they are all empty, return nullptr
        return nullptr;
}

double Queue::getServiceTimeForPriority(int priority){
    if(priority >= 0 && priority < numPrio && serviceTimes.size() > 0){
        if (priority <= (serviceTimes.size() - 1)) return omnetpp::exponential(rng, serviceTimes.at(priority)); // if the serviceTimes array has enough values, return the correct one
        else return omnetpp::exponential(rng, serviceTimes.at(rand() % serviceTimes.size())); // otherwise just return a random time out of all the available ones
    }

    return 0;
}

long Queue::getTotalQueueLength(){
    long len = 0;
    for (int i = 0; i < queues.size(); i++){
        if (cQueue *queue = check_and_cast<cQueue*>(queues[i])){
            len += queue->getLength();
        }
    }
    return len;
}
