# task-scheduler-manip

This library can retrieve ScriptContext/Lua State, easily and efficiently, and hook a virtual function in the virtual function table of the job to achieve script scheduling (with one address)

The way the ScriptContext method works, is it iterates all jobs in the task scheduler, checks if the job is "WaitingScriptJob", and offsets by 392 to retrieve ScriptContext.

The way the vftable function hook works, is it hooks the second job in WaitingScriptJobs virtual function table, which is an empty function that returns 0. This function is executed in Roblox at the same rate as RenderStepped is called, allowing for script scheduling.
