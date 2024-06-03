#include "game_against_player.hpp"
#include "game.hpp"

GameAgainstPlayer::GameAgainstPlayer(   std::shared_ptr<dpp::snowflake> host_id, 
                                        std::shared_ptr<dpp::snowflake> opponent_id, 
                                        std::shared_ptr<dpp::message> game_message): 
                                        Game(host_id, opponent_id, game_message){}

GameAgainstPlayer::GameAgainstPlayer(): Game(){}

void GameAgainstPlayer::update_game(const std::shared_ptr<Position>& new_position, const dpp::snowflake& player_id){
    if( !this->grid->is_position_in_bound(new_position) ||
        !this->is_action_allowed(player_id) ||
        this->grid->cell_at(new_position)->get_content() != GameGridCell::EMPTY ||
        this->grid->cell_at(new_position)->is_disabled()){
        return;
    }
    if(!first_turn)
        this->grid->cell_at(this->current_player->get_position())->set_content(GameGridCell::BOOM);
    this->grid->cell_at(new_position)->set_content(this->current_player->get_content());
    this->current_player->get_position()->row = new_position->row;
    this->current_player->get_position()->col = new_position->col;
    this->update_game_over();
    if(!this->finished){
        if(this->first_turn){
            if(*(this->current_player) == *(this->host)){
                this->grid->enable_grid();
                this->grid->cell_at(this->host->get_position())->disable();
            }
            else{
                first_turn = false;
                this->grid->update_reachable_cells(this->current_player->get_opponent()->get_position());
            }
        }
        else{
            this->grid->update_reachable_cells(this->current_player->get_opponent()->get_position());
        }
    }
    this->current_player = this->current_player->get_opponent();
}

void GameAgainstPlayer::update_game_over(){
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

void GameAgainstPlayer::resign(const dpp::snowflake& resigner_id){
    this->finished = true;
    this->grid->disable_grid();
    if(resigner_id == *(this->host->get_id())){
        this->winner = this->opponent;
    }
    else if(resigner_id == *(this->opponent->get_id())){
        this->winner = this->host;
    }
}

bool GameAgainstPlayer::is_action_allowed(const dpp::snowflake& player_id){
    return ((*(this->host->get_id()) == player_id && this->current_player == this->host) || (*(this->opponent->get_id()) == player_id && this->current_player == this->opponent));
}

bool GameAgainstPlayer::is_known_player(const dpp::snowflake& player_id){
    return (*(this->host->get_id()) == player_id || *(this->opponent->get_id()) == player_id);
}

