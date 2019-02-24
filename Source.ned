simple Source
{
    parameters:
        volatile string interArrivalTimes = default("0.20 0.25 0.30 0.35 0.40");
        volatile int numPrio = default(5); //max 99! if you want more, change the length of the array inside Source.cc
        @display("i=block/source");
    gates:
        output out;
}