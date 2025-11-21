#include <TXLib.h>
#include <stdlib.h>

#include "graphics.h"

const int STR_MAX_LEN = 100;

programm_images images = {};

void window_init (void)
{
    txTextCursor(false);
    txPlaySound ("sounds\\welcome.wav");
    txSleep(1000);

    images.welcome_image  = txLoadImage ("images\\zastavka.bmp");
    txCreateWindow(size_x, size_y);
    txDisableAutoPause();

    txBitBlt  (txDC(), 0, 0, txGetExtentX() , txGetExtentY(), images.welcome_image);

    txSelectFont ("Arial", 40, 10);
    txSetColor (TX_BLACK);

    images.default_image  = txLoadImage ("images\\default.bmp");
    images.write_image    = txLoadImage ("images\\write.bmp");
    images.question_image = txLoadImage ("images\\question.bmp");
    images.fail_image     = txLoadImage ("images\\fail.bmp");
    images.jessi1         = txLoadImage ("images\\jessi1.bmp");
    images.jessi2         = txLoadImage ("images\\jessi2.bmp");
    txSleep(11000);
    txPlaySound ("sounds\\say_my_name.wav");
    txSleep (1500);
    txPlaySound ("sounds\\background.wav", SND_LOOP);
}

void make_menu (const char* menu_text)
{
    txBitBlt (txDC(), 0, 0, txGetExtentX() , txGetExtentY(), images.default_image);
    txDrawText(desk_tl_x, desk_tl_y, desk_dr_x, desk_dr_y, menu_text);
}

void make_fail (void)
{
    txPlaySound ("sounds\\fail.wav");
    txBitBlt (txDC(), 0, 0, txGetExtentX() , txGetExtentY(), images.fail_image);
    txSleep(6000);
    txPlaySound ("sounds\\background.wav", SND_LOOP);
}

void make_success (void)
{
    txPlaySound ("sounds\\right.wav");
    make_output("я знал!\n");

    txPlaySound ("sounds\\success.wav");

    for (int i = 0; i < 10; i++)
    {
        txBitBlt (txDC(), 0, 0, txGetExtentX() , txGetExtentY(), images.jessi1);
        txSleep(300);
        txBitBlt (txDC(), 0, 0, txGetExtentX() , txGetExtentY(), images.jessi2);
        txSleep(300);
    }
    txPlaySound ("sounds\\background.wav", SND_LOOP);
}

void make_output (const char* output_text)
{
    txBitBlt (txDC(), 0, 0, txGetExtentX() , txGetExtentY(), images.write_image);
    txSleep(1000);
    txBitBlt (txDC(), 0, 0, txGetExtentX() , txGetExtentY(), images.question_image);
    txDrawText(desk_tl_x, desk_tl_y, desk_dr_x, desk_dr_y, output_text);
    txSleep(1000);
}

const char* myInputBox(bool need_sound)
{
    const char text    [STR_MAX_LEN] = "¬ведите строку:";
    const char caption [STR_MAX_LEN] = "akinator by Heizenberg";
    const char input   [STR_MAX_LEN] = "";

    if(need_sound)
        txPlaySound ("sounds\\say_my_name.wav", SND_NOSTOP);

    #define ID_TEXT_  101
    #define ID_INPUT_ 102

    short int dialogX = 610;
    short int dialogY = 690;
    short int dialogWidth = 200;
    short int dialogHeight = 50;

    txDialog::Layout layout[] =
        {{ txDialog::DIALOG,  caption,   0,           dialogX, dialogY, dialogWidth, dialogHeight },
         { txDialog::STATIC,  text,      ID_TEXT_,   10, 10, 150,  40, SS_LEFT },
         { txDialog::EDIT,    input,     ID_INPUT_,  10, 25, 180,  15, ES_LEFT | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP },
         { txDialog::END }};

    struct inputDlg : txDialog
    {
        char str[1024];
        int targetX, targetY;

        inputDlg(int x = 0, int y = 0) : str(), targetX(x), targetY(y)
        {}

        TX_BEGIN_MESSAGE_MAP()
            TX_HANDLE(WM_INITDIALOG)
            {
                if (targetX != 0 || targetY != 0)
                {
                    RECT rect;
                    GetWindowRect(_wnd, &rect);
                    MoveWindow(_wnd, targetX, targetY,
                              rect.right - rect.left,
                              rect.bottom - rect.top, TRUE);
                }
                return TRUE;
            }

            TX_COMMAND_MAP
            TX_HANDLE(IDOK) GetDlgItemText(_wnd, ID_INPUT_, str, sizeof(str) - 1);
        TX_END_MESSAGE_MAP
    };

    #undef ID_TEXT_
    #undef ID_INPUT_

    static inputDlg dlg(dialogX, dialogY);
    dlg.dialogBox(layout);

    return dlg.str;
}
