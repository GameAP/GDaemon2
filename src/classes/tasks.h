#ifndef TASKS_H
#define TASKS_H

#include <iostream>
#include <vector>

#include "log.h"

#include "classes/game_server.h"
#include "classes/game_servers_list.h"

#define TASK_WAITING    1
#define TASK_WORKING    2
#define TASK_ERROR      3
#define TASK_SUCCESS    4
#define TASK_CANCELED   5

#define TASK_GAME_SERVER_START      "gsstart"
#define TASK_GAME_SERVER_PAUSE      "gspause" // NOT implemented
#define TASK_GAME_SERVER_STOP       "gsstop"
#define TASK_GAME_SERVER_KILL       "gskill" // NOT implemented
#define TASK_GAME_SERVER_RESTART    "gsrest"
#define TASK_GAME_SERVER_INSTALL    "gsinst"
#define TASK_GAME_SERVER_REINSTALL  "gsreinst" // NOT implemented
#define TASK_GAME_SERVER_UPDATE     "gsupd"
#define TASK_GAME_SERVER_DELETE     "gsdel"
#define TASK_GAME_SERVER_MOVE       "gsmove"
#define TASK_GAME_SERVER_EXECUTE    "cmdexec"

namespace GameAP {

class Task {
public:
    Task(ulong task_id, ulong ds_id, ulong server_id, const char * task, const char * data, const char * cmd, ushort status) {
        m_task_id = task_id;
        m_ds_id = ds_id;
        m_server_id = server_id;

        size_t mcsz = strlen(task);
        if (mcsz > 8) {
            mcsz = 8;
        }
        memcpy(m_task, task, mcsz);

        if (mcsz < 8) {
            m_task[mcsz] = '\0';
        }
        
        m_data = data;
        m_cmd = cmd;
        m_status = status;

        m_gserver = nullptr;
        
        m_cur_outpos = 0;
    }

    ~Task() {
        GAMEAP_LOG_VERBOSE << "Task destructor: " << m_task_id;
    }

    /**
     * Run task
     */
    void run();

    /**
     * Get the result of the task commands
     */
    std::string get_output();

    /**
     * Get task status
     */
    int get_status()
    {
        return m_status;
    }

    /**
     * Get task ID
     */
    ulong get_id()
    {
        return m_task_id;
    }

    /**
     * Get task server ID
     */
    ulong get_server_id()
    {
        return m_server_id;
    }

    ulong run_after(ulong aft)
    {
        m_task_run_after = aft;
        return m_task_run_after;
    }

    ulong run_after()
    {
        return m_task_run_after;
    }
    
    time_t get_time_started() {
        return m_task_started;
    }

private:
    ulong m_task_id;
    ulong m_task_run_after;

    ulong m_ds_id;
    ulong m_server_id;
    char m_task[8];

    std::string m_data;
    std::string m_cmd;
    std::string m_cmd_output;
    std::mutex m_cmd_output_mutex;

    size_t m_cur_outpos;

    enum st {waiting = 1, working, error, success};
    ushort m_status;

    time_t m_task_started;

    GameServer *m_gserver;

    int _exec(std::string cmd);
    int _single_exec(std::string cmd);
    void _append_cmd_output(std::string line);
};


/* End namespace GameAP */
}

#endif
