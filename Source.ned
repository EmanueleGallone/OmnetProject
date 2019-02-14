simple Source
{
    parameters:
        volatile double interArrivalTime1 @unit(s);
        volatile double interArrivalTime2 @unit(s);
        volatile double interArrivalTime3 @unit(s);
        volatile double interArrivalTime4 @unit(s);
        volatile double interArrivalTime5 @unit(s);
        volatile int numPrio;
        @display("i=block/source");
    gates:
        output out;
}