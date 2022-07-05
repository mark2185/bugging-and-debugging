#include <algorithm>
#include <array>
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
#include "registers.hpp"

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

    std::uint64_t get_pc()                          { return get_register_value( pid, Register::rip      ); }
    void          set_pc( std::uint64_t const val ) {        set_register_value( pid, Register::rip, val ); }

    void step_over_breakpoint()
    {
        auto possible_breakpoint_location{ get_pc() - 1 };

        if ( breakpoints.count( possible_breakpoint_location ) ) {
            auto & bp{ breakpoints [ possible_breakpoint_location ] };
            if ( bp.is_enabled() ) {
                set_pc( possible_breakpoint_location );

                bp.disable();
                // from the manpage: [Details of these kinds of stops are yet to be documented.]
                ptrace( PTRACE_SINGLESTEP, pid, nullptr, nullptr );
                wait_for_program();
                bp.enable();
            }
        }
    }

    void continue_execution()
    {
        step_over_breakpoint();
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
        } else if ( command == "register" || is_prefix( command, "reg" ) ) {
            if ( args.size() == 1 ) {
                std::cerr << "Invalid number of args. Usage: register <print/read/write> [reg name]\n";
                return;
            }
            if ( args[ 1 ] == "print" ) {
                reload_registers();
                print_registers();
            } else if ( args[ 1 ] == "read" || is_prefix( args[ 1 ], "r" ) ) {
                reload_registers();
                if ( auto const it  = std::find_if( std::begin( registers ), std::end( registers ), [args]( auto const & rd ) { return rd.name == args[ 2 ]; } );
                                it != std::end( registers ) )
                {
                    std::cout << std::setw(16) << std::hex << it->value << '\n';
                } else {
                    std::cerr << "Cannot find register '" << args[ 2 ] << "'\n";
                }
            } else if ( args[ 1 ] == "write" || is_prefix( args[ 1 ], "w" ) ) {
                std::cout << "Setting register " << args[ 2 ] << " to value " << std::hex << std::stol( args[ 3 ], 0, 16 ) << '\n';
                set_register_value( pid, get_register_from_name( args[ 2 ] ), std::stol( args[ 3 ], 0, 16 ) );
                reload_registers();
            }
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

    void reload_registers()
    {
        for ( auto & r : registers ) {
            r.value = get_register_value( pid, r.r ) ;
        }
    }

    void print_registers()
    {
        reload_registers();
        for ( auto const & r : registers ) {
            std::cout << r.name << " 0x" << std::setfill('0') << std::setw(16) << std::hex << r.value << '\n';
        }
    }

private:
    std::string prog_name{};
    pid_t pid{};
    std::unordered_map< std::intptr_t, Breakpoint > breakpoints{};
    std::array< RegisterDescriptor, 27 > registers{ init_registers() };
};
