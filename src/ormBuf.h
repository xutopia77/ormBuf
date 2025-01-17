/**
 * @file orm_buf.h
 * @brief OrmBuf, for c++ data structure serialization and deserialization
 * @version 1.0.1
 *
 * @copyright Copyright (c) 2024, xutopia
 */

#ifndef _ORM_BUF_H_
#define _ORM_BUF_H_

#include <cstdint>
#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

namespace nsOrmBuf {

/**
 * @brief Base class implementation for OrmBuf.
 *
 * This class provides support for serializing and deserializing C++ fundamental data types and container types.
 * Users need to implement the init_buf function to record metadata of structured data (such as field types, lengths, etc.).
 * For specific implementation details, please refer to the test cases in test.h.
 *
 * - Serialization: Call the encode function to convert data objects into byte streams.
 * - Deserialization: Call the decode function to restore data objects from byte streams.
 *
 * @note 
 * - The init_buf function is used to initialize the buffer and define the layout and format of the data structure.
 * - Data serialization and deserialization can be easily performed using the encode and decode methods.
 */
template <typename T>
class OrmBuf {
public:
    /**
     * @brief encode data
     * @param _t data
     * @param _distBuf dist buffer
     * @return true/false
     */
    bool encode(T &_t, std::vector<uint8_t> &_distBuf) {
        m_bEnc = true;
        auto ret = init_buf(_t);
        _distBuf = m_distVec;
        return ret;
    }
    /**
     * @brief decode data
     * @param _srcBuf source buffer
     * @param _t dist data
     * @return true/false
     */
    bool decode(std::vector<uint8_t> &_srcBuf, T &_t) {
        m_bEnc = false;
        m_inBuf = _srcBuf.data();
        return init_buf(_t);
    }

    /**
     * @brief dump buffer to hex string
     * 
     * @param _vecIn source buffer
     * @param _lineNum line number, 0 means no line break
     * @return std::string 
     */
    static std::string dump_hex(std::vector<uint8_t> &_vecIn, uint8_t _lineNum = 0) {
        char tmpBuf[4] = {0};
        std::stringstream ss;
        for (auto it = _vecIn.begin(); it != _vecIn.end(); it++) {
            sprintf(tmpBuf, "%02x ", *it);
            ss << tmpBuf;
            if (_lineNum > 0 && ((it - _vecIn.begin()) + 1) % _lineNum == 0) {
                ss << std::endl;
            }
        }
        return ss.str();
    }

protected:
    /**
    * @brief Virtual function that users must implement to record metadata of structured data.
    *
    * This function initializes the buffer and defines the basic information of structured data (such as field types, lengths, etc.).
    * Users should implement this function according to specific requirements to ensure that structured data can be correctly serialized and deserialized.
    *
    * @param _struDat The structured data object.
    * @return Returns true on success, false on failure.
    */
    virtual bool init_buf(T &_struDat) = 0;
    /**
     * @brief array register context
     */
    class ArrReg {
    public:
        ArrReg(OrmBuf *_orm) { m_orm = _orm; }

        /**
         * @brief register element
         * @tparam T base c++ type
                    number type, std::string type
         * @param _value 
         */
        template <typename ET>
        void reg_ele(ET &_value) {
            return m_orm->reg_ele(_value);
        };
        /**
         * @brief register array
         * @tparam T array type
         *          std::vector<T>
         * @tparam F register array element function type
         *          void(OrmBuf::ArrReg &eleReg, T* ele);
         * @param _value  array
         * @param regFunc array element register function
         */
        template <typename ET, typename F>
        void reg_arr(ET &_value, F regFunc) {
            return m_orm->reg_arr(_value, regFunc);
        }

    private:
        OrmBuf *m_orm;
    };

    /**
     * @brief register element
     * @tparam T base c++ type
                number type, std::string type
     * @param _value 
     */
    template <typename ET>
    void reg_ele(ET &_value) {
        if (m_bEnc) {
            do_encode_num(_value);
        }
        else {
            do_decode_num(_value);
        }
    }

    /**
     * @brief register array
     * @tparam T array type
                std::vector, std::list
     * @tparam F register array element function type
                void(OrmBuf::ArrReg &eleReg, ET& ele);
     * @param _value  array
     * @param regFunc array element register function
     */
    template <typename ET, typename F>
    void reg_arr(ET &_value, F regFunc) {
        auto sizeArr = _value.size();
        reg_ele(sizeArr);
        using ElementType = typename ET::value_type;
        if (!m_bEnc) {
            for (decltype(sizeArr) i = 0; i < sizeArr; i++) {
                ElementType ele_tmp;
                _value.push_back(ele_tmp);
            }
        }
        ArrReg arrRegCtx(this);
        for (auto &_ele : _value) {
            regFunc(arrRegCtx, _ele);
        }
    }

private:
    template <typename ET>
    void do_encode_num(const ET &_value) {
        auto &outvec = m_distVec;
        EleInfo einfo;
        einfo.l = sizeof(_value);
        outvec.insert(outvec.end(), (uint8_t *)&einfo, (uint8_t *)&einfo + sizeof(einfo));
        outvec.insert(outvec.end(), (uint8_t *)&_value, (uint8_t *)&_value + einfo.l);
    }
    template <typename ET>
    void do_decode_num(ET &_value) {
        auto &inptr = m_inBuf;
        OrmBuf::EleInfo *einfo = (OrmBuf::EleInfo *)inptr;
        inptr += sizeof(OrmBuf::EleInfo);
        _value = *(ET *)inptr;
        inptr += einfo->l;
    }

    void do_encode_num(const std::string &_value) {
        auto &outvec = m_distVec;
        OrmBuf::EleInfo einfo;
        einfo.l = _value.size();
        outvec.insert(outvec.end(), (uint8_t *)&einfo, (uint8_t *)&einfo + sizeof(einfo));
        outvec.insert(outvec.end(), (uint8_t *)_value.data(), (uint8_t *)_value.data() + einfo.l);
    }

    void do_decode_num(std::string &_value) {
        auto &inptr = m_inBuf;
        OrmBuf::EleInfo *einfo = (OrmBuf::EleInfo *)inptr;
        inptr += sizeof(OrmBuf::EleInfo);
        _value.assign((char *)inptr, einfo->l);
        inptr += einfo->l;
    }
    bool m_bEnc = false;
    uint8_t *m_inBuf = nullptr;
    std::vector<uint8_t> m_distVec;
    struct EleInfo {
        uint32_t l;
    };
};
} // namespace nsOrmBuf
#endif