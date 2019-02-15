simple Queue
{
    parameters:
        //pool of service times
        volatile double serviceTime1 @unit(s);
        volatile double serviceTime2 @unit(s);
        volatile double serviceTime3 @unit(s);
        volatile double serviceTime4 @unit(s);
        volatile double serviceTime5 @unit(s);
        
        volatile int numPrio;
        volatile bool isPreemptive = default(false);
        @display("i=block/queue;q=queue");
        
        @signal[qlen](type="long");
        @signal[busy](type="bool");
        @signal[queueingTime](type="simtime_t");
        @signal[responseTime](type="simtime_t");
        
        @statistic[qlen](title="queue length";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[busy](title="server busy state";record=vector?,timeavg;interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue";unit=s;record=vector,mean,max;interpolationmode=none);
        @statistic[responseTime](title="response time at end of service";unit=s;record=vector,mean,max;interpolationmode=none);
    
    gates:
        input in;
        output out;
}