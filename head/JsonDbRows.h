/*
 * JsonDbRows.h
 *
 *  Created on: May 20, 2015
 *      Author: mars
 */

#ifndef JSONDBROWS_H_
#define JSONDBROWS_H_

#include "DbRows.h"
#include <string>

namespace gnl {

class JsonDbRows: public DbRows {
public:
	JsonDbRows(const DbRows* rows);
	virtual ~JsonDbRows();
	//以json格式返回结果集
	std::string get_result_in_json();

private:
	std::string jsonRows;	//以json格式保存的结果集
};

} /* namespace gnl */

#endif /* JSONDBROWS_H_ */
