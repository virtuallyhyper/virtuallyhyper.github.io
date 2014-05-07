#!/bin/sh

#PATH=/sbin:/usr/sbin:/bin:/usr/bin

RETVAL=0

start()
{
    /bin/echo -n "Starting zabbix agent:           "

    if [ -e /opt/var/run/zabbix_agentd.pid ]; then
        if [ -e /proc/$(cat /opt/var/run/zabbix_agentd.pid) ]; then
            /bin/echo "Success (Already running)"
        else
            if [ -e /opt/zabbix/etc/zabbix_agentd.conf ]; then
            	/bin/rm -f /opt/var/run/zabbix_agentd.pid
            	/usr/bin/nohup /opt/zabbix/sbin/zabbix_agentd -c /opt/zabbix/etc/zabbix_agentd.conf>&/dev/null &
            	/bin/echo "Success (Stale pidfile)"
            else
            	echo "Config File missing"
            	exit 1
            fi
        fi
    else
        if [ -e /opt/zabbix/etc/zabbix_agentd.conf ]; then
           /usr/bin/nohup /opt/zabbix/sbin/zabbix_agentd -c /opt/zabbix/etc/zabbix_agentd.conf>&/dev/null &
           echo "Success"
        else
           echo "Config File Missing"
           exit 1
        fi
    fi

    return $RETVAL;
}

stop() {
    /bin/echo -n "Stopping zabbix agent:           "

    if [ -e /opt/var/run/zabbix_agentd.pid ]; then
        PID=$(cat /opt/var/run/zabbix_agentd.pid)
        if [ -e /proc/$PID ]; then
            /bin/kill $PID
            # Wait a second for zabbix agent to die gracefully before SIGKILLing
            for count in 1 2 3 4 5; do
                if [ -e /proc/$PID ]; then
                    /bin/usleep 200000
                else
                    break
                fi
            done
            if [ -e /proc/$PID ]; then
                /bin/kill -KILL $PID
                /bin/rm -f /opt/var/run/zabbix_agentd.pid
                /bin/echo "Success (Killed)"
            else
                /bin/echo "Success"
            fi
        else
            /bin/rm -f /opt/var/run/zabbix_agentd.pid
            /bin/echo "Success (Stale pidfile)"
        fi
    else
        /bin/echo "Success (Not running)"
    fi

    return $RETVAL
}

restart() {
    stop
    start
}

#
# Usage statement.
#

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    restart)
        restart
        ;;
    *)
        /bin/echo "usage: $0 {start|stop|restart}"
        exit 1
        ;;
esac
