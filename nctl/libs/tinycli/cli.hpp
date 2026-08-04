#ifndef CLI_H 
#define CLI_H

#include <iostream>
#include <vector>

class Cli {
    private:
        std::string version = "0.1";
        int arg_len; 
        char **args; 
		int i = 0;
        std::string _title;
        std::string desc;

        struct Command {
            std::string title; 
            std::string desc;
        };


		struct Option {
            std::string title; 
            std::string desc;
			bool is_bool;
			std::string value;
        };
        std::vector<struct Command> commands;
        std::vector<struct Option> options;

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


		void add_option(std::string flag, std::string description, bool is_bool)
		{
			struct Option option = {};		
			option.title = flag; 
			option.desc = description;
			option.is_bool = is_bool; 

			i++;
			if (i >= arg_len)
			{
				std::cout << "Expected argument after option '" << option.title << "'\n" ;
				std::exit(1);
			}
			std::string arg = args[i];
			option.value = arg; 

			options.push_back(option);
		}

        void add_command(std::string title, std::string description)
        {
            Command c; 
            c.title = title; 
            c.desc = description; 
            commands.push_back(c);
        }


		std::string get_option(std::string arg)
		{
			for (auto option: options)
			{
				if (option.title == arg) return option.value;
			}
			return "";	 	
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
			std::cout << "Options:\n"; 
            for (int i=0; i < options.size(); i++)
            {
                auto option = options.at(i);
                std::cout << "  " << option.title << "   " << option.desc << std::endl;
            }
            std::cout << "\nCommands:\n"; 
            for (int i=0; i < commands.size(); i++)
            {
                auto command = commands.at(i);
                std::cout << "  " << command.title << "   " << command.desc << std::endl;
            }
        }
};

#endif

