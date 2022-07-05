#include <cstdio>
#include <unistd.h>

int main( int const argc, char const * argv[] ) {
    if ( argc < 2 ) {
        std::printf( "Program not specified!\n" );
        return -1;
    }

    auto prog{ argv[ 1 ] };
    auto pid{ fork() };

    if ( pid == 0 ) {
        // child process ( debugee )
        execl( prog, prog, nullptr );
    } else if ( pid >= 1 ) {
        // parent process ( debugger )
        printf( "Hello world, I am the parent! I am going to debug!\n" );
    }
    return 0;
}
