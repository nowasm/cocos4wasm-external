/*
 * Minimal libuv stub for Emscripten / WASM builds.
 * Only the types and functions referenced by jsb_global_init.cpp are declared;
 * the debugger code path that actually calls them is never reached on WASM.
 */
#ifndef UV_H_STUB
#define UV_H_STUB

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uv_loop_s    uv_loop_t;
typedef struct uv_tcp_s     uv_tcp_t;
typedef struct uv_stream_s  uv_stream_t;
typedef struct uv_handle_s  uv_handle_t;

typedef void (*uv_connection_cb)(uv_stream_t *server, int status);
typedef void (*uv_close_cb)(uv_handle_t *handle);

struct sockaddr;
struct sockaddr_in;

typedef struct {
    char name[256];
    struct { char ip[46]; } address;
    int is_internal;
} uv_interface_address_t;

struct uv_loop_s  { char _pad[1]; };
struct uv_tcp_s   { char _pad[1]; };
struct uv_stream_s { char _pad[1]; };
struct uv_handle_s { char _pad[1]; };

static inline int  uv_loop_init(uv_loop_t *loop)    { (void)loop; return 0; }
static inline int  uv_loop_close(uv_loop_t *loop)   { (void)loop; return 0; }
static inline int  uv_tcp_init(uv_loop_t *loop, uv_tcp_t *handle) { (void)loop; (void)handle; return 0; }
static inline int  uv_ip4_addr(const char *ip, int port, struct sockaddr_in *addr) { (void)ip; (void)port; (void)addr; return 0; }
static inline int  uv_tcp_bind(uv_tcp_t *handle, const struct sockaddr *addr, unsigned int flags) { (void)handle; (void)addr; (void)flags; return 0; }
static inline int  uv_listen(uv_stream_t *stream, int backlog, uv_connection_cb cb) { (void)stream; (void)backlog; (void)cb; return -1; }
static inline void uv_close(uv_handle_t *handle, uv_close_cb close_cb) { (void)handle; (void)close_cb; }
static inline int  uv_interface_addresses(uv_interface_address_t **addresses, int *count) { *addresses = NULL; *count = 0; return 0; }
static inline void uv_free_interface_addresses(uv_interface_address_t *addresses, int count) { (void)addresses; (void)count; }
static inline const char *uv_strerror(int err) { (void)err; return "stub"; }

#ifdef __cplusplus
}
#endif

#endif /* UV_H_STUB */
