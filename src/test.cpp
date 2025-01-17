
#include "test.h"
#include <cstdint>
#include <cstdio>
#include <list>
#include <vector>

/**
 * @brief Compares two Company objects for equality.
 * 
 * @param c1 First Company object.
 * @param c2 Second Company object.
 * @return true if the Company objects are equal, false otherwise.
 */
bool are_companies_equal(const Company &company1, const Company &company2) {
    if (company1.name != company2.name) {
        return false;
    }
    if (company1.departments.size() != company2.departments.size()) {
        return false;
    }
    auto it1 = company1.departments.begin();
    auto it2 = company2.departments.begin();
    for (; it1 != company1.departments.end(); ++it1, ++it2) {
        if (it1->id != it2->id) {
            return false;
        }
        if (it1->name != it2->name) {
            return false;
        }
        if (it1->employees.size() != it2->employees.size()) {
            return false;
        }
        auto empIt1 = it1->employees.begin();
        auto empIt2 = it2->employees.begin();
        for (; empIt1 != it1->employees.end(); ++empIt1, ++empIt2) {
            if (empIt1->id != empIt2->id) {
                return false;
            }
            if (empIt1->name != empIt2->name) {
                return false;
            }
            if (empIt1->age != empIt2->age) {
                return false;
            }
            if (empIt1->salary != empIt2->salary) {
                return false;
            }
        }
    }
    return true;
}

static void make_test_data_company(Company &company) {
    company.name = "nb_company";
    company.departments.push_back(Department());
    {
        auto &dep = company.departments.back();
        dep.name = "nb_department";
        dep.id = 1;
        dep.employees.push_back(Employee());
        {
            auto &emp = dep.employees.back();
            emp.name = "nb_employee";
            emp.id = 1007;
            emp.age = 35;
            emp.salary = 99999.1;
        }
        dep.employees.push_back(Employee());
        {
            auto &emp = dep.employees.back();
            emp.name = "nb_employee2";
            emp.id = 1008;
            emp.age = 36;
            emp.salary = 99999.2;
        }
    }
}

/**
 * @brief ormBuf example
 */
void main_ormbuf_example() {
    // Create and initialize a Company object with sample data
    Company company;
    make_test_data_company(company);

    printf("------------------------------------\n");
    printf("Original data :\n%s\n", company.dump().c_str());

    // Encode the Company object into a serialized buffer
    std::vector<uint8_t> seralizeBuf;
    {
        OrmBufCompany ormbufCompany;
        ormbufCompany.encode(company, seralizeBuf);
    }

    printf("------------------------------------\n");
    printf("serialize data :\n%s\n", OrmBufCompany::dump_hex(seralizeBuf, 16).c_str());

    // Decode the serialized buffer back into a Company object
    Company decCompany;
    {
        OrmBufCompany ormbufCompany;
        ormbufCompany.decode(seralizeBuf, decCompany);
    }

    printf("------------------------------------\n");
    printf("decode data :\n%s\n", decCompany.dump().c_str());

    printf("------------------------------------\n");
    printf("encode and decode : %s\n", are_companies_equal(company, decCompany) ? "equal" : "not equal");
}

static void make_test_data(Dat &_datOut) {
    auto &dat = _datOut;
    {
        dat.u16 = 101;
        dat.u32 = 102;
        dat.u8 = 3;
        dat.u64 = 104;
        dat.f = 16.16;
        dat.d = 16.18;
        dat.s = "str in Dat";
        dat.arr.push_back(DatEle());
        {
            auto &ele = dat.arr.back();
            ele.u16 = 201;
            ele.s = "DatEle.s in Dat.arr[1]";
            ele.arr.push_back(DatEleEle());
            {
                auto &ele2 = ele.arr.back();
                ele2.u32 = 3;
                ele2.s = "DatEleEle.s in DatEle.arr[0]";
                ele2.e2_arr.push_back(DatEleEleEle());
                {
                    auto &ele3 = ele2.e2_arr.back();
                    ele3.u64 = 301;
                    ele3.s = "DatEleEleEle.s in DatEleEle.arr[0]";
                }
            }
        }
        dat.arr.push_back(DatEle());
        {
            auto &ele = dat.arr.back();
            ele.u16 = 201;
            ele.s = "DatEle.s in Dat.arr[1]";
            ele.arr.push_back(DatEleEle());
            {
                auto &ele2 = ele.arr.back();
                ele2.u32 = 401;
                ele2.s = "DatEleEle.s in DatEle.arr[0]";
                ele2.e2_arr.push_back(DatEleEleEle());
                {
                    auto &ele3 = ele2.e2_arr.back();
                    ele3.u64 = 301;
                    ele3.s = "DatEleEleEle.s in DatEleEle.arr[0]";
                }
            }
        }
        dat.u16 = 101;
        dat.u32 = 102;
        dat.u8 = 3;
        dat.u64 = 104;
        dat.s = "str2_in_Dat";
        dat.arr2.push_back(DatEleEle());
        {
            auto &ele2 = dat.arr2.back();
            ele2.u32 = 401;
            ele2.s = "DatEleEle.s in Dat.arr2[0]";
            ele2.e2_arr.push_back(DatEleEleEle());
            {
                auto &ele3 = ele2.e2_arr.back();
                ele3.u64 = 301;
                ele3.s = "DatEleEleEle.s in DatEleEle.arr[0]";
            }
        }
    }
}

