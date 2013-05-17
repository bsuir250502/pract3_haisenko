#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "nonogram.h"

int cellSize = 20;

GtkWidget* createInterfaceBox(nonogram_t nonogram);
void openButtonClick(GtkWidget *openButton, nonogram_t *nonogram);
void solveButtonClick(GtkWidget *solveButton, GtkWidget *frame);
static gboolean on_draw_event(GtkWidget *darea, cairo_t *cairoDrawPlace, nonogram_t *nonogram);
void do_drawing(cairo_t *cairoDrawPlace, GtkWidget *darea, nonogram_t *nonogram);
void closeButtonClick(GtkWidget *window, nonogram_t *nonogram);
static void *solvingProcess(void *vptr_args);

int main(int argc, char *argv[])
{
    nonogram_t nonogram;
    nonogram = fNonogramInit();

    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *solveButton;
    GtkWidget *openButton;
    GtkWidget *darea;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    //gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_resize(GTK_WINDOW(window), 600, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Nonogram solver");
    gtk_container_add(GTK_CONTAINER(window), createInterfaceBox(nonogram));
    g_signal_connect(window, "destroy", G_CALLBACK(closeButtonClick), &nonogram);
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

GtkWidget* createInterfaceBox(nonogram_t nonogram)
{
    GtkWidget* scrolledwindow;
    GtkWidget* table;
    GtkWidget *solveButton;
    GtkWidget *openButton;
    GtkWidget *darea;

    scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    table = gtk_table_new(2, 2, FALSE);

    darea = gtk_drawing_area_new();
    gtk_widget_set_size_request(darea, nonogram.sizeX * cellSize + 10, nonogram.sizeY * cellSize + 10);

    openButton = gtk_button_new_with_label("Open");
    gtk_widget_set_size_request(openButton, 100, 30);
    gtk_container_set_border_width(GTK_CONTAINER(openButton), 10);
    //gtk_fixed_put(GTK_FIXED(frame), openButton, nonogram.sizeX * cellSize + 10, nonogram.sizeY * cellSize / 7);

    solveButton = gtk_button_new_with_label("Solve");
    gtk_widget_set_size_request(solveButton, 100, 30);
    gtk_container_set_border_width(GTK_CONTAINER(solveButton), 10);
    //gtk_fixed_put(GTK_FIXED(frame), solveButton, nonogram.sizeX * cellSize + 10, nonogram.sizeY * cellSize / 7 + 50);

    gtk_scrolled_window_add_with_viewport(GTK_CONTAINER(scrolledwindow), darea);
    gtk_table_attach_defaults(GTK_TABLE(table), scrolledwindow, 0, 2, 0, 1);
    gtk_table_attach(GTK_TABLE(table), openButton, 0, 1, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_table_attach(GTK_TABLE(table), solveButton, 1, 2, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);

    g_signal_connect(openButton, "clicked", G_CALLBACK(openButtonClick), &nonogram);
    g_signal_connect(solveButton, "clicked", G_CALLBACK(solveButtonClick), scrolledwindow);
    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), &nonogram);
    return table;
}


void openButtonClick(GtkWidget *openButton, nonogram_t *nonogram)
{
    g_print("%d", cellSize);
    gtk_widget_set_sensitive(openButton, FALSE);
    pthread_t outputThread;
    pthread_create(&outputThread, NULL, solvingProcess, (void *) nonogram);
    gtk_widget_set_sensitive(openButton, TRUE);
}

void solveButtonClick(GtkWidget *solveButton, GtkWidget *frame)
{
    gtk_widget_set_sensitive(solveButton, FALSE);
    gtk_widget_queue_draw(frame);
    gtk_widget_set_sensitive(solveButton, TRUE);
}

static gboolean on_draw_event(GtkWidget *darea, cairo_t *cairoDrawPlace, nonogram_t *nonogram)
{
    cairoDrawPlace = gdk_cairo_create(gtk_widget_get_window(darea));
    do_drawing(cairoDrawPlace, darea, nonogram);
    cairo_destroy(cairoDrawPlace);
    return FALSE;
}

void do_drawing(cairo_t *cairoDrawPlace, GtkWidget *darea, nonogram_t *nonogram)
{
    int i, j;
    cairo_set_source_rgb(cairoDrawPlace, 0.761, 0.796, 0.251);
    cairo_paint(cairoDrawPlace);
    cairo_set_source_rgba(cairoDrawPlace, 0.102, 0.094, 0.09, 1);
    cairo_set_line_width(cairoDrawPlace, 2);
    cairo_set_line_cap(cairoDrawPlace, CAIRO_LINE_CAP_ROUND);
    cairo_translate(cairoDrawPlace, 5, 5);

    for (i = 0; i <= nonogram->sizeX; i++) {
        cairo_move_to(cairoDrawPlace, i * cellSize, 0);
        cairo_line_to(cairoDrawPlace, i * cellSize, nonogram->sizeY * cellSize);
    }
    for (i = 0; i <= nonogram->sizeY; i++) {
        cairo_move_to(cairoDrawPlace, 0, i * cellSize);
        cairo_line_to(cairoDrawPlace, nonogram->sizeX * cellSize, i * cellSize);
    }
    cairo_stroke(cairoDrawPlace);

    for (i = 0; i < nonogram->sizeY; i++) {
        for (j = 0; j < nonogram->sizeX; j++) {
            if (nonogram->field[nonogram->sizeY - i - 1][j].cell) {
                cairo_move_to(cairoDrawPlace, j * cellSize, (i + 1) * cellSize);
                cairo_line_to(cairoDrawPlace, (j + 1) * cellSize, i * cellSize);
                cairo_move_to(cairoDrawPlace, j * cellSize, i * cellSize + (cellSize / 2));
                cairo_line_to(cairoDrawPlace, j * cellSize + (cellSize / 2), i * cellSize);
                cairo_move_to(cairoDrawPlace, j * cellSize + (cellSize / 2), i * cellSize + cellSize);
                cairo_line_to(cairoDrawPlace, j * cellSize + cellSize, i * cellSize + (cellSize / 2));
                if (cellSize > 15) {
                    cairo_move_to(cairoDrawPlace, j * cellSize, i * cellSize + (cellSize / 4 * 3));
                    cairo_line_to(cairoDrawPlace, j * cellSize + (cellSize / 4 * 3), i * cellSize);
                    cairo_move_to(cairoDrawPlace, j * cellSize + (cellSize / 4), i * cellSize + cellSize);
                    cairo_line_to(cairoDrawPlace, j * cellSize + cellSize, i * cellSize + (cellSize / 4));
                }
            }
        }
    }
    cairo_stroke(cairoDrawPlace);
}

void closeButtonClick(GtkWidget *window, nonogram_t *nonogram)
{
    freeNonogram(*nonogram);
    gtk_main_quit();
}

static void *solvingProcess(void *vptr_args)
{
    nonogram_t *nonogram;
    nonogram = vptr_args;
    solvingNonogram(*nonogram);
    return NULL;
}
