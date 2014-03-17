package require cmdline

# if the above does not work you could point to the location of cmdline.tcl
# it would mean you do not have the cmdline library available
# source /usr/share/tcltk/tcllib1.14/cmdline/cmdline.tcl
# source cmdline.tcl

# based on example first featured on Hai's Blog
# http://wuhrr.wordpress.com/2009/09/13/parse-command-line-in-a-tcl-script/

# revised for use inside a TCL intrepreter and explained on Ten Thousdand Failures
# https://tenthousandfailures.squarespace.com/blog/2014/3/16/using-tcl-cmdline-library-with-interactive-tools

namespace eval ::cmdline_demo {
}

proc ::cmdline_demo::ssh {args} {

    # Show argv before processing
    puts "-----"
    puts "args = '$args'"
    
    array set arg [::cmdline::getoptions args {\
	{host.arg  ""  "STRING: hostname to connect to"}
	{port.arg  22  "INT:    port to connect to (default 22)"}
	{user.arg  ""  "STRING: username"}
	{verbose       "FLAG:   set the verbose ssh option on (default off)"}
    } "cmdline_demo::ssh ?options?"]
        

    # Verify required parameters
    set requiredParameters {host user}
    foreach parameter $requiredParameters {
	if {$arg($parameter) == ""} {
	    puts stderr "Missing required parameter: -$parameter"
	    exit 1
	}
    }
    
    # Displays the arguments using tcl parray
    puts ""
    parray arg
    puts ""
    
}

proc ::cmdline_demo::ssh_example {} {

    # example calls
    ::cmdline_demo::ssh -host "lsim01" -port 32 -user cmdline_demo -verbose
    ::cmdline_demo::ssh -host "lsim01" -port 23 -user ljohn
    ::cmdline_demo::ssh -host "lsim01" -user cmdline_demo -verbose
    ::cmdline_demo::ssh -host "lsim01" -user ljohn

}

proc ::cmdline_demo::ssh_help {} {

    # these lines will stop the run
    ::cmdline_demo::ssh -?
    ::cmdline_demo::ssh

}
