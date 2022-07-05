#include <sys/ptrace.h>
#include <sys/user.h>

#include <array>
#include <string>

enum class Register {
  r15,
  r14,
  r13,
  r12,
  rbp,
  rbx,
  r11,
  r10,
  r9,
  r8,
  rax,
  rcx,
  rdx,
  rsi,
  rdi,
  orig_rax,
  rip,
  cs,
  eflags,
  rsp,
  ss,
  fs_base,
  gs_base,
  ds,
  es,
  fs,
  gs
};

struct RegisterDescriptor {
    Register r;
    int dwarf_id{};
    std::string name{};
    std::uint64_t value{};
};

inline auto init_registers() {
    using enum Register;
    return std::array< RegisterDescriptor, 27 >{{
        { r15      , 15 , "r15"      , 0 } ,
        { r14      , 14 , "r14"      , 0 } ,
        { r13      , 13 , "r13"      , 0 } ,
        { r12      , 12 , "r12"      , 0 } ,
        { rbp      ,  6 , "rbp"      , 0 } ,
        { rbx      ,  7 , "rbx"      , 0 } ,
        { r11      , 11 , "r11"      , 0 } ,
        { r10      , 10 , "r10"      , 0 } ,
        { r9       ,  9 , "r9"       , 0 } ,
        { r8       ,  8 , "r8"       , 0 } ,
        { rax      ,  0 , "rax"      , 0 } ,
        { rcx      ,  2 , "rcx"      , 0 } ,
        { rdx      ,  1 , "rdx"      , 0 } ,
        { rsi      ,  4 , "rsi"      , 0 } ,
        { rdi      ,  5 , "rdi"      , 0 } ,
        { orig_rax , -1 , "orig_rax" , 0 } ,
        { rip      , -1 , "rip"      , 0 } ,
        { cs       , 51 , "cs"       , 0 } ,
        { eflags   , 49 , "eflags"   , 0 } ,
        { rsp      ,  7 , "rsp"      , 0 } ,
        { ss       , 52 , "ss"       , 0 } ,
        { fs_base  , 58 , "fs_base"  , 0 } ,
        { gs_base  , 59 , "gs_base"  , 0 } ,
        { ds       , 53 , "ds"       , 0 } ,
        { es       , 50 , "es"       , 0 } ,
        { fs       , 54 , "fs"       , 0 } ,
        { gs       , 55 , "gs"       , 0 } ,
    }};
}

inline std::uint64_t get_register_value( pid_t const pid, Register const r ) {
    using enum Register;

    user_regs_struct regs;
    ptrace( PTRACE_GETREGS, pid, nullptr, &regs );

    switch( r ) {
        case r15      : return regs.r15;
        case r14      : return regs.r14;
        case r13      : return regs.r13;
        case r12      : return regs.r12;
        case rbp      : return regs.rbp;
        case rbx      : return regs.rbx;
        case r11      : return regs.r11;
        case r10      : return regs.r10;
        case r9       : return regs.r9;
        case r8       : return regs.r8;
        case rax      : return regs.rax;
        case rcx      : return regs.rcx;
        case rdx      : return regs.rdx;
        case rsi      : return regs.rsi;
        case rdi      : return regs.rdi;
        case orig_rax : return regs.orig_rax;
        case rip      : return regs.rip;
        case cs       : return regs.cs;
        case eflags   : return regs.eflags;
        case rsp      : return regs.rsp;
        case ss       : return regs.ss;
        case fs_base  : return regs.fs_base;
        case gs_base  : return regs.gs_base;
        case ds       : return regs.ds;
        case es       : return regs.es;
        case fs       : return regs.fs;
        case gs       : return regs.gs;
    }
}

