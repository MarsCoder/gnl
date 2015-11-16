/*
 * ParseConfig.cpp
 *
 *  Created on: Jun 20, 2015
 *      Author: mars
 */

#include "../head/ParseConfig.h"
#include <fstream>
#include <algorithm>
#include <iostream>

namespace gnl {

ParseConfig::ParseConfig(const std::string &file_name):config_file_name_(file_name)
{
	// TODO Auto-generated constructor stub
	parse_config();	//解析数据
}

ParseConfig::~ParseConfig()
{
	// TODO Auto-generated destructor stub
}

//判断字符是否为空格
bool ParseConfig::is_space(char c)
{
	if(c == ' ' || c == '\t')
		return true;
	return false;
}

//判断是否时注释字符
bool ParseConfig::is_note_char(char c)
{
	if(c == NOTE_CHAR)
		return true;
	return false;
}

//解析一行数据
bool ParseConfig::parse_line(std::string &line)
{

	if(line.empty())
		return false;
	int start_pos = 0, end_pos = line.size(), pos;
	if((pos = line.find(NOTE_CHAR)) != -1){
		if(pos == 0){		 // 行的第一个字符就是注释字符
			return false;
		}
		end_pos = pos;
	}
	std::string new_line = line.substr(start_pos, end_pos);
	if((pos = new_line.find('=')) == -1){
		return false;	// 没有=号
	}
	std::string key, value;
	std::string::iterator new_end;
	key = new_line.substr(0, pos);
	value = new_line.substr(pos + 1, end_pos - pos);
	new_end = std::remove(key.begin(), key.end(), ' ');//去除空格
	if(key.empty()){
		return false;
	}
	key.erase(new_end, key.end());			//删除多余的元数

	new_end = std::remove(value.begin(), value.end(), ' ');//去除空格
	value.erase(new_end, value.end());		//删除多余的元数
	map_config_[key] = value;

	return true;
}

//解析数据
bool ParseConfig::parse_config()
{
	std::ifstream file(config_file_name_);
	std::string line;
	if(!file){
		std::cout << "file open error" << std::endl;
		return false;
	}
	//读取每一行数据，解析后储存到map_config_中
	while(getline(file, line)){
		parse_line(line);
	}
	file.close();
	return true;
}


} /* namespace gnl */
