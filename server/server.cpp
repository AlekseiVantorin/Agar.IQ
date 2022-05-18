#include "server.h"

server::server() {
    if(this->listen(QHostAddress::Any, 5555)) {
        qDebug() << "Listening";
    } else {
        qDebug() << "Error";
    }
}

server::~server(){}

void to_json(json& j, const Player& p)
{
    j = {{"name", p.player_name}, {"x", p.x_coordinate}, {"y", p.y_coordinate},
         {"rad", p.radius}, {"score", p.score}, {"is_correct", p.is_correct}, {"red_color", p.red_color},
         {"blue_color", p.blue_color}, {"green_color", p.green_color} };
}

void to_json(json& j, const Answer& p)
{
    j = {{"x", p.x_coordinate}, {"y", p.y_coordinate}, {"number", p.get_number()}};
}

void to_json(json& j, const Food& p)
{
    j = {{"x", p.x_coordinate}, {"y", p.y_coordinate}, {"red_color", p.red_color},
         {"green_color", p.green_color}, {"blue_color", p.blue_color}};
}

void to_json(json& j, const std::pair<int, Food> p) {
    j = {{"food_iter", p.first}, {"x", p.second.x_coordinate}, {"y", p.second.y_coordinate}, {"red_color", p.second.red_color},
         {"green_color", p.second.green_color}, {"blue_color", p.second.blue_color}};
}

void server::incomingConnection(qintptr socketDescriptor) {
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &server::readFromClient);
    connect(socket, &QTcpSocket::disconnected, this, &server::sockDisc);

    sockets.push_back(socket);
    Game_scene.new_player("none");

    int newClientID = Game_scene.players.size() - 1;

    // first message from server is reply to connection and
    //       client's id (his number in players_data):
    //       {"status":"connected", "initialization":"yes",
    //       "id":newClientID}

    json initializationMessage;
    initializationMessage["status"] = "connected";
    initializationMessage["initialization"] = "yes";
    initializationMessage["id"] = newClientID;

    socket->waitForBytesWritten(500);
    socket->write(QString::fromStdString(initializationMessage.dump()).toLatin1());

    qDebug() << "new player " << newClientID << "is connected on socket " << socketDescriptor;
}

void server::readFromClient()
{
    socket = (QTcpSocket*)sender();

    json fromClient;
    try {
        fromClient = json::parse(socket->readAll().toStdString());
    }  catch (json::exception& e) {
        qDebug() << e.what() << '\n';
    }

    // every message from client to server is player data:
    //      { "status":"connected", "name":player_name, "id":clientID,
    //       "angle":player_angle }

    // if client is first additionally get settings:
    //       "bits":bits, "operandsCount":operandsCount, "operands":operands

    if (fromClient["status"] == "connected") {
        int clientID = fromClient["id"];
        QString name = QString::fromStdString(fromClient["name"]);
        double player_angle = fromClient["angle"];
        int red_color = fromClient["red_color"];
        int green_color = fromClient["green_color"];
        int blue_color = fromClient["blue_color"];

        if (clientID == 0) {
            Game_scene.bits = fromClient["bits"];
            Game_scene.operandsCount = fromClient["operandsCount"];
            Game_scene.operands = fromClient["operands"];
        }

        Game_scene.update_player(clientID, name, player_angle, red_color, green_color, blue_color);

        Game_scene.update(clientID);

        sendToClient();
    } else {
        int clientID = fromClient["id"];

        Game_scene.disconnected_player(clientID);
        qDebug() << clientID << " player is disconnected";
    }
}

void server::sendToClient() {
    json toClient;

    // every next message is all scene data, include players, answers
    //       and dots datas, and example:
    //       {"status":"connected", "players":[ players data ],
    //        "answers":[ answers data ], "food":[ food data ],
    //        "expr":example }

    toClient["players"] = Game_scene.get_players();
    toClient["answers"] = Game_scene.get_answers();
    toClient["food"] = Game_scene.get_food();
    toClient["status"] = "connected";

    toClient["expr"] = Game_scene.expr;

    std::vector<std::pair<int, Food>> sended_food;
    for (auto food_iter : Game_scene.get_updated_food()) {
        sended_food.push_back({food_iter, Game_scene.food[food_iter]});
    }

    Game_scene.updated_food.clear();

    for (int i = 0; i < sockets.size(); i++) {

        if (Game_scene.players[i].player_initialization == "yes") {
            Game_scene.players[i].player_initialization = "no";
            toClient["food_status"] = "full";
            toClient["food"] = Game_scene.get_food();
        } else {
            toClient["food_status"] = "updated";
            toClient["food"] = sended_food;
        }

        sockets[i]->write(QString::fromStdString(toClient.dump()).toLatin1());

    }
}

void server::sockDisc()
{

}
