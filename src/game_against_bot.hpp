#ifndef GAME_BOT_HPP
#define GAME_BOT_HPP

#include "game.hpp"
#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>

class GameAgainstBot : public Game {
    private: 
        void update_bot_possible_moves(std::vector<std::pair<unsigned int, unsigned int>>& bot_accessible_moves);

        void update_player_grid_position(const std::shared_ptr<Position>& new_position);

        void bot_action();
    public:
        GameAgainstBot( std::shared_ptr<dpp::snowflake> host_id, 
                        std::shared_ptr<dpp::snowflake> opponent_id,
                        std::shared_ptr<dpp::message> game_message);

        GameAgainstBot();

        void update_game(const std::shared_ptr<Position>& new_position, const dpp::snowflake& player_id) override;

        void update_game_over() override;

        void resign(const dpp::snowflake& resigner_id) override;

        bool is_action_allowed(const dpp::snowflake& player_id) override;

        bool is_known_player(const dpp::snowflake& player_id) override;
};

#endif