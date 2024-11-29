// g++ im.cpp -l sqlite3
// ./a.exe

#include <iostream>
#include <string>
#include ".vscode/sqlite/sqlite3.h"
using namespace std;

class Inventory {
    private:
    int rc;
    char* messaggeError; 

    public: 
        void addItem(sqlite3 *db) {
            string name;
            int price;
            cout << "Enter name: ";
            cin.ignore();
            getline(cin,name);
            cout << "Enter price: ";
            cin >> price;
            string query = "INSERT INTO BOX (NAME,PRICE) VALUES('"+ name + "', " + to_string(price) + ");";
            // cout << query << endl;
            rc = sqlite3_exec(db, query.c_str(), NULL, 0, &messaggeError);  
            if (rc != SQLITE_OK) { 
                cerr << "Couldn't run insert query" << endl; 
                sqlite3_free(messaggeError); 
                return;
            } else {
                cout << "Successfully ran insert query" << endl;
            }
        }
        void removeItem(sqlite3 *db) {
            string name;
            cout << "Enter name: ";
            cin.ignore();
            getline(cin,name);
            string query = "DELETE FROM BOX WHERE NAME = '" + name + "';";
            // cout << query << endl;
            rc = sqlite3_exec(db, query.c_str(), NULL, 0, &messaggeError); 
            if (rc != SQLITE_OK) { 
                cerr << "Couldn't run delete query" << endl; 
                sqlite3_free(messaggeError); 
                return;
            } else {
                cout << "Successfully ran deleted query" << endl;
            }
        }
        void updateItem(sqlite3 *db) {
            string name;
            string field;
            string newEntry;
            cout << "Enter name of item to update: ";
            cin.ignore();
            getline(cin,name);
            cout << "Enter name of field to update for " + name + ": ";
            getline(cin,field);
            cout << "Enter new value for " + field + ": ";
            getline(cin,newEntry);
            string query = "UPDATE BOX SET " + field + " = '" + newEntry + "' WHERE NAME = '" + name + "';";
            // cout << query << endl;
            rc = sqlite3_exec(db, query.c_str(), NULL, 0, &messaggeError);
            if (rc != SQLITE_OK) { 
                cerr << "Couldn't run update query" << endl; 
                sqlite3_free(messaggeError); 
                return;
            } else {
                cout << "Successfully ran update query" << endl;
            } 
        }
        void listItems(sqlite3 *db) {
            int id = 1;
            string query = "SELECT * FROM BOX";
            sqlite3_stmt *stmt;
            rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

            if (rc != SQLITE_OK) {
                cerr << "Couldn't prepare list statement" << endl;
                return;
            }

            int num_cols = sqlite3_column_count(stmt);
            for (int i = 0; i < num_cols; i++) {
                cout << sqlite3_column_name(stmt,i);
                if (i < num_cols-1) {
                        cout << " | ";
                    }
            }
            cout << endl;

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                
                for (int i = 0; i < num_cols; i++) {
                    if (sqlite3_column_text(stmt,i)) {
                        cout << sqlite3_column_text(stmt,i);
                    } else {
                        cout << id;
                        id++;
                    }
                    if (i < num_cols-1) {
                        cout << " | ";
                    }
                }
                cout << endl;
            }
            sqlite3_finalize(stmt);            
        }     
        void totalPrice(sqlite3 *db) {
            string query = "SELECT SUM(PRICE) FROM BOX";
            sqlite3_stmt *stmt;
            rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

            if (rc != SQLITE_OK) {
                cerr << "Couldn't prepare price summation" << endl;
                return;
            }
            int add = sqlite3_step(stmt);
            if (add == SQLITE_ROW) {
                cout << "Total Price: " << sqlite3_column_int(stmt,0) << endl; 
            } else if (add != SQLITE_DONE) {
                cerr << "Couldn't execute total price statement" << endl;
                return;
            }
            
        }  
};

int main() {
    Inventory inventory;
    sqlite3* db;

    string sql = "CREATE TABLE BOX("
                      "ID INT IDENTITY(1,1) PRIMARY KEY, "
                      "NAME          TEXT     NOT NULL, "
                      "PRICE         REAL );"; 

    int rc;
    rc = sqlite3_open("inventory.db", &db);
    if (rc != SQLITE_OK) {
        cout << "Coudln't open file" << endl;
    } else {
        cout << "File opened" << endl;
    }

    char* messaggeError; 
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &messaggeError); 

    if (rc != SQLITE_OK) { 
        cerr << "Couldn't create table or table is already created" << endl; 
        sqlite3_free(messaggeError); 
    } else {
        cout << "Table created" << endl;
    }

    char option;
    bool exit = false;  
    while (exit == false) {
        cout << "Choose an option: \n1: View all entries\n2: Add Entry\n3: Delete Entry\n4: Update Entry\n5: Total Price\n6: Quit" << endl;
        cin >> option;
        switch(option) {
            case '1': {
                // cout << "OPTION 1" << endl;
                inventory.listItems(db);
                break;
            }
            case '2': {
                // cout << "OPTION 2" << endl;
                inventory.addItem(db);
                break;
            }
            case '3': {
                // cout << "OPTION 3" << endl;
                inventory.removeItem(db);
                break;
            }
            case '4': {
                // cout << "OPTION 4" << endl;
                inventory.updateItem(db);
                break;
            }
            case '5': {
                // cout << "OPTION 5" << endl;
                inventory.totalPrice(db);
                break;
            }
            case '6': {
                // cout << "OPTION 6" << endl;
                exit = true;
                break;
            }
        }
    }


    sqlite3_close(db);
    return 0;
}