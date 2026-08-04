#ifndef CLI_H 
#define CLI_H

#include <iostream>

class Cli {
    private:
        std::string version = "0.1";
        int arg_len; 
        char **args; 
        std::string _title;
        std::string desc;

        struct Command {
            std::string title; 
            std::string desc;
        };
        std::vector<struct Command> commands;

    public:
        Cli (int argc, char **argv, std::string title, std::string description) 
        {
            arg_len = argc; 
            args = argv;
            _title = title; 
            desc = description;

            if (arg_len < 2) 
            { 
                std::cout << _title << " version: " << version << std::endl;
            }
        }

        void add_command(std::string title, std::string description)
        {
            Command c; 
            c.title = title; 
            c.desc = description; 
            commands.push_back(c);
        }

        bool is_command(std::string arg)
        { 
            bool match = false; 
            for (int i=0; i < arg_len; i++)
            {
                auto c = args[i]; 
                if (arg == c) match = true;
            }
            return match; 
        }

        void print_help() 
        {
            std::cout << "Usage: " << _title << " <command>\n\n"; 
            std::cout << "Commands:\n"; 
            for (int i=0; i < commands.size(); i++)
            {
                auto command = commands.at(i);
                std::cout << "  " << command.title << "   " << command.desc << std::endl;
            }
        }
};

#endif

