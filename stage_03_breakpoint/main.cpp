#include <cstdio>
#include <unistd.h>

#include <sys/ptrace.h>
#include <sys/personality.h>

#include "debugger.hpp"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

int main( int const argc, char const * argv[] ) {
    if ( argc < 2 ) {
        std::printf( "Program not specified!\n" );
        return -1;
    }

    auto prog{ argv[ 1 ] };
    auto pid{ fork() };

    if ( pid == 0 ) {
        // child process ( debugee )
        printf("PID of the child is: %d\n", getpid() );
        personality( ADDR_NO_RANDOMIZE );
        ptrace( PTRACE_TRACEME, 0, nullptr, nullptr );
        execl( prog, prog, nullptr );
    } else if ( pid >= 1 ) {
        // parent process ( debugger )
        std::this_thread::sleep_for( 1s );
        printf( "Hello world, I am the parent! I am going to debug!\n" );

        Debugger dbg{ prog, pid };
        dbg.run();
    }
    return 0;
}
