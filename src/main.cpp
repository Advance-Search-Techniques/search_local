#include "interface.h"
#include <iostream>
extern "C" {
#include "tool.h"
}

int main()
{
	DataBase db;
	int rc=db.open("C:\\Users\\qin\\AppData\\Local\\Temp\\Textgps\\index\\yyt\\yyt.db", 
		"C:\\Users\\qin\\AppData\\Local\\Temp\\Textgps\\index\\yyt\\yyt.log");
	std::cout << rc << std::endl;
	/*rc = db.execute("create virtual table test using fts5(title,extension,cTime,mTime,path,size,content,tokenize=\'simple\');");
	std::cout << rc << std::endl;
	db.commit();*/
	/*rc = db.execute("delete from test;");
	std::cout << rc << std::endl;*/
	/*rc = db.execute("insert into test values(\'pyqt5 show which button was clicked\',\'.docx\',\'526852\',\'693258\',\'d:\\\',\'114\',\'I am new to PyQT5 and I want to have several buttons and have the one clicked last in a checked state. When another button is clicked the previous one gets unchecked while the clicked one gets checked.\');");
	std::cout << rc << std::endl;
	printf("%s",sqlite3_errmsg(db.getDB()));
	db.commit();*/
	/*StringContainer temp;
	temp.push_back(std::vector<std::string>({ "123","123","123","123","123","123","123" }));
	rc=db.execute("INSERT INTO test VALUES(?,?,?,?,?,?,?);", temp);
	db.commit();
	std::cout << rc << std::endl;
	rc = db.execute("INSERT INTO test VALUES(?,?,?,?,?,?,?);", temp);
	db.commit();
	std::cout << rc << std::endl;*/
	/*rc = db.execute("select * from test");
	std::cout << rc << std::endl;
	printf("%s\n", sqlite3_errmsg(db.getDB()));
	StringContainer s = db.fetchAll();
	for (auto c : s)
	{
		for (auto a : c)
		{
			printf("%s\n", a.c_str());
		}
	}*/
	/*db.setCase(true);*/
	/*std::vector<std::string> queries{"fuck"};
	std::vector<std::string> logics;
	db.queryArraySearch(queries,logics);
	db.close();
	rc = db.open("C:\\Users\\qin\\AppData\\Local\\Temp\\Textgps\\index\\abc\\abc.db",
		"C:\\Users\\qin\\AppData\\Local\\Temp\\Textgps\\index\\abc\\abc.log");
	std::cout << rc << std::endl;
	db.queryArraySearch(queries, logics);
	db.close();
	rc = db.open("C:\\Users\\qin\\AppData\\Local\\Temp\\Textgps\\index\\mmm\\mmm.db",
		"C:\\Users\\qin\\AppData\\Local\\Temp\\Textgps\\index\\mmm\\mmm.log");
	std::cout << rc << std::endl;*/
	/*db.queryArraySearch(queries, logics);*/
	/*InfoResult ir = db.getInfo();
	for (auto& c : ir)
	{
		std::cout << c.first[6] << std::endl;
		std::cout << *(c.second.end()-1) << std::endl;
		for (auto it = c.second.begin(); it != c.second.end() - 2; it++)
		{
			std:: cout << *it <<" " << std::endl;
		}
	}*/

	//std::cout << "first one" << std::endl;

	std::vector<std::string> queries={u8"ţ"};
	std::vector<std::string> logics;
	db.queryArraySearch(queries, logics);
	InfoResult ir = db.getInfo();
	for (auto& c : ir)
	{
		/*std::cout << std::get<0>(c)[6] << std::endl;
		std::cout << std::get<2>(c)[1] << std::endl;*/
		std::cout << c.first[6] << std::endl;
		std::cout << *(c.second.end() - 1) << std::endl;
		for (auto it = c.second.begin(); it != c.second.end() - 2; it++)
		{
			std::cout << *it << " " << std::endl;
		}
	}
	
	rc = db.close();
	std::cout << rc << std::endl;
	return 0;
}