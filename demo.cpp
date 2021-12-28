#define TINY_REFLECTION_IMPLEMENTATION
#include "trl.h"

#include <stdio.h>
#include <string>


struct A
{
    int x;
    int y;

    A()
    {
        SET_DESCRIPTOR_PTR();
    }

    /* 8 byte virtual table pointer here */ virtual void SomePureVirtualFunc() = 0;

    REFLECT_BASE_CLASS();
};
REFLECT_ABSTRACT_CLASS_BEGIN(A)
REFLECT_CLASS_FIELD(x)
REFLECT_CLASS_FIELD(y)
REFLECT_CLASS_END()

struct B : A
{
    float z;

    B()
    {
        SET_DESCRIPTOR_PTR();
    }

    void SomePureVirtualFunc() override {}

    std::string str;

    REFLECT_DERIVED_CLASS();
};
REFLECT_CLASS_BEGIN(B)
REFLECT_CLASS_FIELD(z)
REFLECT_CLASS_FIELD(str)
REFLECT_CLASS_END()

int main()
{
    A* APtrToB = new B();
    
    // Gets StructDescriptor of B even though the pointer was an A*
    StructDescriptor BReflection = *(APtrToB->ReflectionPtr);

    printf("%s\n", A::Reflection.name);
    printf("%s\n", A::Reflection.type.name);
    printf("%d\n", A::Reflection.type.size);
    printf("%s\n", A::Reflection.fields[0].name);
    printf("%d\n", A::Reflection.fields[0].offset);
    printf("%s\n", A::Reflection.fields[1].name);
    printf("%d\n", A::Reflection.fields[1].offset);

    printf("%s\n", BReflection.name);
    printf("%s\n", BReflection.type.name);
    printf("%d\n", BReflection.type.size);
    printf("%s\n", BReflection.fields[0].name); // base class fields are included in derived class field list
    printf("%d\n", BReflection.fields[0].offset);
    printf("%s\n", BReflection.fields[1].name);
    printf("%d\n", BReflection.fields[1].offset);
    printf("%s\n", BReflection.fields[2].name);
    printf("%d\n", BReflection.fields[2].offset);
    printf("%s\n", BReflection.fields[3].name);
    printf("%d\n", BReflection.fields[3].offset);

    // Iterate all registered struct/class types
    for (auto& nameAndDescPair : TRLDatabase) {
        printf("Name: %s\n", nameAndDescPair.first);
        printf("Desc type name: %s\n", nameAndDescPair.second->type.name);
    }

    // Construction of class from name as a string
    ClassDescriptor* someStructDescriptor = TRLDatabase["B"];
    if(someStructDescriptor->type.name == typeid(B).name())
    {
        // then we know the actual type is B so we can safely cast to B*
        B* BPtr = (B*) someStructDescriptor->defaultConstructor();
        printf("Constructed a B at: %p\n", BPtr);
    }
}





