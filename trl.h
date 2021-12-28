/*  tiny reflection library - written by Kevin Chin (https://kevch.in/)

    Do this:
        #define TINY_REFLECTION_IMPLEMENTATION
    before you include this file in *one* C++ file to create the implementation.
        // i.e. it should look like this:
        #include ...
        #define TINY_REFLECTION_IMPLEMENTATION
        #include "trl.h"

Features:
        - Construction of class from name as a string
        - Member field/variable information e.g. list all fields of a class and their types
        - Super/base class information from derived class
        - Actual class information from a pointer to a base class
        - Modify private field/variable externally

*/
#ifndef _INCLUDE_TINY_REFLECTION_LIBRARY_H_
#define _INCLUDE_TINY_REFLECTION_LIBRARY_H_

#if (defined _MSC_VER)
    #define _TrlPragma __pragma
#elif (defined __GNUC__)
    #define _TrlPragma _Pragma
#else
    #define _TrlPragma(ignore)
#endif

#include <vector>
#include <functional>

// INTERFACE STARTS HERE

struct Type
{
    const char* name;
    int size;
};

struct FieldDescriptor
{
    Type type;
    const char* name;
    int offset;
};

struct StructDescriptor
{
    Type type;
    const char* name;
    std::vector<FieldDescriptor> fields;
    std::function<void*()> defaultConstructor;
    StructDescriptor* superDescriptor;

    StructDescriptor(void(*init)(StructDescriptor*))
        : superDescriptor(NULL)
    {
        init(this);
    }
}; typedef StructDescriptor ClassDescriptor;


// PUT IN CONSTRUCTORS OF EVERY REFLECTED CLASS
#define SET_DESCRIPTOR_PTR()\
do{\
    if(ReflectionPtr) {\
        Reflection.fields.insert(Reflection.fields.begin(), ReflectionPtr->fields.begin(), ReflectionPtr->fields.end());\
        Reflection.superDescriptor = ReflectionPtr;\
    }\
    ReflectionPtr = &Reflection;\
} while(0)

// IN CLASS BODY (MUST BE PUBLIC)
#define REFLECT_BASE_CLASS()\
    static StructDescriptor Reflection;\
    static void __initReflection(StructDescriptor* desc);\
    StructDescriptor* ReflectionPtr = NULL;

#define REFLECT_DERIVED_CLASS()\
    static StructDescriptor Reflection;\
    static void __initReflection(StructDescriptor* desc);

// IN ONE CPP FILE (TRANSLATION UNIT) WITH ACCESS TO CLASS DECLARATION
#define REFLECT_CLASS_BEGIN(classType)\
    StructDescriptor classType::Reflection{classType::__initReflection}\
    _TrlPragma("clang diagnostic push");\
    _TrlPragma("clang diagnostic ignored \"-Winvalid-offsetof\"");\
    void classType::__initReflection(StructDescriptor* desc)\
    {\
        using T = classType;\
        TRLDatabase[#classType] = desc;\
        desc->type = { typeid(classType).name(), sizeof(classType) };\
        desc->name = #classType;\
        desc->defaultConstructor = []()->void*{return new classType;};\
        desc->fields = {

#define REFLECT_CLASS_FIELD(fieldName)\
            { { typeid(fieldName).name(), sizeof(fieldName) }, #fieldName, offsetof(T, fieldName) },

#define REFLECT_CLASS_END()\
        };\
    }\
    _TrlPragma("clang diagnostic pop");

// use instead of REFLECT_CLASS_BEGIN if abstract class
#define REFLECT_ABSTRACT_CLASS_BEGIN(classType)\
    StructDescriptor classType::Reflection{classType::__initReflection}\
    _TrlPragma("clang diagnostic push");\
    _TrlPragma("clang diagnostic ignored \"-Winvalid-offsetof\"");\
    void classType::__initReflection(StructDescriptor* desc)\
    {\
        using T = classType;\
        TRLDatabase[#classType] = desc;\
        desc->type = { typeid(classType).name(), sizeof(classType) };\
        desc->name = #classType;\
        desc->fields = {

// INTERFACE ENDS HERE

#endif //_INCLUDE_TINY_REFLECTION_LIBRARY_H_


#ifdef TINY_REFLECTION_IMPLEMENTATION

#include <unordered_map>
static std::unordered_map<const char*, StructDescriptor*> TRLDatabase;

#endif





