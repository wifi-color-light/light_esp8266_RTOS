#ifndef __BUTTON_H__
#define __BUTTON_H__
typedef enum
{
    BUTTON_CLEAR = 0,
    BUTTON_CLICK,
    BUTTON_DOUBLE_CLICK,
    BUTTON_PRESS,
}BUTTON_EVENT;
typedef void (*buttonCallback)(BUTTON_EVENT* event);
void buttonInit(buttonCallback btCb);
BUTTON_EVENT checkButtonEvent();
void clearButtonEvent();

#endif