
#if 1
#include "SimpleFactory.h"
int main(){
    SimpleFactory* factory = new SimpleFactory();
    AbstractProduct* product = factory->createProduct(1);
    product->operation();
    delete product;
    product = NULL;
    
    product = factory->createProduct(2);
    product->operation();
    delete product;
    product = NULL;
    return 0;
}

#endif

#if 0
#include "AbstractFactory.h"

int main(){
    AbstractFactory* factory = new FactoryA();
    AbstractProduct* product = factory->createProduct();
    product->operation();
    delete product;
    product = NULL;
    delete factory;
    factory = NULL;
    
    factory = new FactoryB();
    product = factory->createProduct();
    product->operation();
    delete product;
    product = NULL;
    delete factory;
    factory = NULL;
    return 0;
}
#endif