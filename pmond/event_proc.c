#include "event_proc.h"

#include <libcgroup.h>

static proc_t proc_info;

int process_event_uid(struct proc_event proc_ev) {
    memset(&proc_info, 0, sizeof(proc_info));
    get_proc_stats(proc_ev.event_data.id.process_pid, &proc_info);

    flog(LOG_DEBUG, "(0x%08x) UID event: tid=%d pid=%d from %d to %d (cmd = %s)",
        proc_ev.what,
        proc_ev.event_data.id.process_pid,
        proc_ev.event_data.id.process_tgid,
        proc_ev.event_data.id.r.ruid,
        proc_ev.event_data.id.e.euid,
        proc_info.cmd);

    if (proc_info.euid > 500 && proc_info.egid > 500) {
        char buf[256];
        memset(&buf, 0,256);
        sprintf(buf, "bdusers/%d", proc_ev.event_data.id.process_pid);
        flog(LOG_DEBUG, "egid = %d, euid = %d both > 500. Process with pid=%d shuld be put in a new cgroup '%s'",
            proc_info.egid, proc_info.euid, proc_ev.event_data.id.process_pid, buf);

        struct cgroup *cg = cgroup_new_cgroup(buf);
        cgroup_add_controller(cg, "memory");
        cgroup_set_uid_gid(
            cg,
            proc_info.euid,
            proc_info.egid,
            0,0 //this is root
        );

        struct cgroup_controller *cg_memctl = cgroup_get_controller(cg, "memory");

        cgroup_set_value_uint64(cg_memctl, "memory.limit_in_bytes", 10485760); //10MB
        cgroup_create_cgroup(cg, 0);

        cgroup_attach_task_pid(cg, proc_ev.event_data.id.process_pid);

        cgroup_free(&cg);
    }

    return 1;
}



int process_event_exit(struct proc_event proc_ev) {
    flog(LOG_DEBUG, "(0x%08x) EXIT event: tid=%d pid=%d exit_code=%d",
        proc_ev.what,
        proc_ev.event_data.exit.process_pid,
        proc_ev.event_data.exit.process_tgid,
        proc_ev.event_data.exit.exit_code);

    char buf[256];
    memset(&buf, 0, 256);
    sprintf(buf, "bdusers/%d", proc_ev.event_data.exit.process_pid);
    flog(LOG_DEBUG, "Attempting cleanup of cgroup %s", buf);
    struct cgroup *cg = cgroup_new_cgroup(buf);
    int res = 0;
    if ( (res = cgroup_get_cgroup(cg)) == 0) {
        // TODO - here we have to kill all childs spawned from that process
        res = cgroup_delete_cgroup_ext(cg, CGFLAG_DELETE_RECURSIVE | CGFLAG_DELETE_IGNORE_MIGRATION );
        flog(LOG_DEBUG, "Cgroup found => cleaning up %s (clean result = %d)", buf, res);
    } else {
        flog(LOG_DEBUG, "No cgroup %s found => no clean will be done.", buf);
    }
    cgroup_free(&cg);

    return 1;
}
