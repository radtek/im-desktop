#pragma once

#include <rapidjson/document.h>
#include <rapidjson/schema.h>

#include <string>
#include <string_view>

namespace common::json
{
    template <typename T>
    inline std::string rapidjson_get_string(const T& value) { return std::string(value.GetString(), value.GetStringLength()); }

    template <>
    inline std::string rapidjson_get_string<rapidjson::StringBuffer>(const rapidjson::StringBuffer& value) { return std::string(value.GetString(), value.GetSize()); }

    template <typename T>
    inline std::string_view rapidjson_get_string_view(const T& value) { return std::string_view(value.GetString(), value.GetStringLength()); }

    template <>
    inline std::string_view rapidjson_get_string_view<rapidjson::StringBuffer>(const rapidjson::StringBuffer& value) { return std::string_view(value.GetString(), value.GetSize()); }
}

namespace common::json
{
    inline auto make_string_ref(std::string_view str)
    {
        return rapidjson::StringRef(str.data(), str.size());
    };

    template <class T, class U>
    bool unserialize_value(const rapidjson::Value& _node, const U& _name, T& _out_param) = delete;

    template <class U>
    inline bool unserialize_value(const rapidjson::Value& _node, const U& _name, std::string& _out_param)
    {
        if (_node.IsObject())
        {
            if (const auto iter = _node.FindMember(make_string_ref(_name)); iter != _node.MemberEnd() && iter->value.IsString())
            {
                _out_param = rapidjson_get_string(iter->value);
                return true;
            }
        }

        return false;
    }

    template <class U>
    inline bool unserialize_value(const rapidjson::Value& _node, const U& _name, std::string_view& _out_param)
    {
        if (_node.IsObject())
        {
            if (const auto iter = _node.FindMember(make_string_ref(_name)); iter != _node.MemberEnd() && iter->value.IsString())
            {
                _out_param = rapidjson_get_string_view(iter->value);
                return true;
            }
        }

        return false;
    }

    template <class U>
    inline bool unserialize_value(const rapidjson::Value& _node, const U& _name, int& _out_param)
    {
        if (_node.IsObject())
        {
            if (const auto iter = _node.FindMember(make_string_ref(_name)); iter != _node.MemberEnd() && iter->value.IsInt())
            {
                _out_param = iter->value.GetInt();
                return true;
            }
        }

        return false;
    }

    template <class U>
    inline bool unserialize_value(const rapidjson::Value& _node, const U& _name, int64_t& _out_param)
    {
        if (_node.IsObject())
        {
            if (const auto iter = _node.FindMember(make_string_ref(_name)); iter != _node.MemberEnd() && iter->value.IsInt64())
            {
                _out_param = iter->value.GetInt64();
                return true;
            }
        }

        return false;
    }

    template <class U>
    inline bool unserialize_value(const rapidjson::Value& _node, const U& _name, uint64_t& _out_param)
    {
        if (_node.IsObject())
        {
            if (const auto iter = _node.FindMember(make_string_ref(_name)); iter != _node.MemberEnd() && iter->value.IsUint64())
            {
                _out_param = iter->value.GetUint64();
                return true;
            }
        }

        return false;
    }

    template <class U>
    inline bool unserialize_value(const rapidjson::Value& _node, const U& _name, unsigned int& _out_param)
    {
        if (_node.IsObject())
        {
            if (const auto iter = _node.FindMember(make_string_ref(_name)); iter != _node.MemberEnd() && iter->value.IsUint())
            {
                _out_param = iter->value.GetUint();
                return true;
            }
        }

        return false;
    }

    template <class U>
    inline bool unserialize_value(const rapidjson::Value& _node, const U& _name, double& _out_param)
    {
        if (_node.IsObject())
        {
            if (const auto iter = _node.FindMember(make_string_ref(_name)); iter != _node.MemberEnd() && iter->value.IsDouble())
            {
                _out_param = iter->value.GetDouble();
                return true;
            }
        }

        return false;
    }

    template <class U>
    inline bool unserialize_value(const rapidjson::Value& _node, const U& _name, bool& _out_param)
    {
        if (_node.IsObject())
        {
            if (const auto iter = _node.FindMember(make_string_ref(_name)); iter != _node.MemberEnd())
            {
                if (iter->value.IsBool())
                {
                    _out_param = iter->value.GetBool();
                    return true;
                }
                else if (iter->value.IsInt() || iter->value.IsUint())
                {
                    _out_param = iter->value != 0;
                    return true;
                }
            }
        }

        return false;
    }
}
