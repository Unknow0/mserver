exec "normal i#! /bin/sh"
exec "normal o### BEGIN INIT INFO"
exec "normal o# Provides:          mserver"
exec "normal o# Required-Start:    $remote_fs $syslog"
exec "normal o# Required-Stop:     $remote_fs $syslog"
exec "normal o# Default-Start:     2 3 4 5"
exec "normal o# Default-Stop:      0 1 6"
exec "normal o# Short-Description: starting media server"
exec "normal o# Author: Unknow <unknow0@wanadoo.fr>"
exec "normal o### END INIT INFO"
exec "normal o"
exec "normal o# Do NOT \"set -e\""
exec "normal o"
exec "normal o# PATH should only include /usr/* if it runs after the mountnfs.sh script"
exec "normal oPATH=/sbin:/usr/sbin:/bin:/usr/bin"
exec "normal oDESC=\"Start media server\""
exec "normal oNAME=mserver"
exec "normal oDAEMON=".g:path."mserver"
exec "normal oDAEMON_ARGS=\"\""
exec "normal oPIDFILE=/var/run/$NAME.pid"
exec "normal oSCRIPTNAME=/etc/init.d/$NAME"
exec "normal o"
exec "normal o# Exit if the package is not installed"
exec "normal o[ -x \"$DAEMON\" ] || exit 0"
exec "normal o"
exec "normal o# Read configuration variable file if it is present"
exec "normal o[ -r /etc/default/$NAME ] && . /etc/default/$NAME"
exec "normal o"
exec "normal o# Load the VERBOSE setting and other rcS variables"
exec "normal o. /lib/init/vars.sh"
exec "normal o"
exec "normal o# Define LSB log_* functions."
exec "normal o# Depend on lsb-base (>= 3.2-14) to ensure that this file is present"
exec "normal o# and status_of_proc is working."
exec "normal o. /lib/lsb/init-functions"
exec "normal o"
exec "normal o#"
exec "normal o# Function that starts the daemon/service"
exec "normal o#"
exec "normal odo_start()"
exec "normal o{"
exec "normal o	# Return"
exec "normal o	#   0 if daemon has been started"
exec "normal o	#   1 if daemon was already running"
exec "normal o	#   2 if daemon could not be started"
exec "normal o	start-stop-daemon --start --quiet --pidfile $PIDFILE --exec $DAEMON --test > /dev/null \\"
exec "normal o		|| return 1"
exec "normal o	start-stop-daemon --start --quiet --pidfile $PIDFILE --exec $DAEMON -- \\"
exec "normal o		$DAEMON_ARGS \\"
exec "normal o		|| return 2"
exec "normal o	# Add code here, if necessary, that waits for the process to be ready"
exec "normal o	# to handle requests from services started subsequently which depend"
exec "normal o	# on this one.  As a last resort, sleep for some time."
exec "normal o}"
exec "normal o"
exec "normal o#"
exec "normal o# Function that stops the daemon/service"
exec "normal o#"
exec "normal odo_stop()"
exec "normal o{"
exec "normal o	# Return"
exec "normal o	#   0 if daemon has been stopped"
exec "normal o	#   1 if daemon was already stopped"
exec "normal o	#   2 if daemon could not be stopped"
exec "normal o	#   other if a failure occurred"
exec "normal o	start-stop-daemon --stop --quiet --retry=TERM/30/KILL/5 --pidfile $PIDFILE --name $NAME"
exec "normal o	RETVAL=\"$?\""
exec "normal o	[ \"$RETVAL\" = 2 ] && return 2"
exec "normal o	# Wait for children to finish too if this is a daemon that forks"
exec "normal o	# and if the daemon is only ever run from this initscript."
exec "normal o	# If the above conditions are not satisfied then add some other code"
exec "normal o	# that waits for the process to drop all resources that could be"
exec "normal o	# needed by services started subsequently.  A last resort is to"
exec "normal o	# sleep for some time."
exec "normal o	start-stop-daemon --stop --quiet --oknodo --retry=0/30/KILL/5 --exec $DAEMON"
exec "normal o	[ \"$?\" = 2 ] && return 2"
exec "normal o	# Many daemons don't delete their pidfiles when they exit."
exec "normal o	rm -f $PIDFILE"
exec "normal o	return \"$RETVAL\""
exec "normal o}"
exec "normal o"
exec "normal o#"
exec "normal o# Function that sends a SIGHUP to the daemon/service"
exec "normal o#"
exec "normal odo_reload() {"
exec "normal o	#"
exec "normal o	# If the daemon can reload its configuration without"
exec "normal o	# restarting (for example, when it is sent a SIGHUP),"
exec "normal o	# then implement that here."
exec "normal o	#"
exec "normal o	start-stop-daemon --stop --signal 1 --quiet --pidfile $PIDFILE --name $NAME"
exec "normal o	return 0"
exec "normal o}"
exec "normal o"
exec "normal ocase \"$1\" in"
exec "normal o  start)"
exec "normal o	[ \"$VERBOSE\" != no ] && log_daemon_msg \"Starting $DESC\" \"$NAME\""
exec "normal o	do_start"
exec "normal o	case \"$?\" in"
exec "normal o		0|1) [ \"$VERBOSE\" != no ] && log_end_msg 0 ;;"
exec "normal o		2) [ \"$VERBOSE\" != no ] && log_end_msg 1 ;;"
exec "normal o	esac"
exec "normal o	;;"
exec "normal o  stop)"
exec "normal o	[ \"$VERBOSE\" != no ] && log_daemon_msg \"Stopping $DESC\" \"$NAME\""
exec "normal o	do_stop"
exec "normal o	case \"$?\" in"
exec "normal o		0|1) [ \"$VERBOSE\" != no ] && log_end_msg 0 ;;"
exec "normal o		2) [ \"$VERBOSE\" != no ] && log_end_msg 1 ;;"
exec "normal o	esac"
exec "normal o	;;"
exec "normal o  status)"
exec "normal o	status_of_proc \"$DAEMON\" \"$NAME\" && exit 0 || exit $?"
exec "normal o	;;"
exec "normal o  #reload|force-reload)"
exec "normal o	#"
exec "normal o	# If do_reload() is not implemented then leave this commented out"
exec "normal o	# and leave 'force-reload' as an alias for 'restart'."
exec "normal o	#"
exec "normal o	#log_daemon_msg \"Reloading $DESC\" \"$NAME\""
exec "normal o	#do_reload"
exec "normal o	#log_end_msg $?"
exec "normal o	#;;"
exec "normal o  restart|force-reload)"
exec "normal o	#"
exec "normal o	# If the \"reload\" option is implemented then remove the"
exec "normal o	# 'force-reload' alias"
exec "normal o	#"
exec "normal o	log_daemon_msg \"Restarting $DESC\" \"$NAME\""
exec "normal o	do_stop"
exec "normal o	case \"$?\" in"
exec "normal o	  0|1)"
exec "normal o		do_start"
exec "normal o		case \"$?\" in"
exec "normal o			0) log_end_msg 0 ;;"
exec "normal o			1) log_end_msg 1 ;; # Old process is still running"
exec "normal o			*) log_end_msg 1 ;; # Failed to start"
exec "normal o		esac"
exec "normal o		;;"
exec "normal o	  *)"
exec "normal o		# Failed to stop"
exec "normal o		log_end_msg 1"
exec "normal o		;;"
exec "normal o	esac"
exec "normal o	;;"
exec "normal o  *)"
exec "normal o	#echo \"Usage: $SCRIPTNAME {start|stop|restart|reload|force-reload}\" >&2"
exec "normal o	echo \"Usage: $SCRIPTNAME {start|stop|status|restart|force-reload}\" >&2"
exec "normal o	exit 3"
exec "normal o	;;"
exec "normal oesac"
exec "normal o"
exec "normal o:"
