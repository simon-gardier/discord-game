#include "game_against_bot.hpp"
#include "game.hpp"
#include "player.hpp"

/**
 * @class GameAgainstBot
 */
GameAgainstBot::GameAgainstBot( std::shared_ptr<dpp::snowflake> host_id, 
                                std::shared_ptr<dpp::snowflake> opponent_id, 
                                std::shared_ptr<dpp::message> game_message):
                                Game(host_id, opponent_id, game_message){}

GameAgainstBot::GameAgainstBot(){}

void GameAgainstBot::update_game(const std::shared_ptr<Position>& new_position, const dpp::snowflake& player_id){
    if( !this->grid->is_position_in_bound(new_position) ||
        !this->is_action_allowed(player_id) ||
        this->grid->cell_at(new_position)->get_content() != GameGridCell::EMPTY ||
        this->grid->cell_at(new_position)->is_disabled()){
        return;
    }
    if(!first_turn)
        this->grid->cell_at(this->current_player->get_position())->set_content(GameGridCell::BOOM);
    update_player_grid_position(new_position);
    this->update_game_over();
    if(!this->finished){
        if(this->first_turn){
            if(*(this->current_player) == *(this->host)){
                this->grid->enable_grid();
                this->grid->cell_at(this->host->get_position())->disable();
            }
        }else{
            this->grid->update_reachable_cells(this->current_player->get_opponent()->get_position());
        }
        this->bot_action();
    }
}

void GameAgainstBot::bot_action(){
    // Bot moves after player has played
    this->current_player = this->current_player->get_opponent();

    // Get a random cell amongst all the accessible cells for the bot
    std::vector<std::pair<unsigned int, unsigned int>> bot_accessible_moves;
    update_bot_possible_moves(bot_accessible_moves);
    std::pair<unsigned int, unsigned int> random_cell = bot_accessible_moves.back();

    if(!first_turn)
        this->grid->cell_at(this->current_player->get_position())->set_content(GameGridCell::BOOM);
    update_player_grid_position(std::make_shared<Position>(random_cell.first, random_cell.second));
    this->update_game_over();
    if(!this->finished){
        if(this->first_turn){
                first_turn = false;
                this->grid->update_reachable_cells(this->current_player->get_opponent()->get_position());
        }else{    
            this->grid->update_reachable_cells(this->current_player->get_opponent()->get_position());
        }
    }
    this->current_player = this->current_player->get_opponent();
}

void GameAgainstBot::update_game_over(){
    if(this->first_turn)
        return;
    if(this->grid->is_stuck(this->current_player->get_opponent()->get_position())){
        this->winner = this->current_player;
        this->finished = true;
        this->grid->disable_grid();
    }
    else if(this->grid->is_stuck(this->current_player->get_position())){
        this->winner = this->get_current_player()->get_opponent();
        this->finished = true;
        this->grid->disable_grid();
    }
}

void GameAgainstBot::resign(const dpp::snowflake& resigner_id){
    if(!this->is_known_player(resigner_id))
        return;
    this->finished = true;
    this->grid->disable_grid();
    this->winner = this->opponent;
}

bool GameAgainstBot::is_action_allowed(const dpp::snowflake& player_id){
    return (*(this->host->get_id()) == player_id && this->current_player == this->host);
}

bool GameAgainstBot::is_known_player(const dpp::snowflake& player_id){
    return (*(this->host->get_id()) == player_id);
}

void GameAgainstBot::update_bot_possible_moves(std::vector<std::pair<unsigned int, unsigned int>>& bot_accessible_moves){
    for(unsigned int row = 0; row < 5; ++row){
        for(unsigned int column = 0; column < 5; ++column){
            if(!this->grid->cell_at(std::make_shared<Position>(row, column))->is_disabled())
                bot_accessible_moves.push_back(std::pair(row, column));
        }
    }
    auto rng = std::default_random_engine {};
    std::shuffle(bot_accessible_moves.begin(), bot_accessible_moves.end(), rng);
}

void GameAgainstBot::update_player_grid_position(const std::shared_ptr<Position>& new_position){
    this->grid->cell_at(new_position)->set_content(this->current_player->get_content());
    this->current_player->get_position()->row = new_position->row;
    this->current_player->get_position()->col = new_position->col;
}