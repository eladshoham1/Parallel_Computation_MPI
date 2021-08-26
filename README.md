Names:
Elad Shoham - 206001752
Dmitry Inke - 336228317


Run times: (chunk = 16, procs = 4)
Static - 0.000035
dynamic - 0.000050

Static is better in this case because all the processes get equally jobs in the same time so that it will make only one iteration and beacuse the root in static use scatter once and in dynamic the root use send many times so it make it slower.



Run times: (chunk = 2, procs = 4)
Static - 0.000151
Dynamic - 0.000065

Dynamic is better because every process that done his mission, got another mission immediately so that all the time all the processes work in parallel on small chunks so it make it faster.