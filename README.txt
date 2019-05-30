You can only insert the data with format.

0   1   5   20

0 means you are in state of inserting data.
1 means process id
5 means priority
20 means computing time (burst time)

And if you want to indicate the present time,
insert 1 0 0 0.

This source is designed to allocate a certain amount of time quantum.
Default time quantum is 20. You can change this by editing #define TIME_QUANTUM.