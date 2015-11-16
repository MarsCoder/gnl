/*
 * ParseConfigure.h
 *
 *  Created on: Jun 20, 2015
 *      Author: mars
 *     读取配置文件的信息，以map的形式存入
 *      配置文件的格式，以#作为行注释，配置的形式是key = value，中间可有空格，也可没有空格
 */

#ifndef PARSECONFIG_H_
#define PARSECONFIG_H_

#include <string>
#include <map>

namespace gnl {

#define NOTE_CHAR '#'

class ParseConfig {
public:
	ParseConfig(const std::string &filename);
	virtual ~ParseConfig();

	//返回配置文件数据
	std::map<std::string, std::string>& get_map_config(){
		return map_config_;;
	}

private:
	//判断字符是否为空格
	bool is_space(char c);
	//判断是否时注释字符
	bool is_note_char(char c);
	//解析一行数据
	bool parse_line(std::string &line);
	//解析数据
	bool parse_config();

private:
	//以map的形式存入配置文件的信息
	std::map<std::string, std::string> map_config_;
	//配置文件名
	const std::string config_file_name_;
};

} /* namespace gnl */

#endif /* PARSECONFIG_H_ */
