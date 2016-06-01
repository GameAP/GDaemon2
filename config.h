#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>

class Config
{
private:
    Config() {}
    Config( const Config&);  
    Config& operator=( Config& );
public:
    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    uint ds_id;
    uint listen_port;

    std::string daemon_login;
    std::string daemon_password;
    
    std::string db_driver;
    std::string db_host;
    std::string db_user;
    std::string db_passwd;
    std::string db_name;
    uint db_port;

    std::string crypt_key;
    std::string allowed_ip_str;
    uint port;

    std::vector<std::string> if_list = {"eth0"};
    std::vector<std::string> drives_list = {"/"};

    ushort stats_update_period      = 300;
    ushort stats_db_update_period   = 300;
    
    int parse();
};

#endif
