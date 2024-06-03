#include "game.hpp"
#include "game_grid.hpp"
#include "game_against_player.hpp"
#include "game_against_bot.hpp"
#include "entrapment_bot.hpp"
#include <dpp/dpp.h>
#include <dpp/unicode_emoji.h>
#include <unordered_map>
#include <string>
#include <vector>

EntrapmentBot::EntrapmentBot(std::string bot_token):
    players_games(std::unordered_map<dpp::snowflake, std::shared_ptr<GameAgainstPlayer>>()),
    bot_games(std::unordered_map<dpp::snowflake, std::shared_ptr<GameAgainstBot>>()),
    players_games_backup(std::make_shared<JsonBackup<GameAgainstPlayer>>("/players_games")),
    bot_games_backup(std::make_shared<JsonBackup<GameAgainstBot>>("/bot_games")),
    bot(std::make_shared<dpp::cluster>(bot_token)){

    this->add_logs();
    this->add_trap_command();
    this->add_on_trap_command_behavior();
    this->add_on_click_behavior();
    this->add_on_reaction_behavior();
    std::vector<std::shared_ptr<GameAgainstBot>> bot_saves = this->bot_games_backup->load_backup();
    for(std::shared_ptr<GameAgainstBot> save : bot_saves){
        this->bot_games[save->get_game_message().id] = save;
    }
    std::vector<std::shared_ptr<GameAgainstPlayer>> player_saves = this->players_games_backup->load_backup();
    for(std::shared_ptr<GameAgainstPlayer> save : player_saves){
        this->players_games[save->get_game_message().id] = save;
    }
}

void EntrapmentBot::add_logs(){
    this->bot->on_log(dpp::utility::cout_logger());
}

void EntrapmentBot::add_trap_command(){
    this->bot->on_ready([this](const dpp::ready_t &event){
        (void)event;
        if (dpp::run_once<struct register_bot_commands>()){
            dpp::slashcommand trap_command("trap", "Start a game!", bot->me.id);
            trap_command.add_option(dpp::command_option(dpp::co_user, "opponent", "Opponent", true));
            bot->global_command_create({trap_command});
        }
    });
}

void EntrapmentBot::add_on_trap_command_behavior(){
    this->bot->on_slashcommand([this](const dpp::slashcommand_t &event){
        if (event.command.get_command_name() == "trap"){
            // Get users ID
            std::shared_ptr<dpp::snowflake> host_id = std::make_shared<dpp::snowflake>(event.command.get_issuing_user().id);
            std::string host = event.command.get_issuing_user().get_mention();
            std::shared_ptr<dpp::snowflake> opponent_id = std::make_shared<dpp::snowflake>(std::get<dpp::snowflake>(event.get_parameter("opponent")));
            std::string opponent = event.command.get_resolved_member(*opponent_id).get_mention();
            // Send initial response
            dpp::message response = dpp::message();
            response.set_content(host + " " + EntrapmentBot::HOST_EMOJI + "vs " + opponent + EntrapmentBot::OPPONENT_EMOJI);
            event.reply(response);
            // Create game instance and send game grid
            event.get_original_response([this, host_id, opponent_id](const dpp::confirmation_callback_t& event){
                std::shared_ptr<dpp::message> response_retrieved = std::make_shared<dpp::message>(std::get<dpp::message>(event.value));
                dpp::snowflake game_id = dpp::snowflake(std::get<dpp::message>(event.value).id);
                std::shared_ptr<Game> new_game;
                if(*opponent_id == this->bot->me.id){
                    std::shared_ptr<GameAgainstBot> new_bot_game = std::make_shared<GameAgainstBot>(host_id, opponent_id, response_retrieved);
                    this->bot_games_backup->save(new_bot_game->to_json(), game_id.str()+".json");
                    this->bot_games[game_id] = new_bot_game;
                    new_game = new_bot_game;
                }
                else{
                    std::shared_ptr<GameAgainstPlayer> new_player_game = std::make_shared<GameAgainstPlayer>(host_id, opponent_id, response_retrieved);
                    this->players_games_backup->save(new_player_game->to_json(), game_id.str()+".json");
                    this->players_games[game_id] = new_player_game;
                    new_game = new_player_game;
                }
                this->update_game_message(new_game);
            });
        }
    });
}
   
void EntrapmentBot::add_on_click_behavior(){
    this->bot->on_button_click([this](const dpp::button_click_t &event){
        // Retrieve game infos
        const std::shared_ptr<Position> new_position = get_position_from_button(event.custom_id);
        std::shared_ptr<dpp::snowflake> game_id = get_game_id_from_button(event.custom_id);
        auto player_game_it = this->players_games.find(*game_id);
        auto bot_game_it = this->bot_games.find(*game_id);
        std::shared_ptr<Game> game;
        if(bot_game_it != this->bot_games.end()){
            game = bot_game_it->second;
        }
        else if (player_game_it != this->players_games.end()){
            game = player_game_it->second;
        }
        else{
            std::unique_ptr<dpp::message> error_response = std::make_unique<dpp::message>(dpp::message().set_content("Unknown match").set_flags(dpp::m_ephemeral));
            event.reply(*error_response);
            return;
        }
        if(!game->is_known_player(event.command.get_issuing_user().id)){
            std::unique_ptr<dpp::message> error_response = std::make_unique<dpp::message>(dpp::message("You arent part of this game").set_flags(dpp::m_ephemeral));
            event.reply(*error_response);
            return;
        }
        else if(!game->is_action_allowed(event.command.get_issuing_user().id)){
            std::unique_ptr<dpp::message> error_response = std::make_unique<dpp::message>(dpp::message("Its not your turn !").set_flags(dpp::m_ephemeral));
            event.reply(*error_response);
            return;
        }
        event.reply();
        game->update_game(new_position, event.command.get_issuing_user().id);
        if(bot_game_it != this->bot_games.end()){
            this->bot_games_backup->save(game->to_json(), (*game_id).str() + ".json");
        }
        else{
            this->players_games_backup->save(game->to_json(), (*game_id).str() + ".json");
        }
        this->update_game_message(game);
        if(game->is_finished()){
            if(bot_game_it != this->bot_games.end()){
                this->bot_games.erase(*game_id);
                this->bot_games_backup->delete_save((*game_id).str() + ".json");
            }
            else{
                this->players_games.erase(*game_id);
                this->players_games_backup->delete_save((*game_id).str() + ".json");
            }
        }
    });
}

