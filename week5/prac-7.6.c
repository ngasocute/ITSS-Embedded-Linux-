#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

#define SYSLOG_OK 0
#define SYSLOG_NG -1

int main(void)
{
    //start  to use syslog
    openlog("syslogTest", LOG_PID, LOG_USER | LOG_DEBUG);

    // output to syslog
    syslog(LOG_USER | LOG_EMERG, "syslog Test: EMERG");

    // mask
    setlogmask(LOG_MASK(LOG_EMERG) | LOG_MASK(LOG_NOTICE));

    //output to syslog
    syslog(LOG_USER | LOG_EMERG,"syslog MaskTest: EMERG");

    //end  to use  syslog
    closelog();

    return SYSLOG_OK;
}
