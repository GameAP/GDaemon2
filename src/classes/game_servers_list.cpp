#include "consts.h"
#include "config.h"
#include "state.h"

#include "game_servers_list.h"

#include "functions/restapi.h"
#include "functions/gstring.h"

#include <json/json.h>

using namespace GameAP;

// ---------------------------------------------------------------------

int GameServersList::update_list()
{
    Json::Value jvalue;

    try {
        jvalue = Gameap::Rest::get("/gdaemon_api/servers?fields[servers]=id");
    } catch (Gameap::Rest::RestapiException &exception) {
        // Try later
        GAMEAP_LOG_ERROR << exception.what();
        return ERROR_STATUS_INT;
    }

    for (Json::ValueIterator itr = jvalue.begin(); itr != jvalue.end(); ++itr) {
        ulong server_id = getJsonUInt((*itr)["id"]);

        if (servers_list.find(server_id) == servers_list.end()) {

            std::shared_ptr<GameServer> gserver = std::make_shared<GameServer>(server_id);

            try {
                servers_list.insert(
                        servers_list.end(),
                        std::pair<ulong, std::shared_ptr<GameServer>>(server_id, gserver)
                );
            } catch (std::exception &e) {
                GAMEAP_LOG_ERROR << "GameServer #" << server_id << " insert error: " << e.what();
            }
        }
    }

    return SUCCESS_STATUS_INT;
}

// ---------------------------------------------------------------------

void GameServersList::stats_process()
{
    Json::Value jupdate_data;

    State& state = State::getInstance();
    time_t time_diff = std::stoi(state.get(STATE_PANEL_TIMEDIFF));

    for (auto& server : servers_list) {
        Json::Value jserver_data;

        jserver_data["id"] = server.second->get_id();

        if (server.second->m_last_process_check > 0 && server.second->m_installed == SERVER_INSTALLED) {
            time_t last_process_check = server.second->m_last_process_check - time_diff;
            std::tm * ptm = std::gmtime(&last_process_check);
            char buffer[32];
            std::strftime(buffer, 32, "%F %T", ptm);

            jserver_data["last_process_check"] = buffer;
            jserver_data["process_active"] = static_cast<int>(server.second->status_server());
            jupdate_data.append(jserver_data);
        }
    }

    try {
        Gameap::Rest::patch("/gdaemon_api/servers", jupdate_data);
    } catch (Gameap::Rest::RestapiException &exception) {
        GAMEAP_LOG_ERROR << exception.what() << '\n';
    }
}

// ---------------------------------------------------------------------

void GameServersList::loop()
{
    for (auto& server : servers_list) {
        server.second->loop();
    }

    stats_process();
}

// ---------------------------------------------------------------------

void GameServersList::update_all(bool force)
{
    for (auto& server : servers_list) {
        server.second->update(true);
    }
}

// ---------------------------------------------------------------------

GameServer * GameServersList::get_server(ulong server_id)
{
    if (servers_list.find(server_id) == servers_list.end()) {
        if (update_list() == ERROR_STATUS_INT) {
            return nullptr;
        }

        if (servers_list.find(server_id) == servers_list.end()) {
            return nullptr;
        }
    }

    if (servers_list[server_id] == nullptr) {
        return nullptr;
    }

    return servers_list[server_id].get();
}

// ---------------------------------------------------------------------

void GameServersList::delete_server(ulong server_id)
{
    std::map<ulong, std::shared_ptr<GameServer>>::iterator it = servers_list.find(server_id);

    if (it != servers_list.end()) {
        servers_list.erase(it);
    }
}