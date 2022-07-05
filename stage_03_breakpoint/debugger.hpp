#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "breakpoint.hpp"

namespace
{
    inline std::vector< std::string > split( std::string const & s, char const delimiter ) {
        std::vector< std::string > out{};
        std::stringstream ss{ s };
        std::string item;

        while( std::getline(ss, item, delimiter ) ) {
            out.emplace_back(item);
        }

        return out;
    }

    inline bool is_prefix( std::string const & haystack, std::string const & needle ) {
        if ( needle.size() > haystack.size() ) return false;
        return std::equal( std::begin( haystack ), std::end( haystack ), std::begin( needle ) );
    }
}

struct Debugger {

    Debugger( std::string const & prog, pid_t const pid ) : prog_name{ prog }, pid{ pid } {}

    void wait_for_program()
    {
        int wait_status{};
        auto options{ 0 };
        waitpid( pid, &wait_status, options);
    }

    void continue_execution()
    {
        ptrace( PTRACE_CONT, pid, nullptr, nullptr );

        wait_for_program();
    }

    void handle_command( std::string const & line )
    {
        auto args{ split( line, ' ' ) };
        auto command{ args[ 0 ] };

        if ( command == "continue" || command == "c" || is_prefix( command, "cont" ) ) {
            continue_execution();
        } else if ( is_prefix( command, "break" ) ) {
            std::string const addr{ args[ 1 ] };
            set_breakpoint_at_address( std::stol( addr, 0, 16 ) );
        } else {
            std::cerr << "Unknown command\n";
        }
    }

    void run()
    {
        wait_for_program();

        do {
            std::printf( "dbgg> " );
            if ( std::string line; std::getline( std::cin, line ) ) {
                handle_command( line );
            } else {
                break;
            }
        } while ( true );
    }

    void set_breakpoint_at_address( std::intptr_t const addr )
    {
        std::cout << "Setting breakpoint on: " << std::setfill('0') << std::setw(16) << std::hex << addr << '\n';
        Breakpoint bp{ pid, addr };
        bp.enable();
        breakpoints [ addr ] = bp;
    }

private:
    std::string prog_name{};
    pid_t pid{};
    std::unordered_map< std::intptr_t, Breakpoint > breakpoints{};
};