static bool are_dat_equal(const Dat &dat1, const Dat &dat2) {
    if (dat1.u16 != dat2.u16) return false;
    if (dat1.u32 != dat2.u32) return false;
    if (dat1.u8 != dat2.u8) return false;
    if (dat1.u64 != dat2.u64) return false;
    if (dat1.f != dat2.f) return false;
    if (dat1.d != dat2.d) return false;
    if (dat1.s != dat2.s) return false;

    if (dat1.arr.size() != dat2.arr.size()) return false;
    auto it1 = dat1.arr.begin();
    auto it2 = dat2.arr.begin();
    for (; it1 != dat1.arr.end() && it2 != dat2.arr.end(); ++it1, ++it2) {
        const auto &ele1 = *it1;
        const auto &ele2 = *it2;
        if (ele1.u16 != ele2.u16) return false;
        if (ele1.s != ele2.s) return false;

        if (ele1.arr.size() != ele2.arr.size()) return false;
        auto it3 = ele1.arr.begin();
        auto it4 = ele2.arr.begin();
        for (; it3 != ele1.arr.end() && it4 != ele2.arr.end(); ++it3, ++it4) {
            const auto &ele2_1 = *it3;
            const auto &ele2_2 = *it4;
            if (ele2_1.u32 != ele2_2.u32) return false;
            if (ele2_1.s != ele2_2.s) return false;

            if (ele2_1.e2_arr.size() != ele2_2.e2_arr.size()) return false;
            auto it5 = ele2_1.e2_arr.begin();
            auto it6 = ele2_2.e2_arr.begin();
            for (; it5 != ele2_1.e2_arr.end() && it6 != ele2_2.e2_arr.end(); ++it5, ++it6) {
                const auto &ele3_1 = *it5;
                const auto &ele3_2 = *it6;
                if (ele3_1.u64 != ele3_2.u64) return false;
                if (ele3_1.s != ele3_2.s) return false;
            }
        }
    }

    if (dat1.arr2.size() != dat2.arr2.size()) return false;
    auto it7 = dat1.arr2.begin();
    auto it8 = dat2.arr2.begin();
    for (; it7 != dat1.arr2.end() && it8 != dat2.arr2.end(); ++it7, ++it8) {
        const auto &ele1 = *it7;
        const auto &ele2 = *it8;
        if (ele1.u32 != ele2.u32) return false;
        if (ele1.s != ele2.s) return false;

        if (ele1.e2_arr.size() != ele2.e2_arr.size()) return false;
        auto it9 = ele1.e2_arr.begin();
        auto it10 = ele2.e2_arr.begin();
        for (; it9 != ele1.e2_arr.end() && it10 != ele2.e2_arr.end(); ++it9, ++it10) {
            const auto &ele2_1 = *it9;
            const auto &ele2_2 = *it10;
            if (ele2_1.u64 != ele2_2.u64) return false;
            if (ele2_1.s != ele2_2.s) return false;
        }
    }

    return true;
}

/**
 * Main test function to verify the encode and decode functionality of OrmBuf.
 * 
 * This function tests the OrmBuf encoding and decoding process by encoding a data object,
 * then decoding it, and finally comparing the original and decoded data for equality.
 */
void main_test_ormBuf() {
    // Create and initialize test data
    Dat dat;
    make_test_data(dat);

    std::vector<uint8_t> outvec;
    {
        // Instantiate OrmBufDat object for encoding operations
        OrmBufDat ormbuf_dat;
        // Encode the test data into the output buffer
        ormbuf_dat.encode(dat, outvec);
    }

    printf("------------------------------------\n");
    printf("Original data:\n%s\n", dat.dump().c_str());

    printf("------------------------------------\n");
    printf("encode buf:\n%s\n", OrmBufDat::dump_hex(outvec, 16).c_str());

    Dat decDat;
    {
        // Instantiate OrmBufDat object for decoding operations
        OrmBufDat ormbuf_dat;
        // Decode data from the output buffer into a new data object
        ormbuf_dat.decode(outvec, decDat);
    }

    printf("------------------------------------\n");
    printf("decode data:\n%s\n", decDat.dump().c_str());

    printf("------------------------------------\n");
    printf("encode and decode : %s\n", are_dat_equal(dat, decDat) ? "equal" : "not equal");
}



int main() {
    main_ormbuf_example();
    return 0;
}

