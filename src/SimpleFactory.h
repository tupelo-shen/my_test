#ifndef _SIMPLE_FACTORY_H_
#define _SIMPLE_FACTROY_H_

#include <stdio.h>
#include "AbstractProduct.h"

class SimpleFactory
{

public:
    SimpleFactory();
    ~SimpleFactory();
    
public:
    AbstractProduct* createProduct(int type);
};

#endif
