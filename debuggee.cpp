#include <cstdio>
#include <string>

int main()
{
    std::printf( "Hello world! We're here to debug me!\n" );
    int x{ 5 };
    std::string const s{ "I am a string" };

    for ( auto i{ 0U }; i < 5; ++i )
    {
        std::printf( "The value of x now is: %d\n", x );
        x += 1;
    }
    std::printf( "This is the string variable: %s\n", s.c_str() );

    return 0;
}
