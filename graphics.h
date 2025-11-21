#ifndef GRAPHICS_H
#define GRAPHICS_H

const int size_x = 738;
const int size_y = 738;

const int desk_tl_x = 50;
const int desk_tl_y = 130;

const int desk_dr_x = 290;
const int desk_dr_y = 610;

struct programm_images
{
    HDC welcome_image;
    HDC default_image;
    HDC write_image;
    HDC question_image;
    HDC fail_image;
    HDC jessi1;
    HDC jessi2;
};

void window_init (void);
void make_menu   (const char* menu_text);
void make_output (const char* output_text);
void make_fail (void);
void make_success (void);

const char* myInputBox (bool need_sound = true);



#endif
