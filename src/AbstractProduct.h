#ifndef _ABSTRACT_PRODUCT_H_
#define _ABSTRACT_PRODUCT_H_

#include <stdio.h>

/*
 * 产品抽象类
 */
class AbstractProduct
{
public:
    AbstractProduct();
    virtual ~AbstractProduct();
public:
    virtual void operation() = 0;
};

/*
 * 产品子类A
 */
class ProductA : public AbstractProduct
{
public:
    ProductA();
    virtual ~ProductA();
public:
    void operation();
};

/*
 * 产品子类B
 */
class ProductB : public AbstractProduct
{
public:
    ProductB();
    virtual ~ProductB();
public:
    void operation();
};

#endif