void EntrapmentBot::add_on_reaction_behavior(){
    this->bot->on_message_reaction_add([this](const dpp::message_reaction_add_t &event){
        std::unique_ptr<dpp::snowflake> player_id = std::make_unique<dpp::snowflake>(event.reacting_user.id);
        std::unique_ptr<dpp::snowflake> game_id = std::make_unique<dpp::snowflake>(event.message_id);
        auto player_game_it = this->players_games.find(*game_id);
        auto bot_game_it = this->bot_games.find(*game_id);
        std::shared_ptr<Game> game;
        if(bot_game_it != this->bot_games.end()){
            game = bot_game_it->second;
        }
        else if (player_game_it != this->players_games.end()){
            game = player_game_it->second;
        }
        else{
            return;
        }
        if(!game->is_known_player(*player_id)){
            return;
        }
        if( event.reacting_emoji.format() == RESIGN_EMOJI &&
            game->is_known_player(*player_id)){
            game->resign(*player_id);
            this->update_game_message(game);
            if(bot_game_it != this->bot_games.end()){
                this->bot_games.erase(*game_id);
                this->bot_games_backup->delete_save((*game_id).str() + ".json");
            }
            else{
                this->players_games.erase(*game_id);
                this->players_games_backup->delete_save((*game_id).str() + ".json");
            }
        }
    });
}

void EntrapmentBot::update_game_message(std::shared_ptr<Game> game){
    this->update_game_message_content(game);
    this->update_game_message_components(game);
    this->bot->message_edit(game->get_game_message());
}

void EntrapmentBot::update_game_message_content(std::shared_ptr<Game> game){
    std::string new_content = dpp::user::get_mention(game->get_host_id()) + " " + EntrapmentBot::HOST_EMOJI + "vs " + dpp::user::get_mention(game->get_opponent_id()) + EntrapmentBot::OPPONENT_EMOJI + "\n";
    if(game->is_finished()){
        new_content += dpp::user::get_mention(*(game->get_winner()->get_id())) + " won! " + EntrapmentBot::TROPHY_EMOJI;
        game->get_game_message().set_content(new_content);
    }
    else{
        new_content += dpp::user::get_mention(*(game->get_current_player()->get_id())) + ", your turn!";
        game->get_game_message().set_content(new_content);
    }
}

void EntrapmentBot::update_game_message_components(std::shared_ptr<Game> game){
    game->get_game_message().components.clear();
    for(unsigned int i = 0; i < GameGrid::GRID_SIZE; ++i){
        dpp::component curr_row = dpp::component().set_type(dpp::cot_action_row);
        for(unsigned int j = 0; j < GameGrid::GRID_SIZE; ++j){
            std::shared_ptr<GameGridCell> cell = game->cell_at(std::make_shared<Position>(i, j));
            dpp::component button = dpp::component().
                                    set_type(dpp::cot_button).
                                    set_style(dpp::cos_danger).
                                    set_id(generate_button_id(i, j, game->get_game_message().id));
            if(cell->is_disabled()){
                button.set_disabled(true);
            }
            switch (cell->get_content()){
                case GameGridCell::HOST:
                    button.set_emoji(EntrapmentBot::HOST_EMOJI);
                    break;
                case GameGridCell::OPPONENT:
                    button.set_emoji(EntrapmentBot::OPPONENT_EMOJI);
                    break;
                case GameGridCell::BOOM:
                    button.set_emoji(EntrapmentBot::BOOM_EMOJI);
                    break;
                default:
                    button.set_emoji(EntrapmentBot::EMPTY_EMOJI);
                    break;
            }
            curr_row.add_component(button);
        }
        game->get_game_message().add_component(curr_row);
    }
}

const std::shared_ptr<Position> EntrapmentBot::get_position_from_button(const std::string& button_id){
    int row = stoi(button_id.substr(0, 1));
    int col = stoi(button_id.substr(1, 1));
    return std::make_shared<Position>(row, col);
}

const std::string EntrapmentBot::generate_button_id(int row, int col, const dpp::snowflake& game_id){
    return std::to_string(row) + std::to_string(col) + game_id.str();
}

const std::shared_ptr<dpp::snowflake> EntrapmentBot::get_game_id_from_button(const std::string& button_id){
    return std::make_shared<dpp::snowflake>(button_id.substr(2, button_id.length() - 2));
}

std::shared_ptr<EntrapmentBot>& EntrapmentBot::get_instance(std::string bot_token){
    static std::shared_ptr<EntrapmentBot> instance;
    if(instance == nullptr)
        instance = std::shared_ptr<EntrapmentBot>(new EntrapmentBot(bot_token));
    return instance;
}

void EntrapmentBot::start(){
    this->bot->start(dpp::st_wait);
}
