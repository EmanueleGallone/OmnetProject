network Net
{
    parameters:
        volatile int numPriorities = default(5); //max 99! if you want more, change the length of the array inside Source 
        volatile bool preemptive = default(false);
        
        //pool of times needed for differentiate the priorities
        //you can mix these times inside the parameters of Source and Queue
        //unfortunately there is no way to create an array of times
        //actually the first 5 priority queues get the service time sequentially.
        //after the #5 the service time is random. for more info, see Queue::getServiceTimeForPriority(int priority)
        volatile double Time1 @unit(s) = 0.1s;
        volatile double Time2 @unit(s) = 0.2s;
        volatile double Time3 @unit(s) = 0.3s;
        volatile double Time4 @unit(s) = 0.4s;
        volatile double Time5 @unit(s) = 0.5s;
    submodules:
        gen: Source{
            parameters:
                numPrio = numPriorities;
                interArrivalTime1 = Time1;
                interArrivalTime2 = Time2;
                interArrivalTime3 = Time3;
                interArrivalTime4 = Time4;
                interArrivalTime5 = Time5;
                @display("p=89,100");
        }
        sink: Sink {
            parameters:
                numPrio = numPriorities;
                @display("p=329,100");
        }
        queue: Queue {
            numPrio = numPriorities;
            serviceTime1 = Time1;
            serviceTime2 = Time2;
            serviceTime3 = Time3;
            serviceTime4 = Time4;
            serviceTime5 = Time5;
            //@display("p=200,152");
        }
        //test[numPriorities]: Queue{
        //    numPrio = numPriorities;
        //	@display("p=200,200");
        //}
    connections:
        //for i = 0..numPriorities-1 {
        //    gen.out2[i] --> test[i].in;
        //    test[i].out --> sink.in2[i];
        //}
        
        gen.out --> {  delay = 300ms; } --> queue.in;
        queue.out --> sink.in;
}
