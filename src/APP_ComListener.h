#ifndef __APP_COMLISTENER_H__
#define __APP_COMLISTENER_H__

class ComListener
{
public:
    virtual ~ComListener() {};

    virtual void onRecv(char* adu, int len, unsigned short err) = 0;
    virtual void onSend() = 0;
};
#endif /* __APP_COMLISTENER_H__ */