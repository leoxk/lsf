#include "http_service.h"
#include "svr/common/config_manager.h"
#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/common/pb2json.h"
#include "svr/3rd/json.hpp"

using namespace lsf::asio;
using namespace lsf::util;
using namespace lsf::basic;

constexpr static const char *GET_MATCH_LIST = "/GetMatchList";
constexpr static const char *DEL_MATCH      = "/DelMatch";
constexpr static const char *QUERY_MATCH    = "/QueryMatch";
constexpr static const char *GET_CONFIG     = "/GetConfig";
constexpr static const char *PUT_CONFIG     = "/PutConfig";
constexpr static const char *MATCH_GMOPERATION = "/MatchSvr/GmOperation";
constexpr static const char *GENERAL_GMOPERATION = "/GameSvr/GmOperation";

bool HttpService::Init(){
    _server.Init(ConfigManager::Instance()->ServerConfig().http_listen());

    if(!_server.RegisterHandler("/test", [this](std::string query, std::string){
                _server.SendReply("/test", query + " test sucess");
                })){
        return false;
    }

    if(!_server.RegisterHandler(GET_MATCH_LIST, [this](std::string query, std::string body){
                // GetMatchList(query, body);
                })){
        return false;
    }

    if(!_server.RegisterHandler(DEL_MATCH, [this](std::string query, std::string body){
                // DelMatch(query, body);
                })){
        return false;
    }

    if(!_server.RegisterHandler(QUERY_MATCH, [this](std::string query, std::string body){
                // QueryMatch(query, body);
                })){
        return false;
    }

    if(!_server.RegisterHandler(GET_CONFIG, [this](std::string query, std::string body){
                // GetConfig(query, body);
                })){
        return false;
    }

    if(!_server.RegisterHandler(PUT_CONFIG, [this](std::string query, std::string body){
                // PutConfig(query, body);
                })){
        return false;
    }

    if(!_server.RegisterHandler("/TestPutConfig", [this](std::string query, std::string body){
                msg::SS message;
                // message.set_msg_type(msg::SS_TYPE_PLATFORM_UPDATE_CONFIG_REQ);
                ConnectServerMsgTransferService::Instance()->PostInternalMessage(message);
                })){
        return false;
    }

    if(!_server.RegisterHandler(MATCH_GMOPERATION, [this](std::string query, std::string body){
                // MatchSvrGmOperation(query, body);
                })){
        return false;
    }

    if(!_server.RegisterHandler(GENERAL_GMOPERATION, [this](std::string query, std::string body){
                // GeneralSvrGmOperation(query, body);
                })){
        return false;
    }

    if(!_server.Start()){
        LSF_LOG_ERR("http serivce start failed!");
        return false;
    }

    IOService::Instance()->AsyncAddTimerForever(DEF_CHECK_INTERVAL, [this](int){
            _server.Loop(DEF_LOOP_TIME);
            });

    return true;
}

void HttpService::Exit(){
    _server.UnRegisterHandler("/");
    _server.Close();
}

// void HttpService::SendGetMatchListReply(const msg::PlatformGetMatchListRsp& response){
//     _json.clear();
//     Pb2Json::Message2Json(response, _json);
//     std::string json_str = _json.dump();
//     _server.SendReply(GET_MATCH_LIST, json_str);
//     LSF_LOG_INF("GetMatchListReply:%s", json_str.c_str());
// }
//
// void HttpService::SendQueryMatchReply(const msg::PlatformQueryMatchRsp& response){
//     _json.clear();
//     Pb2Json::Message2Json(response, _json);
//     std::string json_str = _json.dump();
//     _server.SendReply(QUERY_MATCH, json_str);
//     LSF_LOG_INF("QueryMatchReply:%s", json_str.c_str());
// }
//
// void HttpService::SendPutConfigReply(bool success, std::string msg){
//     if(success){
//         _server.SendReply(PUT_CONFIG, "Update config success");
//     } else {
//         _server.SendError(PUT_CONFIG, 100, msg);
//     }
// }

