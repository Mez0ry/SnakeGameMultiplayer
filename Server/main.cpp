#include "../Shared/ENetWrapper/ENetWrapper.hpp"
#include "../Shared/Events.hpp"
#include "../Shared/ClientData.hpp"
#include <iostream>
#include <memory>
#include <unordered_map>
#include <cstring>
#include <charconv>
#include "../Shared/Packet.hpp"
#include "../Shared/Vector.hpp"
#include <random>

static std::unordered_map<uint32_t, ClientData*> g_ClientsMap;

static uint32_t g_FoodAmount = 8;

extern int GetRandomIntInRange(int a,int b){
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> distr(a,b);
  return distr(rng);
}

extern Vec2 GetRandomPosition(){
  return {GetRandomIntInRange(1,18),GetRandomIntInRange(1,18)};
}

static std::unordered_map<uint32_t,Vec2> g_FoodMap; // key - id of food, value - their position

static std::unordered_map<uint32_t,int> g_GameScoreMap; //key - client_id, value - score

[[nodiscard]] extern ENetPacket* GetFoodPositionPacket(){
  Network::Packet packet;
  packet << static_cast<uint8_t>(Event::BROADCAST_POSITION) << 0u << static_cast<uint8_t>(EntityType::FOOD);
  for(auto& food : g_FoodMap){
    packet << food.first << food.second;
  }
  return Network::create_packet(packet,ENET_PACKET_FLAG_RELIABLE);
}

int main() {
  ENet::ServerWrapper ServerWrapper;

  for(uint32_t i = 0;i<g_FoodAmount;i++){
    g_FoodMap.insert(std::make_pair(i,GetRandomPosition()));
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
          g_GameScoreMap.insert(std::make_pair(client_id,0));
        }

        ServerWrapper.GetEvent().peer->data = g_ClientsMap[client_id];

        Network::Packet packet;
        packet << static_cast<uint8_t>(Event::CLIENT_ID) << client_id;

        Network::send_packet(ServerWrapper.GetEvent().peer,packet,0,ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(ServerWrapper.GetHost(),0,GetFoodPositionPacket());
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
            for(auto& food : g_FoodMap){
              if(pos == food.second){
                int score_to_add = 1;
                g_GameScoreMap[recv_id] += score_to_add;

                Network::Packet score_packet;
                score_packet << static_cast<uint8_t>(Event::BROADCAST_SCORE) << recv_id << g_ClientsMap[recv_id]->sUsername.c_str() << g_GameScoreMap[recv_id];

                enet_host_broadcast(ServerWrapper.GetHost(),0,Network::create_packet(score_packet,ENET_PACKET_FLAG_RELIABLE));
                
                food.second = GetRandomPosition();
                enet_host_broadcast(ServerWrapper.GetHost(),0,GetFoodPositionPacket());
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
          }
          break;
        }
      }

      enet_packet_destroy (ServerWrapper.GetEvent().packet);
      break;
      } // !ENET_EVENT_TYPE_RECEIVE
      }
    }
  }

  for(auto& client : g_ClientsMap){
    delete client.second;
  }
  return 0;
}