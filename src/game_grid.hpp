#ifndef GAME_GRID_HPP
#define GAME_GRID_HPP
#include <memory>
#include "json_backup.hpp"

/**
 * @brief Cell of the grid
 * 
 */
class GameGridCell : public JsonContent<GameGridCell>{
    public:
        enum Content {HOST, OPPONENT, BOOM, EMPTY};
        /**
         * @brief construct a new Game Grid Cell object
         */
        GameGridCell();

        /**
         * @brief return true if the cell is disabled
         */
        bool is_disabled();

        /**
         * @brief disable the cell
         */
        void disable();

        /**
         * @brief enable the cell
         */
        void enable();

        /**
         * @brief set the content of the cell
         * 
         * @param new_content 
         */
        void set_content(GameGridCell::Content new_content);

        /**
         * @brief return the content of the cell
         * 
         * @return GameGridCell::Content 
         */
        GameGridCell::Content get_content();

        std::shared_ptr<nlohmann::json> to_json() override;
        GameGridCell& from_json(const nlohmann::json& file) override;

    private:
        bool disabled;
        GameGridCell::Content content;
};

/**
 * @brief position util class
 */
class Position : public JsonContent<Position>{
    public:
        int col;
        int row;
        Position(int row, int col);
        Position();
        bool operator==(const Position& other) const;
        std::shared_ptr<nlohmann::json> to_json() override;
        Position& from_json(const nlohmann::json& file) override;
};

/**
 * @brief The grid of a game, is updated by the Game object
 * 
 */
class GameGrid : public JsonContent<GameGrid>{
    public:
        static constexpr const int GRID_SIZE = 5;
        
        /**
         * @brief check if position is in bound of the grid
         * 
         * @param position 
         * @return true 
         * @return false 
         */
        static bool is_position_in_bound(const std::shared_ptr<Position>& position);

        /**
         * @brief construct a new Game Grid object
         */
        GameGrid();

        /**
         * @brief set to enable all the cells in a direction given by row_change and col_change
         * 
         * @param row 
         * @param col 
         * @param row_change 
         * @param col_change 
         */
        void update_reachable_cells_in_direction(int row, int col, const int row_change, const int col_change);

        /**
         * @brief disable all the grid
         */
        void disable_grid();

        /**
         * @brief enable all the grid
         */
        void enable_grid();

        /**
         * @brief returns the cell present at a given position
         * 
         * @param position 
         * @return const std::shared_ptr<GameGridCell>& 
         */
        const std::shared_ptr<GameGridCell>& cell_at(const std::shared_ptr<Position>& position);

        /**
         * @brief checks if all the cells around a position are not empty
         * 
         * @param position 
         * @return true if the position is surrounded
         * @return false 
         */
        bool is_stuck(std::shared_ptr<Position> position);

        /**
         * @brief enable all empty cells around a given position
         * 
         * @param current_player_position 
         */
        void update_reachable_cells(std::shared_ptr<Position> current_player_position);

        std::shared_ptr<nlohmann::json> to_json() override;
        GameGrid& from_json(const nlohmann::json& file) override;
    private:
        // the grid itself
        std::shared_ptr<GameGridCell> grid[GRID_SIZE][GRID_SIZE];
};
#endif
