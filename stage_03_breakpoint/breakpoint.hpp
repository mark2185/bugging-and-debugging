#include <sys/ptrace.h>
#include <sys/types.h>

struct Breakpoint {
    Breakpoint() = default;

    Breakpoint( pid_t const pid, std::intptr_t const addr )
        : pid{ pid }, addr{ addr } {}

    void enable() {
        // save current instruction
        saved_data = ptrace( PTRACE_PEEKDATA, pid, addr, nullptr );

        // overwrite with int3 (0xcc)
        ptrace( PTRACE_POKEDATA, pid, addr, ( saved_data & ~0xff ) | 0xcc );

        enabled = true;
    }

    void disable() {
        // restore instruction
        ptrace( PTRACE_POKEDATA, pid, addr, saved_data );

        saved_data = 0;
        enabled = false;
    }

    bool          is_enabled()  const { return enabled; }
    std::intptr_t get_address() const { return addr;    }

private:
    pid_t pid;
    std::intptr_t addr;
    bool enabled{};
    long saved_data{};
};
