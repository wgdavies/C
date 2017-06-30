#ifndef EVENT_PROC_H
#define EVENT_PROC_H

#include <linux/cn_proc.h>
#include <proc/readproc.h>
#include <string.h>

#include "flog.h"

int process_event_uid(struct proc_event proc_ev);
int process_event_exit(struct proc_event proc_ev);

#endif
