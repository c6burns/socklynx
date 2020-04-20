/*
 * Copyright (c) 2019 Chris Burns <chris@kitty.city>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "socklynx_server/server.h"

#define ASSERT(condition)            \
    if (condition) {                 \
        fprintf(stderr, #condition); \
        exit(1);                     \
    }

struct loadgen_ctx {
    int port;
    int size;
    int rate;
};

static void s_usage(int exit_code) {
    fprintf(stderr, "usage: server [options]\n");
    fprintf(stderr, "\n Options:\n\n");
    fprintf(stderr, "  -p, --port NUMBER: Port to connect to.\n");
    fprintf(stderr, "  -s, --size NUMBER: Packet size to receive.\n");
    fprintf(stderr, "  -r, --rate NUMBER: How often to report.\n");
    fprintf(stderr, "  -h, --help\n");
    fprintf(stderr, "            Display this message and quit.\n");
    exit(exit_code);
}

static struct aws_cli_option s_long_options[] = {
    {"port", AWS_CLI_OPTIONS_REQUIRED_ARGUMENT, NULL, 'p'},
    {"size", AWS_CLI_OPTIONS_REQUIRED_ARGUMENT, NULL, 's'},
    {"rate", AWS_CLI_OPTIONS_REQUIRED_ARGUMENT, NULL, 'r'},
    {"help", AWS_CLI_OPTIONS_NO_ARGUMENT, NULL, 'h'},
    // getopt(3): last element has to be filled with zeros
    {NULL, AWS_CLI_OPTIONS_NO_ARGUMENT, NULL, 0},
};

static void s_parse_options(int argc, char **argv, struct loadgen_ctx *ctx) {
    while (true) {
        int option_index = 0;
        int c =
            aws_cli_getopt_long(argc, argv, "p:s:r:h", s_long_options, &option_index);
        if (c == -1 || c == '?') {
            break;
        }

        switch (c) {
            case 0:
                /* getopt_long() returns 0 if an option.flag is non-null */
                break;
            case 'p':
                ctx->port = atoi(aws_cli_optarg);
                break;
            case 's':
                ctx->size = atoi(aws_cli_optarg);
                break;
            case 'r':
                ctx->rate = atoi(aws_cli_optarg);
                break;
            case 'h':
                s_usage(0);
                break;
            default:
                fprintf(stderr, "Unknown option\n");
                s_usage(1);
        }
    }

    if (ctx->port <= 1024 || ctx->port >= 65536) {
        fprintf(stderr, "Invalid port number\n");
        s_usage(1);
    }
}

int main(int argc, char **argv) {
    struct loadgen_ctx app_ctx;
    AWS_ZERO_STRUCT(app_ctx);
    app_ctx.port = 51234;

    s_parse_options(argc, argv, &app_ctx);

    sl_sys_t ctx;

    ASSERT(sl_sys_setup(&ctx));

    sl_sockaddr4_t loopback = {0};
    loopback.af = ctx.af_inet;
    loopback.port = app_ctx.port;
    loopback.addr = (127 <<  0)
                  | (  0 <<  8)
                  | (  0 << 16)
                  | (  1 << 24);

    sl_endpoint_t ep_server_recv;
    ep_server_recv.addr4 = loopback;

    sl_endpoint_t ep_server;
    ep_server.addr4 = loopback;

    sl_sock_t sock_server = {0};
    sock_server.proto = SL_SOCK_PROTO_UDP;
    sock_server.type = SL_SOCK_TYPE_DGRAM;
    sock_server.endpoint = ep_server;

    char pl_client[app_ctx.size];
    char mem_server[app_ctx.size];

    sl_buf_t buf_server_recv;
    buf_server_recv.base = &mem_server[0];
    buf_server_recv.len = app_ctx.size;

    for (int i = 0; i < app_ctx.size; i++) {
        pl_client[i] = i ^ 0xb7 * (i >> 8);
    }

    struct timeval tval_last, tval_now, tval_delta;
    int received = 0;
    gettimeofday(&tval_last, NULL);

    ASSERT(sl_sock_create(&sock_server, SL_SOCK_TYPE_DGRAM, SL_SOCK_PROTO_UDP));
    ASSERT(sl_sock_bind(&sock_server));
    for(;;) {
        int size = sl_sock_recv(&sock_server, &buf_server_recv, 1, &ep_server_recv);
        ASSERT(app_ctx.size < size);
        ASSERT(memcmp(pl_client, buf_server_recv.base, size));
        received++;

        gettimeofday(&tval_now, NULL);

        timersub(&tval_now, &tval_last, &tval_delta);

        if (1 < tval_delta.tv_sec) {
            gettimeofday(&tval_last, NULL);
            fprintf(stderr, "Received %d packets.\n", received);
            received = 0;
        }
    }
    ASSERT(sl_sock_close(&sock_server));
    ASSERT(sl_sys_cleanup(&ctx));
}
