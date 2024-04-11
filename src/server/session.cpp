#include "server/session.hpp"

#include <iostream>
#include <boost/asio.hpp>
#include <memory>

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket) : socket_(std::move(socket)) {}

void Session::start()
{
    do_read();
}

void Session::do_read()
{
    // make sure current instance of Session remains alive if pending async ops
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                do_write(length);
            }
        });
}

void Session::do_write(std::size_t length)
{
    // make sure current instance of Session remains alive if pending async ops
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                // go back to do_read again when done
                do_read();
            }
        });
}