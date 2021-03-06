#ifndef FILES_COMPONENT_H
#define FILES_COMPONENT_H

#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#endif

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <boost/filesystem.hpp>
#include <sys/stat.h>
#include <sys/types.h>

#include <fstream>

#include <binn.h>

#include "typedefs.h"
#include "daemon_server.h"

// ---------------------------------------------------------------------

class FileServerSess : public std::enable_shared_from_this<FileServerSess> {
public:
    static constexpr auto END_SYMBOLS = "\xFF\xFF\xFF\xFF";

    FileServerSess(std::shared_ptr<Connection> connection) : m_connection(std::move(connection)) {};

    ~FileServerSess() {
        binn_free(m_write_binn);
    };

    void start();

private:
    void do_read();
    void do_write();

    /**
     *  Response okay message
    */
    void write_ok();
    void write_ok(std::string message);

    /**
    * Response message
    *
    * @param snum message code (1 - error, 2 - critical error, 3 - unknown command, 100 - okay)
    * @param sdesc text message
    * @param write
    */
    void response_msg(unsigned int snum, const char * sdesc, bool write);

    void response_msg(unsigned int snum, const char * sdesc) {
        response_msg(snum, sdesc, false);
    }

    /**
     * Clear read variables
     */
    void clear_read_vars();

    /**
     * Clear write variables
     */
    void clear_write_vars();

    /**
     * Main command operations. Read dir, files, stat
     */
    void cmd_process();

    void open_input_file();
    void send_file();
    void close_input_file();
    
    void open_output_file();
    void write_file(size_t length);
    void close_output_file();

    /**
     * Check if read completed
     *
     * @param length
     * @return
     */
    size_t read_complete(size_t length);

    /**
     * Add end symbols
     *
     * @param buf
     * @param length
     * @return
     */
    int append_end_symbols(char * buf, size_t length);

    std::shared_ptr<Connection> m_connection;
    enum { max_length = 1024 };
    
    size_t m_read_length;
    char m_read_buf[max_length];
    char m_write_buf[max_length];

    std::string m_read_msg;
    std::string m_write_msg;
    
    binn *m_write_binn;
    binn *m_read_binn;

    /*
     * 0 - NoAuth
     * 1 - Auth
     * 2 - Cmd
     * 3 - File send
     * */
    int m_mode;

    boost::asio::streambuf request_buf;
    std::string m_filename;
    uint64 m_filesize;

    unsigned char m_sendfile_mode;
    
    std::ifstream m_input_file;
    std::ofstream m_output_file;
};

#endif
