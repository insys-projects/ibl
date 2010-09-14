#*****************************************************************************
#* FILE PURPOSE:Scan C files for include references
#*
#******************************************************************************
#* FILE NAME:   makedep.awk
#*
#* DESCRIPTION:
#*
#* NOTE:        Written for Thompson Automation TAWK compiler
#*
#* NOTE:	    Processes #includes even in false conditional compilation
#*
#* (C) Copyright 1999, Telogy Networks, Inc.
#******************************************************************************

local progname              = "makedep"
local version_string        = "V1.01"
local version_date          = "Aug 17, 2001"

#*******************************************************************************
# Types & Constants
#*******************************************************************************
local true = 1
local false = 0

local c_re_include          =  /^[ \t]*#[ \t]*include[ \t]+/
local s_re_include          =  /^[ \t]*\.(include|copy)[ \t]+/
local re_include            = c_re_include

#*******************************************************************************
# Command Line Options
#*******************************************************************************
local quiet                 = false
local no_warn               = false
local bracket_includes      = true
local quote_includes        = true
local abs_paths             = true
local recurse               = true
local depend_not_found      = false
local stdout_format         = false
local indent_string         = "    "
local objext                = "obj"
local outfile_spec          = ""

local outfile               = ""
local infile                = ""
local infile_basename

local err_file

local orig_argc


local paths[]
local depends[]             # actually [][]
local processed_files[]

#*******************************************************************************
# Syntax
#*******************************************************************************
local comment_start         = "# "
local comment_stop          = ""
local eol_continue          = "\\"

#*******************************************************************************
# File Processing
#*******************************************************************************
local file_error            = false

#*******************************************************************************
# Program Debug & Control
#*******************************************************************************
local trace_file            = "trace.out"
local trace_classes[]
local exit_code             = -1
local banner_done           = false

#*******************************************************************************
# Top Level Functions
#*******************************************************************************
BEGIN \
{
    local i
    local j
    local option
    local arg
    local sub_option
    local input_found = false
    local any_gen = false

    trace_classes[ "includes" ] = 0
    trace_classes[ "flow" ]     = 0
    trace_classes[ "file" ]     = 0
    trace_classes[ "fileops" ]  = 0     # 1 = inhibit file delete rename, etc


    paths[0] = 1
    paths[1] = ""       # place holder for "current place" directory

    trace( "flow", "BEGIN" )

    err_file = stdout

    for ( i=1; i < ARGC; i++)
    {
        if (ARGV[i] ~ /^-/)
        {
            option = substr(ARGV[i], 2, 1)
            arg    = substr(ARGV[i], 3)

            if (option == "I")
            {
                if (arg == "")
                {
                    # don't process this argument as a file
                    ARGV[i] = ""

                    i++
                    arg = ARGV[i]
                }
                paths[0]++
                paths[paths[0]] = arg
            }
            else if (option == "a")
            {
                re_include = s_re_include
            }
            else if (option == "e")
            {
                if (arg == "")
                {
                    # don't process this argument as a file
                    ARGV[i] = ""

                    i++
                    arg = ARGV[i]
                }
                objext = arg	
            }
            else if (option == "f")
            {
                depend_not_found = true
            }
            else if (option == "p")
            {
                stdout_format = true
            }
            else if (option == "o")
            {
                if (arg == "")
                {
                    # don't process this argument as a file
                    ARGV[i] = ""

                    i++
                    arg = ARGV[i]
                }
                outfile_spec = arg	
            }
            else if (option == "r")
            {
                recurse = false
            }
            else if (option == "s")
            {
                bracket_includes = false
            }
            else if (option == "q")
            {
                quiet = true
            }
            else if (option == "w")
            {
                no_warn = true
            }
            else if (option == "d")
            {
                process_trace_option(arg)
            }
            else if (option == "h" || option == "?")
            {
                help()
            }
            else
            {
                fatal_error("bad option:" ARGV[i])
            }

            # don't process this argument as a file
            ARGV[i] = ""
        }
        else
        {
            input_found = true
            ARGV[i] = to_abs_path(ARGV[i])
        }
    }

    if (!quiet)
    {
        banner()
    }

    if (stdout_format)
    {
        if (outfile_spec != "")
        {
            fatal_error("can't specify both -p and -o")
        }

        err_file = stderr
        outfile  = stdout
    }

    if (outfile_spec == "")
    {
        outfile_spec = "./$.d"
    }

    if (!input_found)
    {
        fatal_error("nothing to do, use " progname "-h for help");
    }

    # we add more arguments as we go so remember the number of original args
    orig_argc = ARGC
}

