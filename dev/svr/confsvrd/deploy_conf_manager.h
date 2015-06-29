// File:        deploy_conf_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-09 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/proto/conf_deploy.pb.h"

////////////////////////////////////////////////////////////
// DeployConfigManager
class DeployConfigManager : public lsf::basic::Singleton<DeployConfigManager> {
public:
    bool Init(char const* path);
    conf::Server const* GetServerConfig(conf::ENServerType server_type, uint32_t server_id) const;

    google::protobuf::RepeatedPtrField<conf::Server> const &  GetAllServerConfig() const { return _deploy_config.server_config(); }

private:
    conf::Deploy _deploy_config;
};

// vim:ts=4:sw=4:et:ft=cpp:
