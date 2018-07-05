#include "AbstractProduct.h"

/*
 * 产品抽象类-构造函数
 */
AbstractProduct::AbstractProduct()
{

}
/*
 * 产品抽象类-析构函数
 */
AbstractProduct::~AbstractProduct()
{

}

/*
 * 产品子类A-构造函数
 */
ProductA::ProductA()
{

}
/*
 * 产品子类A-析构函数
 */
ProductA::~ProductA()
{

}

/*
 * 产品子类A-方法1
 */
void ProductA::operation()
{
    fprintf(stderr,"ProductA operation!\n");
}

/*
 * 产品子类B-构造函数
 */
ProductB::ProductB()
{

}
/*
 * 产品子类B-析构函数
 */
ProductB::~ProductB()
{

}

/*
 * 产品子类B-方法1
 */
void ProductB::operation()
{
    fprintf(stderr,"ProductB operation!\n");
}

