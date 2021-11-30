#include <iostream>
#include "leveldb/db.h"
#include "leveldb/iterator.h"
#include <stdio.h>
using namespace std;
 
int main()
{
    leveldb::DB* db;  // 数据库指针
    leveldb::Options options;  // 选项
    options.create_if_missing = true;  
    std::string dbpath = "testdb";  
    leveldb::Status status = leveldb::DB::Open(options, dbpath, &db);  // 直接调用leveldb::DB::Open, 是static函数
    assert(status.ok());  
    std::string key1 = "lyc";  
    std::string value1 = "liyc7711@gamil.com";  
    //cout<<"Open db OK"<<std::endl;  
    string key2 = "haha";
    string value2 = "segfwaga";
 
    std::string v1 = "";  
    string v2 = "";
    leveldb::Status s ;  
    for (int i = 0; i < 10000; i++) {
        s = db->Put(leveldb::WriteOptions(), key1, value1);/*key1和key2作为一对key-value对插入*/  
        s = db->Get(leveldb::ReadOptions(), key1, &v1);/*根据key返回对应的value值*/  
        db->Put(leveldb::WriteOptions(), key2, value2);
        db->Get(leveldb::ReadOptions(), key2, &v2);
    }

    cout<<v1<< " "<< v2<<std::endl; 
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for(it->SeekToFirst(); it->Valid(); it->Next()){
       cout<< it->key().ToString() << ":" << it->value().ToString()<<endl;
    }
    delete it; 
    delete db;/*删除数据库*/  
}

// g++ -o test1 test1.cc -I /home/larry/myproject/leveldb/install/include -L /home/larry/myproject/leveldb/install/lib -lleveldb -lpthread -lsnappy