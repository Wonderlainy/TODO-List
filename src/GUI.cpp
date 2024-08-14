#include "GUI.h"

std::vector<std::string> split(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;
    
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string item::str() {
    return name + "," + (done ? "V" : "X");
}

void date::set(std::string s) {
    std::vector<std::string> tokens = split(s, '/');
    this->day = std::stoi(tokens[0]);
    this->month = std::stoi(tokens[1]);
    this->year = std::stoi(tokens[2]);
}

void date::setToday() {
    SYSTEMTIME st;
    GetLocalTime(&st);

    this->year = st.wYear;
    this->month = st.wMonth;
    this->day = st.wDay;
}

std::string date::str() {
    return std::to_string(this->day) + "/" + std::to_string(this->month) + "/" + std::to_string(this->year);
}

bool date::operator==(date d) {
    return this->year == d.year &&
           this->month == d.month &&
           this->day == d.day;
}

bool date::operator!=(date d) {
    return !(*this == d);
}

void GUI::init() {
    today.setToday();

    std::filesystem::path filePath = "record.txt";
    if (std::filesystem::exists(filePath)) {
        loadRecord();
    } else {
        createRecord();
    }

    deleteMode = false;
    deleteWindow = false;
    tickMode = false;
    addMode = false;
    unsavedChanges = false;
    saveChangesWindow = false;
    newItem = "";

    if(today != record[currentDay].day) {
        record.push_back(record[currentDay]);
        currentDay++;
        record[currentDay].day = today;
        for(unsigned int i = 0; i < record[currentDay].list.size(); i++) {
            record[currentDay].list[i].done = false;
        }
        unsavedChanges = true;
    }
}

void GUI::loadRecord() {
    std::ifstream file("record.txt");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::vector<std::string> tokens = split(line, ';');
            date d;
            d.set(tokens[0]);
            day newDay;
            newDay.day = d;
            for(unsigned int i = 1; i < tokens.size(); i++) {
                std::vector<std::string> listItem = split(tokens[i], ',');
                item it;
                it.name = listItem[0];
                it.done = listItem[1] == "V" ? true : false;
                newDay.list.push_back(it);
            }
            record.push_back(newDay);
        }
    }
    file.close();  // Close the file

    currentDay = record.size() - 1;
}

void GUI::createRecord() {
    std::ofstream file("record.txt");
    if (file.is_open()) {
        file << today.str() + ";";
    }
    file.close();
    loadRecord();
}

void GUI::saveRecord() {
    std::ofstream file("record.txt");
    if (file.is_open()) {
        for(unsigned int i = 0; i < record.size(); i++) {
            std::string line = "";
            line += record[i].day.str();
            for(unsigned int j = 0; j < record[i].list.size(); j++) {
                line += ";" + record[i].list[j].str();
            }
            file << line + "\n";
        }
    }
    file.close();
    unsavedChanges = false;
}

void GUI::action(unsigned int i) {
    if(deleteMode) {
        deleteWindow = true;
        itemToBeDeleted = i;
    }
    if(tickMode) {
        record[currentDay].list[i].done = !record[currentDay].list[i].done;
        unsavedChanges = true;
    }
}

void GUI::drawItem(item i) {
    ImGui::Text("[");
    ImGui::SameLine();
    if(i.done) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "V");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "X");
    }
    ImGui::SameLine();
    ImGui::Text(("] - " + i.name).c_str());
}

void GUI::draw() {
    // Your ImGui code here
    ImGui::Begin("TODO List");
    if(ImGui::Button("<--")) {
        if(!deleteWindow) {
            if(currentDay != 0) currentDay--;
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("-->")) {
        if(!deleteWindow) {
            if(currentDay != record.size() - 1) currentDay++;
        }
    }
    ImGui::SameLine();
    if(ImGui::Button("Add")) {
        addMode = !addMode;
    }
    ImGui::SameLine();
    if(ImGui::Button("Delete")) {
        deleteMode = !deleteMode;
        tickMode = false;
    }
    ImGui::SameLine();
    if(ImGui::Button("Tick")) {
        tickMode = !tickMode;
        deleteMode = false;
    }

    ImGui::Text(("Date: " + record[currentDay].day.str()).c_str());
    for(unsigned int i = 0; i < record[currentDay].list.size(); i++) {
        if(deleteMode || tickMode) {
            if(ImGui::Button(std::to_string(i).c_str())) {
                action(i);
            }
            ImGui::SameLine();
        }
        drawItem(record[currentDay].list[i]);
    }

    ImGui::End();

    if(deleteWindow) {
        ImGui::Begin("Delete item");
        ImGui::Text("Are you sure that you want to delete this item?");
        if(ImGui::Button("Yes")) {
            record[currentDay].list.erase(record[currentDay].list.begin() + itemToBeDeleted);
            deleteWindow = false;
            unsavedChanges = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("No")) {
            deleteWindow = false;
        }
        ImGui::End();
    }

    if(addMode) {
        ImGui::Begin("Add item");
        ImGui::InputText("Item Name", &newItem);
        if(ImGui::Button("Add")) {
            item i;
            i.name = newItem;
            i.done = false;
            record[currentDay].list.push_back(i);
            newItem = "";
            addMode = false;
            unsavedChanges = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Close")) {
            newItem = "";
            addMode = false;
        }
        ImGui::End();
    }

    ImGui::Begin("Save changes");
    if(unsavedChanges) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Unsaved changes");
        if(ImGui::Button("Save")) {
            saveRecord();
        }
    } else {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Saved changes");
    }
    ImGui::End();

    if(saveChangesWindow) {
        ImGui::Begin("Exit saving changes?");
        if(ImGui::Button("Save")) {
            saveRecord();
        }
        ImGui::SameLine();
        if(ImGui::Button("Discard")) {
            unsavedChanges = false;
        }
        ImGui::End();
    }

}