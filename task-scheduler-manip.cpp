#include "task-scheduler-manip.h"

void task_scheduler::hook_vftable(const uintptr_t job, const uintptr_t hook_addr, const size_t size, const int offset)
{
    auto* new_vtable = reinterpret_cast<uintptr_t*>(malloc(size));
    const auto old_vtable = *reinterpret_cast<uintptr_t**>(job);

    if (new_vtable == nullptr)
        return;

    memcpy(new_vtable, old_vtable, size);
    new_vtable[offset / 4] = hook_addr;
    DWORD old;
    VirtualProtect(reinterpret_cast<void*>(job), sizeof(job), PAGE_READWRITE, &old);
    *reinterpret_cast<uintptr_t**>(job) = &new_vtable[0];
    VirtualProtect(reinterpret_cast<void*>(job), sizeof(job), old, &old);
}

uintptr_t task_scheduler::get_state()
{
    const auto script_context = iterate_jobs<uintptr_t>(task_scheduler_, [](const uintptr_t job_index) -> std::optional<uintptr_t>
        {
            if (*reinterpret_cast<std::string*>(*reinterpret_cast<uintptr_t*>(job_index) + 16) == "WaitingScriptJob")
            {
                return *reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(job_index) + 400); // return scriptcontext
            }
            return {};
        });
    const auto state = (script_context + 56 * 0 + 164) ^ *reinterpret_cast<uintptr_t*>(script_context + 56 * 0 + 164); // state obfus
    return state;
}

void task_scheduler::hook_script_job()
{
    iterate_jobs<int>(task_scheduler_, [](const uintptr_t job_index) -> std::optional<int>
        {
            if (*reinterpret_cast<std::string*>(*reinterpret_cast<uintptr_t*>(job_index) + 16) == "WaitingScriptJob")
            {
                hook_vftable(*reinterpret_cast<uintptr_t*>(job_index), reinterpret_cast<uintptr_t>(script_job_hook), 40, 4); // 2nd function in vftable
                return 0;
            }
            return {};
        });
}
