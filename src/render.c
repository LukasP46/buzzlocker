/*
 * render.c
 *
 * Created by buzzert <buzzert@buzzert.net> 2019-01-18
 */

#include "render.h"

static const double kLogoBackgroundWidth = 500.0;

void draw_logo(saver_state_t *state)
{
    if (state->logo_svg_handle == NULL) {
        GError *error = NULL;
        state->logo_svg_handle = rsvg_handle_new_from_file("logo.svg", &error);
        if (error != NULL) {
            fprintf(stderr, "Error loading logo SVG\n");
            return;
        }
    }

    cairo_t *cr = state->ctx;

    cairo_save(cr);
    cairo_set_source_rgb(cr, (208.0 / 255.0), (69.0 / 255.0), (255.0 / 255.0));
    cairo_rectangle(cr, 0, 0, kLogoBackgroundWidth, state->canvas_height);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    
    // Scale and draw logo
    RsvgDimensionData dimensions;
    rsvg_handle_get_dimensions(state->logo_svg_handle, &dimensions);

    const double padding = 100.0;
    double scale_factor = ((kLogoBackgroundWidth - (padding * 2.0)) / dimensions.width);
    double scaled_height = (dimensions.height * scale_factor);
    double y_position = (state->canvas_height - scaled_height) / 2.0;
    cairo_translate(cr, padding, y_position);
    cairo_scale(cr, scale_factor, scale_factor);
    rsvg_handle_render_cairo(state->logo_svg_handle, cr);

    cairo_restore(cr);
}

void draw_password_field(saver_state_t *state)
{
    const double cursor_height = 40.0;
    const double cursor_width  = 30.0;
    const double field_x = kLogoBackgroundWidth + 50.0;
    const double field_y = (state->canvas_height - cursor_height) / 2.0;
    const double field_padding = 10.0;
    
    cairo_t *cr = state->ctx;

    // Draw status text
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    pango_layout_set_font_description(state->pango_layout, state->status_font);
    pango_layout_set_text(state->pango_layout, "Password: ", -1);

    int t_width, t_height;
    pango_layout_get_size(state->pango_layout, &t_width, &t_height);
    double line_height = t_height / PANGO_SCALE;

    cairo_move_to(cr, field_x, field_y - line_height - field_padding);
    pango_cairo_show_layout(cr, state->pango_layout);

    // Draw password asterisks
    if (state->asterisk_svg_handle == NULL) {
        GError *error = NULL;
        state->asterisk_svg_handle = rsvg_handle_new_from_file("asterisk.svg", &error);
        if (error != NULL) {
            fprintf(stderr, "Error loading asterisk SVG\n");
            return;
        }
    }

    const double cursor_padding_x = 10.0;
    double cursor_offset_x = 0.0;

    RsvgDimensionData dimensions;
    rsvg_handle_get_dimensions(state->asterisk_svg_handle, &dimensions);
    
    double asterisk_height = cursor_height - 20.0;
    double scale_factor = (asterisk_height / dimensions.height);
    double scaled_width = (dimensions.width * scale_factor);

    for (unsigned i = 0; i < strlen(state->password_buffer); i++) {
        cairo_save(cr);
        cairo_translate(cr, field_x + cursor_offset_x, field_y + ((cursor_height - asterisk_height) / 2.0));
        cairo_scale(cr, scale_factor, scale_factor);
        rsvg_handle_render_cairo(state->asterisk_svg_handle, cr);
        cairo_restore(cr);

        cursor_offset_x += scaled_width + cursor_padding_x;
    }

    
    // Draw cursor
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, state->cursor_opacity);
    cairo_rectangle(cr, field_x + cursor_offset_x, field_y, cursor_width, cursor_height);
    cairo_fill(cr);
}

