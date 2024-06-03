#include "player.hpp"

Player::Player(std::shared_ptr<dpp::snowflake> id, std::shared_ptr<Position> position,  GameGridCell::Content content):
    content(content),
    id(id),
    position(position){}

Player::Player(){}

std::shared_ptr<Player> Player::set_opponent(std::shared_ptr<Player> opponent){
    return this->opponent = opponent;
}

std::shared_ptr<Player> Player::get_opponent(){
    return this->opponent;
}

std::shared_ptr<dpp::snowflake> Player::get_id(){
    return this->id;
}

std::shared_ptr<Position> Player::get_position(){
    return this->position;
}

bool Player::operator==(const Player& other) const {
    return *(this->position) == *(other.position) && *(this->id) == *(other.id);
}

GameGridCell::Content Player::get_content(){
    return this->content;
}

std::shared_ptr<nlohmann::json> Player::to_json(){
    std::shared_ptr<nlohmann::json> representation = std::make_shared<nlohmann::json>();
    (*representation)["id"] = *(this->id);
    (*representation)["position"] = *(this->position->to_json());
    (*representation)["content"] = this->content;
    return representation;
}

Player& Player::from_json(const nlohmann::json& file){
    std::string id = file["id"];
    this->id = std::make_shared<dpp::snowflake>(id);
    this->position = std::make_shared<Position>((new Position())->from_json(file["position"]));
    this->content = file["content"].get<GameGridCell::Content>();
    return *this;
}
