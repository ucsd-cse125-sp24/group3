#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "server/game/player.hpp"
#include "shared/audio/soundtype.hpp"
#include <chrono>

/*
 * Class for spawning weapon colliders
 */
class WeaponCollider : public Object {
public:
    ServerSFX sound;

    struct WeaponOptions {
        WeaponOptions(int damage, int timeUntilAttack, int attackDuration, bool followPlayer) :
            damage(damage), timeUntilAttack(timeUntilAttack), attackDuration(attackDuration), followPlayer(followPlayer)
        {}

        int damage;
        int timeUntilAttack; //in milliseconds
        int attackDuration; //in milliseconds
        bool followPlayer;
    };

    WeaponCollider(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing, \
        glm::vec3 dimensions, ModelType model, WeaponOptions&& options);

    void doCollision(Object* other, ServerGameState& state) override;
    void updateMovement(ServerGameState& state);
    bool readyTime(ServerGameState& state);
    bool timeOut(ServerGameState& state);
    virtual SharedObject toShared() override;

private:
    std::chrono::time_point<std::chrono::system_clock> preparing_time;
    std::chrono::time_point<std::chrono::system_clock> attacked_time;
    Player* usedPlayer;
    SharedWeaponInfo info;
    WeaponOptions opt;
    bool playSound;
};

class ShortAttack : public WeaponCollider {
public:
    inline static const glm::vec3 DIMENSION = glm::vec3(1.0f, 5.0f, 1.0f);

    ShortAttack(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing):
        WeaponCollider(usedPlayer, corner, facing, DIMENSION, ModelType::Cube,
            WeaponOptions(DAGGER_DMG, DAGGER_PREP, DAGGER_DUR, true))
    {
        this->sound = ServerSFX::Dagger;
    }
};

class MediumAttack : public WeaponCollider {
public:
    inline static const glm::vec3 DIMENSION = glm::vec3(1.5f, 5.0f, 1.5f);

    MediumAttack(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing) :
        WeaponCollider(usedPlayer, corner, facing, DIMENSION, ModelType::Cube,
            WeaponOptions(SWORD_DMG, SWORD_PREP, SWORD_DUR, true))
    {
        this->sound = ServerSFX::Sword;
    }
};

class BigAttack : public WeaponCollider {
public:
    inline static const glm::vec3 DIMENSION = glm::vec3(2.5f, 5.0f, 2.5f);

    BigAttack(Player* usedPlayer, glm::vec3 corner, glm::vec3 facing) :
        WeaponCollider(usedPlayer, corner, facing, DIMENSION, ModelType::Cube,
            WeaponOptions(HAMMER_DMG, HAMMER_PREP, HAMMER_DUR, true))
    {
        this->sound = ServerSFX::Hammer;
    }
};

class Lightning : public WeaponCollider {
public:
    inline static const glm::vec3 DIMENSION = glm::vec3(3.0f, 100.0f, 3.0f);

    Lightning(glm::vec3 corner, glm::vec3 facing) :
        WeaponCollider(nullptr, corner, facing, DIMENSION, ModelType::Cube,
            WeaponOptions(LIGHTNING_DMG, LIGHTNING_PREP, LIGHTNING_DUR, false))
    {}
};