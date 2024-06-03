#ifndef GAME_HPP
#define GAME_HPP
#include "game_grid.hpp"
#include "player.hpp"
#include "json_backup.hpp"
#include <dpp/dpp.h>
#include <dpp/unicode_emoji.h>
#include <dpp/json_interface.h>
#include <memory>
#include <vector>
#include <string>

/**
 * @class Game
 * @brief represent an entrepment game
 */
class Game : public JsonContent<Game>{
    protected:
        bool first_turn;
        bool finished;
        std::string game_id;
        std::shared_ptr<GameGrid> grid;
        std::shared_ptr<Player> host;
        std::shared_ptr<Player> opponent;
        std::shared_ptr<Player> current_player;
        std::shared_ptr<dpp::message> game_message;
        std::shared_ptr<Player> winner;
        std::shared_ptr<JsonBackup<Game>> backup;
        virtual void update_game_over() = 0;
    public:
        /**
         * @brief construct a new Game object
         * 
         * @param host_id the id of the host
         * @param opponent_id the id of the opponent
         * @param game_message the game message to update with the game advancement
         */
        Game(   std::shared_ptr<dpp::snowflake> host_id, 
                std::shared_ptr<dpp::snowflake> opponent_id, 
                std::shared_ptr<dpp::message> game_message);
        Game();
        
        /**
         * @brief Updates the game logic, the grid and the end game state
         * 
         * @param new_position new position chosen by the player
         * @param player_id the player requestion the update
         */
        virtual void update_game(const std::shared_ptr<Position>& new_position, const dpp::snowflake& player_id) = 0;

        /**
         * @brief return true if the game is finished
         * 
         * @return true 
         * @return false 
         */
        bool is_finished();

        /**
         * @brief resign for a given player (if part of the game)
         * 
         * @param resigner_id 
         */
        virtual void resign(const dpp::snowflake& resigner_id) = 0;

        /**
         * @brief return the cell at a given position
         * 
         * @param position 
         * @return const std::shared_ptr<GameGridCell>& 
         */
        const std::shared_ptr<GameGridCell>& cell_at(const std::shared_ptr<Position>& position);

        /**
         * @brief return the host id 
         * 
         * @return const dpp::snowflake 
         */
        const dpp::snowflake get_host_id();

        /**
         * @brief get the opponent id
         * 
         * @return const dpp::snowflake 
         */
        const dpp::snowflake get_opponent_id();

        /**
         * @brief retur the game message
         * 
         * @return dpp::message& 
         */
        dpp::message& get_game_message();

        /**
         * @brief return the game winner
         * 
         * @return const std::shared_ptr<Player> 
         */
        const std::shared_ptr<Player> get_winner();

        /**
         * @brief return the current player
         * 
         * @return const std::shared_ptr<Player> 
         */
        const std::shared_ptr<Player> get_current_player();

        /**
         * @brief return the host
         * 
         * @return const std::shared_ptr<Player> 
         */
        const std::shared_ptr<Player> get_host();

        /**
         * @brief return the opponent
         * 
         * @return const std::shared_ptr<Player> 
         */
        const std::shared_ptr<Player> get_opponent();

        /**
         * @brief checks if a player can play
         * 
         * @param player_id 
         * @return true 
         * @return false 
         */
        virtual bool is_action_allowed(const dpp::snowflake& player_id) = 0;

        /**
         * @brief check if a player is part of the game
         * 
         * @param player_id 
         * @return true 
         * @return false 
         */
        virtual bool is_known_player(const dpp::snowflake& player_id) = 0;

        std::shared_ptr<nlohmann::json> to_json() override;
        Game& from_json(const nlohmann::json& file) override;
};
#endif
