#include "../Shared/ENetWrapper/ENetWrapper.hpp"
#include "../Shared/Events.hpp"
#include "../Shared/ClientData.hpp"
#include <iostream>
#include <unordered_map>
#include "../Shared/Packet.hpp"
#include "../Shared/Vector.hpp"
#include "Random.hpp"
#include "Map.hpp"
#include "Food.hpp"
#include "GameScore.hpp"

static std::unordered_map<uint32_t, ClientData*> g_ClientsMap;

int main() {
  ENet::ServerWrapper ServerWrapper;

  for(size_t i = 0;i < Food::g_FoodAmount;i++){
    Food::Food food;
    food.id = i;
    food.position = Random::GetRandomPositionInRange({0,Map::g_MapSize.x - 2}, {0, Map::g_MapSize.y - 2});

    Food::g_FoodVec.push_back(std::make_shared<Food::Food>(food));
  }

  ServerWrapper.SetAddress(ENET_HOST_ANY, 1337);

  ServerWrapper.CreateHost(32, 2, 0, 0);

  uint32_t client_id = 0;

  while (true) {
    while (enet_host_service(ServerWrapper.GetHost(), &ServerWrapper.GetEvent(), 500) > 0) {
      switch (ServerWrapper.GetEvent().type) {
      case ENET_EVENT_TYPE_CONNECT: {
        std::cout << "New client successfully connected" << '\n';
        client_id++;
        ClientData cl;
        cl.id = client_id;
        
        if (g_ClientsMap.count(client_id) == 0) {
          g_ClientsMap.insert(std::make_pair(client_id, new ClientData(cl)));
          GameScore::GameScore game_score;
          game_score.client_id = client_id;
          game_score.score = 0;

          GameScore::g_GameScoreVec.push_back(game_score);
        }

        ServerWrapper.GetEvent().peer->data = g_ClientsMap[client_id];

        Network::Packet packet;
        packet << static_cast<uint8_t>(Event::CLIENT_ID) << client_id;
        
        Network::send_packet(ServerWrapper.GetEvent().peer,packet,0,ENET_PACKET_FLAG_RELIABLE);
        
        Network::Packet map_size_packet;
        map_size_packet << static_cast<uint8_t>(Event::MAP_SIZE) << Map::g_MapSize;

        Network::send_packet(ServerWrapper.GetEvent().peer,map_size_packet,0,ENET_PACKET_FLAG_RELIABLE);

        enet_host_broadcast(ServerWrapper.GetHost(),0,Food::CreateFoodPositionPacket(Food::g_FoodVec));
        break;
      }
      case ENET_EVENT_TYPE_DISCONNECT: {
        std::cout << "New client successfully disconnected" << '\n';
        break;
      }
      case ENET_EVENT_TYPE_RECEIVE: {

        // printf("A packet of length %u containing %s was received from %s on "
        //        "channel %u.\n",
        //        ServerWrapper.GetEvent().packet->dataLength,
        //        ServerWrapper.GetEvent().packet->data,
        //        ServerWrapper.GetEvent().peer->data,
        //        ServerWrapper.GetEvent().channelID);

      uint8_t event{};

      Network::Packet packet;
      packet.Append(ServerWrapper.GetEvent().packet->data,ServerWrapper.GetEvent().packet->dataLength);
      packet >> event;
      
      switch((Event)event){
        case Event::BROADCAST_POSITION:{
          uint32_t recv_id{};
          uint8_t entity_type;
          Vec2 pos;
          packet >> recv_id >> entity_type
          >> pos;
          
          if((EntityType)entity_type == EntityType::SNAKE){
            for(auto& food : Food::g_FoodVec){
              if(pos == food->position){
                int score_to_add = 1;
                auto& game_score_vec = GameScore::g_GameScoreVec;

                auto it = std::find_if(game_score_vec.begin(),game_score_vec.end(),[&](const GameScore::GameScore& game_score){
                  return (game_score.client_id == recv_id);
                });

                if(it != game_score_vec.end()){
                  it->score += score_to_add;

                  Network::Packet score_packet;
                  score_packet << static_cast<uint8_t>(Event::BROADCAST_SCORE) << recv_id << it->score;

                  enet_host_broadcast(ServerWrapper.GetHost(),0,Network::create_packet(score_packet,ENET_PACKET_FLAG_RELIABLE));
                }

                food->position = Random::GetRandomPositionInRange({0,Map::g_MapSize.x - 2}, {0, Map::g_MapSize.y - 2});
                enet_host_broadcast(ServerWrapper.GetHost(),0,Food::CreateFoodPositionPacket(Food::g_FoodVec));
              }
            }
          }

          enet_host_broadcast(ServerWrapper.GetHost(),0,Network::create_packet(packet,ENET_PACKET_FLAG_RELIABLE));
          
          break;
        }
        case Event::DIRECTION:{
          uint32_t recv_id{};
          Vec2 direction;
          Vec2 current_snake_pos;
          
          packet >> recv_id >> direction >> current_snake_pos;

          current_snake_pos += direction;

          Network::Packet snake_pos_packet;
          snake_pos_packet << static_cast<uint8_t>(Event::BROADCAST_POSITION) << recv_id << static_cast<uint8_t>(EntityType::SNAKE) << current_snake_pos;
          
          enet_host_broadcast(ServerWrapper.GetHost(),0,Network::create_packet(snake_pos_packet,ENET_PACKET_FLAG_RELIABLE));

          break;
        }
        case Event::USERNAME:{
          uint32_t recv_id{};
          char username[1024];
          packet >> recv_id >> username;
          
          if(g_ClientsMap.count(recv_id) >= 1){
            g_ClientsMap[recv_id]->sUsername = username;
            if(g_ClientsMap.size() > 1){
              Network::Packet username_packet;
              username_packet << static_cast<uint8_t>(Event::USERNAME) << recv_id << username;
              enet_host_broadcast(ServerWrapper.GetHost(),0,Network::create_packet(username_packet,ENET_PACKET_FLAG_RELIABLE));
            }
          }
          break;
        }
      }

      enet_packet_destroy (ServerWrapper.GetEvent().packet);
      break;
      } // !ENET_EVENT_TYPE_RECEIVE
      }//! switch
    }
  }

  for(auto& client : g_ClientsMap){
    delete client.second;
  }

  return 0;
}