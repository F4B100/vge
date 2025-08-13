//
// Created by fabio on 10-08-2025.
//
#ifdef VGE_PLATFORM_WAYLAND
#include "waylandCallbacks.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <bits/fcntl-linux.h>
#include <sys/mman.h>

#include "../vgeWindowDefs.h"

static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial) {
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
        .ping = xdg_wm_base_ping,
};

void registry_handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    printf("Wayland global: %s (version %u)\n", interface, version);
    pVgeWindow window = data;
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        vgeGlobalContext->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, version);
    }
    else if (strcmp(interface, wl_shm_interface.name) == 0) {
        window->shm = wl_registry_bind(registry, name, &wl_shm_interface, version);
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        window->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
        xdg_wm_base_add_listener(window->xdg_wm_base, &xdg_wm_base_listener, window);
    }
}

void registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {

}

static void
wl_buffer_release(void *data, struct wl_buffer *wl_buffer)
{
    /* Sent by the compositor when it's no longer using this buffer */
    wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
        .release = wl_buffer_release,
};

static void randname(char *buf) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A'+(r&15)+(r&16)*2;
        r >>= 5;
    }
}

static int32_t create_shm_file(void) {
    int32_t retries = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        randname(name + sizeof(name) - 7);
        --retries;
        int32_t fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);
    return -1;
}

int32_t allocate_shm_file(size_t size) {
    int32_t fd = create_shm_file();
    if (fd < 0)
        return -1;
    int32_t ret;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(fd);
        return -1;
    }
    return fd;
}


struct wl_buffer *draw_frame(pVgeWindow window) {
    int32_t stride = window * 4;
    int32_t size = stride * window->height;

    int32_t fd = allocate_shm_file(size);
    if (fd == -1) {
        return NULL;
    }

    uint32_t *data = mmap(NULL, size,
                          PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return NULL;
    }

    struct wl_shm_pool *pool = wl_shm_create_pool(window->shm, fd, size);
    struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0,
                                                         window->width, window->height, stride, WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    /* Draw checkerboxed background */
    for (int y = 0; y < window->height; ++y) {
        for (int x = 0; x < window->width; ++x) {
            if ((x + y / 8 * 8) % 16 < 8)
                data[y * window->width + x] = 0xFF666666;
            else
                data[y * window->width + x] = 0xFFEEEEEE;
        }
    }

    munmap(data, size);
    wl_buffer_add_listener(buffer, &wl_buffer_listener, NULL);
    return buffer;
}

void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) {
    caalfagaWindow *window = data;
    xdg_surface_ack_configure(xdg_surface, serial);
    wl_surface_commit(window->surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
        .configure = xdg_surface_configure,
};

#endif
