#ifndef ENTRAPMENT_BOT_HPP
#define ENTRAPMENT_BOT_HPP

#include "game.hpp"
#include "game_grid.hpp"
#include "game_against_player.hpp"
#include "game_against_bot.hpp"
#include "json_backup.hpp"
#include <dpp/dpp.h>
#include <dpp/unicode_emoji.h>
#include <unordered_map>
#include <string>
#include <vector>

/**
 * @class main class
 */
class EntrapmentBot {
private:
    // Emojis
    static constexpr const char* HOST_EMOJI = dpp::unicode_emoji::dog;
    static constexpr const char* OPPONENT_EMOJI = dpp::unicode_emoji::cat; 
    static constexpr const char* EMPTY_EMOJI = dpp::unicode_emoji::red_circle; 
    static constexpr const char* BOOM_EMOJI = dpp::unicode_emoji::boom;
    static constexpr const char* TROPHY_EMOJI = dpp::unicode_emoji::trophy;
    static constexpr const char* RESIGN_EMOJI = dpp::unicode_emoji::x;
    // Games map
    std::unordered_map<dpp::snowflake, std::shared_ptr<GameAgainstPlayer>> players_games;
    std::unordered_map<dpp::snowflake, std::shared_ptr<GameAgainstBot>> bot_games;
    // Games backups
    std::shared_ptr<JsonBackup<GameAgainstPlayer>> players_games_backup;
    std::shared_ptr<JsonBackup<GameAgainstBot>> bot_games_backup;
    // Bot itself
    std::shared_ptr<dpp::cluster> bot;

    /**
     * @brief Construct a new Entrapment Bot object
     * 
     * @param bot_token 
     */
    EntrapmentBot(std::string bot_token);

    /**
     * @brief register the logs in the terminal
     */
    void add_logs();

    /**
     * @brief adds a new command
     */
    void add_trap_command();

    /**
     * @brief on trap command behavior, initialize the game logic and the game message
     */
    void add_on_trap_command_behavior();

    /**
     * @brief on click behavior, update the game logic and the game message
     * 
     */
    void add_on_click_behavior();

    /**
     * @brief on reaction behavior, ends the game when conditions meet
     */
    void add_on_reaction_behavior();

    /**
     * @brief update the message (content and components) of a given game
     * 
     * @param game the game from which update the message 
     */
    void update_game_message(std::shared_ptr<Game> game);
    
    /**
     * @brief update the content of a game message
     * 
     * @param game the game from which update the content
     */
    void update_game_message_content(std::shared_ptr<Game> game);

    /**
     * @brief update the components of a game message
     * 
     * @param game the game from which update the components
     */
    void update_game_message_components(std::shared_ptr<Game> game);

    /**
     * @brief get the position from button
     * 
     * @param button_id the id of the button
     * @return const std::shared_ptr<Position> 
     */
    const std::shared_ptr<Position> get_position_from_button(const std::string& button_id);

    /**
     * @brief creates an id representing a game id + a position in the grid
     * 
     * @param row 
     * @param col 
     * @param game_id 
     * @return const std::string 
     */
    const std::string generate_button_id(int row, int col, const dpp::snowflake& game_id);

    /**
     * @brief get the game id from button object
     * 
     * @param button_id 
     * @return const std::shared_ptr<dpp::snowflake> 
     */
    const std::shared_ptr<dpp::snowflake> get_game_id_from_button(const std::string& button_id);

public:
    /**
     * @brief returns the bot instance
     * 
     * @param bot_token 
     * @return std::shared_ptr<EntrapmentBot>& 
     */
    static std::shared_ptr<EntrapmentBot>& get_instance(std::string bot_token);

    /**
     * @brief must be called to start the game
     * 
     */
    void start();
};

#endif