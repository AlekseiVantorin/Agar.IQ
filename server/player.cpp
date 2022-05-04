#include "player.h"

Player::Player(QString name, double x, double y, double rad, int id) : Entity(x, y, rad, id) {
    player_name = name.toStdString();

    type = "player";
}
