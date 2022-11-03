/*
 * Project: Template
 * File: template.c
 * Create by: Rom1 <rom1@canel.ch> - Flipper French Community - https://github.com/FlipperFrenchCommunity
 * Date: 26 octobre 2022
 * License: GNU GENERAL PUBLIC LICENSE v3
 * Description: Template pour créer une application FZ.
 */
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>


typedef struct {
    FuriMessageQueue* event_queue;

    FuriMutex* model_mutex;

    ViewPort* view_port;
    Gui* gui;
} TemplateState;

void template_draw_callback(Canvas* canvas, void* ctx){
    UNUSED(canvas);
    TemplateState* state = ctx;
    furi_check(furi_mutex_acquire(state->model_mutex, 25) == FuriStatusOk);

    furi_mutex_release(state->model_mutex);
}

void template_input_callback(InputEvent* input, void* ctx){
    TemplateState* state = ctx;
    furi_check(furi_mutex_acquire(state->model_mutex, 25) == FuriStatusOk);
    furi_message_queue_put(state->event_queue, input, FuriWaitForever);
    furi_mutex_release(state->model_mutex);
}

TemplateState* template_alloc(){
    TemplateState* state = malloc(sizeof(TemplateState));

    state->view_port = view_port_alloc();
    state->model_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    state->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);

    state->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    
    view_port_draw_callback_set(state->view_port, template_draw_callback, state);
    view_port_input_callback_set(state->view_port, template_input_callback, state);
    
    return state;
}

void template_free(TemplateState* state){
    view_port_enabled_set(state->view_port, false);
    gui_remove_view_port(state->gui, state->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(state->view_port);

    furi_message_queue_free(state->event_queue);
    furi_mutex_free(state->model_mutex);

    free(state);
}

int32_t template_app(void* p){
    UNUSED(p);

    TemplateState* state = template_alloc();

    InputEvent event;
    for(bool processing=true ; processing ; ){
        FuriStatus event_status = furi_message_queue_get(state->event_queue,
        &event, 100);

        furi_check(furi_mutex_acquire(state->model_mutex, 25) == FuriStatusOk);

        if(event_status == FuriStatusOk) {
            if(event.type == InputTypePress) {
                switch(event.key) {
                    case InputKeyUp:
                    case InputKeyDown:
                    case InputKeyLeft:
                    case InputKeyRight:
                    case InputKeyOk:
                    case InputKeyBack:
                        processing = false;
                        break;
                }
            }
        }
        furi_mutex_release(state->model_mutex);
        view_port_update(state->view_port);
    }

    template_free(state);
    return 0;
}
