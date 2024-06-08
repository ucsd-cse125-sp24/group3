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
     * @param dist_orb distance to orb, to see if it should be shaded blue
     * @param dist_exit distance to the exit, to see if it should be shaded white
	 */
	explicit Torchlight(glm::vec3 corner, float dist_orb, float dist_exit);

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
     * @param the position the lightning hit (if exists)
     * @param the position of the light cut action (if exists)
     */
    bool doTick(ServerGameState& state, std::optional<glm::vec3> lightning_light_cut_pos, std::optional<glm::vec3> action_light_cut_pos);

    /**
     * @brief manually set torchlight intensity, for use in intro cutscene
     */
    void overrideIntensity(float val);

    /**
     * @brief get current intensity of torch from 0-1
     */
    float getIntensity() const;

    bool is_cut;
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