// void HttpService::GetMatchList(std::string query, std::string body){
//     msg::SS message;
//     if(QueryStr2Message(query, *message.mutable_platform_get_match_list_req())){
//         message.set_msg_type(msg::SS_TYPE_PLATFORM_GET_MATCH_LIST_REQ);
//         ConnectServerMsgTransferService::Instance()->PostInternalMessage(message);
//     } else {
//         _server.SendError(GET_MATCH_LIST, 100, "Trans to inner msg error!");
//     }
// }
//
// void HttpService::DelMatch(std::string query, std::string body){
//    if(query.empty()){
//         _server.SendError(DEL_MATCH, 100, "Need parameters");
//         return;
//     }
//
//     msg::SS message;
//     if(QueryStr2Message(query, *message.mutable_platform_del_match_req())){
//         message.set_msg_type(msg::SS_TYPE_PLATFORM_DEL_MATCH_REQ);
//         ConnectServerMsgTransferService::Instance()->PostInternalMessage(message);
//         _server.SendReply(DEL_MATCH, "OK");
//     } else {
//         _server.SendError(DEL_MATCH, 100, "Trans to inner msg error!");
//     }
// }
//
// void HttpService::QueryMatch(std::string query, std::string body){
//    if(query.empty()){
//        _server.SendError(QUERY_MATCH, 100, "Need parameters");
//        return;
//    }
//
//    msg::SS message;
//     if(QueryStr2Message(query, *message.mutable_platform_query_match_req())){
//         message.set_msg_type(msg::SS_TYPE_PLATFORM_QUERY_MATCH_REQ);
//         ConnectServerMsgTransferService::Instance()->PostInternalMessage(message);
//     } else {
//         _server.SendError(QUERY_MATCH, 100, "Trans to inner msg error!");
//     }
// }
//
// void HttpService::GetConfig(std::string query, std::string body){
//     auto table_conf_path = ConfigManager::Instance()->ServerConfig().table_conf_path();
//     auto cmdline = std::string("cd ") + table_conf_path + "&& zip config *.xlsx *.xls";
//     if(!System::RunShellCmd(cmdline)){
//         _server.SendError(GET_CONFIG, 100,
//                 std::string("Deflate config file to zip error!"));
//         return;
//     }
//
//     auto conf_zip_name = table_conf_path + "/config.zip";
//     if(!System::IsExist(conf_zip_name)){
//         if(!System::RunShellCmd(std::string("rm -f ") + conf_zip_name)){
//             _server.SendError(GET_CONFIG, 100, "Delete old config.zip error!");
//             return;
//         }
//     }
//
//     std::ifstream ifile;
//     ifile.open(conf_zip_name, std::ios::in);
//     if(!ifile.is_open()) {
//         _server.SendError(GET_CONFIG, 100, "Read config.zip error!");
//         return;
//     }
//
//     std::stringstream ss;
//     ss << ifile.rdbuf();
//
//     _server.SendFile(GET_CONFIG, ss.str());
//     ifile.close();
// }
//
// void HttpService::PutConfig(std::string query, std::string body){
//     auto table_conf_path = ConfigManager::Instance()->ServerConfig().table_conf_path();
//     auto conf_zip_name = table_conf_path + "/config.zip";
//
//     if(System::IsExist(conf_zip_name)){
//         if(!System::RunShellCmd(std::string("rm -f ") + conf_zip_name)){
//             _server.SendError(PUT_CONFIG, 100, "Write config.zip error!");
//             return;
//         }
//     }
//
//     std::ofstream ofile;
//     ofile.open(conf_zip_name, std::ios::out);
//     if(!ofile.is_open()) {
//         _server.SendError(PUT_CONFIG, 100, "Write config.zip error!");
//         return;
//     }
//
//     ofile << body;
//     ofile.close();
//
//     auto bak_dir = ".bak/";
//     auto bak_path = table_conf_path + bak_dir;
//     if(!System::IsExist(bak_path)){
//         System::MkDir(bak_path);
//     }
//
//     Date date;
//     auto bak_name = StringExt::Format("config%04d%02d%02d%02d%02d.zip", date.GetYear(), date.GetMonth(), date.GetDay(), date.GetHour(), date.GetMinute());
//     auto cmdline = std::string("cd ") + table_conf_path
//                 + "&& zip " + bak_name + " *.xlsx *.xls && "
//                 + "mv " + bak_name + " " + bak_dir
//                 + "&& unzip -o config.zip &&"
//                 + "for _file in $(ls *.xlsx); do xlsx2csv $_file $(basename $_file .xlsx).csv;done && "
//                 + "for _file in $(ls *.xls); do xls2csv $_file $(basename $_file .xls).csv;done";
//
//     if(!System::RunShellCmd(cmdline)){
//         _server.SendError(PUT_CONFIG, 100, "Inflate config.zip error!");
//         return;
//     }
//
//     msg::SS message;
//     message.set_msg_type(msg::SS_TYPE_PLATFORM_UPDATE_CONFIG_REQ);
//     ConnectServerMsgTransferService::Instance()->PostInternalMessage(message);
// }
//
// void HttpService::MatchSvrGmOperation(std::string query, std::string body){
//     if(ParseJson(MATCH_GMOPERATION, body)){
//         msg::SS message;
//         if(Pb2Json::Json2Message(_json, *message.mutable_gm_operation_notify())){
//             auto& notify = *message.mutable_gm_operation_notify();
//             if(notify.oper_type() == "GmNotice"){
//                 notify.mutable_notice()->set_content(Base64Decode(notify.notice().content()));
//             }
//
//             message.set_msg_type(msg::SS_TYPE_MATCH_GM_OPERATION);
//             ConnectServerMsgTransferService::Instance()->PostInternalMessage(message);
//             _server.SendReply(MATCH_GMOPERATION, "OK");
//         } else {
//             _server.SendError(MATCH_GMOPERATION, 100, "Trans to inner msg error!");
//         }
//     }
// }
//
// void HttpService::GeneralSvrGmOperation(std::string query, std::string body){
//      if(ParseJson(GENERAL_GMOPERATION, body)){
//         msg::SS message;
//         if(Pb2Json::Json2Message(_json, *message.mutable_gm_operation_notify())){
//             auto& notify = *message.mutable_gm_operation_notify();
//             if(notify.oper_type() == "GmNotice"){
//                 notify.mutable_notice()->set_content(Base64Decode(notify.notice().content()));
//             }
//
//             message.set_msg_type(msg::SS_TYPE_GAME_GM_OPERATION);
//             ConnectServerMsgTransferService::Instance()->PostInternalMessage(message);
//             _server.SendReply(GENERAL_GMOPERATION, "OK");
//         } else {
//             _server.SendError(GENERAL_GMOPERATION, 100, "Trans to inner msg error!");
//         }
//     }
// }
//
bool HttpService::ParseJson(std::string uri, std::string const content){
    try{
        _json = Json::parse(content);
        return true;
    } catch(std::invalid_argument const& e) {
        auto error = std::string("json parse failed, ") + e.what() + ", content:" + content;
        LSF_LOG_ERR("%s", error.c_str());
        _server.SendError(uri, 100, error);
        return false;
    }
}

