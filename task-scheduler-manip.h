#include <Windows.h>
#include <string>
#include <optional>

typedef uintptr_t(__cdecl* get_task_scheduler_def)();
inline auto get_task_scheduler = reinterpret_cast<get_task_scheduler_def>(0xEB3AD0 - 0x400000 + reinterpret_cast<uintptr_t>(GetModuleHandleA(nullptr)));

/* not used in source, but included, if you want to expand the struct use ReClass */
struct task_job {
    void* vtable; // 0
    int unk, unk0, unk1; // 4 8 12
    std::string name; // 16
    int padding[4]; // 20 24 28 32
    double elapsed; // 36
    char padding1[12]; // 44
    double time_spent_in_job; // 56
    char padding2[8]; // 64
    uintptr_t state; // 72
};


/*
 * WaitingScriptJob VFTable
 * 0000 function0
 * 0004 function1
 * 0008 function2
 * 000C function3
 * 0010 function4
 * 0014 function5
 */

inline char script_job_hook()
{
    // do script scheduling, whatever you want to do
    printf("RenderStepped/WaitingScriptsJob called\n");
    return 0;
}

class task_scheduler
{
public:
    static void hook_vftable(uintptr_t job, uintptr_t hook_addr, size_t size, int offset);
	
    template <class C, typename F> // ret, callback type
    C iterate_jobs(const uintptr_t task_scheduler, F&& callback)
    {
        auto job_index = *(reinterpret_cast<uintptr_t*>(task_scheduler) + 81);
        for (const auto job_end = *(reinterpret_cast<uintptr_t*>(task_scheduler) + 82); job_index != job_end; job_index += 8) /* iterate jobs, found in most function calls after gettaskscheduler */
        {
            /* this is just calling functor (a lambda essentially basically which handles job_index) */
            const auto ret = callback(job_index);
            if (ret.has_value())
            {
                return ret.value();
            }
        }
        return 0;
    }

    uintptr_t get_state();

    void hook_script_job();

private:
    uintptr_t task_scheduler_ = get_task_scheduler();
};
