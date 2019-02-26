simple Sink
{
    parameters:
        @display("i=block/sink");
        @signal[arrivedMsg](type="long");
        
        // General
        @signal[responseTime](type="simtime_t");
        
        // Per-class
        @signal[responseTime0](type="simtime_t");
        @signal[responseTime1](type="simtime_t");
        @signal[responseTime2](type="simtime_t");
        @signal[responseTime3](type="simtime_t");
        @signal[responseTime4](type="simtime_t");
        
        // General
        @statistic[responseTime](title="lifetime of arrived msg"; unit=s; record=mean; interpolationmode=none);
        
        // Per-class
        @statistic[responseTime0](title="lifetime of arrived msg"; unit=s; record=mean; interpolationmode=none);
        @statistic[responseTime1](title="lifetime of arrived msg"; unit=s; record=mean; interpolationmode=none);
        @statistic[responseTime2](title="lifetime of arrived msg"; unit=s; record=mean; interpolationmode=none);
        @statistic[responseTime3](title="lifetime of arrived msg"; unit=s; record=mean; interpolationmode=none);
        @statistic[responseTime4](title="lifetime of arrived msg"; unit=s; record=mean; interpolationmode=none);
        gates:
        input in;
}
