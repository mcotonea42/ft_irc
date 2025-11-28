#pragma once

#include <iostream>
#include <string>
#include <ctime>

enum LogLevel{
	DEBUG = 0,
	INFO = 1,
	CMD = 2,
	ERROR = 3,
};

class Logger{
	public:

		static void log(LogLevel level, const std::string& message){
			std::string prefix;
			std::string color;
			const std::string reset = "\033[0m";

			switch (level){
				case DEBUG:
					prefix = "[DEBUG]";
					color = "\033[0;37m";
					break;
				case INFO:
					prefix = "[INFO]";
					color = "\033[0;32m";
					break;
				case CMD:
					prefix = "[CMD]";
					color = "\033[0;33m";
					break;
				case ERROR:
					prefix = "[ERROR]";
					color = "\033[0;31m";
					break;
				}

		char time_buff[80];
		std::time_t now = std::time(0);
		std::strftime(time_buff, sizeof(time_buff), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

		if (level == ERROR)
				std::cerr << std::string(time_buff) << " " << color << prefix << " " << message << reset << std::endl;
		else
				std::cout << std::string(time_buff) << " " << color << prefix << " " << message << reset << std::endl;
		}
};