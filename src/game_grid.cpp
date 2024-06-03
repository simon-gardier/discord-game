#include "game_grid.hpp"
#include <memory>
#include <iostream>

/**
 * @class GameGridCell
 */
GameGridCell::GameGridCell():disabled(false), content(GameGridCell::EMPTY){}

bool GameGridCell::is_disabled(){
    return this->disabled;
}

void GameGridCell::disable(){
    this->disabled = true;
}

void GameGridCell::enable(){
    this->disabled = false;
}

void GameGridCell::set_content(GameGridCell::Content new_content){
    this->content = new_content;
}

GameGridCell::Content GameGridCell::get_content(){
    return this->content;
}

std::shared_ptr<nlohmann::json> GameGridCell::to_json(){
    std::shared_ptr<nlohmann::json> representation = std::make_shared<nlohmann::json>();
    (*representation)["disabled"] = this->disabled;
    (*representation)["content"] = this->content;
    return representation;
}

GameGridCell& GameGridCell::from_json(const nlohmann::json& file){
    this->disabled = file["disabled"];
    this->content = file["content"];
    return *this;
}

/**
 * @class Position
 */

Position::Position(int row, int col):col(col), row(row){}

Position::Position(){}

bool Position::operator==(const Position& other) const{
    return this->col == other.col && this->row == other.row;
}

std::shared_ptr<nlohmann::json> Position::to_json(){
    std::shared_ptr<nlohmann::json> representation = std::make_shared<nlohmann::json>();
    (*representation)["row"] = this->row;
    (*representation)["col"] = this->col;
    return representation;
}

Position& Position::from_json(const nlohmann::json& file){
    this->row = file["row"];
    this->col = file["col"];
    return *this;
}

/**
 * @class GameGrid
 */

GameGrid::GameGrid(){
    for (int i = 0; i < GRID_SIZE; ++i){
        for (int j = 0; j < GRID_SIZE; ++j){
            grid[i][j] = std::make_shared<GameGridCell>();
        }
    }
}

bool GameGrid::is_position_in_bound(const std::shared_ptr<Position>& position){
    return position->row >= 0 && position->col >= 0 && position->row < GRID_SIZE && position->col < GRID_SIZE;
}

void GameGrid::update_reachable_cells_in_direction(int row, int col, const int row_change, const int col_change){
    while (row >= 0 && row < GameGrid::GRID_SIZE && col >= 0 && col < GameGrid::GRID_SIZE){
        if (this->grid[row][col]->get_content() != GameGridCell::EMPTY)
            break;
        this->grid[row][col]->enable();
        row += row_change;
        col += col_change;
    }
}

void GameGrid::update_reachable_cells(std::shared_ptr<Position> current_player_position){
    this->disable_grid();
    update_reachable_cells_in_direction(current_player_position->row-1, current_player_position->col-1, -1, -1);
    update_reachable_cells_in_direction(current_player_position->row-1, current_player_position->col, -1, 0);
    update_reachable_cells_in_direction(current_player_position->row-1, current_player_position->col+1, -1, 1);
    update_reachable_cells_in_direction(current_player_position->row, current_player_position->col+1, 0, 1);
    update_reachable_cells_in_direction(current_player_position->row+1, current_player_position->col+1, 1, 1);
    update_reachable_cells_in_direction(current_player_position->row+1, current_player_position->col, 1, 0);
    update_reachable_cells_in_direction(current_player_position->row+1, current_player_position->col-1, 1, -1);
    update_reachable_cells_in_direction(current_player_position->row, current_player_position->col-1, 0, -1);
}

bool GameGrid::is_stuck(std::shared_ptr<Position> position){
    if(position->row-1 >= 0 && position->col-1 >= 0 && grid[position->row-1][position->col-1]->get_content() == GameGridCell::EMPTY)
        return false;
    else if(position->row-1 >= 0 && grid[position->row-1][position->col]->get_content() == GameGridCell::EMPTY)
        return false;
    else if(position->row-1 >= 0 && position->col+1 < GRID_SIZE && grid[position->row-1][position->col+1]->get_content() == GameGridCell::EMPTY)
        return false;
    else if(position->col-1 >= 0 && grid[position->row][position->col-1]->get_content() == GameGridCell::EMPTY)
        return false;
    else if(position->col+1 < GRID_SIZE && grid[position->row][position->col+1]->get_content() == GameGridCell::EMPTY)
        return false;
    else if(position->row+1 < GRID_SIZE && position->col-1 >= 0 && grid[position->row+1][position->col-1]->get_content() == GameGridCell::EMPTY)
        return false;
    else if(position->row+1 < GRID_SIZE && grid[position->row+1][position->col]->get_content() == GameGridCell::EMPTY)
        return false;
    else if(position->row+1 < GRID_SIZE && position->col+1 < GRID_SIZE && grid[position->row+1][position->col+1]->get_content() == GameGridCell::EMPTY)
        return false;
    return true;
}

void GameGrid::disable_grid(){
    for(int i = 0; i < GameGrid::GRID_SIZE; i++){
        for(int j = 0; j < GameGrid::GRID_SIZE; j++){
            this->grid[i][j]->disable();
        }
    }
}

void GameGrid::enable_grid(){
    for(int i = 0; i < GameGrid::GRID_SIZE; i++){
        for(int j = 0; j < GameGrid::GRID_SIZE; j++){
            this->grid[i][j]->enable();
        }
    }
}

const std::shared_ptr<GameGridCell>& GameGrid::cell_at(const std::shared_ptr<Position>& position){
    return this->grid[position->row][position->col];
}

std::shared_ptr<nlohmann::json> GameGrid::to_json(){
    std::shared_ptr<nlohmann::json> representation = std::make_shared<nlohmann::json>();
    for (int i = 0; i < GRID_SIZE; ++i){
        for (int j = 0; j < GRID_SIZE; ++j){
            (*representation)[std::to_string(i)][std::to_string(j)] = *(this->grid[i][j]->to_json());
        }
    }
    return representation;
}

GameGrid& GameGrid::from_json(const nlohmann::json& file){

    for (int i = 0; i < GRID_SIZE; ++i){
        for (int j = 0; j < GRID_SIZE; ++j){
            this->grid[i][j]->from_json(file[std::to_string(i)][std::to_string(j)]);
        }
    }
    return *this;
}
