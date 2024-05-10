#pragma once

/**
 * @brief Enumeration of every render model in the game.
 */
enum class ModelType {
	Cube,
	Player,
	WarrenBear
};

/**
 * @brief Struct containing an object's render information for the client.
 */
struct SharedModel {
	/**
	 * @brief Type of render model that the object containing this SharedModel
	 * should be rendered as
	 */
	ModelType type;

	/**
	 * @brief Dimensions of a rectangular prism that circumscribes the model.
	 * These dimensions are derived from the model's dimensions in its render
	 * model object file.
	 */
	glm::vec3 dimensions;

	/**
	 * @brief 3-D vector whose components are multipliers for this model's
	 * dimensions.
	 * @note The effective dimensions of the render model (and of the object 
	 * that contains this SharedModel struct) are given by
	 * SharedModel::dimensions * SharedModel::scale
	 */
	glm::vec3 scale;

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& type& dimensions& scale;
	}
};