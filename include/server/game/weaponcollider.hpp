#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "server/game/player.hpp"
#include "shared/audio/soundtype.hpp"
#include "shared/game/point_light.hpp"
#include "shared/game/sharedmodel.hpp"
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

protected:
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

    Lightning(glm::vec3 corner, glm::vec3 facing, const PointLightProperties& properties) :
        WeaponCollider(nullptr, corner, facing, DIMENSION, ModelType::Lightning,
            WeaponOptions(LIGHTNING_DMG, LIGHTNING_PREP, LIGHTNING_DUR, false)),
        properties(properties)
    {
        this->sound = ServerSFX::Thunder;
    }
	virtual SharedObject toShared() override {
        auto so = WeaponCollider::toShared();
        so.pointLightInfo = SharedPointLightInfo {
            .intensity = (this->info.attacked) ? 1.0f : 0.3f,
            .ambient_color = this->properties.ambient_color,
            .diffuse_color = this->properties.diffuse_color,
            .specular_color = this->properties.specular_color,
            .attenuation_linear = this->properties.attenuation_linear,
            .attenuation_quadratic = this->properties.attenuation_quadratic,
        };
        return so;
    }
private:
    PointLightProperties properties;
};