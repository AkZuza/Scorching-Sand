#pragma once

#include <window/window.h>
#include <renderer/renderer_core.h>

typedef struct Renderer {
    RendererCore core;
    Window window;
} Renderer;

void r_initialize(Renderer *renderer);
void r_destroy(const Renderer *renderer);

void r_begin_frame();
void r_end_frame();
void r_render_frame();

const Renderer *r_get_renderer();
