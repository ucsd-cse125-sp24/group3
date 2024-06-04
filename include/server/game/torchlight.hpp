#pragma once

#include "server/game/constants.hpp"
#include "server/game/object.hpp"
#include "shared/game/point_light.hpp"
#include "shared/game/sharedobject.hpp"

class Torchlight : public Object {
public:
	/**
     * Creates a torchight with default lighting properties.
	 * @param corner Corner position of the surface
	 */
	explicit Torchlight(glm::vec3 corner);

	/**
	 * @param corner Corner position of the surface
     * @param properties allows for customization of lighting
     * and flickering properties
	 */
	Torchlight(glm::vec3 corner, const PointLightProperties& properties);
	~Torchlight();

    SharedObject toShared() override;

    /**
     * @brief runs on every server tick to update torchlight flickering 
     * animations
     * @parm current ServerGameState
     */
    void doTick(ServerGameState& state);

    /**
     * @brief get current intensity of torch from 0-1
     */
    float getIntensity() const;
private:
    PointLightProperties properties;

    // current intensity from 0-1 that gets
    // sent to client
    float curr_intensity;

    // state needed for animated flickering 

    // curr_step from 0-1 in flickering animation
    float curr_step;
    // how much the intensity should change on every server tick
    float flickering_speed; 
    // if the flickering animation is inceasing in
    // intensity or decreasing
    bool inc_intensity;

    // shared initialization between multiple constructors 
    void init();
};
