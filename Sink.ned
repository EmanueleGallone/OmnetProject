simple Sink
{
    parameters:
        volatile int numPrio;
        @display("i=block/sink");
        @signal[lifetime](type="simtime_t");
        @signal[arrivedMsg](type="long");
        @statistic[lifetime](title="lifetime of arrived msg"; unit=s; record=histogram,vector,mean,max; interpolationmode=none);
        @statistic[arrivedMsg](title="number of arrived msg"; record=vector,last; interpolationmode=none);
        gates:
        input in;
}
