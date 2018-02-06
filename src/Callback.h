#ifndef __CALLBACK_H__
#define __CALLBACK_H__

/*
 * enumerate definition
 */
enum SCCallbackType {
    SCCallbackTypeTAP = 0,
    SCCallbackTypeFLICK,
    SCCallbackTypeLONGTAP,
    SCCallbackTypeCHANGEVALUE,
    SCCallbackTypeSETFOCUS,
    SCCallbackTypeKILLFOCUS
};

/*
 * Class definition
 */
class Callback
{
public:
    Callback(int type, void (*func)(int), int client_data);
    virtual ~Callback();

    void            DoCallback();
    Callback*       Next() const {return(m_next);}
    void            Next(SCCallback* next) {m_next = next;}
    int             GetType() const {return m_type;}

private:
    void            (*m_func)(int client_data);
    int             m_type;
    int             m_client_data;
    Callback*       m_next;    
}
#endif /* __CALLBACK_H__ */