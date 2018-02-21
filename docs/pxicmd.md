# PXICMD Protocol

> _Literally the simplest protocol ever_ - Some dude.


#### Features


Pros:

- extremely simple to implement and debug

- basically no overhead


Cons:

- terribly limited to 256 commands and 16 words per command

- designed as blocking

- only two-way communication, no arbitrary producer(s) - consumer(s)

- no defined shared memory (could be a good thing or a bad thing, it's bad in this case)

- blindly assumes no caching is performed on either processor `A` or `B`, so all cache writeback/invalidation must be performed manually (if necessary)


#### How it works

After both processor `A` and `B` have been synchronized to an initial "standard" state, whenever processor `A` wants to send a command to `B` it sets up its remote port to the command ID, puts the arguments in the send FIFO and triggers an interrupt that will be caught by processor `B`. Processor `A` is put to sleep until the receive FIFO is not empty.

Once processor `B` gets the interrupt, it reads the remote port to get the command ID and empties its receive FIFO. It processes the command and then puts a single word in its send FIFO, which gets read by processor `A` afterwards as a result/return code. The logic code in processor `A` then acts accordingly.
