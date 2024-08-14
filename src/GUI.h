#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <string>
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

std::vector<std::string> split(const std::string& line, char delimiter);

struct item {
    std::string name;
    bool done;

    std::string str();
};

struct date {
    int year, month, day;

    void set(std::string s);
    void setToday();
    std::string str();
    bool operator==(date d);
    bool operator!=(date d);
};
 
struct day {
    date day;
    std::vector<item> list;
};

struct GUI {

    date today;

    std::vector<day> record;
    unsigned int currentDay;

    bool unsavedChanges;
    bool saveChangesWindow;

    bool deleteMode;
    unsigned int itemToBeDeleted;
    bool deleteWindow;

    bool tickMode;

    std::string newItem;
    bool addMode;

    void init();
    void loadRecord();
    void createRecord();
    void saveRecord();

    void action(unsigned int i);

    void drawItem(item i);
    void draw();
};