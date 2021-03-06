/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2008 Alex Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <IceUtil/Options.h>
#include <Slice/Preprocessor.h>

#include "gen.h"

using namespace std;
using namespace Slice;

void
usage(const char* n)
{
    cerr << "Usage: " << n << " --module MOD [output] [options] slice-files...\n";
    cerr <<     
    // alexm: our custom options
        "--module                 The nominal module name of this project.\n"
        "                         e.g. Orca project : '--module orca'\n"
        "                              Empty project: '--module empty'\n"
        "Output (one of the following):\n"
        "--util                   Generate utility functions, e.g. toString(), zeroAndClear().\n"
        "--log                    Generate ASCII log/replay functions.\n"
    // alexm: standard options
        "Options:\n"
        "-h, --help               Show this message.\n"
        "-v, --version            Display the Ice version.\n"
        "--header-ext EXT         Use EXT instead of the default `h' extension.\n"
        "--source-ext EXT         Use EXT instead of the default `cpp' extension.\n"
        "--add-header HDR[,GUARD] Add #include for HDR (with guard GUARD) to generated source file.\n"
        "-DNAME                   Define NAME as 1.\n"
        "-DNAME=DEF               Define NAME as DEF.\n"
        "-UNAME                   Remove any definition for NAME.\n"
        "-IDIR                    Put DIR in the include file search path.\n"
//         "-E                       Print preprocessor output on stdout.\n"
        "--include-dir DIR        Use DIR as the header include directory in source files.\n"
        "--output-dir DIR         Create files in the directory DIR.\n"
//         "--dll-export SYMBOL      Use SYMBOL for DLL exports.\n"
//         "--impl                   Generate sample implementations.\n"
        "--depend                 Generate Makefile dependencies.\n"
        "-d, --debug              Print debug messages.\n"
//         "--ice                    Permit `Ice' prefix (for building Ice source code only)\n"
//         "--checksum               Generate checksums for Slice definitions.\n"
//         "--stream                 Generate marshaling support for public stream API.\n"
        ;
    // Note: --case-sensitive is intentionally not shown here!
}

int
main(int argc, char* argv[])
{
    IceUtilInternal::Options opts;
    opts.addOpt("h", "help");
    opts.addOpt("v", "version");
    opts.addOpt("", "header-ext", IceUtilInternal::Options::NeedArg, "h");
    opts.addOpt("", "source-ext", IceUtilInternal::Options::NeedArg, "cpp");
    opts.addOpt("", "add-header", IceUtilInternal::Options::NeedArg, "", IceUtilInternal::Options::Repeat);
    opts.addOpt("D", "", IceUtilInternal::Options::NeedArg, "", IceUtilInternal::Options::Repeat);
    opts.addOpt("U", "", IceUtilInternal::Options::NeedArg, "", IceUtilInternal::Options::Repeat);
    opts.addOpt("I", "", IceUtilInternal::Options::NeedArg, "", IceUtilInternal::Options::Repeat);
    opts.addOpt("E");
    opts.addOpt("", "include-dir", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "output-dir", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "dll-export", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "impl");
    opts.addOpt("", "depend");
    opts.addOpt("d", "debug");
    opts.addOpt("", "ice");
    opts.addOpt("", "checksum");
    opts.addOpt("", "stream");
    opts.addOpt("", "case-sensitive");
    // alexm: our custom options
    opts.addOpt("", "module", IceUtilInternal::Options::NeedArg);
    opts.addOpt("", "util");
    opts.addOpt("", "log");

    vector<string> args;
    try
    {
        args = opts.parse(argc, (const char**)argv);
    }
    catch(const IceUtilInternal::BadOptException& e)
    {
        cerr << argv[0] << ": " << e.reason << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if(opts.isSet("help"))
    {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if(opts.isSet("version"))
    {
        cout << ICE_STRING_VERSION << endl;
        return EXIT_SUCCESS;
    }

    string headerExtension = opts.optArg("header-ext");
    string sourceExtension = opts.optArg("source-ext");
    
    vector<string> extraHeaders = opts.argVec("add-header");

    vector<string> cppArgs;
    vector<string> optargs = opts.argVec("D");
    vector<string>::const_iterator i;
    for(i = optargs.begin(); i != optargs.end(); ++i)
    {
        cppArgs.push_back("-D" + *i);
    }

    optargs = opts.argVec("U");
    for(i = optargs.begin(); i != optargs.end(); ++i)
    {
        cppArgs.push_back("-U" + *i);
    }

    vector<string> includePaths;
    includePaths = opts.argVec("I");
    for(i = includePaths.begin(); i != includePaths.end(); ++i)
    {
        cppArgs.push_back("-I" + Preprocessor::normalizeIncludePath(*i));
    }

//     bool preprocess = opts.isSet("E");

    string include = opts.optArg("include-dir");

    string output = opts.optArg("output-dir");

    string dllExport = opts.optArg("dll-export");

    bool impl = opts.isSet("impl");

    bool depend = opts.isSet("depend");

    bool debug = opts.isSet("debug");

    bool ice = opts.isSet("ice");

    bool checksum = opts.isSet("checksum");

    bool stream = opts.isSet("stream");

    bool caseSensitive = opts.isSet("case-sensitive");

    // alexm: custom options

    string module = opts.optArg("module");

    bool genUtil = opts.isSet("util");

    bool genLog = opts.isSet("log");

    if ( !depend && !genUtil && !genLog ) 
    {    
        cerr << argv[0] << ": no output type specified" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if ( genUtil && genLog ) 
    {    
        cerr << argv[0] << ": more than one output type specified" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    // alexm: end custom options

    if(args.empty())
    {
        cerr << argv[0] << ": no input file" << endl;
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;

    for(i = args.begin(); i != args.end(); ++i)
    {
        if(depend)
        {
            Preprocessor icecpp(argv[0], *i, cppArgs);
            icecpp.printMakefileDependencies(Preprocessor::CPlusPlus, includePaths);
        }
        else
        {
            Preprocessor icecpp(argv[0], *i, cppArgs);
            FILE* cppHandle = icecpp.preprocess(false);
    
            if(cppHandle == 0)
            {
                return EXIT_FAILURE;
            }
    
            UnitPtr u = Unit::createUnit(false, false, ice, caseSensitive);
            int parseStatus = u->parse(*i, cppHandle, debug);
        
            if(!icecpp.close())
            {
                u->destroy();
                return EXIT_FAILURE;
            }
    
            if(parseStatus == EXIT_FAILURE)
            {
                status = EXIT_FAILURE;
            }
            else
            {
                slice2orca::Gen gen(argv[0], icecpp.getBaseName(), headerExtension, sourceExtension, extraHeaders, include,
                        includePaths, dllExport, output, impl, checksum, stream, ice,
                        module, genUtil, genLog );
                if(!gen)
                {
                    u->destroy();
                    return EXIT_FAILURE;
                }
                gen.generate(u);
            }
    
            u->destroy();
        }
    }

    return status;
}
