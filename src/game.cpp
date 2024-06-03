#include "game.hpp"
#include "game_grid.hpp"
#include "entrapment_bot.hpp"
#include <dpp/dpp.h>
#include <memory>
#include <vector>
#include <string>

Game::Game( std::shared_ptr<dpp::snowflake> host_id, 
            std::shared_ptr<dpp::snowflake> opponent_id, 
            std::shared_ptr<dpp::message> game_message):
    first_turn(true),
    finished(false),
    grid(std::make_shared<GameGrid>()),
    game_message(game_message){
    host = std::make_shared<Player>(host_id, std::make_shared<Position>(0, 0), GameGridCell::Content::HOST);
    opponent = std::make_shared<Player>(opponent_id, std::make_shared<Position>(0, 0), GameGridCell::Content::OPPONENT);
    this->host->set_opponent(this->opponent);
    this->opponent->set_opponent(this->host);
    this->current_player = host;
    this->game_id = game_message->id.str();
}

Game::Game(){}

bool Game::is_finished(){
    return this->finished;
}

const std::shared_ptr<Player> Game::get_winner(){
    return this->winner;
}

const std::shared_ptr<Player> Game::get_current_player(){
    return this->current_player;
}

const std::shared_ptr<Player> Game::get_host(){
    return this->host;
}

const std::shared_ptr<Player> Game::get_opponent(){
    return this->opponent;
}

const dpp::snowflake Game::get_host_id(){
    return *(this->host->get_id());
}

const dpp::snowflake Game::get_opponent_id(){
    return *(this->opponent->get_id());
}

dpp::message& Game::get_game_message(){
    return *(this->game_message);
}

const std::shared_ptr<GameGridCell>& Game::cell_at(const std::shared_ptr<Position>& position){
    return this->grid->cell_at(position);
}

std::shared_ptr<nlohmann::json> Game::to_json(){
    std::shared_ptr<nlohmann::json> representation = std::make_shared<nlohmann::json>();
    (*representation)["host"] = *(this->host->to_json());
    (*representation)["opponent"] = *(this->opponent->to_json());
    (*representation)["current_player"] = *(this->current_player->to_json());
    if(this->winner != nullptr)
        (*representation)["winner"] = *(this->winner->to_json());
    (*representation)["grid"] = *(this->grid->to_json());
    (*representation)["game_message"] = this->game_message->build_json();
    (*representation)["finished"] = this->finished;
    (*representation)["first_turn"] = this->first_turn;
    (*representation)["game_id"] = this->game_id;
    return representation;
}

Game& Game::from_json(const nlohmann::json& file){
    this->host = std::make_shared<Player>((new Player())->from_json(file["host"]));
    this->opponent = std::make_shared<Player>((new Player())->from_json(file["opponent"]));
    this->host->set_opponent(this->opponent);
    this->opponent->set_opponent(this->host);
    this->current_player = std::make_shared<Player>((new Player())->from_json(file["current_player"]));
    if(*(this->current_player) == *(this->host))
        this->current_player = this->host;
    else
        this->current_player = this->opponent;
    if(file.contains("winner"))
    {
        this->winner = std::make_shared<Player>((new Player())->from_json(file["winner"]));
        this->winner == this->host ? this->winner = this->host : this->winner = this->opponent;
    }
    this->grid = std::make_shared<GameGrid>((new GameGrid())->from_json(file["grid"]));
    this->game_message = std::make_shared<dpp::message>();
    this->game_id = file["game_id"];
    std::string as_string = file["game_message"];
    nlohmann::json message_json = nlohmann::json::parse(as_string);
    this->game_message->fill_from_json(&message_json);
    this->game_message->id = dpp::snowflake(this->game_id);
    this->finished = file["finished"];
    this->first_turn = file["first_turn"];
    return *this;
}
