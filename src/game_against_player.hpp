#ifndef GAME_PLAYER_HPP
#define GAME_PLAYER_HPP

#include "game.hpp"

/**
 * @class GameAgainstPlayer
 * @brief Represent a Game instance against a player 
 */
class GameAgainstPlayer : public Game {
    protected: 
        void update_game_over() override;

    public:
        GameAgainstPlayer(  std::shared_ptr<dpp::snowflake> host_id, 
                            std::shared_ptr<dpp::snowflake> opponent_id, 
                            std::shared_ptr<dpp::message> game_message);

        GameAgainstPlayer();

        void update_game(const std::shared_ptr<Position>& new_position, const dpp::snowflake& player_id) override;

        void resign(const dpp::snowflake& resigner_id) override;

        bool is_action_allowed(const dpp::snowflake& player_id) override;

        bool is_known_player(const dpp::snowflake& player_id) override;
};

#endif