#include "stdafx.h"

#include "../common.shared/config/config.h"

#include "../corelib/collection_helper.h"
#include "../gui/core_dispatcher.h"
#include "utils/gui_coll_helper.h"

#include "app_config.h"

UI_NS_BEGIN

namespace
{
    AppConfigUptr AppConfig_;
}

AppConfig::AppConfig(const core::coll_helper &collection)
    : IsContextMenuFeaturesUnlocked_(collection.get<bool>("dev.unlock_context_menu_features"))
    , IsServerHistoryEnabled_(collection.get<bool>("history.is_server_history_enabled"))
    , IsCrashEnable_(collection.get<bool>("enable_crash"))
    , IsTestingEnable_(collection.get<bool>("enable_testing"))
    , IsFullLogEnabled_(collection.get<bool>("fulllog"))
    , IsUpdateble_(collection.get<bool>("updateble"))
    , IsShowMsgIdsEnabled_(collection.get<bool>("dev.show_message_ids"))
    , IsSaveCallRTPEnabled_(collection.get<bool>("dev.save_rtp_dumps", false))
    , IsServerSearchEnabled_(collection.get<bool>("dev.server_search", true))
    , IsShowHiddenThemes_(collection.get<bool>("show_hidden_themes", false))
    , WatchGuiMemoryEnabled_(false)
    , ShowMsgOptionHasChanged_(false)
    , GDPR_UserHasAgreed_(collection.get<bool>("gdpr.user_has_agreed") || config::get().is_on(config::features::auto_accepted_gdpr))
    , GDPR_AgreementReportedToServer_(collection.get<int32_t>("gdpr.agreement_reported_to_server") || config::get().is_on(config::features::auto_accepted_gdpr))
    , GDPR_UserHasLoggedInEver_(collection.get<bool>("gdpr.user_has_logged_in_ever") || config::get().is_on(config::features::auto_accepted_gdpr))
    , CacheHistoryContolPagesFor_(collection.get<int>("dev.cache_history_pages_secs"))
    , deviceId_(collection.get<std::string>("dev_id"))
    , urlMacUpdateAlpha_(collection.get<std::string>("urls.url_update_mac_alpha"))
    , urlMacUpdateBeta_(collection.get<std::string>("urls.url_update_mac_beta"))
    , urlMacUpdateRelease_(collection.get<std::string>("urls.url_update_mac_release"))
    , urlAttachPhone_(collection.get<std::string>("urls.url_attach_phone"))
{

}

bool AppConfig::IsContextMenuFeaturesUnlocked() const noexcept
{
    return environment::is_develop() || IsContextMenuFeaturesUnlocked_;
}

bool AppConfig::IsServerHistoryEnabled() const noexcept
{
    return IsServerHistoryEnabled_;
}

bool AppConfig::isCrashEnable() const noexcept
{
    return IsCrashEnable_;
}

bool AppConfig::IsTestingEnable() const noexcept
{
    return IsTestingEnable_;
}

bool AppConfig::IsFullLogEnabled() const noexcept
{
    return IsFullLogEnabled_;
}

bool AppConfig::IsUpdateble() const noexcept
{
    return IsUpdateble_;
}

bool AppConfig::IsShowMsgIdsEnabled() const noexcept
{
    return IsShowMsgIdsEnabled_;
}

bool AppConfig::IsSaveCallRTPEnabled() const noexcept
{
    return IsSaveCallRTPEnabled_;
}

bool AppConfig::IsServerSearchEnabled() const noexcept
{
    return IsServerSearchEnabled_;
}

bool AppConfig::IsShowHiddenThemes() const noexcept
{
    return IsShowHiddenThemes_;
}

bool AppConfig::WatchGuiMemoryEnabled() const noexcept
{
    return WatchGuiMemoryEnabled_;
}

bool AppConfig::ShowMsgOptionHasChanged() const noexcept
{
    return ShowMsgOptionHasChanged_;
}

bool AppConfig::GDPR_UserHasAgreed() const noexcept
{
    return GDPR_UserHasAgreed_;
}

int32_t AppConfig::GDPR_AgreementReportedToServer() const noexcept
{
    return GDPR_AgreementReportedToServer_;
}

bool AppConfig::GDPR_AgreedButAwaitingSend() const noexcept
{
    static std::set<GDPR_Report_To_Server_State> AwaitingResponseStates = {
        GDPR_Report_To_Server_State::Sent,
        GDPR_Report_To_Server_State::Sent_to_Core,
        GDPR_Report_To_Server_State::Failed,
    };

    auto currentState = static_cast<GDPR_Report_To_Server_State>(GDPR_AgreementReportedToServer());

    return GDPR_UserHasAgreed() && (AwaitingResponseStates.find(currentState) != AwaitingResponseStates.end());

}

bool AppConfig::GDPR_UserHasLoggedInEver() const noexcept
{
    return GDPR_UserHasLoggedInEver_;
}

const std::string& AppConfig::getMacUpdateAlpha() const noexcept
{
    return urlMacUpdateAlpha_;
}

