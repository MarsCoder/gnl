/*
 * JsonDbRows.cpp
 *
 *  Created on: May 20, 2015
 *      Author: mars
 */

#include "../head/JsonDbRows.h"
#include "../jsonCpp/include/json/json.h"

namespace gnl {

JsonDbRows::JsonDbRows(const DbRows* rows):DbRows(rows) {
	// TODO Auto-generated constructor stub

}

JsonDbRows::~JsonDbRows() {
	// TODO Auto-generated destructor stub
}

//以json格式返回结果集
std::string JsonDbRows::get_result_in_json()
{
	//结果集的字段名
	std::vector<const char*> tempNames = get_name();
	//结果集的全部记录
	const std::vector<DbRow*> tempRows = get_rows();
	Json::Value root;
	Json::Value row;
	Json::Value rows;
	std::vector<DbRow*>::const_iterator rowsIt = tempRows.begin();
	std::vector<const char*>::const_iterator nameIt = tempNames.begin();
	int i;
	int n = tempNames.size();

	for(; rowsIt < tempRows.end(); ++rowsIt){
		for(i=0; i < n; ++i){
			row[*(&*nameIt+i)] = (*(*rowsIt))[(unsigned int)i];
		}
		rows.append(row);
	}
	root["array"] = rows;
	jsonRows =  root.toStyledString();	//返回
	return jsonRows;
}
/*
//解析json数据并储存
std::vector<std::string> JsonDbRows::store_result_from_json(std::string json)
{
	Json::Reader reader;
	Json::Value root;
	Json::Value rows;
	Json::Value row;
	std::string str;
	if(reader.parse(json, root)){
		rows = root["array"];
		//获得字段名
		json_name_ = rows[(unsigned int)0].getMemberNames();
		int rowsSize = rows.size();		//取得结果集的中记录的数量
		int nameSize = json_name_.size();		//每条记录的字段数

		for(int i=0; i< rowsSize; ++i){

		}
	}
}
*/

} /* namespace gnl */
