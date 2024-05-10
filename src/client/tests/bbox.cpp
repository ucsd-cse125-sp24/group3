#include <gtest/gtest.h>

#include "client/util.hpp"

TEST(Bbox, BboxCombine) {
    struct TestCase {
        std::string name;
        
        Bbox bbox1;
        Bbox bbox2;
        Bbox expectedCombined;
    };
    
    const std::vector<TestCase> testCases{{
        {
            .name = "two 1 unit boxes",
            .bbox1 = Bbox {
                .min = glm::vec3(-1.0f, -1.0f, -1.0f),
                .max = glm::vec3(0.0f, 0.0f, 0.0f),
            },
            .bbox2 = Bbox {
                .min = glm::vec3(0.0f, 0.0f, 0.0f),
                .max = glm::vec3(1.0f, 1.0f, 1.0f),
            },
            .expectedCombined = Bbox {
                .min = glm::vec3(-1.0f, -1.0f, -1.0f),
                .max = glm::vec3(1.0f, 1.0f, 1.0f),
            }
        },
        {
            .name = "one small and one large",
            .bbox1 = Bbox {
                .min = glm::vec3(-100.0f, -78.0f, -69.0f),
                .max = glm::vec3(-30.0f, -42.0f, -13.0f),
            },
            .bbox2 = Bbox {
                .min = glm::vec3(10.0f, 10.0f, 10.0f),
                .max = glm::vec3(11.0f, 11.0f, 11.0f),
            },
            .expectedCombined = Bbox {
                .min = glm::vec3(-100.0f, -78.0f, -69.0f),
                .max = glm::vec3(11.0f, 11.0f, 11.0f),
            }
        },
        {
            .name = "overlapping",
            .bbox1 = Bbox {
                .min = glm::vec3(5.0f, 5.0f, 5.0f),
                .max = glm::vec3(10.0f, 10.0f, 10.0f),
            },
            .bbox2 = Bbox {
                .min = glm::vec3(7.5f, 7.5f, 7.5f),
                .max = glm::vec3(15.0f, 15.0f, 15.0f),
            },
            .expectedCombined = Bbox {
                .min = glm::vec3(5.0f, 5.0f, 5.0f),
                .max = glm::vec3(15.0f, 15.0f, 15.0f),
            }
        },
        {
            .name = "one inside another",
            .bbox1 = Bbox {
                .min = glm::vec3(0.0f, 0.0f, 0.0f),
                .max = glm::vec3(10.0f, 10.0f, 10.0f),
            },
            .bbox2 = Bbox {
                .min = glm::vec3(0.01f, 0.01f, 0.01f),
                .max = glm::vec3(9.99f, 9.99f, 9.99f),
            },
            .expectedCombined = Bbox {
                .min = glm::vec3(0.0f, 0.0f, 0.0f),
                .max = glm::vec3(10.0f, 10.0f, 10.0f),
            }
        },
    }};

    for (const auto &tc : testCases) {
        Bbox combined = combineBboxes(tc.bbox1, tc.bbox2);
        EXPECT_EQ(combined, tc.expectedCombined);
    };
}
