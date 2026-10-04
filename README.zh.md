# OrmBuf

[English](./README.md) | **中文**

## 概述

`OrmBuf` 是一个轻量级、非侵入式的 C++11 序列化与反序列化库，旨在为 C++ 基础数据类型提供高效的自动序列化与反序列化功能。该库以仅头文件（header-only）形式分发，无需编译或安装，只需将其包含到项目中即可开始使用。与 Protobuf 等传统方案相比，OrmBuf 更加简洁高效，适合对性能有较高要求的应用场景。

## 特性

* **支持的数据类型**：
  * C++ 基础数据类型（如整数、浮点数）
  * 字符串类型（`std::string`；不支持 `char*`）
  * 标准容器类型（`std::vector`、`std::list`）
  * 不支持指针类型（通常没有必要）
* **灵活性**：
  * 用户可以选择将哪些数据成员纳入序列化过程、哪些排除，从而增强灵活性。

## 快速开始

### 步骤概览

1. **定义可序列化的数据结构**：创建你希望序列化的结构体（例如 `Dat`）。
2. **定义序列化工具类**：为每个数据结构定义一个对应的序列化工具类（例如 `OrmBufDat`），继承自 `nsOrmBuf::OrmBuf`。
3. **实现初始化函数**：重写纯虚函数 `init_buf`，注册数据结构中的成员变量和嵌套结构。

按照以上步骤操作，你可以轻松实现数据结构的序列化与反序列化，同时保证数据的一致性和灵活性。如果序列化工具中包含了数据结构中不存在的字段，编译器会在编译期报错，从而确保数据一致性。

### 示例代码

待序列化的数据结构可能如下所示，包含基础数值类型、字符串类型、`std::vector` 和 `std::list`。

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
        ss << "age:" << static_cast<int>(age) << ", ";
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
            ss << e.dump(_strPrefix + " ");
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
            ss << d.dump(_strPrefix + " ");
        }
        ss << "],\n";
        return ss.str();
    }
};
```

#### 定义序列化工具

序列化工具需要继承自 `OrmBuf` 模板类，并实现 `init_buf` 函数。

```cpp
class OrmBufCompany : public nsOrmBuf::OrmBuf<Company> {
private:
    virtual bool init_buf(Company &company) override {
        // 注册结构体元素
        reg_ele(company.name);
        // 注册结构体数组元素
        reg_arr(company.departments, [](OrmBuf::ArrReg &arrReg, Department &department) {
            // 注册结构体元素
            arrReg.reg_ele(department.id);
            arrReg.reg_ele(department.name);
            // 注册结构体数组元素
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

#### `init_buf` 函数说明

`init_buf` 函数是 `OrmBuf` 类中的核心函数，用于初始化和注册待序列化/反序列化数据结构的元数据。该函数接收一个引用参数，代表待处理的数据结构实例。

* **基础类型与 `std::string` 类型**：对于 C++ 基础数据类型（如整数、浮点数）和 `std::string` 类型，可以直接调用 `reg_ele` 来注册这些元素的元数据。`reg_ele` 会自动处理这些类型的序列化与反序列化逻辑。
* **容器类型（`std::vector` 和 `std::list`）**：对于 `std::vector` 或 `std::list` 类型的元素，使用 `reg_arr` 进行注册。`reg_arr` 接收两个参数：
  * 第一个参数是结构体中的数组（即 `std::vector` 或 `std::list` 类型的成员变量）。
  * 第二个参数是一个 lambda 函数，用于定义如何注册数组中每个元素的元数据。该 lambda 函数接收两个参数：
    * 一个 `ArrReg` 对象，用于注册数组元素的元数据。
    * 当前正在注册的数组元素的引用。

当数组中存在嵌套数组时，可以在 lambda 函数内部再次调用 `reg_arr`，以递归方式注册嵌套数组中元素的元数据。

通过以上代码示例，你可以看到 `init_buf` 函数如何灵活地注册不同类型的数据成员，确保序列化与反序列化过程中数据结构的完整性和一致性。这种设计不仅提高了代码的可读性和可维护性，还保证了数据的一致性和灵活性。

#### 使用示例

维护好结构体数据的元数据后，即可执行自动序列化与反序列化。

```cpp
Company company;

// 将 Company 对象编码为序列化缓冲区
std::vector<char> seralizeBuf;
{
    OrmBufCompany ormbufCompany;
    ormbufCompany.encode(company, seralizeBuf);
}

// 将序列化缓冲区解码回 Company 对象
Company decCompany;
{
    OrmBufCompany ormbufCompany;
    ormbufCompany.decode(seralizeBuf, decCompany);
}
```

经过以上操作后，`decCompany` 与 `company` 的数据相等。

#### 一致性保证

如果序列化工具中包含了数据结构中不存在的字段，编译器会在编译期报错，从而确保数据一致性。

#### 编译

作为仅头文件库，OrmBuf 无需编译。只需在项目中包含相关头文件即可。

#### 测试

测试代码位于 `test.cpp` 文件中，入口函数名为 `main_test_ormBuf`。建议在开发环境中运行测试用例，以验证库的功能。
