#ifndef __FOOD_HPP__
#define __FOOD_HPP__
#include "../Shared/Packet.hpp"
#include "../Shared/Vector.hpp"
#include "../Shared/Events.hpp"

#include <vector>
#include <memory>

namespace Food{

    typedef struct Food{
        uint32_t id;
        Vec2 position;
    };
    
    static uint32_t g_FoodAmount = 8;
    
    static std::vector<std::shared_ptr<Food>> g_FoodVec;

    [[nodiscard]] extern ENetPacket* CreateFoodPositionPacket(const std::vector<std::shared_ptr<Food>>& food_list){
        Network::Packet packet;
        packet << static_cast<uint8_t>(Event::BROADCAST_POSITION) << 0u << static_cast<uint8_t>(EntityType::FOOD) << g_FoodAmount;

        for(auto& food : food_list){
         packet << food->id << food->position;
        }

        return Network::create_packet(packet,ENET_PACKET_FLAG_RELIABLE);
    }
    
};//!namespace
#endif //! __FOOD_HPP__