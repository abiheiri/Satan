#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <strings.h>

#define PORT 6666
#define BUFFER_SIZE 1024

static const char *frames[] = {
    "\033[2J\033[H\n"
    "...........................\n"
    "..........................\n"
    ".........................\n"
    "............../´¯//´¯¯//´¯¯`·¸\n"
    ".........../´/...../......./¨¯\\\n"
    "........('(...´...´.... ¯~/'...')\n"
    ".........\\.................'...../\n"
    "..........''...\\.......... _.·´\n"
    "............\\..............(\n"
    "..............\\.............\\...\n"
    "...............\\.............\\...\n",

    "\033[2J\033[H\n"
    "...........................\n"
    "..........................\n"
    ".................../´¯¯´/\n"
    "............../´¯/'...'/´¯¯`·¸\n"
    ".........../´/.../..../......./¨¯\\\n"
    "........('(...´...´.... ¯~/'...')\n"
    ".........\\.................'...../\n"
    "..........''...\\.......... _.·´\n"
    "............\\..............(\n"
    "..............\\.............\\...\n"
    "...............\\.............\\...\n",

    "\033[2J\033[H\n"
    "...........................\n"
    "....................,/´¯¯´/\n"
    ".................../..../\n"
    "............./´¯/'...'/´¯¯`·¸\n"
    "........../'/.../..../......./¨¯\\\n"
    "........('(...´...´.... ¯~/'...')\n"
    ".........\\.................'...../\n"
    "..........''...\\.......... _.·´\n"
    "............\\..............(\n"
    "..............\\.............\\...\n"
    "...............\\.............\\...\n",


    "\033[2J\033[H\n"
    "....................../´¯/)\n"
    "....................,/¯../\n"
    ".................../..../\n"
    "............./´¯/'...'/´¯¯`·¸\n"
    "........../'/.../..../......./¨¯\\\n"
    "........('(...´...´.... ¯~/'...')\n"
    ".........\\.................'...../\n"
    "..........''...\\.......... _.·´\n"
    "............\\..............(\n"
    "..............\\.............\\...\n"
    "...............\\.............\\...\n",

    "\033[2J\033[H\n"
    "....................../´¯/)\n"
    "....................,/¯../\n"
    ".................../..../\n"
    "............./´¯/'...'/´¯¯`·¸\n"
    "........../'/.../..../......./¨¯\\\n"
    "........('(...´...´.... ¯~/'...')\n"
    ".........\\.................'...../\n"
    "..........''...\\.......... _.·´\n"
    "............\\..............(\n"
    "..............\\.............\\...\n"
    "...............\\.............\\...\n"
};

static void play_animation(int fd) {
    size_t n = sizeof(frames) / sizeof(frames[0]);
    for (size_t i = 0; i < n; i++) {
        if (write(fd, frames[i], strlen(frames[i])) < 0) break;
        usleep(500000);
    }
}

static int starts_with_echo(const char *s) {
    while (isspace((unsigned char)*s)) s++;
    return strncasecmp(s, "echo", 4) == 0;
}

int main(void) {
    int server_fd;
    struct sockaddr_in addr;
    int opt = 1;
    pid_t pid;
    FILE *pf;

    printf("satan: starting on port %d\n", PORT);
    fflush(stdout);

    pf = fopen("/tmp/satan.pid", "w");
    if (pf) {
        fprintf(pf, "%d\n", (int)getpid());
        fclose(pf);
    }

    signal(SIGCHLD, SIG_IGN);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) return 1;

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) return 1;
    if (listen(server_fd, 5) < 0) return 1;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) continue;

        pid = fork();
        if (pid == 0) {
            close(server_fd);
            char buf[BUFFER_SIZE];
            ssize_t nread = read(client_fd, buf, sizeof(buf) - 1);
            if (nread > 0) {
                buf[nread] = '\0';
                if (starts_with_echo(buf)) {
                    play_animation(client_fd);
                } else {
                    const char *msg =
                        "Usage: echo <message>\n"
                        "The daemon will respond with a special animation.\n";
                    write(client_fd, msg, strlen(msg));
                }
            }
            close(client_fd);
            _exit(0);
        } else if (pid > 0) {
            close(client_fd);
        }
    }

    return 0;
}
