#define _GNU_SOURCE
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <stdlib.h>

/* Fork a holder child that inherits every open fd (including forged
 * pipe_buffer slots) and sleeps forever. Called by any child whose
 * normal exit would tear down pipes containing forged slots.
 *
 * The holder clears PDEATHSIG so it outlives its parent, ignores
 * common signals, and detaches from the controlling terminal. It is
 * expected to leak until device reboot. */
void g4hold_now(void) {
    pid_t pid = fork();
    if (pid < 0) {
        return;   /* fork failed; parent proceeds to _exit as before */
    }
    if (pid == 0) {
        prctl(PR_SET_PDEATHSIG, 0);
        signal(SIGTERM, SIG_IGN);
        signal(SIGINT,  SIG_IGN);
        signal(SIGHUP,  SIG_IGN);
        signal(SIGPIPE, SIG_IGN);
        setsid();
        for (;;) pause();
        _exit(0);   /* unreachable */
    }
    /* Parent returns immediately. Holder is intentionally orphaned. */
}