inline void set_register_value( pid_t const pid, Register const r, std::uint64_t const value ) {
    using enum Register;

    user_regs_struct regs;
    ptrace( PTRACE_GETREGS, pid, nullptr, &regs );

    switch( r ) {
        case r15      : regs.r15      = value; break;
        case r14      : regs.r14      = value; break;
        case r13      : regs.r13      = value; break;
        case r12      : regs.r12      = value; break;
        case rbp      : regs.rbp      = value; break;
        case rbx      : regs.rbx      = value; break;
        case r11      : regs.r11      = value; break;
        case r10      : regs.r10      = value; break;
        case r9       : regs.r9       = value; break;
        case r8       : regs.r8       = value; break;
        case rax      : regs.rax      = value; break;
        case rcx      : regs.rcx      = value; break;
        case rdx      : regs.rdx      = value; break;
        case rsi      : regs.rsi      = value; break;
        case rdi      : regs.rdi      = value; break;
        case orig_rax : regs.orig_rax = value; break;
        case rip      : regs.rip      = value; break;
        case cs       : regs.cs       = value; break;
        case eflags   : regs.eflags   = value; break;
        case rsp      : regs.rsp      = value; break;
        case ss       : regs.ss       = value; break;
        case fs_base  : regs.fs_base  = value; break;
        case gs_base  : regs.gs_base  = value; break;
        case ds       : regs.ds       = value; break;
        case es       : regs.es       = value; break;
        case fs       : regs.fs       = value; break;
        case gs       : regs.gs       = value; break;
    }

    ptrace( PTRACE_SETREGS, pid, nullptr, &regs );
}

/*
// Register - DRAWF register number
// taken from DWARF x86_64 ABI - https://www.uclibc.org/docs/psABI-x86_64.pdf
//  rax      -  0
//  rdx      -  1
//  rcx      -  2
//  rbx      -  3
//  rsi      -  4
//  rdi      -  5
//  rbp      -  6
//  rsp      -  7
//  r8       -  8
//  r9       -  9
//  r10      - 10
//  r11      - 11
//  r12      - 12
//  r13      - 13
//  r14      - 14
//  r15      - 15
//  eflags   - 49
//  es       - 50
//  cs       - 51
//  ss       - 52
//  ds       - 53
//  fs       - 54
//  gs       - 55
//  fs_base  - 58
//  gs_base  - 59
//  orig_rax - -1
//  rip      - -1
//
inline std::uint64_t get_register_value_from_dwarf_register( pid_t pid, unsigned int regnum ) {
    using enum Register;
    switch ( regnum ) {
        case  0: return get_register_value( pid, rax );
        case  1: return get_register_value( pid, rdx ); 
        case  2: return get_register_value( pid, rcx );
        case  3: return get_register_value( pid, rbx );
        case  4: return get_register_value( pid, rsi );
        case  5: return get_register_value( pid, rdi );
        case  6: return get_register_value( pid, rbp );
        case  7: return get_register_value( pid, rsp );
        case  8: return get_register_value( pid, r8 );
        case  9: return get_register_value( pid, r9 );
        case 10: return get_register_value( pid, r10 );
        case 11: return get_register_value( pid, r11 );
        case 12: return get_register_value( pid, r12 );
        case 13: return get_register_value( pid, r13 );
        case 14: return get_register_value( pid, r14 );
        case 15: return get_register_value( pid, r15 );
        case 49: return get_register_value( pid, eflags );
        case 50: return get_register_value( pid, es );
        case 51: return get_register_value( pid, cs );
        case 52: return get_register_value( pid, ss );
        case 53: return get_register_value( pid, ds );
        case 54: return get_register_value( pid, fs );
        case 55: return get_register_value( pid, gs );
        case 58: return get_register_value( pid, fs_base );
        case 59: return get_register_value( pid, gs_base );
    }
    return {};
}
*/

inline Register get_register_from_name( std::string const & name ) {
    using enum Register;
    if ( name == "rax"      ) return rax;
    if ( name == "rdx"      ) return rdx;
    if ( name == "rcx"      ) return rcx;
    if ( name == "rbx"      ) return rbx;
    if ( name == "rsi"      ) return rsi;
    if ( name == "rdi"      ) return rdi;
    if ( name == "rbp"      ) return rbp;
    if ( name == "rsp"      ) return rsp;
    if ( name == "r8 "      ) return r8;
    if ( name == "r9 "      ) return r9;
    if ( name == "r10"      ) return r10;
    if ( name == "r11"      ) return r11;
    if ( name == "r12"      ) return r12;
    if ( name == "r13"      ) return r13;
    if ( name == "r14"      ) return r14;
    if ( name == "r15"      ) return r15;
    if ( name == "eflags"   ) return eflags;
    if ( name == "es"       ) return es;
    if ( name == "cs"       ) return cs;
    if ( name == "ss"       ) return ss;
    if ( name == "ds"       ) return ds;
    if ( name == "fs"       ) return fs;
    if ( name == "gs"       ) return gs;
    if ( name == "fs_base"  ) return fs_base;
    if ( name == "gs_base"  ) return gs_base;
    if ( name == "orig_rax" ) return orig_rax;
    if ( name == "rip"      ) return rip;

    return rip;
}
