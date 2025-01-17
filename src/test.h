/**
 * @file test.h
 * @brief for OrmBuf test and example
 * @version 1.0.1
 *
 * @copyright Copyright (c) 2024, xutopia
 */

#ifndef _TEST_H
#define _TEST_H

#include <cstdint>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include "ormBuf.h"

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

class OrmBufCompany : public nsOrmBuf::OrmBuf<Company> {
private:
    virtual bool init_buf(Company &company) override {
        // register structure elements
        reg_ele(company.name);
        // register structure array elements
        reg_arr(company.departments, [](OrmBuf::ArrReg &arrReg, Department &department) {
            // register structure elements
            arrReg.reg_ele(department.id);
            arrReg.reg_ele(department.name);
            // register structure array elements
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

// test structure data
class DatEleEleEle {
public:
    uint64_t u64 = 0;
    std::string s = "";
    std::string dump() {
        std::stringstream ss;
        ss << "{";
        ss << "u64:" << u64 << ", ";
        ss << "s:" << s << "}, ";
        return ss.str();
    }
};
// test structure data
class DatEleEle {
public:
    uint32_t u32 = 0;
    std::string s = "";
    std::vector<DatEleEleEle> e2_arr;
    std::string dump(std::string _sr_prefix = "") {
        std::stringstream ss;
        ss << _sr_prefix << "{";
        ss << "u32:" << u32 << ", ";
        ss << "s:" << s << ", ";
        ss << "arr:[";
        for (auto &e : e2_arr) {
            ss << e.dump();
        }
        ss << "]";
        ss << "}, \n";
        return ss.str();
    }
};
// test structure data
class DatEle {
public:
    uint16_t u16 = 0;
    std::string s = "";
    std::vector<DatEleEle> arr;
    std::string dump(std::string _strPrefix = "") {
        std::stringstream ss;
        ss << _strPrefix << "{";
        ss << "u16:" << static_cast<uint32_t>(u16) << ", ";
        ss << "s:" << s << ", ";
        ss << "arr:[";
        ss << (arr.size() ? "\n" : "");
        for (auto &e : arr) {
            ss << e.dump(_strPrefix + "  ");
        }
        ss << _strPrefix << "],";
        ss << "\n" << _strPrefix << "}\n";
        return ss.str();
    }
};

/**
 * @brief Class representing a data structure with various data types and nested structures.
 * 
 * The Dat class contains a mix of integer types, strings, and vectors of nested structures.
 * It provides a method to serialize its contents into a human-readable string format.
 */
class Dat {
public:
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint8_t u8 = 0;
    uint64_t u64 = 0;
    float f = 0;
    double d = 0;
    std::string s = "";
    std::vector<DatEle> arr;
    std::list<DatEleEle> arr2;
    std::string dump(std::string _str = "") {
        std::stringstream ss;
        ss << "u16:" << static_cast<uint32_t>(u16) << ", ";
        ss << "u32:" << u32 << ", ";
        ss << "u8:" << static_cast<uint32_t>(u8) << ", ";
        ss << "u64:" << u64 << ", ";
        ss << "s:" << s << ", ";
        ss << "f:" << f << ", ";
        ss << "d:" << d << ", ";
        ss << "arr:[";
        ss << (arr.size() ? "\n" : "");
        for (auto &e : arr) {
            ss << e.dump("  ");
        }
        ss << "]\n";
        ss << "arr:[";
        ss << (arr.size() ? "\n" : "");
        for (auto &e : arr2) {
            ss << e.dump("  ");
        }
        ss << "],\n";
        return ss.str();
    }
};

/**
 * @brief Class representing a specialized buffer for ORM data.
 * 
 * The OrmBufDat class inherits from nsOrmBuf::OrmBuf and overrides the init_buf method
 * to register various data elements and nested arrays for serialization and deserialization.
 */
class OrmBufDat : public nsOrmBuf::OrmBuf<Dat> {
private:
    virtual bool init_buf(Dat &dat) override {
        reg_ele(dat.u16);
        reg_ele(dat.u32);
        reg_ele(dat.u8);
        reg_ele(dat.f);
        reg_ele(dat.d);
        reg_ele(dat.u64);
        reg_ele(dat.s);
        reg_arr(dat.arr, [](OrmBuf::ArrReg &arrReg, DatEle &ele) {
            arrReg.reg_ele(ele.u16);
            arrReg.reg_ele(ele.s);
            arrReg.reg_arr(ele.arr, [](OrmBuf::ArrReg &arrReg2, DatEleEle &ele) {
                arrReg2.reg_ele(ele.u32);
                arrReg2.reg_ele(ele.s);
                arrReg2.reg_arr(ele.e2_arr, [](OrmBuf::ArrReg &arrReg3, DatEleEleEle &ele) {
                    arrReg3.reg_ele(ele.u64);
                    arrReg3.reg_ele(ele.s);
                });
            });
        });
        reg_arr(dat.arr2, [](OrmBuf::ArrReg &arrReg2, DatEleEle &ele) {
            arrReg2.reg_ele(ele.u32);
            arrReg2.reg_ele(ele.s);
            arrReg2.reg_arr(ele.e2_arr, [](OrmBuf::ArrReg &arrReg3, DatEleEleEle &ele) {
                arrReg3.reg_ele(ele.u64);
                arrReg3.reg_ele(ele.s);
            });
        });
        return true;
    }
};

// ormBuf test entry
void main_test_ormBuf();

// ormBuf example entry
void main_ormbuf_example();

#endif
