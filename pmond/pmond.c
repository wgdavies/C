
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <linux/cn_proc.h>
#include <proc/readproc.h>
#include <syslog.h>
#include <pwd.h>
#include <grp.h>
#include <libcgroup.h>

#include "macros.h"
#include "flog.h"
#include "event_proc.h"

FILE*   logfile;                     /// the daemon log file
int     loglevel = LOG_DEBUG;        /// current log level
int     logfacility = LOG_LOCAL0;    /// where do we log

/*
 * Create an endpoint for communication. Use the kernel user interface device (PF_NETLINK) which is a datagram oriented
 * service (SOCK_DGRAM). The protocol used is the connector protocol (NETLINK_CONNECTOR)
 *
 * @returns netlink socket, or -1 on error
 */
static int nl_connect() {
    int rc;
    int nl_sock;
    struct sockaddr_nl sa_nl;

    nl_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
    if (nl_sock == -1) {
        perror("socket");
        return -1;
    }

    sa_nl.nl_family = AF_NETLINK;
    sa_nl.nl_groups = CN_IDX_PROC;
    sa_nl.nl_pid = getpid();

    rc = bind(nl_sock, (struct sockaddr *)&sa_nl, sizeof(sa_nl));
    if (rc == -1) {
        perror("bind");
        close(nl_sock);
        return -1;
    }

    return nl_sock;
}

/*
 * Subscribe to the correct netlink channel so we can start capturing messages.
 *
 * @return 0 when success or -1 in case of error
 */
static int set_proc_ev_listen(int nl_sock, bool enable) {
    int rc;
    struct __attribute__ ((aligned(NLMSG_ALIGNTO))) {
        struct nlmsghdr nl_hdr;
        struct __attribute__ ((__packed__)) {
            struct cn_msg cn_msg;
            enum proc_cn_mcast_op cn_mcast;
        };
    } nlcn_msg;

    memset(&nlcn_msg, 0, sizeof(nlcn_msg));
    nlcn_msg.nl_hdr.nlmsg_len = sizeof(nlcn_msg);
    nlcn_msg.nl_hdr.nlmsg_pid = getpid();
    nlcn_msg.nl_hdr.nlmsg_type = NLMSG_DONE;

    nlcn_msg.cn_msg.id.idx = CN_IDX_PROC;
    nlcn_msg.cn_msg.id.val = CN_VAL_PROC;
    nlcn_msg.cn_msg.len = sizeof(enum proc_cn_mcast_op);

    nlcn_msg.cn_mcast = enable ? PROC_CN_MCAST_LISTEN : PROC_CN_MCAST_IGNORE;

    rc = send(nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
    if (rc == -1) {
        perror("netlink send");
        return -1;
    }

    return 0;
}

/*
 * Handle a single process event. This is the actual main loop in which the process spends its time processing  dgrams
 * received on the listen channel. It gets out is SIGHUP sets need_ext = true
 */
static volatile bool need_exit = false;
static int handle_proc_ev(int nl_sock) {
    int rc;
    struct __attribute__ ((aligned(NLMSG_ALIGNTO))) {
        struct nlmsghdr nl_hdr;
        struct __attribute__ ((__packed__)) {
            struct cn_msg cn_msg;
            struct proc_event proc_ev;
        };
    } nlcn_msg;

    //this will be used to fetch process info if needed (just example for now - doing it for fork events)
    proc_t proc_info;
    char buf[256];

    while (!need_exit) {
        rc = recv(nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
        if (rc == 0) {
            /* shutdown? */
            return 0;
        } else if (rc == -1) {
            if (errno == EINTR) continue;
            perror("netlink recv");
            flog(LOG_ERR, "NETLINK recv error.");
            return -1;
        }
        //printf("(0x%08x) ", nlcn_msg.proc_ev.what);
        switch (nlcn_msg.proc_ev.what) {
            case PROC_EVENT_NONE:
                flog(LOG_INFO ,"EVT_NONE event: NETLINK channel listener opened with success");
                break;

            case PROC_EVENT_UID:
                process_event_uid(nlcn_msg.proc_ev);
                break;

            case PROC_EVENT_GID:
                process_event_uid(nlcn_msg.proc_ev);
                break;

            case PROC_EVENT_EXIT:
                process_event_exit(nlcn_msg.proc_ev);
                break;
        }
    }

    return 0;
}

static void on_sigint(int unused) {
    need_exit = true;
}

int main(int argc, const char *argv[]) {

    /* initialize logging - we log only in our own log file  - FIX THIS !!!! */
    logfile = fopen("./pmond.log", "a");

    int nl_sock;
    int rc = EXIT_SUCCESS;
    int ret = 0;

    if ((ret = cgroup_init()) != 0) {
        flog(LOG_ERR, "cgroups lib initialization failed, %s\n", cgroup_strerror(ret));
        goto out;
    }
    flog(LOG_INFO, "cgroups lib initialized with success");

    signal(SIGINT, &on_sigint);
    siginterrupt(SIGINT, true);

    nl_sock = nl_connect();
    if (nl_sock == -1)
        exit(EXIT_FAILURE);

    rc = set_proc_ev_listen(nl_sock, true);
    if (rc == -1) {
        rc = EXIT_FAILURE;
        goto out;
    }

    rc = handle_proc_ev(nl_sock);
    if (rc == -1) {
        rc = EXIT_FAILURE;
        goto out;
    }

    set_proc_ev_listen(nl_sock, false);

out:
    close(nl_sock);
    exit(rc);
}