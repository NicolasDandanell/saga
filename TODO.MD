#To Do

* Add better interrupt support:
    What happens if an interrupt triggers while a log is being added to the buffer, and that interrupt also wants to log? There needs to be support for this of some sort...

    Maybe add counter and fix header forward first, and copy data after? Thus the next add operation will be performed as if the data was already copied?

    Atomic mutex
    Separate interrupt buffer

    Mutex only in the head and byte count ? + Atomic

    Lock-free

    Buffer struct should be volatile

* Add multithreading support:
    The same as above is also true for multithreaded environments. How should this be tackled? Maybe it's a good opportunity to learn how semaphores and mutexes work in practice
