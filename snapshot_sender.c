#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void saveScreenshot(XImage *image, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        printf("Failed to open file for writing: %s\n", filename);
        return;
    }

    // Write PNG header
    unsigned char png_header[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    fwrite(png_header, 1, 8, file);

    // Write image data row by row
    unsigned char *row_data = malloc(image->bytes_per_line);
    int x, y;
    for (y = 0; y < image->height; y++)
    {
        for (x = 0; x < image->bytes_per_line; x++)
        {
            row_data[x] = (unsigned char)(image->data[y * image->bytes_per_line + x]);
        }
        fwrite(row_data, 1, image->bytes_per_line, file);
    }

    fclose(file);
    free(row_data);

    printf("Screenshot captured and saved as %s\n", filename);
}

int main()
{
    Display *display = XOpenDisplay(NULL);
    if (!display)
    {
        printf("Failed to open display\n");
        return 1;
    }

    Window root = DefaultRootWindow(display);

    XWindowAttributes attributes;
    if (!XGetWindowAttributes(display, root, &attributes))
    {
        printf("Failed to get window attributes\n");
        XCloseDisplay(display);
        return 1;
    }

    XImage *image = XShmCreateImage(display, DefaultVisual(display, DefaultScreen(display)), attributes.depth,
                                    ZPixmap, NULL, &(XShmSegmentInfo){0}, attributes.width, attributes.height);
    if (!image)
    {
        printf("Failed to create XImage\n");
        XCloseDisplay(display);
        return 1;
    }

    XShmSegmentInfo shminfo;
    shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT | 0666);
    if (shminfo.shmid == -1)
    {
        printf("Failed to allocate shared memory\n");
        XDestroyImage(image);
        XCloseDisplay(display);
        return 1;
    }
    shminfo.shmaddr = image->data = shmat(shminfo.shmid, 0, 0);
    if (shminfo.shmaddr == (void *)-1)
    {
        printf("Failed to attach shared memory\n");
        shmctl(shminfo.shmid, IPC_RMID, 0);
        XDestroyImage(image);
        XCloseDisplay(display);
        return 1;
    }
    shminfo.readOnly = False;
    if (!XShmAttach(display, &shminfo))
    {
        printf("Failed to attach shared memory\n");
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);
        XDestroyImage(image);
        XCloseDisplay(display);
        return 1;
    }

    XShmGetImage(display, root, image, 0, 0, AllPlanes);

    saveScreenshot(image, "/mnt/01D8B284E8986BC0/Programming/VMs/Shared/images/sh1.png");

    XShmDetach(display, &shminfo);
    shmdt(shminfo.shmaddr);
    shmctl(shminfo.shmid, IPC_RMID, 0);
    XDestroyImage(image);
    XCloseDisplay(display);

    return 0;

}
