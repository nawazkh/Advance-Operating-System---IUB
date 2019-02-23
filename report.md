About my implementation:
1. to run the stream_proc, use the command stream_proc -s <number of consumers> -w <depth of each work queue> -t <time window> -o <output_time>
2. Globally accessible queues are created for each stream_proc ( i.e. seperate queue for each consumer).
3. Both phase 1 and phase 2 of the assignments have been implemented.

Questions:
1. What can you do about mixed input during debugging?
	a. I have put checks on the number of inputs stream_proc woulg get. If the number of arguments is less than 9. do not start the stream_proc.
	b. To remove overlapping of outputs from producers and consumers, I have used mutexes in consumerrs. These mutexes are linked in producers as well. And if a consumer is not processing as fast as the producer is producing the data, th mutex will block the producer from reading the data and placing it in the queue.
	c. Static input stream can be sanitized to check if all the input charracters are numerics or not.
	d. User can make mistakes in giving inputs to -s, -t , -w, -o. Those inputs inputs can be checked before parsing.

2. Where do you needd mutexes?
	a. We need mutexes in places where there is a shared resource which is being accessed by multiple processes. To block simultaneous access and provide chance to each and every process, we use mutexes.

