#include <testing/test.h>
#include <algorithm>
#include "monster_generated.h"

using namespace bench;

template<typename T>
class BlitzbuffersArchiver : public ISerializerTest {
private:
    T& _builder;
    std::string _name;

public:
    BlitzbuffersArchiver(T& builder, std::string name) : _builder(builder), _name(name) {
    }

    Buf serialize(const std::vector<MyTypes::Monster> &data) override {
        _builder.clear();
 
        auto list = MonstersList::new_on(_builder);
        auto monsters = list.insert_data(data.size());
        
        for (size_t i = 0; i < data.size(); i++) {
            auto& org = data[i];
            auto enc = monsters[i];
            
            enc.insert_name(org.name);
            enc.color = static_cast<Color>(org.color);
            enc.pos = { org.pos.x, org.pos.y, org.pos.z };
            enc.hp = org.hp;
            enc.mana = org.mana;

            enc.equipped.insert_name(org.equipped.name);
            enc.equipped.damage = org.equipped.damage;
            
            auto weapons = enc.insert_weapons(org.weapons.size());
            for (size_t j = 0; j < org.weapons.size(); j++) {
                auto& org_w = org.weapons[j];
                auto enc_w = weapons[j];

                enc_w.damage = org_w.damage;
                enc_w.insert_name(org_w.name);
            }

            auto items = enc.insert_inventory(org.inventory.size());
            for (size_t j = 0; j < org.inventory.size(); j++) {
                items[j] = org.inventory[j];
            }

            auto path = enc.insert_path(org.path.size());
            for (size_t j = 0; j < org.path.size(); j++) {
                auto& pos = org.path[j];
                path[j] = { pos.x, pos.y, pos.z };
            }

        }

        auto [size, buffer] = _builder.build();
        return Buf{ buffer, size };
    }

    void deserialize(Buf buf, std::vector<MyTypes::Monster> &res) override {
        auto list = MonstersList::view(buf.ptr);
        res.resize(list.data().size());
        for (size_t i = 0; i < list.data().size(); i++)
        {
            auto monster = list.data()[i];
            auto& resM = res[i];

            resM.equipped.damage = monster.equipped.damage;
            resM.equipped.name = monster.equipped.name();

            resM.name = monster.name();
            resM.mana = monster.mana;
            resM.hp = monster.hp;
            resM.pos = MyTypes::Vec3 { monster.pos.x, monster.pos.y, monster.pos.z };
            resM.color = static_cast<MyTypes::Color>(monster.color);

            auto inventory = monster.inventory();
            resM.inventory.resize(inventory.size());
            std::copy(inventory.begin(), inventory.end(), resM.inventory.begin());
            
            auto weapons = monster.weapons();
            resM.weapons.resize(weapons.size());
            for (size_t j = 0; j < weapons.size(); j++) {
                auto weapon = weapons[j];
                auto& res_w = resM.weapons[j];

                res_w.damage = weapon.damage;
                res_w.name = weapon.name();
            }

            auto path = monster.path();
            resM.path.resize(path.size());
            for (size_t j = 0; j < path.size(); j++) {
                auto pos = path[j];
                auto& res_p = resM.path[j];

                res_p = { pos.x, pos.y, pos.z };
            }
        }
    };

    TestInfo testInfo() const override {
        return {
                SerializationLibrary::BLITZBUFFERS,
                _name,
                ""
        };
    }
};
