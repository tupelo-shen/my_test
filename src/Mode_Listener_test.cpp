#include "Mode_Listener_test.h"

void StateListener::actionPerformed(int state)
{
    // 这儿就是对自己感兴趣的事件的相应处理
    switch (state)
    {
        case 1:
            cout << "Event 1 is triggered!" << endl;
        break;
        case 2:
            cout << "Event 2 is triggered!" << endl;
        break;
        default:
            cout << "Other Event is triggered!" << endl;
        break;
    }
}