bool HttpService::QueryStr2Message(std::string query, ProtobufMsg& message){
    constexpr static char const* HTTP_QUERY_ELE_DILIMETER = "&";
    constexpr static char const* HTTP_QUERY_PARAM_DILIMETER = "=";

    if(query.empty())
        return true;

    std::map<std::string, std::string> query_map;

    auto vec = StringExt::SplitStr(query, HTTP_QUERY_ELE_DILIMETER);
    for(auto ele : vec){
        auto key = StringExt::SplitGet(ele, HTTP_QUERY_PARAM_DILIMETER, 0);
        auto value = StringExt::SplitGet(ele, HTTP_QUERY_PARAM_DILIMETER, 1);
        query_map[key] = value;
    }

    if(query_map.empty())
        return false;

    auto descriptor = message.GetDescriptor();
    auto reflection = message.GetReflection();
    if(nullptr == descriptor || nullptr == reflection)
        return false;

    for(auto pair : query_map){
        auto field = descriptor->FindFieldByName(pair.first);
        if(nullptr == field)
            field = reflection->FindKnownExtensionByName(pair.first);

        if(nullptr != field){
            switch(field->type()){
                case ProtobufFieldDescriptor::TYPE_INT32:
                    reflection->SetInt32(&message, field, (int32_t)atoi(pair.second.c_str()));
                    break;
                case ProtobufFieldDescriptor::TYPE_UINT32:
                    reflection->SetUInt32(&message, field, (uint32_t)atoi(pair.second.c_str()));
                    break;
                case ProtobufFieldDescriptor::TYPE_INT64:
                    reflection->SetInt64(&message, field, (int64_t)atoll(pair.second.c_str()));
                    break;
                case ProtobufFieldDescriptor::TYPE_UINT64:
                    reflection->SetUInt64(&message, field, (uint64_t)atoll(pair.second.c_str()));
                    break;
                case ProtobufFieldDescriptor::TYPE_FLOAT:
                    reflection->SetFloat(&message, field, (float)atof(pair.second.c_str()));
                    break;
                case ProtobufFieldDescriptor::TYPE_DOUBLE:
                    reflection->SetDouble(&message, field, (double)atof(pair.second.c_str()));
                    break;
                case ProtobufFieldDescriptor::TYPE_BYTES:
                case ProtobufFieldDescriptor::TYPE_STRING:
                    reflection->SetString(&message, field, pair.second);
                    break;
                default:
                    break;
            }
        }
    }
    return true;
}




