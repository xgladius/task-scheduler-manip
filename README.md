# task-scheduler-manip

This library can retrieve ScriptContext/Lua State, easily and efficiently, and hook a virtual function in the virtual function table of the job to achieve script scheduling (with one address)

The way the ScriptContext method works, is it iterates all jobs in the task scheduler, checks if the job is "WaitingScriptJob", and offsets by 392 to retrieve ScriptContext.

The way the vftable function hook works, is it hooks the second job in WaitingScriptJobs virtual function table, which is an empty function that returns 0. This function is executed in Roblox at the same rate as RenderStepped is called, allowing for script scheduling.

The virtual function can be hooked because it's located in the rdata section, memcheck doesn't check .rdata.

Example usage (non external):
```c
task_scheduler t;
printf("Roblox lua state: %x\n", t.get_state());
t.hook_script_job();
```


To update:
Replace
```c
inline auto get_task_scheduler = reinterpret_cast<get_task_scheduler_def>(0xEB3AD0 - 0x400000 + reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr)));
```
with the updated address, and replace
```c
const auto state = script_context + 164 + 56 * 0 - *reinterpret_cast<uintptr_t*>(script_context + 164 + 56 * 0); // state obfus
```
with the updated state offset
