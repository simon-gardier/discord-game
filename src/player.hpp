#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "game_grid.hpp"
#include "json_backup.hpp"
#include <dpp/dpp.h>
#include <memory>

class Player : public JsonContent<Player>{
    public:
        /**
         * @brief Construct a new Player object
         * 
         * @param id
         * @param position 
         * @param content content representing the player on the game grid
         */
        Player(std::shared_ptr<dpp::snowflake> id, std::shared_ptr<Position> position, GameGridCell::Content content);
        Player();

        /**
         * @brief Set the opponent, used to change the game turn in the Game classes.
         * 
         * @param opponent 
         * @return std::shared_ptr<Player> 
         */
        std::shared_ptr<Player> set_opponent(std::shared_ptr<Player> opponent);

        /**
         * @brief Get the opponent of the player
         * 
         * @return std::shared_ptr<Player> 
         */
        std::shared_ptr<Player> get_opponent();

        /**
         * @brief Get the id of the player (discord user id)
         * 
         * @return std::shared_ptr<dpp::snowflake> 
         */
        std::shared_ptr<dpp::snowflake> get_id();

        /**
         * @brief Get the position of the player on the grid, (0, 0) when starting the game
         * 
         * @return std::shared_ptr<Position> 
         */
        std::shared_ptr<Position> get_position();

        /**
         * @brief Get the content representing the player on the game grid
         * 
         * @return GameGridCell::Content 
         */
        GameGridCell::Content get_content();

        /**
         * @brief Overload == for comparaison
         * 
         * @param other 
         * @return true 
         * @return false 
         */
        bool operator==(const Player& other) const;

        std::shared_ptr<nlohmann::json> to_json() override;
        Player& from_json(const nlohmann::json& file) override;
    private:
        GameGridCell::Content content;
        std::shared_ptr<Player> opponent;
        std::shared_ptr<dpp::snowflake> id;
        std::shared_ptr<Position> position;
};
#endif
