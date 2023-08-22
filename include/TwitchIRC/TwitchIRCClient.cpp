/*
 * Copyright (C) 2011 Fredi Machado <https://github.com/fredimachado>
 * TwitchIRCClient is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * http://www.gnu.org/licenses/lgpl.html 
 */

#include <iostream>
#include <algorithm>
#include "TwitchIRCClient.hpp"

std::vector<std::string> split(std::string const& text, char sep)
{
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos)
    {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

bool TwitchIRCClient::InitSocket()
{
    return _socket.Init();
}

bool TwitchIRCClient::Connect()
{
    return _socket.Connect("irc.chat.twitch.tv", 6667);
}

void TwitchIRCClient::Disconnect()
{
    _socket.Disconnect();
}

bool TwitchIRCClient::SendIRC(std::string data)
{
    data.append("\n");
    return _socket.SendData(data.c_str());
}

bool TwitchIRCClient::Login(std::string nick, std::string oauth)
{
	if (!oauth.empty() && !SendIRC("PASS " + oauth))
		return false;
	if (SendIRC("NICK " + nick))
		return true;

    return false;
}

// changes string to lowercase in-place
void strToLower(std::string& str) {
    std::for_each(str.begin(), str.end(), [](char& c) { c = ::tolower(c); });
}
bool TwitchIRCClient::JoinChannel(std::string channel)
{
    strToLower(channel);
    LeaveChannel();
    _currentChannel = channel;
	if (SendIRC("JOIN #" + channel))
		return true;

    return false;
}

bool TwitchIRCClient::LeaveChannel()
{
    if(!_currentChannel.empty())
        if (SendIRC("PART #" + _currentChannel))
		    return true;
    return false;
}

bool TwitchIRCClient::SendChatMessage(std::string message)
{
	if (SendIRC("PRIVMSG #" + _currentChannel + " :" + message))
		return true;

    return false;
}

void TwitchIRCClient::ReceiveData()
{
    std::string buffer = _socket.ReceiveData();

    std::string line;
    std::istringstream iss(buffer);
    while(getline(iss, line))
    {
        if (line.find("\r") != std::string::npos)
            line = line.substr(0, line.size() - 1);
        Parse(line);
    }
}

void TwitchIRCClient::Parse(std::string data)
{
    std::string original(data);
    IRCCommandPrefix cmdPrefix;

    // if command has prefix
    if (data.substr(0, 1) == ":")
    {
        cmdPrefix.Parse(data);
        data = data.substr(data.find(" ") + 1);
    }

    std::string command = data.substr(0, data.find(" "));
    std::transform(command.begin(), command.end(), command.begin(), towupper);
    if (data.find(" ") != std::string::npos)
        data = data.substr(data.find(" ") + 1);
    else
        data = "";

    std::vector<std::string> parameters;

    if (data != "")
    {
        if (data.substr(0, 1) == ":")
            parameters.push_back(data.substr(1));
        else
        {
            size_t pos1 = 0, pos2;
            while ((pos2 = data.find(" ", pos1)) != std::string::npos)
            {
                parameters.push_back(data.substr(pos1, pos2 - pos1));
                pos1 = pos2 + 1;
                if (data.substr(pos1, 1) == ":")
                {
                    parameters.push_back(data.substr(pos1 + 1));
                    break;
                }
            }
            if (parameters.empty())
                parameters.push_back(data);
        }
    }

    if (command == "ERROR")
    {
        Disconnect();
        return;
    }

    if (command == "PING")
    {
        SendIRC("PONG :" + parameters.at(0));
        return;
    }

    IRCMessage ircMessage(command, cmdPrefix, parameters);

    // Try to call hook (if any matches)
    CallHook(command, ircMessage);
}

void TwitchIRCClient::HookIRCCommand(std::string command, void (*function)(IRCMessage /*message*/, TwitchIRCClient* /*client*/))
{
    IRCCommandHook hook;

    hook.command = command;
    hook.function = function;

    _hooks.push_back(hook);
}

void TwitchIRCClient::CallHook(std::string command, IRCMessage message)
{
    if (_hooks.empty())
        return;

    for (std::list<IRCCommandHook>::const_iterator itr = _hooks.begin(); itr != _hooks.end(); ++itr)
    {
        if (itr->command == command)
        {
            (*(itr->function))(message, this);
            break;
        }
    }
}
