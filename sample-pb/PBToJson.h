/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <nlohmann/json.hpp>
#include <boost/algorithm/string.hpp>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

class PBToJson
{
public:
    static bool Json2Message(const nlohmann::json &json, ::google::protobuf::Message &message, bool str2enum = false)
    {
        auto descriptor = message.GetDescriptor();
        auto reflection = message.GetReflection();
        if (nullptr == descriptor || nullptr == reflection)
            return false;

        auto count = descriptor->field_count();
        for (auto i = 0; i < count; ++i)
        {
            const auto field = descriptor->field(i);
            if (nullptr == field)
                continue;

            const std::string &field_name = field->name();
            if (json.end() == json.find(field_name))
                continue;

            auto &value = json[field_name];
            if (value.is_null())
                continue;

            if (field->is_repeated())
            {
                if (!value.is_array())
                    return false;

                Json2RepeatedMessage(value, message, field, reflection, str2enum);
                continue;
            }

            switch (field->type())
            {
            case ::google::protobuf::FieldDescriptor::TYPE_BOOL:
            {
                if (value.is_boolean())
                    reflection->SetBool(&message, field, value.get<bool>());
                else if (value.is_number_integer())
                    reflection->SetBool(&message, field, value.get<uint32_t>() != 0);
                else if (value.is_string())
                {
                    std::string str = value.get<std::string>();
                    boost::algorithm::to_lower(str);

                    if (str == "true" || str == "1")
                        reflection->SetBool(&message, field, true);
                    else if (str == "false" || str == "0")
                        reflection->SetBool(&message, field, false);
                }
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_ENUM:
            {
                auto const *pedesc = field->enum_type();
                const ::google::protobuf::EnumValueDescriptor *pevdesc = nullptr;

                if (str2enum)
                    pevdesc = pedesc->FindValueByName(value.get<std::string>());
                else
                    pevdesc = pedesc->FindValueByNumber(value.get<int>());

                if (nullptr != pevdesc)
                    reflection->SetEnum(&message, field, pevdesc);
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_INT32:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT32:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            {
                if (value.is_number())
                    reflection->SetInt32(&message, field, value.get<int32_t>());
                else if (value.is_string())
                {
                    std::string str = value.get<std::string>();
                    try
                    {
                        int32_t num = std::stoi(str);
                        reflection->SetInt32(&message, field, num);
                    }
                    catch (...)
                    {
                        reflection->SetInt32(&message, field, 0);
                    }
                }
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_UINT32:
            case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
            {
                if (value.is_number())
                    reflection->SetUInt32(&message, field, value.get<uint32_t>());
                else if (value.is_string())
                {
                    std::string str = value.get<std::string>();
                    try
                    {
                        uint32_t num = std::stoul(str);
                        reflection->SetUInt32(&message, field, num);
                    }
                    catch (...)
                    {
                        reflection->SetUInt32(&message, field, 0);
                    }
                }
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_INT64:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT64:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            {
                if (value.is_number())
                    reflection->SetInt64(&message, field, value.get<int64_t>());
                else if (value.is_string())
                {
                    std::string str = value.get<std::string>();
                    try
                    {
                        int64_t num = std::stoll(str);
                        reflection->SetInt64(&message, field, num);
                    }
                    catch (...)
                    {
                        reflection->SetInt64(&message, field, 0);
                    }
                }
            }
            break;
            case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
            case ::google::protobuf::FieldDescriptor::TYPE_FIXED64:
            {
                if (value.is_number())
                    reflection->SetUInt64(&message, field, value.get<uint64_t>());
                else if (value.is_string())
                {
                    std::string str = value.get<std::string>();
                    try
                    {
                        uint64_t num = std::stoull(str);
                        reflection->SetUInt64(&message, field, num);
                    }
                    catch (...)
                    {
                        reflection->SetUInt64(&message, field, 0);
                    }
                }
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
            {
                if (value.is_number())
                    reflection->SetFloat(&message, field, value.get<float>());
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            {
                if (value.is_number())
                    reflection->SetDouble(&message, field, value.get<double>());
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_STRING:
            case ::google::protobuf::FieldDescriptor::TYPE_BYTES:
            {
                if (value.is_string())
                    reflection->SetString(&message, field, value.get<std::string>());
                else if (value.is_number())
                    reflection->SetString(&message, field, value.dump());
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            {
                if (value.is_object())
                    Json2Message(value, *reflection->MutableMessage(&message, field));
            }
            break;

            default:
                assert(false);
                break;
            }
        }
        return true;
    }

    static bool Json2RepeatedMessage(const nlohmann::json &json, ::google::protobuf::Message &message, const ::google::protobuf::FieldDescriptor *field, const ::google::protobuf::Reflection *reflection, bool str2enum = false)
    {
        int count = json.size();
        for (auto j = 0; j < count; ++j)
        {
            switch (field->type())
            {
            case ::google::protobuf::FieldDescriptor::TYPE_BOOL:
            {
                if (json[j].is_boolean())
                    reflection->AddBool(&message, field, json[j].get<bool>());
                else if (json[j].is_number_integer())
                    reflection->AddBool(&message, field, json[j].get<uint32_t>() != 0);
                else if (json[j].is_string())
                {
                    if (json[j].get<std::string>() == "true")
                        reflection->AddBool(&message, field, true);
                    else if (json[j].get<std::string>() == "false")
                        reflection->AddBool(&message, field, false);
                }
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_ENUM:
            {
                auto const *pedesc = field->enum_type();
                const ::google::protobuf::EnumValueDescriptor *pevdesc = nullptr;
                if (str2enum)
                    pevdesc = pedesc->FindValueByName(json[j].get<std::string>());
                else
                    pevdesc = pedesc->FindValueByNumber(json[j].get<int32_t>());

                if (nullptr != pevdesc)
                    reflection->AddEnum(&message, field, pevdesc);
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_INT32:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT32:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            {
                if (json[j].is_number())
                    reflection->AddInt32(&message, field, json[j].get<int32_t>());
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_UINT32:
            case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
            {
                if (json[j].is_number())
                    reflection->AddUInt32(&message, field, json[j].get<uint32_t>());
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_INT64:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT64:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            {
                if (json[j].is_number())
                    reflection->AddInt64(&message, field, json[j].get<int64_t>());
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
            case ::google::protobuf::FieldDescriptor::TYPE_FIXED64:
            {
                if (json[j].is_number())
                    reflection->AddUInt64(&message, field, json[j].get<uint64_t>());
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
            {
                if (json[j].is_number())
                    reflection->AddFloat(&message, field, json[j].get<float>());
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            {
                if (json[j].is_number())
                    reflection->AddDouble(&message, field, json[j].get<double>());
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            {
                if (json[j].is_object())
                    Json2Message(json[j], *reflection->AddMessage(&message, field));
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_STRING:
            case ::google::protobuf::FieldDescriptor::TYPE_BYTES:
            {
                if (json[j].is_string())
                    reflection->AddString(&message, field, json[j].get<std::string>());
            }
            break;

            default:
                assert(false);
                break;
            }
        }
        return true;
    }

    static void Message2Json(const ::google::protobuf::Message &message, nlohmann::json &json, bool enum2str = false)
    {
        auto descriptor = message.GetDescriptor();
        auto reflection = message.GetReflection();
        assert(descriptor);
        assert(reflection);

        const int count = descriptor->field_count();
        for (auto i = 0; i < count; ++i)
        {
            const auto field = descriptor->field(i);
            const std::string &field_name = field->name();

            if (field->is_repeated())
            {
                if (reflection->FieldSize(message, field) > 0)
                    RepeatedMessage2Json(message, field, reflection, json[field_name], enum2str);
                continue;
            }

            switch (field->type())
            {
            case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            {
                const ::google::protobuf::Message &tmp_message = reflection->GetMessage(message, field);
                if (0 != tmp_message.ByteSize())
                    Message2Json(tmp_message, json[field_name]);
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_BOOL:
                json[field_name] = reflection->GetBool(message, field) ? true : false;
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_ENUM:
            {
                auto *enum_value_desc = reflection->GetEnum(message, field);
                if (enum2str)
                    json[field_name] = enum_value_desc->name();
                else
                    json[field_name] = enum_value_desc->number();
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_INT32:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT32:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED32:
                json[field_name] = reflection->GetInt32(message, field);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_UINT32:
            case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
                json[field_name] = reflection->GetUInt32(message, field);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_INT64:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT64:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED64:
                json[field_name] = reflection->GetInt64(message, field);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
            case ::google::protobuf::FieldDescriptor::TYPE_FIXED64:
                json[field_name] = reflection->GetUInt64(message, field);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
                json[field_name] = reflection->GetFloat(message, field);
                break;
            case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                json[field_name] = reflection->GetDouble(message, field);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_STRING:
            case ::google::protobuf::FieldDescriptor::TYPE_BYTES:
                json[field_name] = reflection->GetString(message, field);
                break;

            default:
                assert(false);
                break;
            }
        }
    }

    static void RepeatedMessage2Json(const ::google::protobuf::Message &message, const ::google::protobuf::FieldDescriptor *field, const ::google::protobuf::Reflection *reflection, nlohmann::json &json, bool enum2str = false)
    {
        assert(field);
        assert(reflection);

        const int count = reflection->FieldSize(message, field);
        for (auto i = 0; i < count; ++i)
        {
            nlohmann::json tmp_json;
            switch (field->type())
            {
            case ::google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            {
                const ::google::protobuf::Message &tmp_message = reflection->GetRepeatedMessage(message, field, i);
                if (0 != tmp_message.ByteSize())
                    Message2Json(tmp_message, tmp_json);
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_BOOL:
                tmp_json = reflection->GetRepeatedBool(message, field, i) ? true : false;
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_ENUM:
            {
                auto *enum_value_desc = reflection->GetRepeatedEnum(message, field, i);
                if (enum2str)
                    tmp_json = enum_value_desc->name();
                else
                    tmp_json = enum_value_desc->number();
            }
            break;

            case ::google::protobuf::FieldDescriptor::TYPE_INT32:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT32:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED32:
                tmp_json = reflection->GetRepeatedInt32(message, field, i);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_UINT32:
            case ::google::protobuf::FieldDescriptor::TYPE_FIXED32:
                tmp_json = reflection->GetRepeatedUInt32(message, field, i);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_INT64:
            case ::google::protobuf::FieldDescriptor::TYPE_SINT64:
            case ::google::protobuf::FieldDescriptor::TYPE_SFIXED64:
                tmp_json = reflection->GetRepeatedInt64(message, field, i);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_UINT64:
            case ::google::protobuf::FieldDescriptor::TYPE_FIXED64:
                tmp_json = reflection->GetRepeatedUInt64(message, field, i);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_FLOAT:
                tmp_json = reflection->GetRepeatedFloat(message, field, i);
                break;
            case ::google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                tmp_json = reflection->GetRepeatedDouble(message, field, i);
                break;

            case ::google::protobuf::FieldDescriptor::TYPE_STRING:
            case ::google::protobuf::FieldDescriptor::TYPE_BYTES:
                tmp_json = reflection->GetRepeatedString(message, field, i);
                break;

            default:
                assert(false);
                break;
            }
            json += std::move(tmp_json);
        }
    }
};
