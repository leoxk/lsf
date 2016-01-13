// File:        config_manager.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-14 by leoxiang

#include "svr/common/config_manager.h"
#include "svr/common/common_func.h"

void ConfigManager::GameConfigMerge(conf::Game const& game_config) {
    // get descriptor and reflection
    auto const* pdesc = game_config.GetDescriptor();
    auto const* pref  = game_config.GetReflection();

    // traverse all field, if has then clear and copy
    for (int i = 0; i < pdesc->field_count(); ++i) {
        // get field descriptor
        auto const* pfdesc = pdesc->field(i);

        // ignore none repeated
        if (pfdesc->label() != google::protobuf::FieldDescriptor::LABEL_REPEATED) continue;

        // check repeated field size
        if (pref->FieldSize(game_config, pfdesc) == 0) continue;

        // clear first
        pref->ClearField(&_game_config, pfdesc);

        //copy
        for (int i = 0; i < pref->FieldSize(game_config, pfdesc); ++i) {
            auto& src = pref->GetRepeatedMessage(game_config, pfdesc, i);
            pref->AddMessage(&_game_config, pfdesc)->CopyFrom(src);
        }
    }
}

bool ConfigManager::IsTest(uint64_t uid) {
    // check test env
    if (_server_config.test_env()) return true;

    // check test uid
    for (auto test_uid : _server_config.test_uids()) {
        if (test_uid == uid) return true;
    }
    return false;
}

// vim:ts=4:sw=4:et:ft=cpp:
