network Net
{        
    parameters:
    
    submodules:
        gen: Source{
            parameters:
                @display("p=89,100");
        }
        sink: Sink {
            parameters:
                @display("p=329,100");
        }
        queue: Queue {
            parameters:
        }
        
    connections:
        gen.out --> {  delay = 300ms; } --> queue.in;
        queue.out --> sink.in;
}
