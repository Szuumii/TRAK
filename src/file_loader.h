#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

const std::string MODELS_PATH_POSTFIX = "/assets/models";
const std::string SKYBOXES_PATH_POSTFIX = "/assets/textures/skyboxes";
const std::string DEFAULT_MODEL_PATH = "";
const std::string DEFAULT_SKYBOX_PATH = "/assets/models/";

class FileLoader
{
public:
    std::string projectRoot;
    std::string modelsPath;
    std::string skyboxesPath;

    std::string chosenObjectPath;
    std::string chosenSkyboxPath;

    FileLoader(std::string root)
    {
        this->projectRoot = root;
        this->modelsPath = root + MODELS_PATH_POSTFIX;
        this->skyboxesPath = root + SKYBOXES_PATH_POSTFIX;

        this->chosenObjectPath = this->modelsPath + "/tree.obj";
        this->chosenSkyboxPath = this->skyboxesPath + "/mountains";
    }

    std::string choosePathFromDir(std::string dir)
    {
        std::string path = dir;
        int index = 0;
        std::vector<std::string> filepaths;

        for (const auto &entry : fs::directory_iterator(path))
        {
            std::string file_path = std::string(entry.path());
            std::size_t found = file_path.find_last_of("/\\");
            std::string file_name = file_path.substr(found + 1);

            if (file_name == ".DS_Store")
                continue;

            filepaths.push_back(file_path);

            std::cout << index++ << ": " << file_name << std::endl;
        }
        std::cout << "Type chosen index: ";
        int chosen_index = -1;
        while (chosen_index < 0 || chosen_index > filepaths.size() - 1)
        {
            std::cout << "Please choose number between 0 and " << filepaths.size() - 1 << ": ";
            std::cin >> chosen_index;
        }

        return filepaths.at(chosen_index);
    }

    std::string chooseObject()
    {
        std::cout << "Choose index of available objects to load:" << std::endl;
        std::string path = choosePathFromDir(this->modelsPath);

        std::cout << "Loading object from " << path << "..." << std::endl;

        this->chosenObjectPath = path;

        return path;
    }

    std::string chooseSkybox()
    {
        std::cout << "Choose index of available skyboxes to load:" << std::endl;
        std::string path = choosePathFromDir(this->skyboxesPath);

        std::cout << "Loading skybox from " << path << "..." << std::endl;

        this->chosenSkyboxPath = path;

        return path;
    }
};
