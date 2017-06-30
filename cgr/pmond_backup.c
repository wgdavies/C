#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <linux/netlink.h>
#include <libcgroup.h>

#include "macros.h"
#include "flog.h"

FILE*   logfile;        /// the daemon log file
int     loglevel;       /// current log level

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
            return -1;
        }
        switch (nlcn_msg.proc_ev.what) {
            case PROC_EVENT_NONE:
                printf("set mcast listen ok\n");
                break;
                /*
            case PROC_EVENT_FORK:

                //get process info from /proc
                memset(&proc_info, 0, sizeof(proc_info));
                get_proc_stats(nlcn_msg.proc_ev.event_data.fork.child_pid, &proc_info);
                //read_cmdline(&buf,256,nlcn_msg.proc_ev.event_data.fork.child_pid);
                //cmdline is not read
                printf("fork: parent tid=%d pid=%d -> child tid=%d pid=%d (cmd: %s)\n",
                        nlcn_msg.proc_ev.event_data.fork.parent_pid,
                        nlcn_msg.proc_ev.event_data.fork.parent_tgid,
                        nlcn_msg.proc_ev.event_data.fork.child_pid,
                        nlcn_msg.proc_ev.event_data.fork.child_tgid,
                        proc_info.cmd);
                break;
            case PROC_EVENT_EXEC:
                printf("exec: tid=%d pid=%d\n",
                        nlcn_msg.proc_ev.event_data.exec.process_pid,
                        nlcn_msg.proc_ev.event_data.exec.process_tgid);
                break;
                */
            case PROC_EVENT_UID:
                memset(&proc_info, 0, sizeof(proc_info));
                get_proc_stats(nlcn_msg.proc_ev.event_data.id.process_pid, &proc_info);

                printf("uid change: tid=%d pid=%d from %d to %d (cmd = %s)\n",
                        nlcn_msg.proc_ev.event_data.id.process_pid,
                        nlcn_msg.proc_ev.event_data.id.process_tgid,
                        nlcn_msg.proc_ev.event_data.id.r.ruid,
                        nlcn_msg.proc_ev.event_data.id.e.euid,
                        proc_info.cmd);


                break;
            case PROC_EVENT_GID:
                memset(&proc_info, 0, sizeof(proc_info));
                get_proc_stats(nlcn_msg.proc_ev.event_data.id.process_pid, &proc_info);

                printf("gid change: tid=%d pid=%d from %d to %d (cmd = %s)\n",
                        nlcn_msg.proc_ev.event_data.id.process_pid,
                        nlcn_msg.proc_ev.event_data.id.process_tgid,
                        nlcn_msg.proc_ev.event_data.id.r.rgid,
                        nlcn_msg.proc_ev.event_data.id.e.egid,
                        proc_info.cmd);
                break;

                /*
            case PROC_EVENT_SID:
                printf("sid change: tid=%d pid=%d \n",
                        nlcn_msg.proc_ev.event_data.id.process_pid,
                        nlcn_msg.proc_ev.event_data.id.process_tgid);
                break;
                */

            case PROC_EVENT_EXIT:
                memset(&proc_info, 0, sizeof(proc_info));
                get_proc_stats(nlcn_msg.proc_ev.event_data.exit.process_pid, &proc_info);

                printf("exit: tid=%d pid=%d exit_code=%d\n",
                        nlcn_msg.proc_ev.event_data.exit.process_pid,
                        nlcn_msg.proc_ev.event_data.exit.process_tgid,
                        nlcn_msg.proc_ev.event_data.exit.exit_code,
                        proc_info.cmd);
                break;
/*
            default:
                printf("unhandled proc event: \n");
                break;
    */
        }
    }

    return 0;
}

static void on_sigint(int unused) {
    need_exit = true;
}

int main(int argc, const char *argv[]) {

    /* initialize logging - we log only in our own log file */
    openlog ( "pmond.log", LOG_PERROR|LOG_PID, LOG_USER);

    int nl_sock;
    int rc = EXIT_SUCCESS;
    int ret = 0;

    if ((ret = cgroup_init()) != 0) {
        flog(stderr, "Error: libcgroup initialization failed, %s\n", cgroup_strerror(ret));
        goto out;
    }

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