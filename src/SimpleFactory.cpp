#include "SimpleFactory.h"

SimpleFactory::SimpleFactory()
{ }

SimpleFactory::~SimpleFactory()
{ }

AbstractProduct* SimpleFactory::createProduct(int type)
{
    AbstractProduct* temp = NULL;
    switch(type)
    {
    case 1:
        temp = new ProductA();
        break;
    case 2:
        temp = new ProductB();
        break;
    default:
        break;
    }
    return temp;
}