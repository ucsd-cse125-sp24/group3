#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "server/game/player.hpp"
#include <chrono>

/*
 * Class for spawning weapon colliders
 */
class WeaponCollider : public Object {
public:

    struct WeaponOptions {
        WeaponOptions(int damage, float timeUntilAttack, int attackDuration) :
            damage(damage), timeUntilAttack(timeUntilAttack), attackDuration(attackDuration)
        {}

        int damage;
        int timeUntilAttack; //in milliseconds
        int attackDuration; //in milliseconds
    };

    WeaponCollider(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing, \
        glm::vec3 dimensions, ModelType model, WeaponOptions&& options);

    void doCollision(Object* other, ServerGameState& state) override;
    void updateMovement(ServerGameState& state);
    bool readyTime();
    bool timeOut();
    void removeAttack(ServerGameState& state);

private:
    std::chrono::time_point<std::chrono::system_clock> preparing_time;
    std::chrono::time_point<std::chrono::system_clock> attacked_time;
    bool attacked;
    Player* usedPlayer;
    WeaponOptions opt;
};

class ShortAttacks : public WeaponCollider {
public:
    //TODO find good dimension
    inline static const glm::vec3 DIMENSION = glm::vec3(1.0f, 5.0f, 1.0f);
    inline static const int DMG = DAGGER_DMG;
    inline static const float PREP_TIME = 150;
    inline static const float ATTACK_DURATION = 150;

    ShortAttacks(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing):
        WeaponCollider(usedPlayer, corner, facing, DIMENSION, ModelType::Cube,
            WeaponOptions(DMG, PREP_TIME, ATTACK_DURATION))
    {}
};

class LongAttacks : public WeaponCollider {
public:
    inline static const glm::vec3 DIMENSION = glm::vec3(1.5f, 5.0f, 1.5f);
    inline static const int DMG = KATANA_DMG;
    inline static const float PREP_TIME = 250;
    inline static const float ATTACK_DURATION = 300;

    LongAttacks(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing) :
        WeaponCollider(usedPlayer, corner, facing, DIMENSION, ModelType::Cube,
            WeaponOptions(DMG, PREP_TIME, ATTACK_DURATION))
    {}
};

class BigAttacks : public WeaponCollider {
public:
    inline static const glm::vec3 DIMENSION = glm::vec3(2.5f, 5.0f, 2.5f);
    inline static const int DMG = HAMMER_DMG;
    inline static const float PREP_TIME = 400;
    inline static const float ATTACK_DURATION = 500;

    BigAttacks(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing) :
        WeaponCollider(usedPlayer, corner, facing, DIMENSION, ModelType::Cube,
            WeaponOptions(DMG, PREP_TIME, ATTACK_DURATION))
    {}
};