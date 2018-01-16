#ifndef _CLASS_TYPE_H_
#define _CLASS_TYPE_H_

#include <iostream>

#define M_DELARE_RUNTIME(class_name) \
protected: \
    virtual const ClassInfo* GetClassInfo() const {return (&m_classInfo);} \
public: \
    virtual int GetUpdateType() const {return(GetClassInfo()->updateType);} \
    static const ClassInfo m_classInfo;

#define M_IMPLEMENT_RUNTIME(class_name, update_type, super_class_name) \
    const ClassInfo class_name::m_classInfo = { #class_name, update_type, &super_class_name::m_classInfo};

#define M_RUNTIME_TYPE(class_name)      (&class_name::m_classInfo)

/*
 * 类信息构造体
 */
struct ClassInfo
{
    const char*                 pClassName;
    const int                   updateType;
    const struct ClassInfo*     pSuperClassInfo;
};

/*
 * Core类定义
 */
class SCCore
{
M_DELARE_RUNTIME(SCCore)

public:
    SCCore(){};
    virtual ~SCCore() = 0;

    bool                        isClass(const ClassInfo* classInfo) const;
    bool                        isFamily(const ClassInfo* classInfo) const;
    virtual void                printClassInfo(void);
};

// 类信息设定
const ClassInfo SCCore::m_classInfo = {"SCCore", 0x01, 0};

/**
 * @brief   destructor
 *
 * @param[in]  
 */
SCCore::~SCCore(
)
{
}
/****************************************************************************/
/**
 * @brief   IsClass
 *
 * @param[in]   const ClassInfo* classInfo
 *
 * @return  bool
 * @retval  true
 * @retval  false
 */
/****************************************************************************/
bool SCCore::isClass(const ClassInfo* classInfo) const
{
    if(this != 0) {
        if(GetClassInfo() == classInfo) {
            return(true);
        }
    }

    return(false);
}
/****************************************************************************/
/**
 * @brief   isFamily
 *
 * @param[in]   const ClassInfo* classInfo
 *
 * @return  bool
 * @retval  true
 * @retval  false
 */
/****************************************************************************/
bool SCCore::isFamily(const ClassInfo* classInfo) const
{
    if(this != NULL) {
        const ClassInfo* pInfo = GetClassInfo();

        while(pInfo != NULL) {
            if(pInfo == classInfo) {
                return(true);
            }

            pInfo = pInfo->pSuperClassInfo;
        }
    }

    return(false);
}
/****************************************************************************/
/**
 * @brief   printClassName
 *
 * @param[in]   none
 *
 * @return  void
 * @retval  void
 * @retval  void
 */
/****************************************************************************/
void SCCore::printClassName(void)
{
    if(this != NULL) 
    {
        const ClassInfo* pInfo = GetClassInfo();
        std::cout << pInfo->pClassName << std::endl;
    }
}
#endif /* _CLASS_TYPE_H_ */
