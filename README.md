# OrmBuf

## Overview

`OrmBuf` is a lightweight, non-intrusive C++11 serialization and deserialization library designed to provide efficient automatic serialization and deserialization functionality for C++ primitive data types. The library is distributed as header-only, meaning it does not require compilation or installation; you can simply include it in your project to start using it. Compared to traditional solutions like Protobuf, OrmBuf offers greater simplicity and efficiency, making it suitable for performance-critical applications.

## Features

- **Supported Data Types**:
  - C++ primitive data types (such as integers, floating-point numbers)
  - String types (`std::string`; `char*` is not supported)
  - Standard container types (`std::vector`, `std::list`)
  - Pointer types are not supported (typically unnecessary)

- **Flexibility**:
  - Users can choose which data members to include in the serialization process and which to exclude, enhancing flexibility.

## Quick Start

### Steps Overview

1. **Define Serializable Data Structures**: Create the structures you wish to serialize (e.g., `Dat`).
2. **Define Serialization Tool Classes**: For each data structure, define a corresponding serialization tool class (e.g., `OrmBufDat`) that inherits from `nsOrmBuf::OrmBuf`.
3. **Implement Initialization Functions**: Override the pure virtual function `init_buf` to register member variables and nested structures within the data structure.

By following these steps, you can easily implement serialization and deserialization of data structures while ensuring data consistency and flexibility. If the serialization tool contains fields that do not exist in the data structure, the compiler will report an error at compile time, thus ensuring data consistency.

### Example Code

The data structures to be serialized might look like this, including basic numeric types, string types, `std::vector`, and `std::list`.

```cpp
struct Employee {
    uint32_t id;
    std::string name;
    uint8_t age;
    float salary;
    std::string dump(std::string _strPrefix = "") {
        std::stringstream ss;
        ss << _strPrefix << "{";
        ss << "id:" << id << ", ";
        ss << "name:" << name << ", ";
        ss << "age:" << static_cast<uint32_t>(age) << ", ";
        ss << "salary:" << salary << "}, ";
        return ss.str();
    }
};

struct Department {
    uint32_t id;
    std::string name;
    std::vector<Employee> employees;

    std::string dump(std::string _strPrefix = "") {
        std::stringstream ss;
        ss << _strPrefix << "{";
        ss << "id:" << id << ", ";
        ss << "name:" << name << ", ";
        ss << "employees:[";
        ss << (employees.size() ? "\n" : "");
        for (auto &e : employees) {
            ss << e.dump(_strPrefix + "  ");
        }
        ss << _strPrefix << "],";
        ss << "\n" << _strPrefix << "}\n";
        return ss.str();
    }
};

struct Company {
    std::string name;
    std::list<Department> departments;
    std::string dump(std::string _strPrefix = "") {
        std::stringstream ss;
        ss << "Company Name:" << name << ", ";
        ss << "Departments:[";
        ss << (departments.size() ? "\n" : "");
        for (auto &d : departments) {
            ss << d.dump(_strPrefix + "  ");
        }
        ss << "],\n";
        return ss.str();
    }
};
```

#### Defining Serialization Tools

Serialization tools need to inherit from the `OrmBuf` template class and implement the `init_buf` function.

```cpp
class OrmBufCompany : public nsOrmBuf::OrmBuf<Company> {
private:
    virtual bool init_buf(Company &company) override {
        // Register structure elements
        reg_ele(company.name);
        // Register structure array elements
        reg_arr(company.departments, [](OrmBuf::ArrReg &arrReg, Department &department) {
            // Register structure elements
            arrReg.reg_ele(department.id);
            arrReg.reg_ele(department.name);
            // Register structure array elements
            arrReg.reg_arr(department.employees, [](OrmBuf::ArrReg &arrReg, Employee &employee) {
                arrReg.reg_ele(employee.id);
                arrReg.reg_ele(employee.name);
                arrReg.reg_ele(employee.age);
                arrReg.reg_ele(employee.salary);
            });
        });
        return true;
    }
};
```

#### Explanation of `init_buf` Function

The `init_buf` function is the core function in the `OrmBuf` class used to initialize and register metadata for the data structure to be serialized or deserialized. This function takes a reference parameter representing an instance of the data structure to be processed.

- **Primitive and `std::string` Types**: For C++ primitive data types (like integers, floating-point numbers) and `std::string` types, you can directly call `reg_ele` to register the metadata of these elements. `reg_ele` automatically handles the serialization and deserialization logic for these types.
- **Container Types (`std::vector` and `std::list`)**: For `std::vector` or `std::list` type elements, use `reg_arr` for registration. `reg_arr` takes two parameters:
  - The first parameter is the array in the structure (i.e., a member variable of type `std::vector` or `std::list`).
  - The second parameter is a lambda function that defines how to register the metadata of each element in the array. This lambda function accepts two parameters:
    - An `ArrReg` object for registering the metadata of array elements.
    - A reference to the current array element being registered.

When there are nested arrays within the array, you can call `reg_arr` again inside the lambda function to recursively register the metadata of elements in the nested arrays.

Through the above code examples, you can see how the `init_buf` function flexibly registers different types of data members, ensuring the integrity and consistency of the data structure during the serialization and deserialization processes. This design not only improves the readability and maintainability of the code but also ensures data consistency and flexibility.

#### Usage Examples

After maintaining the metadata of the structure's data, automatic serialization and deserialization can be performed.

```cpp
Company company;

// Encode the Company object into a serialized buffer
std::vector<uint8_t> seralizeBuf;
{
    OrmBufCompany ormbufCompany;
    ormbufCompany.encode(company, seralizeBuf);
}

// Decode the serialized buffer back into a Company object
Company decCompany;
{
    OrmBufCompany ormbufCompany;
    ormbufCompany.decode(seralizeBuf, decCompany);
}
```

After the above operations, `decCompany` and `company` have equal data.

#### Consistency Assurance

If the serialization tool contains fields that do not exist in the data structure, the compiler will report an error at compile time, thus ensuring data consistency.

#### Compilation

As a header-only library, OrmBuf does not require compilation. You only need to include the relevant header files in your project.

#### Testing

Test codes are located in the `test.cpp` file, with the entry function named `main_test_ormBuf`. It is recommended to run test cases in the development environment to verify the functionality of the library.
