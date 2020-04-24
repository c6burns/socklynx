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

#include "socklynx_loadgen/loadgen.h"

#define ASSERT(condition)            \
    if (condition) {                 \
        fprintf(stderr, #condition); \
        exit(1);                     \
    }

struct loadgen_ctx {
    unsigned char* ip;
    int port;
    int size;
    int rate;
};

static void s_usage(int exit_code) {
    fprintf(stderr, "usage: loadgen [options] ip\n");
    fprintf(stderr, " ip: ip to connect to. The default is localhost.\n");
    fprintf(stderr, "\n Options:\n\n");
    fprintf(stderr, "  -p, --port NUMBER: Port to connect to.\n");
    fprintf(stderr, "  -s, --size NUMBER: Packet size to send.\n");
    fprintf(stderr, "  -r, --rate NUMBER: Target packets/second.\n");
    fprintf(stderr, "            Default 0 for at-will sending.\n");
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

/** Takes a nul terminated address string `str` and a char[4] `addr` where the
 * address bytes will be stored.
 * Enforces some address format rules, but some invalid addresses parse
 * successfully like `127...1` for example.
 */
int parse_ipv4(char *str, unsigned char *addr) {
    size_t index = 0;
    // clear first byte in case we're overwriting
    addr[index] = 0;
    while (*str) {
        if ('0' <= *str && '9' >= *str) {
            size_t value = addr[index] * 10 + *str - '0';
            if (255 < value) return 1;
            addr[index] = value;
        } else {
            if ('.' != *str) return 1;
            // ipv4 has only 3 separators
            if (3 == index) return 1;
            index++;
            // clear in case we're overwriting
            addr[index] = 0;
        }
        str++;
    }
    return 0;
}

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
                // getopt_long() returns 0 if an option.flag is non-null
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
                fprintf(stderr, "Unknown option %d\n", c);
                s_usage(1);
        }
    }

    if (ctx->port <= 1024 || ctx->port >= 65536) {
        fprintf(stderr, "Invalid port number\n");
        s_usage(1);
    }

    unsigned char addr[4] = {0};
    if (aws_cli_optind < argc) {
        char* str = argv[aws_cli_optind++];

        if (parse_ipv4(str, addr)) {
            fprintf(stderr, "Invalid IP string\n");
            s_usage(1);
        }
    } else {
        addr[0] = 127;
        addr[3] = 1;
    }
    ctx->ip = addr;
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
    loopback.addr = (app_ctx.ip[0] <<  0)
                  | (app_ctx.ip[1] <<  8)
                  | (app_ctx.ip[2] << 16)
                  | (app_ctx.ip[3] << 24);

    sl_endpoint_t ep_server;
    ep_server.addr4 = loopback;

    sl_endpoint_t ep_client;
    loopback.port += 1;
    ep_client.addr4 = loopback;

    sl_sock_t sock_client = {0};
    sock_client.proto = SL_SOCK_PROTO_UDP;
    sock_client.type = SL_SOCK_TYPE_DGRAM;
    sock_client.endpoint = ep_client;

    char pl_client[app_ctx.size];

    sl_buf_t buf_client_send;
    buf_client_send.base = &pl_client[0];
    buf_client_send.len = app_ctx.size;

    for (int i = 0; i < app_ctx.size; i++) {
        pl_client[i] = i ^ 0xb7 * (i >> 8);
    }

    struct timeval tval_last, tval_now, tval_delta;

    ASSERT(sl_sock_create(&sock_client, SL_SOCK_TYPE_DGRAM, SL_SOCK_PROTO_UDP));
    ASSERT(sl_sock_bind(&sock_client));

    for(;;) {
        gettimeofday(&tval_last, NULL);

        ASSERT(app_ctx.size != sl_sock_send(&sock_client, &buf_client_send, 1, &ep_server));

        gettimeofday(&tval_now, NULL);
        timersub(&tval_now, &tval_last, &tval_delta);

        if (app_ctx.rate) {
            while(tval_delta.tv_usec < (1000000 / app_ctx.rate)) {
                gettimeofday(&tval_now, NULL);
                timersub(&tval_now, &tval_last, &tval_delta);
            }
        }
    }
    ASSERT(sl_sock_close(&sock_client));
    ASSERT(sl_sys_cleanup(&ctx));
}
