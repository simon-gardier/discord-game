#ifndef BACKUP_HPP
#define BACKUP_HPP

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <dpp/json.h>

/**
 * @class JsonContent
 * @brief Interface for objects that can be represented as json / created from json
 * @tparam Content the content to backup
 */
template <typename Content>
class JsonContent {
    public:
        /**
         * @brief convert the object implementing the inferface to a json object
         * 
         * @return std::shared_ptr<nlohmann::json>
         */
        virtual std::shared_ptr<nlohmann::json> to_json() = 0;

        /**
         * @brief fill the object implementing the inferface from a json object
         * 
         * @param file the json object to fill the object with
         * @return Content& 
         */
        virtual Content& from_json(const nlohmann::json& file) = 0;
};


/**
 * @class JsonBackup 
 * @brief   Represents a backup of a serie of JsonContent.
 *          The backup is saved in a folder and can be loaded from it.
 *          The JsonContent objects are store in different files under 
 *          the save_folder directory and are registered in save_file_register. 
 */
template <typename Content>
class JsonBackup {
    private:
        static const std::string BACKUP_FOLDER;     // path to the backup folder for all the backups
    protected:
        const std::string save_folder;              // folder where the backup is saved
    public:
        /**
         * @brief Construct a new Json Backup object. Creates the save_folder if it does not already exist.
         * 
         * @param save_folder 
         */
        JsonBackup(const std::string save_folder);

        /**
         * @brief Save a JsonContent object in the save_folder directory. Adds the object to the save_file_register if not already present.
         * 
         * @param content 
         */
        void save(std::shared_ptr<nlohmann::json> content, std::string filename);

        /**
         * @brief Delete a backup file from the backup
         * 
         * @param filename 
         */
        void delete_save(std::string filename);

        /**
         * @brief Puts in a vector all the objets of type Content saved in the save_folder directory.
         * 
         * @return std::vector<std::shared_ptr<Content>> the generated Content objects
         */
        std::vector<std::shared_ptr<Content>> load_backup();
};

template <typename Content>
const std::string JsonBackup<Content>::BACKUP_FOLDER = "./backup";

template <typename Content>
JsonBackup<Content>::JsonBackup(const std::string save_folder): save_folder(save_folder) {
    if (!std::filesystem::exists(JsonBackup::BACKUP_FOLDER)) {
        std::filesystem::create_directory(JsonBackup::BACKUP_FOLDER);
    }
    if (!std::filesystem::exists(JsonBackup::BACKUP_FOLDER+save_folder)) {
        std::filesystem::create_directory(JsonBackup::BACKUP_FOLDER+save_folder);
    }
}

template <typename Content>
void JsonBackup<Content>::save(std::shared_ptr<nlohmann::json> content, std::string filename){
    if (!std::filesystem::exists(JsonBackup::BACKUP_FOLDER)) {
        std::filesystem::create_directory(JsonBackup::BACKUP_FOLDER);
    }
    if (!std::filesystem::exists(JsonBackup::BACKUP_FOLDER+save_folder)) {
        std::filesystem::create_directory(JsonBackup::BACKUP_FOLDER+save_folder);
    }
    std::ofstream save_file(JsonBackup::BACKUP_FOLDER + this->save_folder + "/" + filename);
    if (!save_file || !save_file.is_open()) {
        std::cerr << "Unable to open backup file" << std::endl;
        return;
    }
    save_file << (*content).dump(2);
    save_file.close();
}

template <typename Content>
std::vector<std::shared_ptr<Content>> JsonBackup<Content>::load_backup(){
    if (!std::filesystem::exists(JsonBackup::BACKUP_FOLDER)) {
        std::filesystem::create_directory(JsonBackup::BACKUP_FOLDER);
    }
    if (!std::filesystem::exists(JsonBackup::BACKUP_FOLDER+save_folder)) {
        std::filesystem::create_directory(JsonBackup::BACKUP_FOLDER+save_folder);
    }
    std::vector<std::shared_ptr<Content>> content;
    for (const auto& entry : std::filesystem::directory_iterator(JsonBackup::BACKUP_FOLDER + this->save_folder)) {
        if (entry.is_regular_file()) {
            std::ifstream curr_save(entry.path());
            nlohmann::json curr_save_json;
            curr_save >> curr_save_json;
            std::shared_ptr<Content> retrieved = std::make_shared<Content>();
            retrieved->from_json(curr_save_json);
            content.push_back(retrieved);
            curr_save.close();
        }
    }
    return content;
}

template <typename Content>
void JsonBackup<Content>::delete_save(std::string filename){
    std::filesystem::remove(JsonBackup::BACKUP_FOLDER + this->save_folder + "/" + filename);
}

#endif