match( $0, re_include ) != 0 \
{
    local num_depends
    local filename
    local abs_filename
    local this_dir

    trace("includes", "#include line:" $0)

    # find directory of current file
    this_dir = file_path(FILENAME)
    if (this_dir == "")
    {
        this_dir = "./"
    }

    # assume that the file name is the second arg
    filename = $2

    # is this a bracket include?
    if (filename ~ /^</)
    {
        # yes, should we process it?
        if (!bracket_includes)
        {
            # no
            next
        }
        paths[1] = ""
    }
    else
    {
        # no, must be a quote include, should we process it?
        if (!quote_includes)
        {
            # no
            next
        }
        paths[1] = this_dir
    }

    # strip off the quotes or brackets
	if (filename ~ /^[<"]/)
	{
       filename = substr(filename, 2, length(filename)-2)
	}

    abs_filename = find_file(filename, paths);
    if (abs_filename == "")
    {
        warn("include file not found", filename)
        abs_filename = filename
    }
    else
    {
        if (recurse && !(abs_filename in processed_files))
        {
            trace("flow", "queueing file " abs_filename)
            ARGV[ARGC] = abs_filename
            processed_files[abs_filename] = 1
            ARGC++
            depends[abs_filename][0] = 0
        }
    }

    num_depends                     = depends[FILENAME][0]+1
    depends[FILENAME][num_depends]  = abs_filename
    depends[FILENAME][0]            = num_depends

    next
}

# all other lines do nothing
{
    next
}

END \
{
    local arg

    trace( "flow", "END" )

    if (exit_code >= 0)
    {
        exit(exit_code)
    }

    for (arg=1; arg < orig_argc; arg++)
    {
        delete processed_files
        infile = ARGV[arg]

        if (infile != "" && !stdout_format)
        {
            file_done()

            infile_basename = basename(infile)
            infile_basename = strip_ext(infile_basename)
            outfile = outfile_spec
            gsubs("$", infile_basename, outfile, 0)

            gen_header()
            file_error = false
        }

        process_depends(infile, 1)
    }
}

#*******************************************************************************
# Command Line Functions
#*******************************************************************************
function process_trace_option(arg)
{
    local i
    local setting

    if ( arg ~ /^\+/ )
    {
        setting = 1
    }
    else if ( arg ~ /^-/ )
    {
        setting = 0
    }
    else
    {
        fatal_error("bad debug option:" ARGV[i])
    }

    arg = substr(arg,2)
    if (arg == "")
    {
        for (i in trace_classes)
        {
            trace_classes[i] = setting
        }
    }
    else if (arg in trace_classes)
    {
        trace_classes[arg] = setting
    }
    else
    {
        banner()
        print "Valid Trace Classes are:"
        for (i in trace_classes)
        {
            print "\t" i
        }
        fatal_error("unknown trace class " arg)
    }
}

function banner()
{
    if (banner_done)
    {
        return
    }

    banner_done = true
    print toupper(progname) " " version_string ": C include dependency generator/lister" >err_file
    print "Last updated " version_date >err_file
}

function help()
{
    banner()
    print "usage " progname " <options> <file>"
    print "where options is 0 or more of the following:"
    print "(default values are listed in paren's)"
    print "    -Idir                add an include path"
    print "    -a                   use asm format includes"
    print "    -eext                set obj extension to ext"
    print "    -s                   don't process standard include files ie. <>"
    print "    -r                   don't recurse to find dependencies of found include file"
    print "    -o                   specify output file (./$.d)"
    print "    -p                   pipe output to stdout w/o extra formating"
    print "    -q                   quiet mode (no banner)"
    print "    -w                   suppress warnings"
    print "    -d(+|-)[trace class] this program's diagnostics (see source)"
    print "    -h or -?             this help"
    exit_code = 10
    exit
}

#*******************************************************************************
# File Functions
#*******************************************************************************
function file_done()
{
    close( outfile )
    if ( file_error )
    {
        delete_file( outfile )
    }
}

function file_time( name )
{
    return ctime(filetime(name))
}

function file_exists( name )
{
    return ( filemode(name) != "" )
}

function rename_file( old_name, new_name )
{
    trace( "file", "rename " old_name " to " new_name )
    if ( !traceif( "fileops" ) )
    {
        delete_file( new_name )
        if ( !file_exists( old_name ) )
        {
            print "File does not exist: " old_name
        }
        else
        {
            rename(old_name,new_name)
        }
    }
}

function delete_file( name )
{
    trace( "file", "delete " name )
    if ( !traceif( "fileops" ) && file_exists( name ) )
    {
        rmfile(name)
    }
}

# return filename without extension
function strip_ext( filename )
{
    local new_name

    # match the extension ( last dot and file component after it )
    if ( match( filename, /\.[^ \t/\\.]*$/ ) != 0 )
    {
        # has an extension, return everything up to but not including it
        new_name = substr( filename, 1, RSTART-1 )
    }
    else
    {
        # does not have an extension, return it all
        new_name = filename
    }

    trace( "file", "strip_ext of " filename " gives " new_name )
    return new_name
}

# return extension
function file_ext( filename )
{
    local new_name

    # match the extension ( last dot and file component after it )
    if ( match( filename, /\.[^ \t/\\.]*$/ ) != 0 )
    {
        # has an extension, return it
        new_name = substr( filename, RSTART )
    }
    else
    {
        # does not have an extension, return it all
        new_name = ""
    }

    trace( "file", "filename_ext of " filename " gives " new_name )
    return new_name
}

# return filename.ext w/o leading paths
function basename( filename )
{
    local new_name
    
    new_name = filename

    # turn all backslashes to forward slashes
    gsubs("\\", "/", new_name)

    # match the basename (everything after the last slash)
    if ( match( new_name, /\/[^/]*$/ ) != 0 )
    {
        # match found, return it
        new_name = substr(new_name, RSTART+1)
    }
    else
    {
        # no slashes found, strip off any drive spec
        # for this purpose a drive spec is everything before the first colon
        if ( match( new_name, /:[^:]*$/ ) != 0 )
        {
            # match found, return it
            new_name = substr(new_name, RSTART+1)
        }
    }

    trace( "file", "basename of " filename " gives " new_name )
    return new_name
}

# return drive & directories of filename
function file_path( filename )
{
    local new_name
    
    new_name = filename

    # turn all backslashes to forward slashes
    gsubs("\\", "/", new_name)

    # match the basename (everything after the last slash)
    if ( match( new_name, /\/[^/]*$/ ) != 0 )
    {
        # match found, return it
        new_name = substr(new_name, 1, RSTART-1)
    }
    else
    {
        # no slashes found, strip off any drive spec
        # for this purpose a drive spec is everything before the first colon
        if ( match( new_name, /:[^:]*$/ ) != 0 )
        {
            # match found, return it
            new_name = substr(new_name, 1, RSTART-1)
        }
        else
        {
            # no drive & no directories
            new_name = ""
        }
    }

    trace( "file", "basename of " filename " gives " new_name )
    return new_name
}

function to_abs_path(filename)
{
    # not implemented yet
    return filename
}

function find_file(filename, path_array)
{
    local i
    local test_file
    local num_paths

    num_paths = path_array[0]

    for (i=1; i <= num_paths; i++)
    {
        test_file = path_array[i]

        if (test_file != "")
        {
            if (!(test_file ~ /\/$/))
            {
                test_file = test_file "/"
            }
            test_file = test_file filename
            test_file = to_abs_path(test_file)

            if (file_exists(test_file))
            {
                return test_file
            }
        }
    }

    return ""
}

#*******************************************************************************
# Output Processing
#*******************************************************************************
function process_depends(filename, level)
{
    local i
    local j
    local abs_file
    local num_depends

    num_depends = depends[filename][0]

    for (i=1; i <= num_depends; i++)
    {
        abs_file = depends[filename][i]

        for (j=0; j < level; j++)
        {
            printf("%s", indent_string) >outfile
        }

        if (abs_file in processed_files)
        {
#            printf("%s %-40s already included %s %s\n", \
#                comment_start, abs_file, comment_stop, eol_continue) >outfile
        }
        else
        {
            processed_files[abs_file] = 1

            if (abs_file in depends)
            {
                printf("%-40s %s\n", abs_file, eol_continue) >outfile
#                printf("%-40s %s %s %s %s\n", \
#                    abs_file, comment_start, file_time(abs_file), \
#                    comment_stop, eol_continue)   >outfile
                process_depends(abs_file, level+1)
            }
            else
            {
                if (depend_not_found)
                {
				    printf("%-40s %s\n", abs_file, eol_continue) >outfile
#                    printf("%-40s %s not found %s %s\n", \
#                        abs_file, comment_start, comment_stop, eol_continue) >outfile
                }
                else
                {
#                    printf("%s %-40s not found %s %s\n", \
#                        comment_start, abs_file, comment_stop, eol_continue) >outfile
                }
            }
        }
    }
}

function gen_header()
{
    # print header

    local   infile_ext
    local   outfile_ext

    infile_ext      = file_ext(infile)
    outfile_ext     = file_ext(outfile)

    print comment_start "Dependency file for " infile comment_stop >outfile
    print comment_start "Generated on " ctime() comment_stop >outfile 
    print comment_start "From " infile " dated " ctime(filetime(infile)) comment_stop >outfile
    print comment_start "This file was generated by " progname ", do not edit!!" comment_stop >outfile
    print "" >outfile
    print infile_basename "." objext, ":", infile, infile_basename outfile_ext, eol_continue >outfile
}

#*******************************************************************************
# Support Functions
#*******************************************************************************
function error( desc )
{
    print "Error: " desc " in " FILENAME " line " FNR >err_file
    file_error = true
}

function warn( desc, arg )
{
    local err

    if (no_warn)
    {
        return
    }

    if (arg != "")
    {
        arg = sprintf("for item %s ", arg)
    }

    printf("Warning: %s%s in %s line %d\n", arg, desc, FILENAME, FNR) >err_file
}

function fatal_error( desc )
{
    print "Error: " desc >err_file

    exit_code = 3
    exit
}

function trace( class, msg )
{
    if ( traceif( class ) )
    {
        print msg >trace_file
    }
}

function traceif( class )
{
    if ( !(class in trace_classes) )
    {
        print "trace class", class, "missing" >trace_file
        return true
    }
    return ( trace_classes[class] != 0 )
}
