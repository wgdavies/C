#include "event_proc.h"

#include <libcgroup.h>
#include <stdlib.h>

static proc_t proc_info;

static inline make_cgroup_name(char **ret_string, pid_t pid) {
    memset(*ret_string, 0, sizeof(ret_string));
    sprintf(*ret_string, "bdusers/%d", pid);
}

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
        char *cgroup_name = malloc(256 * sizeof(char));
        make_cgroup_name(&cgroup_name, proc_ev.event_data.id.process_pid);
        flog(LOG_DEBUG, "egid = %d, euid = %d both > 500. Process with pid=%d shuld be put in a new cgroup => '%s'",
            proc_info.egid, proc_info.euid, proc_ev.event_data.id.process_pid, cgroup_name);

        struct cgroup *cg = cgroup_new_cgroup(cgroup_name);
        cgroup_add_controller(cg, "memory");
        cgroup_set_uid_gid(
            cg,
            proc_info.euid,
            proc_info.egid,
            0,0 //this is root
        );

        struct cgroup_controller *cg_memctl = cgroup_get_controller(cg, "memory");

        cgroup_set_value_uint64(cg_memctl, "memory.limit_in_bytes", 10485760); //10MB
        cgroup_set_value_bool(cg_memctl, "notify_on_release", 1);
        cgroup_create_cgroup(cg, 0);

        cgroup_attach_task_pid(cg, proc_ev.event_data.id.process_pid);

        cgroup_free(&cg);
        free(cgroup_name);
    } //from if

    return 1;
}

