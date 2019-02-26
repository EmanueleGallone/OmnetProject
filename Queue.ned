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
        
        // Global
        @signal[queueingTime](type="simtime_t");
        
        @signal[eServiceTime](type="simtime_t");
        
        // Per-class
        @signal[queueingTime0](type="simtime_t");
        @signal[queueingTime1](type="simtime_t");
        @signal[queueingTime2](type="simtime_t");
        @signal[queueingTime3](type="simtime_t");
        @signal[queueingTime4](type="simtime_t");
        
        @signal[eServiceTime0](type="simtime_t");
        @signal[eServiceTime1](type="simtime_t");
        @signal[eServiceTime2](type="simtime_t");
        @signal[eServiceTime3](type="simtime_t");
        @signal[eServiceTime4](type="simtime_t");
        
        @statistic[qlen](title="queue length";record=vector,timeavg;interpolationmode=sample-hold);
        @statistic[busy](title="server busy state";record=timeavg;interpolationmode=sample-hold);
        
        // Global
        @statistic[queueingTime](title="queueing time";unit=s;record=mean;interpolationmode=none);
        
        @statistic[eServiceTime](title="extended service time";unit=s;record=mean;interpolationmode=none);
        
        // Per-class
    	@statistic[queueingTime0](title="queueing time";unit=s;record=mean;interpolationmode=none);
    	@statistic[queueingTime1](title="queueing time";unit=s;record=mean;interpolationmode=none);
    	@statistic[queueingTime2](title="queueing time";unit=s;record=mean;interpolationmode=none);
    	@statistic[queueingTime3](title="queueing time";unit=s;record=mean;interpolationmode=none);
    	@statistic[queueingTime4](title="queueing time";unit=s;record=mean;interpolationmode=none);
    	
    	@statistic[eServiceTime0](title="extended service time";unit=s;record=mean;interpolationmode=none);
    	@statistic[eServiceTime1](title="extended service time";unit=s;record=mean;interpolationmode=none);
    	@statistic[eServiceTime2](title="extended service time";unit=s;record=mean;interpolationmode=none);
    	@statistic[eServiceTime3](title="extended service time";unit=s;record=mean;interpolationmode=none);
    	@statistic[eServiceTime4](title="extended service time";unit=s;record=mean;interpolationmode=none);
    gates:
        input in;
        output out;
}