const std::string& AppConfig::getMacUpdateBeta() const noexcept
{
    return urlMacUpdateBeta_;
}

const std::string& AppConfig::getMacUpdateRelease() const noexcept
{
    return urlMacUpdateRelease_;
}

const std::string& AppConfig::getUrlAttachPhone() const noexcept
{
    return urlAttachPhone_;
}

const std::string& AppConfig::getDevId() const noexcept
{
    return deviceId_;
}

int AppConfig::CacheHistoryControlPagesFor() const noexcept
{
    return CacheHistoryContolPagesFor_;
}

void AppConfig::SetFullLogEnabled(bool enabled) noexcept
{
    IsFullLogEnabled_ = enabled;
}

void AppConfig::SetUpdateble(bool enabled) noexcept
{
    IsUpdateble_ = enabled;
}

void AppConfig::SetSaveCallRTPEnabled(bool enabled) noexcept
{
    IsSaveCallRTPEnabled_ = enabled;
}

void AppConfig::SetServerSearchEnabled(bool enabled) noexcept
{
    IsServerSearchEnabled_ = enabled;
}

void AppConfig::SetShowHiddenThemes(bool enabled) noexcept
{
    IsShowHiddenThemes_ = enabled;
}

void AppConfig::SetContextMenuFeaturesUnlocked(bool unlocked) noexcept
{
    IsContextMenuFeaturesUnlocked_ = unlocked;
}

void AppConfig::SetShowMsgIdsEnabled(bool doShow) noexcept
{
    IsShowMsgIdsEnabled_ = doShow;
}

void AppConfig::SetShowMsgOptionHasChanged(bool changed) noexcept
{
    ShowMsgOptionHasChanged_ = changed;
}

void AppConfig::SetGDPR_UserHasLoggedInEver(bool hasLoggedIn) noexcept
{
    GDPR_UserHasLoggedInEver_ = hasLoggedIn;
}

void AppConfig::SetGDPR_AgreementReportedToServer(GDPR_Report_To_Server_State state) noexcept
{
    GDPR_AgreementReportedToServer_ = static_cast<int32_t>(state);
}

void AppConfig::SetCacheHistoryControlPagesFor(int secs) noexcept
{
    CacheHistoryContolPagesFor_ = secs;
}

void AppConfig::SetWatchGuiMemoryEnabled(bool _watch) noexcept
{
    WatchGuiMemoryEnabled_ = _watch;
}

void AppConfig::SetCustomDeviceId(bool _custom) noexcept
{
    if (_custom)
        deviceId_ = feature::default_dev_id();
    else
        deviceId_.clear();
}

bool AppConfig::hasCustomDeviceId() const
{
    return !deviceId_.empty() && deviceId_ != common::get_dev_id();
}

const AppConfig& GetAppConfig()
{
    assert(AppConfig_);

    return *AppConfig_;
}

void SetAppConfig(AppConfigUptr&& appConfig)
{
    assert(appConfig);

    if (AppConfig_)
    {
        if (AppConfig_->IsShowMsgIdsEnabled() != appConfig->IsShowMsgIdsEnabled())
        {
            appConfig->SetShowMsgOptionHasChanged(true);
        }
    }

    AppConfig_ = std::move(appConfig);
}

void ModifyAppConfig(AppConfig _appConfig, message_processed_callback _callback, QObject *_qobj, bool postToCore)
{
    Ui::gui_coll_helper collection(Ui::GetDispatcher()->create_collection(), true);
    collection.set_value_as_bool("fulllog", _appConfig.IsFullLogEnabled());
    collection.set_value_as_bool("updateble", _appConfig.IsUpdateble());
    collection.set_value_as_bool("dev.show_message_ids", _appConfig.IsShowMsgIdsEnabled());
    collection.set_value_as_bool("dev.save_rtp_dumps", _appConfig.IsSaveCallRTPEnabled());
    collection.set_value_as_bool("dev.unlock_context_menu_features", _appConfig.IsContextMenuFeaturesUnlocked());
    collection.set_value_as_bool("gdpr.user_has_logged_in_ever", _appConfig.GDPR_UserHasLoggedInEver());
    collection.set_value_as_int("dev.cache_history_pages_secs", _appConfig.CacheHistoryControlPagesFor());
    collection.set_value_as_bool("dev.server_search", _appConfig.IsServerSearchEnabled());
    collection.set_value_as_string("dev_id", _appConfig.getDevId());

    if (!postToCore)
    {
        SetAppConfig(std::make_unique<AppConfig>(std::move(_appConfig)));
        return;
    }

    GetDispatcher()->post_message_to_core("change_app_config", collection.get(), _qobj,
        [_appConfig{ std::move(_appConfig) }, _callback{ std::move(_callback) }](core::icollection* _coll) {
            Ui::gui_coll_helper coll(_coll, false);
            auto err = coll.get_value_as_int("error");
            Q_UNUSED(err);

            SetAppConfig(std::make_unique<AppConfig>(std::move(_appConfig)));

            if (_callback)
                _callback(_coll);
    });
}

UI_NS_END
