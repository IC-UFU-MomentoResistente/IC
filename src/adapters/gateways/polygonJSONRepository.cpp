#include "polygonJSONRepository.h"

PolygonJSONRepository::PolygonJSONRepository(const std::string dataPathDirectory)
    : dataDirectory(dataPathDirectory), nextId(1)
{
    if (!fs::exists(dataDirectory)) 
    { 
        fs::create_directories(dataDirectory); 
    }

    // Carregar o próximo ID a partir do arquivo
    loadNextId();
}

std::string PolygonJSONRepository::generateNameArchive(const std::string& name) const
{
    std::string cleanName = name;
    std::replace_if(cleanName.begin(), cleanName.end(), [](char c) { return !isalnum(c) && c != '_'; }, '_');
    return (dataDirectory / (cleanName + ".json")).string();
}

std::string PolygonJSONRepository::generateNameArchive(uint64_t id) const
{
    return (dataDirectory / (std::to_string(id) + ".json")).string();
}

void PolygonJSONRepository::loadNextId()
{
    uint64_t maxId = 0;
    if (fs::exists(dataDirectory) && fs::is_directory(dataDirectory))
    {
        for (const auto& entry : fs::directory_iterator(dataDirectory))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                try
                {
                    std::ifstream ifs(entry.path());
                    if (ifs.is_open())
                    {
                        cereal::JSONInputArchive archive(ifs);
                        Polygon tempPolygon;
                        archive(tempPolygon);

                        if (tempPolygon.id > maxId)
                        {
                            maxId = tempPolygon.id;
                        }
                    }
                }
                catch (const cereal::Exception& e)
                {
                    std::cerr << "Erro ao tentar ler o ID do arquivo" << entry.path() << ": " << e.what() << '\n';
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Erro geral ao ler o ID do arquivo" << entry.path() << ": " << e.what() << '\n';
                }
            }
        }
    }
    nextId = maxId + 1;
}

void PolygonJSONRepository::save(Polygon& polygon)
{
    if (polygon.id == 0)
    {
        polygon.id = nextId++;
    }
    else if (polygon.id >= nextId)
    {
        nextId = polygon.id + 1;
    }

    std::string pathArchive = generateNameArchive(polygon.name);
    std::ofstream ofs(pathArchive);

    if (!ofs.is_open())
    {
        throw std::runtime_error("Não foi possível abrir o arquivo para escrita: " + pathArchive);
    }

    try
    {
        cereal::JSONOutputArchive archive(ofs);
        archive(cereal::make_nvp("polygon", polygon));
    }
    catch(const cereal::Exception& e)
    {
        throw std::runtime_error(std::string("Erro de serialização Cereal ao salvar: ") + e.what());
    }
}

std::optional<Polygon> PolygonJSONRepository::loadByName(const std::string& name)
{
    std::string pathArchive = generateNameArchive(name);

    if (!fs::exists(pathArchive))
    {
        return std::nullopt;
    }

    std::ifstream ifs(pathArchive);

    if (!ifs.is_open())
    {
        return std::nullopt;
    }

    try
    {
        cereal::JSONInputArchive archive(ifs);
        Polygon polygon;
        archive(cereal::make_nvp("polygon", polygon));
        return polygon;
    }
    catch(const cereal::Exception& e)
    {
        std::cerr << "Erro de desserialização Cereal ao carregar '" << name << "': " << e.what() << '\n';
        return std::nullopt;
    }
}

std::optional<Polygon> PolygonJSONRepository::loadById(uint64_t id)
{
    if (fs::exists(dataDirectory) && fs::is_directory(dataDirectory))
    {
        for (const auto& entry : fs::directory_iterator(dataDirectory))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".json")
            {
                try
                {
                    std::ifstream ifs(entry.path());
                    if (!ifs.is_open()) continue;

                    cereal::JSONInputArchive archive(ifs);
                    Polygon polygon;
                    archive(cereal::make_nvp("polygon", polygon));
                    if (polygon.id == id)
                    {
                        return polygon;
                    }
                }
                catch(const cereal::Exception& e)
                {
                    std::cerr << "Aviso: Erro ao tentar ler o arquivo " << entry.path() 
                    << " para encontrar ID " << id << ": " << e.what() << '\n';
                }   
            }
        }
    }
    return std::nullopt;
}

std::vector<std::string> PolygonJSONRepository::listSavedSectionNames()
{
    std::vector<std::string> names;

    if (fs::exists(dataDirectory) && fs::is_directory(dataDirectory))
    {
        for (const auto& entry : fs::directory_iterator(dataDirectory))
        {
            try
            {
                std::ifstream ifs(entry.path());
                if (!ifs.is_open()) continue;

                cereal::JSONInputArchive archive(ifs);
                Polygon polygon;
                archive(cereal::make_nvp("polygon", polygon));
                names.push_back(polygon.name);
            }
            catch(const cereal::Exception& e)
            {
                std::cerr << "Aviso: não foi possivel ler o nome da seção do arquivo" 
                << entry.path().filename().string() << " - " << e.what() << '\n';
            }
            
        }
    }
    return names;
}