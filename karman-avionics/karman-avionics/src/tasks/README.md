############################
README for task functions
############################

Task functions should follow the following conventions:

Each task and associated utlities should appear in the same file, <Task>.c.
Declare every function in a header file, <Task>.h.

The function prototype for the task needs to be void <task>_func(void);
No exceptions! If your task needs "arguments", define them in a global struct that you 
initialize during your <task>_init function.

Add your task to the task list in Tasks.c. Always keep the background task last!
The task list requires a task_freq field. Keep in mind how long it takes to execute your
task and how important it is when choosing a frequency!
