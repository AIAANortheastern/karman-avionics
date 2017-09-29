The setup of the project is as follows:

-Project in Atmel Stuido 7 with the XMEGA256A3U microcontroller and the "user board" template.

-A whole bunch of Atmel Software Framework (ASF) libraries added.
-One header file designed to make the Timer Controller ASF driver actually compile.

-A "framework" in the src/framwork directory that includes a timer to provide a 
2Khz tick, and a simple cooperative scheduler to schedule tasks to be run.

The list of tasks to be run is known at compile-time, stored in the file framwork/Tasks.c.
Each individual task is defined in the directory tasks.
Drivers go in the drivers directory.

Each task/driver can define a "background task" to add to the background task list.
These will be run at the end of every "tick" (every 500us). Suggestions for what to put here
include polling, transmission, or resource intensive calculations.

Note! The current implementation includes no sense of "task priority". If your function takes forever, 
you'll slow everything down.

Task functions should follow the following conventions:

Each task and associated utlities should appear in the same file, e.g. MyTask.c.
Declare every function in a header file, e.g. MyTask.h.

The function prototype for the task needs to be void task_func(void);
No exceptions! If your task needs "arguments", define them in a global struct that you 
initialize during your task_init function.

Add your task to the task list in Tasks.c. Always keep the background task last!
The task list requires a task_freq field. Keep in mind how long it takes to execute your
task and how important it is when choosing a frequency!
