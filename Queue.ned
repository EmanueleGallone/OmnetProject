simple Queue
{
    parameters:
        //pool of service times
        volatile string serviceTimes = default("0.20 0.25 0.30 0.35 0.40");
        
        volatile int numPrio = default(5);
        volatile bool preemptive = default(false);
        volatile bool resume = default(false);
        @display("i=block/queue;q=queue");
        
        @signal[qlen](type="long");
        @signal[busy](type="bool");
        @signal[queueingTime](type="simtime_t");
        @signal[responseTime](type="simtime_t");
        
        @statistic[qlen](title="queue length";record=vector,timeavg;interpolationmode=sample-hold);
        @statistic[busy](title="server busy state";record=timeavg;interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue";unit=s;record=vector,mean,max;interpolationmode=none);
        @statistic[responseTime](title="response time at end of service";unit=s;record=vector,mean,max;interpolationmode=none);
    
    gates:
        input in;
        output out;
}